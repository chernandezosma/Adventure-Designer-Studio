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

#include "ProjectTreePanel.h"
#include "IDE/DesignTokens.h"
#include "Entities/Scene.h"
#include "Entities/Character.h"
#include "Entities/Item.h"
#include "IconsFontAwesome4.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <algorithm>
#include <cctype>
#include <format>

namespace ADS::IDE::Panels {

using namespace ADS::IDE::Colors;

// ---------------------------------------------------------------------------
// Badge style table
// ---------------------------------------------------------------------------

struct BadgeStyle {
    ImVec4      textColor;
    ImVec4      bgColor;
    const char* label;
};

static BadgeStyle getBadgeStyle(NodeBadge badge)
{
    switch (badge) {
        case NodeBadge::Start:       return {C_ITEM,   withAlpha(C_ITEM,   0.15f), "inicio"};
        case NodeBadge::End:         return {C_ERROR,  withAlpha(C_ERROR,  0.15f), "fin"};
        case NodeBadge::Win:         return {C_OK,     withAlpha(C_OK,     0.20f), "victoria"};
        case NodeBadge::Conditional: return {C_PUZZLE, withAlpha(C_PUZZLE, 0.15f), "condicional"};
        case NodeBadge::Locked:      return {C_VAR,    withAlpha(C_VAR,    0.15f), "bloqueado"};
        case NodeBadge::Key:         return {C_SCENE,  withAlpha(C_SCENE,  0.15f), "llave"};
        case NodeBadge::Usable:      return {C_PUZZLE, withAlpha(C_PUZZLE, 0.12f), "usable"};
        case NodeBadge::Mission:     return {C_PUZZLE, withAlpha(C_PUZZLE, 0.15f), "misión"};
        case NodeBadge::Secret:      return {C_VAR,    withAlpha(C_VAR,    0.15f), "secreto"};
        case NodeBadge::Error:       return {C_ERROR,  withAlpha(C_ERROR,  0.15f), "err"};
        case NodeBadge::Warning:     return {C_WARN,   withAlpha(C_WARN,   0.15f), "warn"};
        default:                     return {{}, {}, nullptr};
    }
}

// ---------------------------------------------------------------------------
// Icon helpers per NodeType
// ---------------------------------------------------------------------------

static const char* iconForType(NodeType type)
{
    switch (type) {
        case NodeType::Scene:            return ICON_FA_MAP_MARKER;
        case NodeType::NPC:              return ICON_FA_USER;
        case NodeType::Item:             return ICON_FA_CUBE;
        case NodeType::Puzzle:           return ICON_FA_PUZZLE_PIECE;
        case NodeType::Variable:         return ICON_FA_CODE;
        case NodeType::Audio:            return ICON_FA_MUSIC;
        case NodeType::SceneDescription: return ICON_FA_ALIGN_LEFT;
        case NodeType::SceneOptions:     return ICON_FA_LIST;
        case NodeType::NPCDialogs:       return ICON_FA_COMMENTS;
        case NodeType::NPCStats:         return ICON_FA_BAR_CHART;
        case NodeType::PuzzleClues:      return ICON_FA_EYE;
        case NodeType::PuzzleSolution:   return ICON_FA_CHECK;
        default:                         return ICON_FA_CIRCLE;
    }
}

static ImVec4 colorForType(NodeType type)
{
    switch (type) {
        case NodeType::Scene:            return C_SCENE;
        case NodeType::NPC:              return C_NPC;
        case NodeType::Item:             return C_ITEM;
        case NodeType::Puzzle:           return C_PUZZLE;
        case NodeType::Variable:         return C_VAR;
        case NodeType::Audio:            return C_AUDIO;
        default:                         return TEXT2;
    }
}

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

ProjectTreePanel::ProjectTreePanel()
    : BasePanel("Proyecto")
{}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void ProjectTreePanel::setProject(Core::Project* project)
{
    m_project = project;
    rebuildFromProject();
}

void ProjectTreePanel::rebuildFromProject()
{
    m_scenes.clear();
    m_npcs.clear();
    m_items.clear();
    m_puzzles.clear();
    m_variables.clear();
    m_audio.clear();

    if (m_project == nullptr)
        return;

    // Scenes
    for (const auto& entity : m_project->getScenes()) {
        const std::string& id = entity->getId();
        TreeNode node;
        node.id          = id;
        node.label       = entity->getDisplayName();
        node.type        = NodeType::Scene;
        node.hasChildren = true;

        // Badge: mark the starting scene
        auto* sceneEntity = dynamic_cast<Entities::Scene*>(entity.get());
        if (sceneEntity && sceneEntity->isStartScene())
            node.badge = NodeBadge::Start;

        // Warn when description is still empty
        if (sceneEntity) {
            const auto& desc = sceneEntity->getDescription();
            if (desc.empty()) {
                node.badge2 = NodeBadge::Warning;
            }
        }

        TreeNode desc;
        desc.id    = id + "_desc";
        desc.label = "descripción";
        desc.type  = NodeType::SceneDescription;
        desc.depth = 1;
        node.children.push_back(desc);

        TreeNode opts;
        opts.id    = id + "_opts";
        opts.label = "opciones";
        opts.type  = NodeType::SceneOptions;
        opts.depth = 1;
        node.children.push_back(opts);

        m_scenes.push_back(std::move(node));
    }

    // NPCs / Characters
    for (const auto& entity : m_project->getCharacters()) {
        const std::string& id = entity->getId();
        TreeNode node;
        node.id          = id;
        node.label       = entity->getDisplayName();
        node.type        = NodeType::NPC;
        node.hasChildren = true;

        // Badge: distinguish player character
        auto* charEntity = dynamic_cast<Entities::Character*>(entity.get());
        if (charEntity && charEntity->isPlayer())
            node.badge = NodeBadge::Mission;

        TreeNode dialogs;
        dialogs.id    = id + "_dialogs";
        dialogs.label = "diálogos";
        dialogs.type  = NodeType::NPCDialogs;
        dialogs.depth = 1;
        node.children.push_back(dialogs);

        m_npcs.push_back(std::move(node));
    }

    // Items
    for (const auto& entity : m_project->getItems()) {
        TreeNode node;
        node.id    = entity->getId();
        node.label = entity->getDisplayName();
        node.type  = NodeType::Item;

        // Badge: usable items get Usable, pickable-only get Key as a placeholder
        auto* itemEntity = dynamic_cast<Entities::Item*>(entity.get());
        if (itemEntity) {
            if (itemEntity->isUsable())
                node.badge = NodeBadge::Usable;
            else if (itemEntity->isPickable())
                node.badge = NodeBadge::Key;
        }

        m_items.push_back(std::move(node));
    }
}

// ---------------------------------------------------------------------------
// render()
// ---------------------------------------------------------------------------

void ProjectTreePanel::render()
{
    if (!m_isVisible)
        return;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, BG1);
    bool windowOpen = ImGui::Begin(getImGuiLabel().c_str());
    ImGui::PopStyleColor();

