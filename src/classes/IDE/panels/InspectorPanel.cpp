/**
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is part of this project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v3.0.
 *
 * This program is distributed WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details:
 * https://www.gnu.org/licenses/
 */

#include "InspectorPanel.h"
#include "IDE/DesignTokens.h"
#include "IDE/dialogs/ModalScaffold.h"
#include "Inspector/Editors/EditorLayout.h"
#include "Core/PathService.h"
#include "Core/Project.h"
#include "Entities/BaseEntity.h"
#include "Entities/State.h"
#include "app.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <SDL3/SDL.h>
#include <nfd.hpp>
#include "UI/NfdWindowHandle.h"
#include "app.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include "Inspector/PropertyValue.h"
#include "image_formats.h"

namespace ADS::IDE::Panels {

using namespace ADS::IDE::Colors;

InspectorPanel::InspectorPanel()
    : BasePanel("hInspector"),
      m_selectedObject(nullptr),
      m_needsRefresh(false)
{
    m_titleKey = "INSPECTOR";

    // Editors link only against Dear ImGui and cannot reach the translations
    // manager; hand them a resolver so their own literals (empty-option
    // notices, dialog buttons) localize with the rest of the UI.
    auto* tm = this->getTranslationsManager();
    Inspector::Editors::IPropertyEditor::setTranslator(
        [tm](std::string_view key) { return tm->_t(std::string(key)); });

    // Editor-owned popups (StringEditor's expand dialogs) are plain modals, not
    // ModalScaffold ones — give them the modal-dialog blue so they stay
    // distinct from the deeper docked-panel caption.
    Inspector::Editors::IPropertyEditor::setDialogAccent(Colors::C_DIALOG_TITLE);
    Inspector::Editors::IPropertyEditor::setDialogTextColor(Colors::C_CAPTION_TEXT);
}

// ---------------------------------------------------------------------------
// renderObjectHeader
// ---------------------------------------------------------------------------

/**
 * @brief Render the object header (type and name)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */
void InspectorPanel::renderObjectHeader()
{
    if (!m_selectedObject) return;

    // Type badge
    ImGui::PushStyleColor(ImGuiCol_Text, C_SCENE);
    ImGui::Text("[%s]", m_selectedObject->getTypeName().c_str());
    ImGui::PopStyleColor();

    ImGui::SameLine(0, 6);
    ImGui::TextColored(TEXT0, "%s", m_selectedObject->getDisplayName().c_str());
}

// ---------------------------------------------------------------------------
// renderCategory
// ---------------------------------------------------------------------------

/**
 * @brief Render properties for a category
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 *
 * @param category Category name
 * @param properties Properties in this category
 */
void InspectorPanel::renderCategory(
    const std::string& category,
    const std::vector<Inspector::PropertyDescriptor>& properties)
{
    ImGui::PushStyleColor(ImGuiCol_Header,        BG3);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, withAlpha(C_SCENE, 0.20f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,  withAlpha(C_SCENE, 0.35f));
    ImGui::PushStyleColor(ImGuiCol_Text,          TEXT0);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen
                             | ImGuiTreeNodeFlags_SpanAvailWidth
                             | ImGuiTreeNodeFlags_FramePadding;

    bool open = ImGui::CollapsingHeader(category.c_str(), flags);

    ImGui::PopStyleColor(4);

    if (open) {
        ImGui::Indent(8.0f);

        // Measure the widest visible label in this category so every row
        // shares one column split instead of each row picking its own
        // width (which misaligned e.g. Scene's "Exits" category, where
        // "North" and "Northeast" split at different x-positions).
        float maxLabelWidth = 0.0f;
            for (const auto& descriptor : properties) {
                if (!descriptor.isVisible(m_selectedObject))
                    continue;
                maxLabelWidth = std::max(maxLabelWidth,
                    ImGui::CalcTextSize(descriptor.getDisplayName().c_str()).x + 16.0f);
            }

        {
            Inspector::Editors::CategoryColumnWidthScope columnScope(maxLabelWidth);

            // Rows may carry a one-level subcategory (e.g. Item's Effects
            // category split into "Damage" / "Heal"); render each run of
            // rows sharing a non-empty subcategory under its own collapsible
            // node. Rows with no subcategory render directly as before.
            std::string currentSub;
            bool inSub   = false;   // a subgroup node has been emitted
            bool subOpen = false;   // and it is expanded

            const auto closeSub = [&]() {
                if (inSub) {
                    if (subOpen) {
                        ImGui::Unindent(8.0f);
                        ImGui::TreePop();
                    }
                    inSub   = false;
                    subOpen = false;
                }
            };

            for (const auto& descriptor : properties) {
                if (!descriptor.isVisible(m_selectedObject))
                    continue;

                const std::string& sub = descriptor.getSubcategory();
                if (sub != currentSub) {
                    closeSub();
                    currentSub = sub;
                    if (!sub.empty()) {
                        subOpen = ImGui::TreeNodeEx(
                            sub.c_str(),
                            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth);
                        inSub = true;
                        if (subOpen)
                            ImGui::Indent(8.0f);
                    }
                }

                if (inSub && !subOpen)
                    continue; // subgroup collapsed — skip its rows

                renderProperty(descriptor);
            }
            closeSub();
        }

        ImGui::Unindent(8.0f);
        ImGui::Spacing();
    }
}

// ---------------------------------------------------------------------------
// renderProperty
// ---------------------------------------------------------------------------

/**
 * @brief Render a single property using the appropriate editor
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 *
 * @param descriptor Property metadata
 */
void InspectorPanel::renderProperty(const Inspector::PropertyDescriptor& descriptor)
{
    Inspector::PropertyValue currentValue =
        m_selectedObject->getPropertyValue(descriptor.getId());

    Inspector::Editors::IPropertyEditor* editor =
        m_editorRegistry.getEditorForProperty(descriptor);

    if (!editor) {
        ImGui::TextDisabled("%s: (no editor)", descriptor.getDisplayName().c_str());
        return;
    }

    // Pre-compute the widget's ImGuiID. All editors call PushID(descriptor.getId())
    // and then name their primary widget "##value", so we can predict the ID without
    // rendering anything. This lets us check g.ActiveId after the render.
    ImGui::PushID(descriptor.getId().c_str());
    ImGuiID widgetId = ImGui::GetID("##value");
    ImGui::PopID();

    // Apply design-token styling to all input widgets
    ImGui::PushStyleColor(ImGuiCol_FrameBg,        BG1);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, BG2);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,  BG3);
    ImGui::PushStyleColor(ImGuiCol_Border,         BORDER);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));

    Inspector::Editors::EditResult result = editor->render(
        descriptor, currentValue, descriptor.isReadOnly());

    // After the editor returns, g.LastItemData still holds the main widget (column 1).
    // Capture the rect BEFORE popping styles (rect data is independent of style state).
    ImVec2 widgetMin = ImGui::GetItemRectMin();
    ImVec2 widgetMax = ImGui::GetItemRectMax();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(4);

    // Overdraw a BORDER_HI rectangle around just the widget when it is active.
    // g.ActiveId persists for the duration of the interaction (typing, dragging),
    // and widgetId was pre-computed to match exactly this editor's "##value" widget.
    ImGuiContext& g = *ImGui::GetCurrentContext();
    if (g.ActiveId == widgetId) {
        ImGui::GetWindowDrawList()->AddRect(
            widgetMin, widgetMax,
            ImGui::ColorConvertFloat4ToU32(BORDER_HI),
            4.0f, 0, 1.5f
        );
    }

    // Image / avatar fields get a thumbnail directly under the row.
    renderImageThumbnail(descriptor);

    if (result.fileDialogRequested) {
        // Never call NFD here — this runs mid-frame, before SDL_RenderPresent.
        // Queue it; processPendingFileDialog() performs the actual dialog
        // after the frame is presented (see CLAUDE.md's deferred dialog pattern).
        m_pendingFileDialog = true;
        m_pendingFileDialogPropertyId = descriptor.getId();
        m_pendingFileDialogExtensions = result.fileDialogExtensions;
    } else if (result.labelDialogRequested) {
        // A pure ImGui modal (no OS interaction) — safe to open next frame
        // via renderLabelDialog(), no deferred-dialog treatment needed.
        m_pendingLabelDialog = true;
        m_pendingLabelDialogPropertyId = descriptor.getId();
        m_pendingLabelDialogCategory = descriptor.getCategory();
        m_pendingLabelDialogDisplayName = descriptor.getDisplayName();
        m_pendingLabelDialogBitPosition = result.labelDialogBitPosition;
        m_pendingLabelDialogPendingValue = result.labelDialogPendingValue;
    } else if (result.createNewRequested) {
        // Fires the exact same creation as the tree's "Add > State" — no
        // separate Name/Next dialog (see onCreateState's docstring). Every
        // Select field with the "+" button (Scene's "state", a State's own
        // "next", a StateChain's own "head") references a State directly,
        // so this always creates a State. The new state starts fully
        // unlinked, and *this* field's own current selection is left
        // untouched (stays "(None)" if it already was) — the user picks it
        // from the dropdown themselves afterward, the same as any other
        // option.
        if (onCreateState) onCreateState();
        m_needsRefresh = true;
        if (onPropertyChanged) onPropertyChanged();
    } else if (result.changed) {
        m_selectedObject->setPropertyValue(descriptor.getId(), result.newValue);
        if (onPropertyChanged) onPropertyChanged();
    }
}

