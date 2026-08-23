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
#include "Entities/State.h"
#include "Entities/StateChain.h"
#include "IconsFontAwesome4.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "app.h"

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
    std::string label;
};

static BadgeStyle getBadgeStyle(NodeBadge badge)
{
    const i18n::i18n* t = Core::App::getTranslationsManager();
    switch (badge) {
        case NodeBadge::Start:       return {C_ITEM,   withAlpha(C_ITEM,   0.15f), t->_t("BADGE.START")};
        case NodeBadge::End:         return {C_ERROR,  withAlpha(C_ERROR,  0.15f), t->_t("BADGE.END")};
        case NodeBadge::Win:         return {C_OK,     withAlpha(C_OK,     0.20f), t->_t("BADGE.WIN")};
        case NodeBadge::Conditional: return {C_PUZZLE, withAlpha(C_PUZZLE, 0.15f), t->_t("BADGE.CONDITIONAL")};
        case NodeBadge::Locked:      return {C_VAR,    withAlpha(C_VAR,    0.15f), t->_t("BADGE.LOCKED")};
        case NodeBadge::Key:         return {C_SCENE,  withAlpha(C_SCENE,  0.15f), t->_t("BADGE.KEY")};
        case NodeBadge::Usable:      return {C_PUZZLE, withAlpha(C_PUZZLE, 0.12f), t->_t("BADGE.USABLE")};
        case NodeBadge::Mission:     return {C_PUZZLE, withAlpha(C_PUZZLE, 0.15f), t->_t("BADGE.MISSION")};
        case NodeBadge::Secret:      return {C_VAR,    withAlpha(C_VAR,    0.15f), t->_t("BADGE.SECRET")};
        case NodeBadge::Error:       return {C_ERROR,  withAlpha(C_ERROR,  0.15f), t->_t("BADGE.ERROR")};
        case NodeBadge::Warning:     return {C_WARN,   withAlpha(C_WARN,   0.15f), t->_t("BADGE.WARNING")};
        default:                     return {{}, {}, ""};
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
        case NodeType::State:            return ICON_FA_FLAG;
        case NodeType::Chain:            return ICON_FA_LINK;
        case NodeType::SceneDescription: return ICON_FA_ALIGN_LEFT;
        case NodeType::SceneOptions:     return ICON_FA_LIST;
        case NodeType::NPCDialogs:       return ICON_FA_COMMENTS;
        case NodeType::NPCStats:         return ICON_FA_BAR_CHART;
        case NodeType::PuzzleClues:      return ICON_FA_EYE;
        case NodeType::PuzzleSolution:   return ICON_FA_CHECK;
        default:                         return ICON_FA_CIRCLE;
    }
}

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

