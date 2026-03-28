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

#pragma once

#include "BasePanel.h"
#include "Core/Project.h"
#include <functional>
#include <string>
#include <vector>

namespace ADS::IDE::Panels {

    /**
     * @brief Type of a node in the project tree.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     */
    enum class NodeType {
        Scene, NPC, Item, Puzzle, Variable, Audio,
        // Sub-node types (children of a parent entity)
        SceneDescription, SceneOptions,
        NPCDialogs, NPCStats,
        PuzzleClues, PuzzleSolution
    };

    /**
     * @brief Visual badge rendered as a coloured pill next to a node label.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     */
    enum class NodeBadge {
        None, Start, End, Win, Conditional,
        Locked, Key, Usable, Error, Warning, Mission, Secret
    };

    /**
     * @brief Data for a single row in the project tree.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Holds all display data for one tree row: identifier, visible label,
     * entity type, optional badges, an optional small type-tag, and the
     * child hierarchy.  The struct does not own project model data — it is
     * rebuilt from the model by @c ProjectTreePanel::rebuildFromProject()
     * whenever the project changes.
     */
    struct TreeNode {
        std::string id;
        std::string label;
        NodeType    type;
        NodeBadge   badge        = NodeBadge::None;
        NodeBadge   badge2       = NodeBadge::None;
        std::string typeTag;
        std::string typeTagValue;
        bool        hasChildren  = false;
        bool        isExpanded   = false;
        bool        hasError     = false;
        bool        hasWarning   = false;
        int         depth        = 0;
        std::vector<TreeNode> children;
    };

    /**
     * @brief Project tree panel — left-side hierarchy of all game entities.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Renders the full entity hierarchy (scenes, NPCs, items, puzzles,
     * variables, audio) as a custom tree with:
     * - Real-time search / filter
     * - Stats strip (coloured dot + count per category)
     * - Collapsible sections with colour-coded headers
     * - Custom indent guide lines drawn via ImDrawList
     * - Coloured badge pills per node
     * - Hover action buttons (edit / duplicate / delete)
     * - Footer with "+ Nueva escena" and "Añadir…" popup
     *
     * The panel does not mutate the project model directly.  All model
     * changes are triggered through the public callbacks, keeping the
     * panel as a pure view with callback-based communication.
     */
    class ProjectTreePanel : public BasePanel {
    public:
        /**
         * @brief Construct a new ProjectTreePanel.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         */
        ProjectTreePanel();

        /**
         * @brief Default destructor.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         */
        ~ProjectTreePanel() override = default;

        /**
         * @brief Render the panel contents.  Call once per frame.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Draws the search bar, stats strip, collapsible sections, and
         * footer inside the current ImGui window context.
         */
        void render() override;

        /**
         * @brief Bind the active project and rebuild the internal node list.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param project Non-owning pointer to the project (nullptr clears the tree).
         */
        void setProject(Core::Project* project);

        /**
         * @brief Rebuild the internal tree nodes from the current project.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Must be called after any modification to the project model so that
         * the displayed tree stays in sync.  Safe to call with a null project.
         */
        void rebuildFromProject();

        /**
         * @brief Return the id of the currently selected node (empty if none).
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return const std::string& Selected node id.
         */
        const std::string& getSelectedNodeId() const { return m_selectedId; }

        // ------------------------------------------------------------------
        // Callbacks — wire to the rest of the IDE after construction
        // ------------------------------------------------------------------

        /** Called when the user single-clicks a node. */
        std::function<void(const std::string& nodeId, NodeType)> onNodeSelected;

        /** Called when the user double-clicks a node. */
        std::function<void(const std::string& nodeId, NodeType)> onNodeDoubleClicked;

        /** Called when the user requests a new node via the footer or context menu. */
        std::function<void(NodeType)> onAddNode;

        /** Called when the user requests deletion of a node. */
        std::function<void(const std::string& nodeId)> onDeleteNode;

        /** Called when the user requests duplication of a node. */
        std::function<void(const std::string& nodeId)> onDuplicateNode;

    private:
        // ------------------------------------------------------------------
        // Render helpers
        // ------------------------------------------------------------------

        /**
         * @brief Render the search / filter input box.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         */
        void renderSearchBar();

        /**
         * @brief Render the compact statistics strip below the search bar.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Shows a coloured dot + count for each entity category.
         */
        void renderStatsStrip();

        /**
         * @brief Render a single collapsible section.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param label    Section header text (shown in uppercase).
         * @param color    Accent colour for the header icon and text.
         * @param icon     FontAwesome icon character for the section.
         * @param nodes    Node list to render when the section is open.
         * @param expanded In/out flag tracking the collapsed state.
         */
        void renderSection(const char* label, ImVec4 color, const char* icon,
                           std::vector<TreeNode>& nodes, bool& expanded);

        /**
         * @brief Render a single tree node row.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param node   Node data to display.
         * @param isLast Whether this node is the last child of its parent.
         */
        void renderNode(TreeNode& node, bool isLast = false);

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
        void drawIndentLines(int depth, bool isLast, ImVec2 nodePos);

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
        void drawBadgeAt(NodeBadge badge, ImVec2 pos);

        /**
         * @brief Render the right-click context menu for a node.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param node The node that was right-clicked.
         */
        void renderContextMenu(TreeNode& node);

        /**
         * @brief Render the footer bar with action buttons.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Shows "+ Nueva escena" and an "Añadir…" popup button.
         */
        void renderFooter();

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
        bool matchesSearch(const TreeNode& node) const;

        // ------------------------------------------------------------------
        // State
        // ------------------------------------------------------------------

        std::string m_selectedId;
        char        m_searchBuf[256] = {};

        bool m_secScenes   = true;
        bool m_secNPCs     = true;
        bool m_secItems    = true;
        bool m_secPuzzles  = true;
        bool m_secVars     = true;
        bool m_secAudio    = true;

        // ------------------------------------------------------------------
        // Node lists — populated by rebuildFromProject()
        // ------------------------------------------------------------------

        Core::Project*         m_project   = nullptr;
        std::vector<TreeNode>  m_scenes;
        std::vector<TreeNode>  m_npcs;
        std::vector<TreeNode>  m_items;
        std::vector<TreeNode>  m_puzzles;
        std::vector<TreeNode>  m_variables;
        std::vector<TreeNode>  m_audio;
    };

} // namespace ADS::IDE::Panels