// ---------------------------------------------------------------------------
// renderImageThumbnail
// ---------------------------------------------------------------------------

/**
 * @brief Draw a small image preview under an image / avatar field
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 *
 * No-op unless @p descriptor is a file-path field whose allowed
 * extensions are image formats and whose current value names a file
 * that exists on disk. The thumbnail is scaled to fit a 128 px box
 * while preserving aspect ratio.
 *
 * @param descriptor The property being rendered
 */
void InspectorPanel::renderImageThumbnail(const Inspector::PropertyDescriptor& descriptor)
{
    const auto& constraints = descriptor.getConstraints();
    if (!constraints.isFilePath || !m_selectedObject) {
        return;
    }

    // Only image-typed file fields get a preview (Constants::ImageFormats is
    // the one place the accepted set is defined).
    const auto imageExts = ADS::Constants::ImageFormats::supportedExtensions();
    bool isImageField = false;
    for (const auto& ext : constraints.fileExtensions) {
        if (std::find(imageExts.begin(), imageExts.end(), ext) != imageExts.end()) {
            isImageField = true;
            break;
        }
    }
    if (!isImageField) {
        return;
    }

    const std::string path = Inspector::getValueOr<std::string>(
        m_selectedObject->getPropertyValue(descriptor.getId()), std::string{});
    if (path.empty()) {
        return;
    }

    // A value that is not a readable PNG / JPEG / WebP file (a bare filename, a
    // base64 blob, an unset path, a mis-named non-image) simply shows no
    // preview.
    std::error_code ec;
    if (!std::filesystem::exists(path, ec) || ec ||
        !ADS::Constants::ImageFormats::isSupportedImageFile(path)) {
        return;
    }

    SDL_Texture* texture = m_thumbnailCache.get(path);
    if (texture == nullptr) {
        return;
    }

    float texW = 0.0f;
    float texH = 0.0f;
    SDL_GetTextureSize(texture, &texW, &texH);
    if (texW <= 0.0f || texH <= 0.0f) {
        return;
    }

    // Fit inside a 128 px box without upscaling past the native size.
    constexpr float kMaxBox = 128.0f;
    const float scale = std::min({kMaxBox / texW, kMaxBox / texH, 1.0f});
    const ImVec2 drawSize(texW * scale, texH * scale);

    ImGui::Indent(8.0f);
    ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<intptr_t>(texture)), drawSize);
    ImGui::Unindent(8.0f);
    ImGui::Spacing();
}