    if (!windowOpen) {
        ImGui::End();
        return;
    }

    renderSearchBar();
    ImGui::Spacing();
    renderStatsStrip();
    ImGui::Separator();

    // Scrollable tree area — reserve exact space for separator + buttons + margins
    float btnH    = ImGui::GetFrameHeight();
    float footerH = btnH + ImGui::GetStyle().ItemSpacing.y * 2.0f + 4.0f;
    ImGui::BeginChild("##tree_scroll", ImVec2(0.0f, -footerH));

    renderSection("ESCENAS",    C_SCENE,  ICON_FA_MAP_MARKER,  m_scenes,    m_secScenes);
    renderSection("PERSONAJES", C_NPC,    ICON_FA_USER,         m_npcs,      m_secNPCs);
    renderSection("OBJETOS",    C_ITEM,   ICON_FA_CUBE,         m_items,     m_secItems);
    renderSection("PUZZLES",    C_PUZZLE, ICON_FA_PUZZLE_PIECE, m_puzzles,   m_secPuzzles);
    renderSection("VARIABLES",  C_VAR,    ICON_FA_CODE,         m_variables, m_secVars);
    renderSection("AUDIO",      C_AUDIO,  ICON_FA_MUSIC,        m_audio,     m_secAudio);

    ImGui::EndChild();

