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
#include "imgui_internal.h"
#include <fmt/chrono.h>

namespace ADS::IDE {
    IDERenderer::IDERenderer() : IDEBase(),
        m_layoutManager(nullptr),
        m_menuBarRenderer(nullptr),
        m_toolBarRenderer(nullptr),
        m_statusBarPanel(nullptr),
        m_entitiesPanel(nullptr),
        m_propertiesPanel(nullptr),
        m_inspectorPanel(nullptr),
        m_workingAreaPanel(nullptr)
    {
        initializePanels();
    }

    IDERenderer::~IDERenderer()
    {
        delete m_statusBarPanel;
        delete m_entitiesPanel;
        delete m_propertiesPanel;
        delete m_inspectorPanel;
        delete m_workingAreaPanel;
        delete m_toolBarRenderer;
        delete m_menuBarRenderer;
        delete m_layoutManager;
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
        m_propertiesPanel = new Panels::PropertiesPanel();
        m_inspectorPanel = new Panels::InspectorPanel();
        m_workingAreaPanel = new Panels::WorkingAreaPanel();
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

        // Create the dockspace
        ImGuiID dockSpaceId = ImGui::GetID("MyDockSpace");
        ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), dockSpaceFlags);

        // Setup docking layout
        m_layoutManager->setDockSpaceId(dockSpaceId);
        m_layoutManager->setupDockingLayout();

        // Render menu bar
        if (ImGui::BeginMenuBar()) {
            m_menuBarRenderer->render();
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void IDERenderer::render()
    {
        // Render main dockspace window (with menu bar)
        renderMainWindow();

        // Render toolbar
        m_toolBarRenderer->render();

        // Render status bar at the bottom
        m_statusBarPanel->render();

        // Render all dockable panels
        m_entitiesPanel->render();
        m_propertiesPanel->render();
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

    Panels::PropertiesPanel *IDERenderer::getPropertiesPanel() const
    {
        return m_propertiesPanel;
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