ProjectTreePanel::ProjectTreePanel()
    : BasePanel("Proyecto")
{
    m_titleKey = "TREE.WINDOW_TITLE";
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

/**
 * @brief Bind the active project and rebuild the internal node list.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * @param project Non-owning pointer to the project (nullptr clears the tree).
 */
void ProjectTreePanel::setProject(Core::Project* project)
{
    m_project = project;
    rebuildFromProject();
}

/**
 * @brief Rebuild the internal tree nodes from the current project.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * Must be called after any modification to the project model so that
 * the displayed tree stays in sync.  Safe to call with a null project.
 */
void ProjectTreePanel::rebuildFromProject()
{
    m_scenes.clear();
    m_npcs.clear();
    m_items.clear();
    m_puzzles.clear();
    m_variables.clear();
    m_audio.clear();
    m_states.clear();
    m_chains.clear();

    if (m_project == nullptr)
        return;

    // Scenes
    for (const auto& entity : m_project->getScenes()) {
        const std::string& id = entity->getId();
        TreeNode node;
        node.id          = id;
        node.label       = entity->getDisplayName();
        node.type        = NodeType::Scene;

        // Badge: mark the starting scene
        auto* sceneEntity = dynamic_cast<Entities::Scene*>(entity.get());
        if (sceneEntity && sceneEntity->isStartScene())
            node.badge = NodeBadge::Start;

        // Warn when the author hasn't typed any "Normal" description text
        // yet in any language. getDescriptions().normal is a LexEngine
        // entry id that no current code path ever populates (no authored
        // text -> LexEngine compilation step exists yet), so checking it
        // here would flag every scene, always. getDescriptionTexts().normal
        // is the actual author-facing draft text the Inspector's "Normal"
        // description field writes to, so it's the correct signal.
        if (sceneEntity) {
            if (sceneEntity->getDescriptionTexts().normal.empty()) {
                node.badge2 = NodeBadge::Warning;
            }
        }

        m_scenes.push_back(std::move(node));
    }

    // NPCs / Characters
    for (const auto& entity : m_project->getCharacters()) {
        const std::string& id = entity->getId();
        TreeNode node;
        node.id          = id;
        node.label       = entity->getDisplayName();
        node.type        = NodeType::NPC;

        // Badge: distinguish player character
        auto* charEntity = dynamic_cast<Entities::Character*>(entity.get());
        if (charEntity && charEntity->isPlayer())
            node.badge = NodeBadge::Mission;

        m_npcs.push_back(std::move(node));
    }

    // Items
    for (const auto& entity : m_project->getItems()) {
        TreeNode node;
        node.id    = entity->getId();
        node.label = entity->getDisplayName();
        node.type  = NodeType::Item;

        // Badge: containers get Usable, items with a "Takeable" affordance
        // get Key as a placeholder.
        auto* itemEntity = dynamic_cast<Entities::Item*>(entity.get());
        if (itemEntity) {
            if (itemEntity->isContainer()) {
                node.badge = NodeBadge::Usable;
            } else {
                const auto& affordances = itemEntity->getAffordances();
                bool takeable = std::any_of(affordances.begin(), affordances.end(),
                    [](const Data::Affordance& a) { return a.name == "Takeable"; });
                if (takeable) node.badge = NodeBadge::Key;
            }
        }

        m_items.push_back(std::move(node));
    }

    // States
    for (const auto& entity : m_project->getStates()) {
        TreeNode node;
        node.id    = entity->getId();
        node.label = entity->getDisplayName();
        node.type  = NodeType::State;
        m_states.push_back(std::move(node));
    }

    // State chains
    for (const auto& entity : m_project->getChains()) {
        TreeNode node;
        node.id    = entity->getId();
        node.label = entity->getDisplayName();
        node.type  = NodeType::Chain;
        m_chains.push_back(std::move(node));
    }
}

// ---------------------------------------------------------------------------
// render()
// ---------------------------------------------------------------------------

/**
 * @brief Render the panel contents.  Call once per frame.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * Draws the search bar, stats strip, collapsible sections, and
 * footer inside the current ImGui window context.
 */
void ProjectTreePanel::render()
{
    if (!m_isVisible)
        return;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, BG1);
    bool windowOpen = beginWindow();
    ImGui::PopStyleColor();

    if (!windowOpen) {
        endWindow();
        return;
    }

    // Clear interface until a project is opened: just a centred hint.
    if (m_project == nullptr) {
        const std::string hint = getTranslationsManager()->_t("TREE.NO_PROJECT_HINT");
        const float availW = ImGui::GetContentRegionAvail().x;
        const float textW  = ImGui::CalcTextSize(hint.c_str(), nullptr, false, availW - 16.0f).x;
        ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y * 0.4f));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.0f, (availW - textW) * 0.5f));
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + textW);
        ImGui::TextDisabled("%s", hint.c_str());
        ImGui::PopTextWrapPos();
        endWindow();
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

    renderSection(getTranslationsManager()->_t("TREE.SECTION_SCENES").c_str(),     C_SCENE,  ICON_FA_MAP_MARKER,  NodeType::Scene,    m_scenes,    m_secScenes);
    renderSection(getTranslationsManager()->_t("TREE.SECTION_CHARACTERS").c_str(), C_NPC,    ICON_FA_USER,         NodeType::NPC,      m_npcs,      m_secNPCs);
    renderSection(getTranslationsManager()->_t("TREE.SECTION_ITEMS").c_str(),      C_ITEM,   ICON_FA_CUBE,         NodeType::Item,     m_items,     m_secItems);
    // Puzzles are not a supported entity — section, stat and "Add" entry removed.
    renderSection(getTranslationsManager()->_t("TREE.SECTION_VARIABLES").c_str(),  C_VAR,    ICON_FA_CODE,         NodeType::Variable, m_variables, m_secVars);
    renderSection(getTranslationsManager()->_t("TREE.SECTION_AUDIO").c_str(),      C_AUDIO,  ICON_FA_MUSIC,        NodeType::Audio,    m_audio,     m_secAudio);
    renderSection(getTranslationsManager()->_t("TREE.SECTION_STATES").c_str(),     C_STATE,  ICON_FA_FLAG,         NodeType::State,    m_states,    m_secStates);
    // Chain is an internal linking concept (State's own "next"/"head" chain
    // through it) — not something authored/selected directly from the tree,
    // so it isn't shown here. Still a full entity/CRUD/Inspector under the
    // hood (m_chains stays populated) for whoever needs it programmatically.

    ImGui::EndChild();

    renderFooter();
    endWindow();
}

