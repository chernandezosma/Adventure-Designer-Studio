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


#ifndef ADS_INSPECTOR_PANEL_H
#define ADS_INSPECTOR_PANEL_H

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>
#include "BasePanel.h"
#include "IDE/TextureCache.h"
#include "Inspector/IInspectable.h"
#include "Inspector/PropertyDescriptor.h"
#include "Inspector/PropertyEditorRegistry.h"
#include "UI/AsyncFileDialog.h"

namespace ADS::Entities {
    class State;
}

namespace ADS::IDE::Panels {
    /**
     * @brief Inspector panel for displaying and editing entity properties
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Shows detailed information about the currently selected entity
     * including its type, and all editable properties organized by
     * category. Uses a Visual Basic-like property grid interface.
     */
    class InspectorPanel : public BasePanel {
    private:
        /// Currently selected inspectable object
        Inspector::IInspectable* m_selectedObject;

        /// Registry of property editors
        Inspector::PropertyEditorRegistry m_editorRegistry;

        /// Cached property descriptors grouped by category, in first-seen
        /// (declaration) order — not alphabetical. The read-only "id"
        /// property is excluded here; it's rendered separately, pinned
        /// above the category list. See refreshCategoryCache().
        std::vector<std::pair<std::string, std::vector<Inspector::PropertyDescriptor>>> m_categorizedProperties;

        /// The "id" property descriptor, rendered pinned above the
        /// category list (empty id string if the selected object has none).
        Inspector::PropertyDescriptor m_idDescriptor{"", "", Inspector::PropertyType::Unknown};
        bool m_hasIdDescriptor = false;

        /// Flag indicating if category cache needs refresh
        bool m_needsRefresh;

        /// Set when a StringEditor's "Browse…" button was clicked this
        /// frame (PropertyConstraints::filePath()). Consumed by
        /// processPendingFileDialog(), called after SDL_RenderPresent —
        /// see CLAUDE.md's deferred dialog pattern; NFD must never be
        /// opened inline during rendering.
        bool m_pendingFileDialog = false;
        std::string m_pendingFileDialogPropertyId;
        std::vector<std::string> m_pendingFileDialogExtensions;

        /// Background runner for the Browse… dialog's blocking NFD call, so
        /// the main render loop keeps presenting frames (and the app window
        /// keeps repainting while the dialog is dragged) instead of stalling
        /// for the dialog's entire lifetime. @see UI::AsyncFileDialog
        UI::AsyncFileDialog m_fileDialog;

        /// Property id captured when m_fileDialog.start() is called, since
        /// m_pendingFileDialogPropertyId may be overwritten by a later,
        /// unrelated Browse… click before this dialog's result arrives.
        std::string m_activeFileDialogPropertyId;

        /// True when the in-flight file dialog targets an image field, so its
        /// result is content-checked (Constants::ImageFormats) before it is
        /// written — a mis-named non-image (a PDF as .png) is rejected.
        bool m_activeFileDialogIsImage = false;

        /// Set when a SelectEditor checkbox for a "(User Defined *)" option
        /// was just checked ON (PropertyDescriptor::isUserDefinedOption).
        /// Consumed by renderLabelDialog(), called every frame from
        /// render(). Unlike the NFD file dialog above, this is a pure
        /// ImGui modal (no OS interaction) and may be opened synchronously
        /// mid-frame — see CLAUDE.md's deferred dialog pattern, which only
        /// applies to native/NFD dialogs.
        bool m_pendingLabelDialog = false;
        std::string m_pendingLabelDialogPropertyId;
        std::string m_pendingLabelDialogCategory;
        std::string m_pendingLabelDialogDisplayName;
        uint8_t m_pendingLabelDialogBitPosition = 0;
        Inspector::PropertyValue m_pendingLabelDialogPendingValue;

        /// Persistent backing buffer for the label dialog's ImGui::InputText
        /// — same convention as ProjectTreePanel's m_searchBuf.
        char m_labelDialogInputBuffer[128] = {};

        /// GPU textures for the thumbnails shown under image / avatar fields,
        /// keyed by file path. Cleared whenever the selection changes so a
        /// switch between entities does not leak textures. @see renderProperty()
        ADS::IDE::TextureCache m_thumbnailCache;

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
        void renderImageThumbnail(const Inspector::PropertyDescriptor& descriptor);

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
        std::string importImageAsset(const std::string& sourcePath);

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
        void renderLabelDialog();

        /**
         * @brief Render the object header (type and name)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         */
        void renderObjectHeader();

        /**
         * @brief Render properties for a category
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param category Category name
         * @param properties Properties in this category
         */
        void renderCategory(
            const std::string& category,
            const std::vector<Inspector::PropertyDescriptor>& properties
        );

        /**
         * @brief Render a single property using the appropriate editor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param descriptor Property metadata
         */
        void renderProperty(const Inspector::PropertyDescriptor& descriptor);

        /**
         * @brief Refresh the category cache from the selected object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         */
        void refreshCategoryCache();

        /**
         * @brief Render placeholder when no object is selected
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         */
        void renderNoSelection();

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
        float computeMinPanelWidth() const;

    public:
        /**
         * @brief Construct a new InspectorPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the inspector panel with name "Inspector".
         */
        InspectorPanel();

        /**
         * @brief Destroy the InspectorPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * The thumbnail texture cache (m_thumbnailCache) frees its own
         * textures via its destructor; nothing else needs cleanup here.
         */
        ~InspectorPanel() override = default;

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
        void render() override;

        /**
         * @brief Set the currently selected object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param object The inspectable object to display (can be nullptr)
         */
        void setSelectedObject(Inspector::IInspectable* object);

        /**
         * @brief Get the currently selected object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return Inspector::IInspectable* Pointer to selected object, or nullptr
         */
        Inspector::IInspectable* getSelectedObject() const;

        /**
         * @brief Clear the current selection
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         */
        void clearSelection();

        /**
         * @brief Force a refresh of the property display
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Call this after the selected object's properties have changed
         * externally (e.g., via undo/redo).
         */
        void refresh();

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
        void processPendingFileDialog();

        /**
         * @brief Whether the Browse… file dialog is currently running
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * NFD's blocking call has no cancel/close API, and the dialog window
         * belongs to a separate portal process, so it can't be force-closed
         * from here. Callers should use this to avoid quitting the app while
         * a dialog is in flight, which would leave it orphaned on screen.
         *
         * @return true if a background dialog thread is currently running
         */
        bool isFileDialogInProgress() const { return m_fileDialog.isRunning(); }

        /**
         * @brief Callback invoked whenever the user edits a property value.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Set by IDERenderer to refresh the project tree and mark unsaved changes.
         * Signature: void()
         */
        std::function<void()> onPropertyChanged;

        /**
         * @brief Callback that creates a new bare State, for the SelectEditor
         *        "+" button (PropertyDescriptor::isAllowCreateNew()).
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Set by IDERenderer to the exact same creation used by the tree's
         * "Add > State" — no separate Name/Next dialog, and the new state
         * is left fully unlinked. Every field with the "+" button (Scene's
         * "state", a State's own "next", a StateChain's own "head")
         * references a State directly, and keeps its own current selection
         * untouched — the user picks the new option from the dropdown
         * themselves afterward. Signature: Entities::State*()
         */
        std::function<Entities::State*()> onCreateState;
    };
}

#endif //ADS_INSPECTOR_PANEL_H
