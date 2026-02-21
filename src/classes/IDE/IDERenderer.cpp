/*
 * Adventure Designer Studio
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is licensed under the GNU General Public License version 3 (GPLv3).
 * See LICENSE.md and COPYING for full license details.
 *
 * This software includes an additional requirement for visible attribution:
 * The original author's name must be displayed in any user interface or
 * promotional material.
 */


#include "IDERenderer.h"
#include "imgui.h"
#include "spdlog/spdlog.h"

namespace ADS::IDE {
    IDERenderer::IDERenderer() : IDEBase(),
        m_layoutManager(nullptr),
        m_menuBarRenderer(nullptr),
        m_toolBarRenderer(nullptr),
        m_statusBarPanel(nullptr),
        m_entitiesPanel(nullptr),
        m_inspectorPanel(nullptr),
        m_workingAreaPanel(nullptr),
        m_project(nullptr)
    {
        initializePanels();
    }

    IDERenderer::~IDERenderer()
    {
        delete m_statusBarPanel;
        delete m_entitiesPanel;
        delete m_inspectorPanel;
        delete m_workingAreaPanel;
        delete m_toolBarRenderer;
        delete m_menuBarRenderer;
        delete m_layoutManager;
        delete m_project;
    }

    void IDERenderer::initializePanels()
    {
        // Create layout manager and renderers
        m_layoutManager = new LayoutManager();
        m_menuBarRenderer = new MenuBarRenderer(m_layoutManager);
        m_toolBarRenderer = new ToolBarRenderer(m_layoutManager);

        // Create all panels
        m_statusBarPanel = new Panels::StatusBarPanel();
        m_entitiesPanel = new Panels::EntitiesPanel();
        m_inspectorPanel = new Panels::InspectorPanel();
        m_workingAreaPanel = new Panels::WorkingAreaPanel();

        // Create project with demo entities
        m_project = new Core::Project("Demo Project");
        m_project->addScene("scene_1", "Forest Entrance");
        m_project->addScene("scene_2", "Dark Cave");
        m_project->addCharacter("char_1", "Hero");
        m_project->addCharacter("char_2", "Merchant");
        m_project->addItem("item_1", "Magic Sword");
        m_project->addItem("item_2", "Health Potion");

        // Wire panels: entity click → inspector update
        m_entitiesPanel->setProject(m_project);
        m_entitiesPanel->setSelectionCallback([this](Inspector::IInspectable* entity) {
            m_inspectorPanel->setSelectedObject(entity);
        });

        // Wire navigation: File > New checks project state and can create a new one
        m_menuBarRenderer->setNavigationCallbacks(
            [this]() { return m_project != nullptr; },
            [this]() { this->newProject(); }
        );

        // Wire file I/O: receive paths selected by the native OS dialogs
        m_menuBarRenderer->setFileCallbacks(
            [](const std::string& path) {
                // TODO: implement project loading from path
                spdlog::info("IDERenderer: open project requested — {}", path);
            },
            [](const std::string& path) {
                // TODO: implement project serialisation to path
                spdlog::info("IDERenderer: save project requested — {}", path);
            }
        );
    }

    void IDERenderer::renderMainWindow()
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Calculate the main window size (subtract status bar height)
        float statusBarHeight = m_statusBarPanel->getHeight();
        ImVec2 mainWindowPos = viewport->Pos;
        ImVec2 mainWindowSize = ImVec2(viewport->Size.x, viewport->Size.y - statusBarHeight);

        // Setup the main dockspace window
        ImGui::SetNextWindowPos(mainWindowPos);
        ImGui::SetNextWindowSize(mainWindowSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        windowFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        windowFlags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("MainDockSpace", nullptr, windowFlags);
        ImGui::PopStyleVar(3);

        // Render menu bar
        if (ImGui::BeginMenuBar()) {
            m_menuBarRenderer->render();
            ImGui::EndMenuBar();
        }

        // Render any pending navigation dialogs (e.g. "New project" confirmation)
        // Must be called inside an ImGui window, outside any menu scope
        m_menuBarRenderer->renderDialogs();

        // Render toolbar content inline (before DockSpace so it reserves space)
        m_toolBarRenderer->renderContent();

        // Setup docking layout before creating the DockSpace
        ImGuiID dockSpaceId = ImGui::GetID("MyDockSpace");
        m_layoutManager->setDockSpaceId(dockSpaceId);
        m_layoutManager->setupDockingLayout();

        // Create the dockspace (will use remaining space after menu bar and toolbar)
        ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), dockSpaceFlags);

        ImGui::End();
    }

    /**
     * @brief Create a fresh empty project, discarding the current one
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Replaces the active project with a new empty Core::Project instance.
     * Clears the inspector selection and updates the entities panel data source
     * so the UI reflects the empty state immediately. The old project is deleted.
     *
     * @note Any unsaved data in the previous project is discarded
     * @see NavigationService::fileNewHandler()
     */
    void IDERenderer::newProject()
    {
        // Clear inspector before destroying the entities it might reference
        m_inspectorPanel->clearSelection();

        // Replace the project
        delete m_project;
        m_project = new Core::Project("New Project");

        // Refresh the entities panel with the empty project
        m_entitiesPanel->setProject(m_project);
    }

    void IDERenderer::render()
    {
        // Render main dockspace window (with menu bar and toolbar)
        renderMainWindow();

        // Render status bar at the bottom
        m_statusBarPanel->render();

        // Render all dockable panels
        m_entitiesPanel->render();
        m_inspectorPanel->render();
        m_workingAreaPanel->render();
    }

    Panels::StatusBarPanel *IDERenderer::getStatusBar() const
    {
        return m_statusBarPanel;
    }

    Panels::EntitiesPanel *IDERenderer::getEntitiesPanel() const
    {
        return m_entitiesPanel;
    }

    Panels::InspectorPanel *IDERenderer::getInspectorPanel() const
    {
        return m_inspectorPanel;
    }

    Panels::WorkingAreaPanel *IDERenderer::getWorkingAreaPanel() const
    {
        return m_workingAreaPanel;
    }
}