    renderFooter();
    ImGui::End();
}

// ---------------------------------------------------------------------------
// Search bar
// ---------------------------------------------------------------------------

void ProjectTreePanel::renderSearchBar()
{
    ImGui::PushStyleColor(ImGuiCol_FrameBg,        BG3);
    ImGui::PushStyleColor(ImGuiCol_Border,         BORDER);
    ImGui::PushStyleColor(ImGuiCol_Text,           TEXT0);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 8.0f);
    ImGui::InputTextWithHint("##search", "Buscar...", m_searchBuf, sizeof(m_searchBuf));

    ImGui::PopStyleColor(3);
}

// ---------------------------------------------------------------------------
// Stats strip
// ---------------------------------------------------------------------------

void ProjectTreePanel::renderStatsStrip()
{
    struct Stat { ImVec4 color; int count; const char* label; };
    Stat stats[] = {
        {C_SCENE,  static_cast<int>(m_scenes.size()),    "esc"},
        {C_NPC,    static_cast<int>(m_npcs.size()),      "npc"},
        {C_ITEM,   static_cast<int>(m_items.size()),     "obj"},
        {C_PUZZLE, static_cast<int>(m_puzzles.size()),   "puz"},
        {C_VAR,    static_cast<int>(m_variables.size()), "var"},
        {C_AUDIO,  static_cast<int>(m_audio.size()),     "aud"},
    };

    for (auto& s : stats) {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddCircleFilled(
            {p.x + 4.0f, p.y + 7.0f}, 3.0f,
            ImGui::ColorConvertFloat4ToU32(s.color));
        ImGui::Dummy({10.0f, 14.0f});
        ImGui::SameLine(0, 2);
        ImGui::TextColored(TEXT0, "%d", s.count);
        ImGui::SameLine(0, 2);
        ImGui::TextColored(TEXT2, "%s", s.label);
        ImGui::SameLine(0, 8);
    }
    ImGui::NewLine();
}

// ---------------------------------------------------------------------------
// Collapsible section
// ---------------------------------------------------------------------------

void ProjectTreePanel::renderSection(const char* label, ImVec4 color,
    const char* icon, std::vector<TreeNode>& nodes, bool& expanded)
{
    // Use a clearly distinct header colour so sections are visible on BG1
    ImGui::PushStyleColor(ImGuiCol_Header,        BG3);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, withAlpha(color, 0.15f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,  withAlpha(color, 0.25f));
    ImGui::PushStyleColor(ImGuiCol_Text,          color);

    // Build a visible label string: "icon  LABEL  (N)"
    std::string fullLabel = std::string(icon) + "  " + label;
    if (!nodes.empty())
        fullLabel += "  (" + std::to_string(static_cast<int>(nodes.size())) + ")";
    fullLabel += "##sec";

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth
                             | ImGuiTreeNodeFlags_FramePadding;
    if (expanded) flags |= ImGuiTreeNodeFlags_DefaultOpen;

    bool open = ImGui::CollapsingHeader(fullLabel.c_str(), flags);
    expanded = open;

    ImGui::PopStyleColor(4);

    if (open) {
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
        for (std::size_t i = 0; i < nodes.size(); ++i)
            renderNode(nodes[i], i == nodes.size() - 1);
        ImGui::PopStyleVar();
    }
}

// ---------------------------------------------------------------------------
// Indent guide lines
// ---------------------------------------------------------------------------