// ---------------------------------------------------------------------------
// renderLabelDialog
// ---------------------------------------------------------------------------

/**
 * @brief Render the "type a label" popup for a user-defined bitmap bit, if pending or open
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * Must be called once per frame from render(). OK commits both the
 * label (via IInspectable::setUserDefinedOptionLabel()) and the
 * checkbox toggle that triggered it (via setPropertyValue()).
 * Cancel needs no revert: SelectEditor never wrote the checked bit
 * into the property's persisted value in the first place.
 */
void InspectorPanel::renderLabelDialog()
{
    static const char* POPUP_ID = "###inspector_user_defined_label_popup";

    if (m_pendingLabelDialog) {
        std::string existing;
        if (m_selectedObject) {
            existing = m_selectedObject->getUserDefinedOptionLabel(
                m_pendingLabelDialogPropertyId, m_pendingLabelDialogBitPosition);
        }
        std::snprintf(m_labelDialogInputBuffer, sizeof(m_labelDialogInputBuffer), "%s", existing.c_str());
        ImGui::OpenPopup(POPUP_ID);
        m_pendingLabelDialog = false;
    }

    const std::string caption = m_pendingLabelDialogCategory + " - " + m_pendingLabelDialogDisplayName
        + " - " + this->getTranslationsManager()->_t("DIALOG.NEW_VALUE");

    ADS::IDE::ModalStyle style;
    style.initialSize = ImVec2(380.0f, 0.0f);
    style.autoResize  = true;
    if (auto* fonts = Core::App::getFontManager()) {
        style.captionFont = fonts->getFont("mediumFont");
    }

    if (ADS::IDE::beginModal(POPUP_ID, caption, style)) {
        // Escape aborts, same as the Cancel button (nothing to revert).
        if (ADS::IDE::modalEscapeRequested()) {
            ImGui::CloseCurrentPopup();
            ADS::IDE::endModal();
            return;
        }

        ImGui::SetNextItemWidth(240.0f);
        ImGui::InputText("##inspector_label_dialog_input", m_labelDialogInputBuffer, sizeof(m_labelDialogInputBuffer));

        if (ImGui::Button(this->getTranslationsManager()->_t("DIALOG.OK").c_str(), ImVec2(90, 0))) {
            if (m_selectedObject) {
                m_selectedObject->setUserDefinedOptionLabel(
                    m_pendingLabelDialogPropertyId, m_pendingLabelDialogBitPosition,
                    std::string(m_labelDialogInputBuffer));
                m_selectedObject->setPropertyValue(
                    m_pendingLabelDialogPropertyId, m_pendingLabelDialogPendingValue);
                m_needsRefresh = true;
                if (onPropertyChanged) onPropertyChanged();
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button(this->getTranslationsManager()->_t("DIALOG.CANCEL").c_str(), ImVec2(90, 0))) {
            // Nothing to revert: SelectEditor never committed the checked
            // bit into the property's persisted value.
            ImGui::CloseCurrentPopup();
        }

        ADS::IDE::endModal();
    }
}


// ---------------------------------------------------------------------------
// refreshCategoryCache
// ---------------------------------------------------------------------------

/**
 * @brief Refresh the category cache from the selected object
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */
void InspectorPanel::refreshCategoryCache()
{
    m_categorizedProperties.clear();
    m_hasIdDescriptor = false;
    if (!m_selectedObject) return;

    for (const auto& descriptor : m_selectedObject->getPropertyDescriptors()) {
        // "id" is pinned above the category list, not rendered as part of
        // a collapsible category — see renderIdRow().
        if (descriptor.getId() == "id") {
            m_idDescriptor = descriptor;
            m_hasIdDescriptor = true;
            continue;
        }

        std::string cat = descriptor.getCategory();
        if (cat.empty()) cat = "General";

        // First-seen order: append a new bucket the first time this
        // category name appears, matching getPropertyDescriptors()'
        // declaration order instead of sorting alphabetically.
        auto it = std::find_if(m_categorizedProperties.begin(), m_categorizedProperties.end(),
            [&cat](const auto& entry) { return entry.first == cat; });
        if (it == m_categorizedProperties.end()) {
            m_categorizedProperties.emplace_back(cat, std::vector<Inspector::PropertyDescriptor>{});
            it = std::prev(m_categorizedProperties.end());
        }
        it->second.push_back(descriptor);
    }
}

// ---------------------------------------------------------------------------
// renderNoSelection
// ---------------------------------------------------------------------------

/**
 * @brief Render placeholder when no object is selected
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */
void InspectorPanel::renderNoSelection()
{
    ImGui::Spacing();
    float w = ImGui::GetContentRegionAvail().x;
    const std::string msg = this->getTranslationsManager()->_t("INSPECTOR.NO_SELECTION");
    ImGui::SetCursorPosX((w - ImGui::CalcTextSize(msg.c_str()).x) * 0.5f);
    ImGui::TextColored(TEXT2, "%s", msg.c_str());
}

// ---------------------------------------------------------------------------
// computeMinPanelWidth
// ---------------------------------------------------------------------------

/**
 * @brief Compute the narrowest the panel can get without clipping a row
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 *
 * Measures the widest cached property label (see refreshCategoryCache(),
 * same ImGui::CalcTextSize()+16 formula as beginPropertyColumns()) and
 * adds a comfortable minimum value-column width — enough for a usable
 * combo plus a SelectEditor "+" button (PropertyDescriptor::
 * isAllowCreateNew()) — plus the scrollbar and window padding actually
 * in effect for the current font/DPI. Applied via
 * ImGui::SetNextWindowSizeConstraints() in render() so the docked
 * panel can never be dragged narrow enough to hide the "+" button
 * behind the window edge, without hardcoding a pixel guess that would
 * drift with locale (label length) or font/DPI changes.
 *
 * Uses whatever's currently cached, which may lag one frame behind a
 * just-changed selection — harmless, since the constraint only needs
 * to be in the right ballpark, not pixel-exact to this exact frame.
 *
 * @return float Minimum panel width in pixels
 */
float InspectorPanel::computeMinPanelWidth() const
{
    float widestLabel = 0.0f;
    auto measure = [&](const Inspector::PropertyDescriptor& d) {
        float w = ImGui::CalcTextSize(d.getDisplayName().c_str()).x;
        widestLabel = std::max(widestLabel, w);
    };
    if (m_hasIdDescriptor) measure(m_idDescriptor);
    for (const auto& [category, properties] : m_categorizedProperties) {
        (void)category;
        for (const auto& d : properties) measure(d);
    }

    // Same "+16" as beginPropertyColumns()'s own label-width formula, and
    // the same 80px floor its std::clamp() enforces.
    float labelColumnWidth = std::max(widestLabel + 16.0f, 80.0f);

    // A usable combo (not squeezed to SelectEditor's own 40px floor) plus
    // spacing plus a "+" button — see SelectEditor.cpp's comboWidth comment.
    float buttonWidth = ImGui::GetFrameHeight();
    float spacing = ImGui::GetStyle().ItemSpacing.x;
    // Wide enough that a single-select combo stays readable next to its "+"
    // button instead of collapsing to SelectEditor's 40px floor.
    float comfortableComboWidth = 150.0f;
    float valueColumnWidth = comfortableComboWidth + spacing + buttonWidth;

    // The property list scrolls in a BeginChild(), and ImGui::Columns()
    // itself eats a few px for the drag handle between columns.
    float scrollbar = ImGui::GetStyle().ScrollbarSize;
    float windowPadding = ImGui::GetStyle().WindowPadding.x * 2.0f;
    float columnGutter = 8.0f;

    return labelColumnWidth + valueColumnWidth + scrollbar + windowPadding + columnGutter;
}

// ---------------------------------------------------------------------------
// render
// ---------------------------------------------------------------------------

/**
 * @brief Render the inspector panel
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 *
 * Displays a property grid for the selected entity with
 * properties organized by category. Each property uses
 * an appropriate editor control based on its type.
 *
 * @note Returns early if panel is not visible
 */
void InspectorPanel::render()
{
    if (!m_isVisible) return;

    // Refresh the editor-owned popup colours every frame so they follow a
    // live theme switch (the design tokens are re-assigned by DarkTheme /
    // LightTheme::apply()).
    Inspector::Editors::IPropertyEditor::setDialogAccent(Colors::C_DIALOG_TITLE);
    Inspector::Editors::IPropertyEditor::setDialogTextColor(Colors::C_CAPTION_TEXT);

    // Keeps the docked panel from being dragged narrower than its widest
    // row needs — see computeMinPanelWidth(). Must be set before Begin().
    ImGui::SetNextWindowSizeConstraints(ImVec2(computeMinPanelWidth(), 0.0f), ImVec2(FLT_MAX, FLT_MAX));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, BG1);
    beginWindow();
    ImGui::PopStyleColor();

    if (!m_selectedObject) {
        renderNoSelection();
        renderLabelDialog();
        endWindow();
        return;
    }

    if (m_needsRefresh) {
        refreshCategoryCache();
        m_needsRefresh = false;
    }

    renderObjectHeader();
    ImGui::Separator();
    ImGui::Spacing();

    // Scrollable property area
    ImGui::BeginChild("##inspector_scroll", ImVec2(0, 0), false);

    if (m_hasIdDescriptor) {
        renderProperty(m_idDescriptor);
        ImGui::Spacing();
    }

    for (const auto& [category, properties] : m_categorizedProperties)
        renderCategory(category, properties);

    ImGui::EndChild();

    renderLabelDialog();

    endWindow();
}