// ---------------------------------------------------------------------------
// Search bar
// ---------------------------------------------------------------------------

/**
 * @brief Render the search / filter input box.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 */
void ProjectTreePanel::renderSearchBar()
{
    ImGui::PushStyleColor(ImGuiCol_FrameBg,        BG3);
    ImGui::PushStyleColor(ImGuiCol_Border,         BORDER);
    ImGui::PushStyleColor(ImGuiCol_Text,           TEXT0);

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 8.0f);
    ImGui::InputTextWithHint("##search", getTranslationsManager()->_t("TREE.SEARCH_PLACEHOLDER").c_str(), m_searchBuf, sizeof(m_searchBuf));

    ImGui::PopStyleColor(3);
}

// ---------------------------------------------------------------------------
// Stats strip
// ---------------------------------------------------------------------------

/**
 * @brief Render the compact statistics strip below the search bar.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * Shows a coloured dot + count for each entity category.
 */
void ProjectTreePanel::renderStatsStrip()
{
    struct Stat { ImVec4 color; int count; std::string label; };
    Stat stats[] = {
        {C_SCENE,  static_cast<int>(m_scenes.size()),    getTranslationsManager()->_t("TREE.STAT_SCENES")},
        {C_NPC,    static_cast<int>(m_npcs.size()),      getTranslationsManager()->_t("TREE.STAT_CHARACTERS")},
        {C_ITEM,   static_cast<int>(m_items.size()),     getTranslationsManager()->_t("TREE.STAT_ITEMS")},
        {C_VAR,    static_cast<int>(m_variables.size()), getTranslationsManager()->_t("TREE.STAT_VARIABLES")},
        {C_AUDIO,  static_cast<int>(m_audio.size()),     getTranslationsManager()->_t("TREE.STAT_AUDIO")},
        {C_STATE,  static_cast<int>(m_states.size()),    getTranslationsManager()->_t("TREE.STAT_STATES")},
        // Chain is hidden from the tree — see the render() comment above
        // renderSection() for STATES.
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
        ImGui::TextColored(TEXT2, "%s", s.label.c_str());
        ImGui::SameLine(0, 8);
    }
    ImGui::NewLine();
}

// ---------------------------------------------------------------------------
// Collapsible section
// ---------------------------------------------------------------------------

/**
 * @brief Render a single collapsible section.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * @param label    Section header text (shown in uppercase).
 * @param color    Accent colour for the header icon and text.
 * @param icon     FontAwesome icon character for the section.
 * @param addType  Node type the header's right-aligned "+" button
 *                 creates — fires @c onAddNode(addType), the same
 *                 path as "+ New ▸ <type>" in the footer.
 * @param nodes    Node list to render when the section is open.
 * @param expanded In/out flag tracking the collapsed state.
 */