void ProjectTreePanel::drawIndentLines(int depth, bool isLast, ImVec2 nodePos)
{
    if (depth <= 0)
        return;

    ImDrawList* dl      = ImGui::GetWindowDrawList();
    ImU32       lineCol = ImGui::ColorConvertFloat4ToU32(withAlpha(BORDER, 0.6f));
    const float indentW = 18.0f;
    const float nodeH   = 22.0f;

    for (int i = 0; i < depth; ++i) {
        float x         = nodePos.x + i * indentW + indentW * 0.5f;
        bool  lastLevel = (i == depth - 1);

        if (!lastLevel) {
            dl->AddLine({x, nodePos.y}, {x, nodePos.y + nodeH}, lineCol, 1.0f);
        } else if (isLast) {
            dl->AddLine({x, nodePos.y},
                        {x, nodePos.y + nodeH * 0.5f}, lineCol, 1.0f);
            dl->AddLine({x, nodePos.y + nodeH * 0.5f},
                        {x + indentW * 0.5f, nodePos.y + nodeH * 0.5f}, lineCol, 1.0f);
        } else {
            dl->AddLine({x, nodePos.y},
                        {x, nodePos.y + nodeH}, lineCol, 1.0f);
            dl->AddLine({x, nodePos.y + nodeH * 0.5f},
                        {x + indentW * 0.5f, nodePos.y + nodeH * 0.5f}, lineCol, 1.0f);
        }
    }
}

// ---------------------------------------------------------------------------
// Badge pill helpers
// ---------------------------------------------------------------------------

static float badgePillWidth(NodeBadge badge)
{
    BadgeStyle s = getBadgeStyle(badge);
    if (!s.label) return 0.0f;
    const float padX = 4.0f;
    return ImGui::CalcTextSize(s.label).x + padX * 2.0f;
}

void ProjectTreePanel::drawBadgeAt(NodeBadge badge, ImVec2 pos)
{
    BadgeStyle s = getBadgeStyle(badge);
    if (!s.label)
        return;

    ImVec2 textSz = ImGui::CalcTextSize(s.label);
    const float padX = 4.0f, padY = 1.0f, rounding = 7.0f;
    ImVec2 bMax = {pos.x + textSz.x + padX * 2.0f,
                   pos.y + textSz.y + padY * 2.0f};

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(pos, bMax,
        ImGui::ColorConvertFloat4ToU32(s.bgColor), rounding);
    dl->AddText({pos.x + padX, pos.y + padY},
        ImGui::ColorConvertFloat4ToU32(s.textColor), s.label);
}

// ---------------------------------------------------------------------------
// Single node row
// ---------------------------------------------------------------------------