// ---------------------------------------------------------------------------
// Selection management
// ---------------------------------------------------------------------------

/**
 * @brief Set the currently selected object
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 *
 * @param object The inspectable object to display (can be nullptr)
 */
void InspectorPanel::setSelectedObject(Inspector::IInspectable* object)
{
    if (m_selectedObject != object) {
        m_selectedObject = object;
        m_needsRefresh = true;
        // Drop the previous entity's image textures — a different entity is
        // very unlikely to reference the same files.
        m_thumbnailCache.clear();
        refreshCategoryCache();
    }
}

/**
 * @brief Get the currently selected object
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * @return Inspector::IInspectable* Pointer to selected object, or nullptr
 */
Inspector::IInspectable* InspectorPanel::getSelectedObject() const
{
    return m_selectedObject;
}

/**
 * @brief Clear the current selection
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */
void InspectorPanel::clearSelection()
{
    setSelectedObject(nullptr);
}

/**
 * @brief Force a refresh of the property display
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 *
 * Call this after the selected object's properties have changed
 * externally (e.g., via undo/redo).
 */
void InspectorPanel::refresh()
{
    m_needsRefresh = true;
}

// ---------------------------------------------------------------------------
// importImageAsset
// ---------------------------------------------------------------------------

namespace {

    /**
     * @brief Byte-for-byte compare two files.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Used to make re-picking the same image idempotent: a name clash whose
     * content already matches is reused instead of spawning a `_N` copy.
     * Any I/O error is reported as "not equal" so the caller copies.
     *
     * @param a First file
     * @param b Second file
     * @return bool true when both files exist, are the same size and hold the
     *              same bytes
     */
    bool sameFileContent(const std::filesystem::path& a, const std::filesystem::path& b)
    {
        std::error_code ec;
        const auto sizeA = std::filesystem::file_size(a, ec);
        if (ec) {
            return false;
        }
        const auto sizeB = std::filesystem::file_size(b, ec);
        if (ec || sizeA != sizeB) {
            return false;
        }

        std::ifstream fa(a, std::ios::binary);
        std::ifstream fb(b, std::ios::binary);
        if (!fa || !fb) {
            return false;
        }

        constexpr std::size_t kChunk = 64 * 1024;
        std::vector<char> bufA(kChunk);
        std::vector<char> bufB(kChunk);
        while (fa && fb) {
            fa.read(bufA.data(), static_cast<std::streamsize>(kChunk));
            fb.read(bufB.data(), static_cast<std::streamsize>(kChunk));
            const std::streamsize got = fa.gcount();
            if (got != fb.gcount()) {
                return false;
            }
            if (got > 0 &&
                std::memcmp(bufA.data(), bufB.data(), static_cast<std::size_t>(got)) != 0) {
                return false;
            }
        }
        return fa.eof() && fb.eof();
    }

} // namespace