void ProjectTreePanel::renderSection(const char* label, ImVec4 color,
    const char* icon, NodeType addType, std::vector<TreeNode>& nodes, bool& expanded)
{
    // Use a clearly distinct header colour so sections are visible on BG1
    ImGui::PushStyleColor(ImGuiCol_Header,        BG3);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, withAlpha(color, 0.15f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,  withAlpha(color, 0.25f));
    ImGui::PushStyleColor(ImGuiCol_Text,          color);

    // Build a visible label string: "icon  LABEL" — no per-group count badge
    // (the stats strip already carries the totals).
    std::string fullLabel = std::string(icon) + "  " + label + "##sec";

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth
                             | ImGuiTreeNodeFlags_FramePadding;
    if (expanded) flags |= ImGuiTreeNodeFlags_DefaultOpen;

    // Let the trailing "+" button sit on top of the full-width header and
    // still take its own clicks.
    ImGui::SetNextItemAllowOverlap();
    bool open = ImGui::CollapsingHeader(fullLabel.c_str(), flags);
    expanded = open;

    ImGui::PopStyleColor(4);

    // Right-aligned "+" quick-add — same effect as "+ New ▸ <type>" below.
    const float btnW = ImGui::GetFrameHeight();
    ImGui::SameLine(ImGui::GetContentRegionMax().x - btnW - 2.0f);
    ImGui::PushStyleColor(ImGuiCol_Button,        withAlpha(color, 0.18f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, withAlpha(color, 0.35f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  withAlpha(color, 0.50f));
    ImGui::PushStyleColor(ImGuiCol_Text,          color);
    ImGui::PushID(label);
    if (ImGui::Button(ICON_FA_PLUS "##sec_add", ImVec2(btnW, 0.0f))) {
        if (onAddNode) onAddNode(addType);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", getTranslationsManager()->_t("TREE.SECTION_ADD_TOOLTIP").c_str());
    }
    ImGui::PopID();
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

/**
 * @brief Draw tree indent guide lines for a node.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * Uses ImDrawList to paint T/L shaped connector lines matching the
 * depth and sibling position of the node.
 *
 * @param depth   Indentation depth (0 = root level).
 * @param isLast  True if this is the last sibling at its level.
 * @param nodePos Top-left screen position of the node row.
 */
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
    if (s.label.empty()) return 0.0f;
    const float padX = 4.0f;
    return ImGui::CalcTextSize(s.label.c_str()).x + padX * 2.0f;
}

/**
 * @brief Draw a coloured pill badge at an explicit screen position.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * Draws the badge directly via ImDrawList so it can be placed at
 * an arbitrary location (e.g. right-aligned inside the node row)
 * without advancing the ImGui cursor.
 *
 * @param badge Badge type to draw.  NodeBadge::None is a no-op.
 * @param pos   Top-left screen coordinate for the pill.
 */
void ProjectTreePanel::drawBadgeAt(NodeBadge badge, ImVec2 pos)
{
    BadgeStyle s = getBadgeStyle(badge);
    if (s.label.empty())
        return;

    ImVec2 textSz = ImGui::CalcTextSize(s.label.c_str());
    const float padX = 4.0f, padY = 1.0f, rounding = 7.0f;
    ImVec2 bMax = {pos.x + textSz.x + padX * 2.0f,
                   pos.y + textSz.y + padY * 2.0f};

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(pos, bMax,
        ImGui::ColorConvertFloat4ToU32(s.bgColor), rounding);
    dl->AddText({pos.x + padX, pos.y + padY},
        ImGui::ColorConvertFloat4ToU32(s.textColor), s.label.c_str());
}

// ---------------------------------------------------------------------------
// Single node row
// ---------------------------------------------------------------------------

/**
 * @brief Render a single tree node row.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * @param node   Node data to display.
 * @param isLast Whether this node is the last child of its parent.
 */
void ProjectTreePanel::renderNode(TreeNode& node, bool isLast)
{
    if (!matchesSearch(node))
        return;

    const float indentW = 18.0f;
    const float indent  = node.depth * indentW;

    bool selected = (m_selectedId == node.id && m_selectedType == node.type);

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
    rowLabel += "##node_" + std::to_string(static_cast<int>(node.type)) + "_" + node.id;

    ImGuiSelectableFlags selFlags = ImGuiSelectableFlags_AllowDoubleClick
                                  | static_cast<ImGuiSelectableFlags>(ImGuiSelectableFlags_SpanAvailWidth);
    bool clicked = ImGui::Selectable(rowLabel.c_str(), selected, selFlags, ImVec2(0, 0));

    ImGui::PopStyleColor(3);

    // Capture row rect before handling click (rect is set by Selectable)
    ImVec2 rowMin = ImGui::GetItemRectMin();
    ImVec2 rowMax = ImGui::GetItemRectMax();

    if (clicked) {
        m_selectedId = node.id;
        m_selectedType = node.type;
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

/**
 * @brief Render the right-click context menu for a node.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * @param node The node that was right-clicked.
 */
void ProjectTreePanel::renderContextMenu(TreeNode& node)
{
    // node.id alone collides across sections (a Scene and a Character can
    // share the same raw id string — see the analogous m_selectedType fix
    // for tree selection), which made ImGui treat every same-id node's
    // context menu as literally the same popup — hence "N visible items
    // with conflicting ID" once several such nodes were on screen at once.
    std::string popupId = "ctx_" + std::to_string(static_cast<int>(node.type)) + "_" + node.id;
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        ImGui::OpenPopup(popupId.c_str());

    if (ImGui::BeginPopup(popupId.c_str())) {
        if (ImGui::MenuItem((std::string(ICON_FA_PENCIL) + "  " + getTranslationsManager()->_t("TREE.CONTEXT_EDIT")).c_str()))
            if (onNodeSelected) onNodeSelected(node.id, node.type);
        if (ImGui::MenuItem((std::string(ICON_FA_PLUS) + "  " + getTranslationsManager()->_t("TREE.CONTEXT_CREATE")).c_str()))
            if (onAddNode) onAddNode(node.type);
        if (ImGui::MenuItem((std::string(ICON_FA_FILES_O) + "  " + getTranslationsManager()->_t("TREE.CONTEXT_DUPLICATE")).c_str()))
            if (onDuplicateNode) onDuplicateNode(node.id, node.type);
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, C_ERROR);
        if (ImGui::MenuItem((std::string(ICON_FA_TRASH) + "  " + getTranslationsManager()->_t("TREE.CONTEXT_DELETE")).c_str()))
            if (onDeleteNode) onDeleteNode(node.id, node.type);
        ImGui::PopStyleColor();
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------------------
// Footer
// ---------------------------------------------------------------------------

/**
 * @brief Render the footer bar with action buttons.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * Shows "+ Nueva escena" and an "Añadir…" popup button.
 */
void ProjectTreePanel::renderFooter()
{
    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_Button,        BG2);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, BG3);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  withAlpha(C_SCENE, 0.3f));
    ImGui::PushStyleColor(ImGuiCol_Text,          TEXT0);

    // One full-width button; every "new element" choice lives in its popup —
    // there is no separate per-type quick-add. Disabled with no project open.
    float btnHeight = ImGui::GetFrameHeight();
    ImGui::BeginDisabled(m_project == nullptr);
    if (ImGui::Button(getTranslationsManager()->_t("TREE.FOOTER_NEW").c_str(),
                      {ImGui::GetContentRegionAvail().x, btnHeight}))
        ImGui::OpenPopup("add_element_popup");
    ImGui::EndDisabled();

    ImGui::PopStyleColor(4);

    if (ImGui::BeginPopup("add_element_popup")) {
        if (ImGui::MenuItem((std::string(ICON_FA_MAP_MARKER) + "  " + getTranslationsManager()->_t("TREE.ADD_SCENE")).c_str()))
            if (onAddNode) onAddNode(NodeType::Scene);
        if (ImGui::MenuItem((std::string(ICON_FA_USER) + "  " + getTranslationsManager()->_t("TREE.ADD_CHARACTER")).c_str()))
            if (onAddNode) onAddNode(NodeType::NPC);
        if (ImGui::MenuItem((std::string(ICON_FA_CUBE) + "  " + getTranslationsManager()->_t("TREE.ADD_ITEM")).c_str()))
            if (onAddNode) onAddNode(NodeType::Item);
        if (ImGui::MenuItem((std::string(ICON_FA_CODE) + "  " + getTranslationsManager()->_t("TREE.ADD_VARIABLE")).c_str()))
            if (onAddNode) onAddNode(NodeType::Variable);
        if (ImGui::MenuItem((std::string(ICON_FA_MUSIC) + "  " + getTranslationsManager()->_t("TREE.ADD_AUDIO")).c_str()))
            if (onAddNode) onAddNode(NodeType::Audio);
        if (ImGui::MenuItem((std::string(ICON_FA_FLAG) + "  " + getTranslationsManager()->_t("TREE.ADD_STATE")).c_str()))
            if (onAddNode) onAddNode(NodeType::State);
        // No "Add > Chain" — Chain is hidden from the tree, see render().
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------------------
// Search filter
// ---------------------------------------------------------------------------

/**
 * @brief Return true if @p node passes the current search filter.
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version May 2026
 *
 * Matching is case-insensitive substring search against the node label.
 *
 * @param node Node to test.
 * @return bool True when the node should be visible.
 */
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