void ProjectTreePanel::renderNode(TreeNode& node, bool isLast)
{
    if (!matchesSearch(node))
        return;

    const float indentW = 18.0f;
    const float indent  = node.depth * indentW;

    bool selected = (m_selectedId == node.id);

    // Draw indent guide lines before advancing cursor
    ImVec2 nodePos = ImGui::GetCursorScreenPos();
    drawIndentLines(node.depth, isLast, nodePos);

    // Apply indentation
    if (indent > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + indent);

    // Style the selectable row
    ImGui::PushStyleColor(ImGuiCol_Header,        withAlpha(C_SCENE, 0.20f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, withAlpha(BG2, 0.80f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,  BG3);

    // Build the visible label: [expander] icon  label  [badges]
    std::string rowLabel;
    if (node.hasChildren)
        rowLabel += node.isExpanded ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT;
    rowLabel += "  ";
    rowLabel += iconForType(node.type);
    rowLabel += "  ";
    rowLabel += node.label;
    rowLabel += "##node_" + node.id;

    ImGuiSelectableFlags selFlags = ImGuiSelectableFlags_AllowDoubleClick
                                  | ImGuiSelectableFlags_SpanAvailWidth;
    bool clicked = ImGui::Selectable(rowLabel.c_str(), selected, selFlags, ImVec2(0, 0));

    ImGui::PopStyleColor(3);

    // Capture row rect before handling click (rect is set by Selectable)
    ImVec2 rowMin = ImGui::GetItemRectMin();
    ImVec2 rowMax = ImGui::GetItemRectMax();

    if (clicked) {
        m_selectedId = node.id;
        if (node.hasChildren)
            node.isExpanded = !node.isExpanded;
        if (ImGui::IsMouseDoubleClicked(0)) {
            if (onNodeDoubleClicked) onNodeDoubleClicked(node.id, node.type);
        } else {
            if (onNodeSelected) onNodeSelected(node.id, node.type);
        }
    }

    // Draw badges right-aligned within the row via ImDrawList
    {
        const float padY      = 1.0f;
        const float badgeGap  = 3.0f;
        const float rightPad  = 4.0f;
        float badgePillH = ImGui::GetTextLineHeight() + padY * 2.0f;
        float badgeY     = rowMin.y + (rowMax.y - rowMin.y - badgePillH) * 0.5f;
        float rightX     = rowMax.x - rightPad;

        if (node.badge2 != NodeBadge::None) {
            float w  = badgePillWidth(node.badge2);
            rightX  -= w;
            drawBadgeAt(node.badge2, {rightX, badgeY});
            rightX  -= badgeGap;
        }
        if (node.badge != NodeBadge::None) {
            float w  = badgePillWidth(node.badge);
            rightX  -= w;
            drawBadgeAt(node.badge, {rightX, badgeY});
        }
    }

    renderContextMenu(node);

    // Expanded children
    if (node.hasChildren && node.isExpanded) {
        for (std::size_t i = 0; i < node.children.size(); ++i)
            renderNode(node.children[i], i == node.children.size() - 1);
    }
}

// ---------------------------------------------------------------------------
// Context menu
// ---------------------------------------------------------------------------

void ProjectTreePanel::renderContextMenu(TreeNode& node)
{
    std::string popupId = "ctx_" + node.id;
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        ImGui::OpenPopup(popupId.c_str());

    if (ImGui::BeginPopup(popupId.c_str())) {
        if (ImGui::MenuItem(ICON_FA_PENCIL "  Editar"))
            if (onNodeSelected) onNodeSelected(node.id, node.type);
        if (ImGui::MenuItem(ICON_FA_FILES_O "  Duplicar"))
            if (onDuplicateNode) onDuplicateNode(node.id);
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, C_ERROR);
        if (ImGui::MenuItem(ICON_FA_TRASH "  Eliminar"))
            if (onDeleteNode) onDeleteNode(node.id);
        ImGui::PopStyleColor();
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------------------
// Footer
// ---------------------------------------------------------------------------

void ProjectTreePanel::renderFooter()
{
    ImGui::Separator();
    float halfW = (ImGui::GetContentRegionAvail().x - 4.0f) * 0.5f;

    ImGui::PushStyleColor(ImGuiCol_Button,        BG2);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BG3);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  withAlpha(C_SCENE, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_Text,          TEXT0);

    float btnHeight = ImGui::GetFrameHeight();
    if (ImGui::Button("+ Nueva escena", {halfW, btnHeight}))
        if (onAddNode) onAddNode(NodeType::Scene);

    ImGui::SameLine(0, 4);

    if (ImGui::Button("Añadir...", {halfW, btnHeight}))
        ImGui::OpenPopup("add_element_popup");

    ImGui::PopStyleColor(4);

    if (ImGui::BeginPopup("add_element_popup")) {
        if (ImGui::MenuItem(ICON_FA_MAP_MARKER "  Escena"))
            if (onAddNode) onAddNode(NodeType::Scene);
        if (ImGui::MenuItem(ICON_FA_USER "  Personaje"))
            if (onAddNode) onAddNode(NodeType::NPC);
        if (ImGui::MenuItem(ICON_FA_CUBE "  Objeto"))
            if (onAddNode) onAddNode(NodeType::Item);
        if (ImGui::MenuItem(ICON_FA_PUZZLE_PIECE "  Puzzle"))
            if (onAddNode) onAddNode(NodeType::Puzzle);
        if (ImGui::MenuItem(ICON_FA_CODE "  Variable"))
            if (onAddNode) onAddNode(NodeType::Variable);
        if (ImGui::MenuItem(ICON_FA_MUSIC "  Audio"))
            if (onAddNode) onAddNode(NodeType::Audio);
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------------------
// Search filter
// ---------------------------------------------------------------------------

bool ProjectTreePanel::matchesSearch(const TreeNode& node) const
{
    if (m_searchBuf[0] == '\0')
        return true;

    std::string haystack = node.label;
    std::string needle   = m_searchBuf;

    // Case-insensitive comparison
    std::transform(haystack.begin(), haystack.end(), haystack.begin(),
        [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    std::transform(needle.begin(), needle.end(), needle.begin(),
        [](unsigned char c){ return static_cast<char>(std::tolower(c)); });

    return haystack.find(needle) != std::string::npos;
}

} // namespace ADS::IDE::Panels