/**
 * @brief Copy a picked image into the project's asset folder.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Sep 2026
 *
 * When the Browse… dialog for an image field returns a file from
 * anywhere on disk, the studio keeps the project self-contained by
 * copying it into `<project dir>/assets/images/` and storing that
 * copy's path on the property instead of the original location. The
 * project directory is the parent of @c Core::Project::getFilePath();
 * it is resolved from the selected object via
 * @c Entities::BaseEntity::getProject().
 *
 * A source already inside that folder is referenced in place. On a
 * name clash the byte-identical file is reused, otherwise the copy is
 * given a `_N` suffix. If the project has never been saved, or the
 * copy fails, the original path is returned unchanged and a warning
 * is logged.
 *
 * @param sourcePath Absolute path the file dialog returned (UTF-8)
 * @return std::string Path to store on the property
 */
std::string InspectorPanel::importImageAsset(const std::string& sourcePath)
{
    namespace fs = std::filesystem;

    auto* entity = dynamic_cast<Entities::BaseEntity*>(m_selectedObject);
    Core::Project* project = (entity != nullptr) ? entity->getProject() : nullptr;
    if (project == nullptr || !project->isSaved()) {
        spdlog::warn("InspectorPanel: project not saved — image '{}' referenced in place",
                     sourcePath);
        return sourcePath;
    }

    std::error_code ec;
    const fs::path src     = fs::path(sourcePath);
    const fs::path destDir = project->getFilePath().parent_path() / "assets" / "images";

    // Source already inside the project's image folder → reference as-is.
    // (each weakly_canonical overload resets ec on success.)
    const fs::path srcParent  = fs::weakly_canonical(src.parent_path(), ec);
    const bool     srcOk      = !ec;
    const fs::path destCanon  = fs::weakly_canonical(destDir, ec);
    if (srcOk && !ec && srcParent == destCanon) {
        return Core::PathService::toUtf8(src);
    }

    fs::create_directories(destDir, ec);
    if (ec) {
        spdlog::error("InspectorPanel: cannot create '{}' ({}) — image '{}' referenced in place",
                      Core::PathService::toUtf8(destDir), ec.message(), sourcePath);
        return sourcePath;
    }

    fs::path dest = destDir / src.filename();
    if (fs::exists(dest, ec) && !sameFileContent(src, dest)) {
        const std::string stem = Core::PathService::toUtf8(src.stem());
        const std::string ext  = Core::PathService::toUtf8(src.extension());
        for (int n = 1; ; ++n) {
            const fs::path candidate = destDir / Core::PathService::pathFromUtf8(
                stem + "_" + std::to_string(n) + ext);
            if (!fs::exists(candidate, ec)) {
                dest = candidate;
                break;
            }
            if (sameFileContent(src, candidate)) {
                return Core::PathService::toUtf8(candidate);
            }
        }
    }

    fs::copy_file(src, dest, fs::copy_options::overwrite_existing, ec);
    if (ec) {
        spdlog::error("InspectorPanel: failed to copy '{}' -> '{}' ({}) — referenced in place",
                      sourcePath, Core::PathService::toUtf8(dest), ec.message());
        return sourcePath;
    }

    spdlog::info("InspectorPanel: imported image '{}' -> '{}'", sourcePath, Core::PathService::toUtf8(dest));
    return Core::PathService::toUtf8(dest);
}

// ---------------------------------------------------------------------------
// processPendingFileDialog
// ---------------------------------------------------------------------------

/**
 * @brief Perform any pending native file dialog, if one was requested
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * Called by IDERenderer::processPendingDialogs(), which App::run()
 * invokes strictly after render()/SDL_RenderPresent() — never call
 * this mid-frame. Must be called once per frame: it both polls any
 * dialog already running on a background thread (see
 * UI::AsyncFileDialog) and starts a new one if a "Browse…" button
 * was clicked. The actual NFD call never runs on the main thread, so
 * this method itself never blocks.
 */
void InspectorPanel::processPendingFileDialog()
{
    // Service a dialog already in flight first.
    std::optional<std::string> result;
    if (m_fileDialog.poll(result)) {
        // Selection may have changed while the dialog was open (e.g. the
        // user picked a different entity before it was serviced).
        if (result && m_selectedObject) {
            // Reject a file whose bytes are not a supported image, even when it
            // carries a valid extension (a PDF renamed .png, say).
            if (m_activeFileDialogIsImage &&
                !ADS::Constants::ImageFormats::isSupportedImageFile(*result)) {
                spdlog::warn("InspectorPanel: rejected '{}' — not a PNG/JPEG/WebP image",
                             *result);
            } else {
                // Images are pulled into the project's assets folder so the
                // project stays self-contained; other file fields keep the
                // path the user picked.
                const std::string stored = m_activeFileDialogIsImage
                    ? importImageAsset(*result)
                    : *result;
                m_selectedObject->setPropertyValue(m_activeFileDialogPropertyId, stored);
                m_needsRefresh = true;
                if (onPropertyChanged) onPropertyChanged();
            }
        }
    }

    if (!m_pendingFileDialog || m_fileDialog.isRunning()) return;
    m_pendingFileDialog = false;

    if (!m_selectedObject) return;

    std::string spec;
    for (size_t i = 0; i < m_pendingFileDialogExtensions.size(); ++i) {
        if (i > 0) spec += ",";
        spec += m_pendingFileDialogExtensions[i];
    }
    nfdwindowhandle_t parentWindow = ADS::UI::getNfdParentWindowHandle(
        Core::App::getMainWindow() ? Core::App::getMainWindow()->getWindow() : nullptr);

    m_activeFileDialogPropertyId = m_pendingFileDialogPropertyId;

    // Remember whether this dialog targets an image field, so the result can be
    // content-checked before it is written (see the poll handler above).
    const auto imageExts = ADS::Constants::ImageFormats::supportedExtensions();
    m_activeFileDialogIsImage = std::any_of(
        m_pendingFileDialogExtensions.begin(), m_pendingFileDialogExtensions.end(),
        [&](const std::string& ext) {
            return std::find(imageExts.begin(), imageExts.end(), ext) != imageExts.end();
        });

    // Runs on a background thread (see UI::AsyncFileDialog) so the main loop
    // keeps rendering — and the app window keeps repainting while the user
    // drags the dialog — instead of stalling on NFD's blocking call.
    m_fileDialog.start([spec, parentWindow]() -> std::optional<std::string> {
        nfdfilteritem_t filter{"File", spec.c_str()};
        NFD::Guard guard;
        NFD::UniquePath outPath;
        nfdresult_t dialogResult = NFD::OpenDialog(outPath, &filter, 1, nullptr, parentWindow);

        if (dialogResult == NFD_OKAY) return std::string(outPath.get());
        if (dialogResult == NFD_ERROR) spdlog::error("InspectorPanel: NFD error — {}", NFD::GetError());
        return std::nullopt;
    });
}

} // namespace ADS::IDE::Panels
