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


#include "IDERenderer.h"
#include "DesignTokens.h"
#include "IconsFontAwesome4.h"
#include "imgui.h"
#include "spdlog/spdlog.h"

namespace ADS::IDE {
    IDERenderer::IDERenderer() : IDEBase(),
        m_layoutManager(nullptr),
        m_menuBarRenderer(nullptr),
        m_toolBarRenderer(nullptr),
        m_statusBarPanel(nullptr),
        m_projectTreePanel(nullptr),
        m_inspectorPanel(nullptr),
        m_workingAreaPanel(nullptr),
        m_project(nullptr)
    {
        initializePanels();
    }

    IDERenderer::~IDERenderer()
    {
        delete m_statusBarPanel;
        delete m_projectTreePanel;
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
        m_statusBarPanel   = new Panels::StatusBarPanel();
        m_projectTreePanel = new Panels::ProjectTreePanel();
        m_inspectorPanel   = new Panels::InspectorPanel();
        m_workingAreaPanel = new Panels::WorkingAreaPanel();

        // Create project with demo entities
        m_project = new Core::Project("La Cripta del Rey Olvidado");
        m_project->addScene("intro",          "intro");
        m_project->addScene("sala_entrada",   "sala_entrada");
        m_project->addScene("pasillo_oscuro", "pasillo_oscuro");
        m_project->addScene("camara_trampa",  "camara_trampa");
        m_project->addScene("altar_final",    "altar_final");
        m_project->addCharacter("guardian_sombra",  "guardian_sombra");
        m_project->addCharacter("espectro_rey",     "espectro_rey");
        m_project->addCharacter("mercader_sombras", "mercader_sombras");
        m_project->addItem("llave_antigua", "llave_antigua");
        m_project->addItem("antorcha",      "antorcha");
        m_project->addItem("mapa_cripta",   "mapa_cripta");

        // Wire project tree panel to inspector and status bar
        m_projectTreePanel->setProject(m_project);
        m_statusBarPanel->setProjectName(m_project->getName());
        m_statusBarPanel->setCounts(
            static_cast<int>(m_project->getScenes().size()),
            static_cast<int>(m_project->getCharacters().size()),
            0, 0);

        m_projectTreePanel->onNodeSelected = [this](const std::string& id, Panels::NodeType) {
            // Forward selection to inspector — search each vector by entity id
            auto findById = [&id](const auto& vec) -> Inspector::IInspectable* {
                for (const auto& e : vec)
                    if (e->getId() == id) return e.get();
                return nullptr;
            };
            if (auto* e = findById(m_project->getScenes()))     { m_inspectorPanel->setSelectedObject(e); return; }
            if (auto* e = findById(m_project->getCharacters()))  { m_inspectorPanel->setSelectedObject(e); return; }
            if (auto* e = findById(m_project->getItems()))       { m_inspectorPanel->setSelectedObject(e); }
        };

        m_projectTreePanel->onAddNode = [this](Panels::NodeType type) {
            static int counter = 0;
            ++counter;
            switch (type) {
                case Panels::NodeType::Scene:
                    m_project->addScene(
                        "scene_new_" + std::to_string(counter),
                        "Nueva escena " + std::to_string(counter));
                    break;
                case Panels::NodeType::NPC:
                    m_project->addCharacter(
                        "npc_new_" + std::to_string(counter),
                        "Nuevo personaje " + std::to_string(counter));
                    break;
                case Panels::NodeType::Item:
                    m_project->addItem(
                        "item_new_" + std::to_string(counter),
                        "Nuevo objeto " + std::to_string(counter));
                    break;
                default: break;
            }
            m_projectTreePanel->rebuildFromProject();
            m_statusBarPanel->setCounts(
                static_cast<int>(m_project->getScenes().size()),
                static_cast<int>(m_project->getCharacters().size()),
                0, 0);
            m_hasUnsavedChanges = true;
        };

        // When the inspector edits any property, refresh badges and mark dirty
        m_inspectorPanel->onPropertyChanged = [this]() {
            m_projectTreePanel->rebuildFromProject();
            m_hasUnsavedChanges = true;
        };

        // Wire navigation callbacks
        m_menuBarRenderer->setNavigationCallbacks(
            [this]() { return m_project != nullptr; },
            [this]() { this->newProject(); }
        );

        // Wire file I/O: receive paths selected by the native OS dialogs
        m_menuBarRenderer->setFileCallbacks(
            [this](const std::string& path) {
                // TODO: implement project deserialisation from path
                spdlog::info("IDERenderer: open project requested — {}", path);
                m_project->setFilePath(path);
            },
            [this](const std::string& path) {
                // TODO: implement project serialisation to path
                spdlog::info("IDERenderer: save project requested — {}", path);
                m_project->setFilePath(path);
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

        // Project info bar — thin strip between toolbar and dockspace
        {
            using namespace ADS::IDE::Colors;
            float barH = ImGui::GetFrameHeight() + 4.0f;

            ImGui::PushStyleColor(ImGuiCol_ChildBg, BG2);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

            ImGui::BeginChild("##projectinfobar", ImVec2(0, barH), false,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            // Top/bottom border lines
            ImDrawList* dl  = ImGui::GetWindowDrawList();
            ImVec2      wp  = ImGui::GetWindowPos();
            float       ww  = ImGui::GetWindowWidth();
            ImU32       col = ImGui::ColorConvertFloat4ToU32(BORDER);
            dl->AddLine({wp.x, wp.y},        {wp.x + ww, wp.y},        col, 1.0f);
            dl->AddLine({wp.x, wp.y + barH}, {wp.x + ww, wp.y + barH}, col, 1.0f);

            float textH = ImGui::GetTextLineHeight();
            float posY  = (barH - textH) * 0.5f;

            ImGui::SetCursorPos({10.0f, posY});

            // Saved/unsaved dot indicator (ICON_FA_CIRCLE guaranteed in loaded icon font)
            ImVec4 dotColor = m_hasUnsavedChanges ? C_WARN : C_OK;
            ImGui::TextColored(dotColor, ICON_FA_CIRCLE);
            ImGui::SameLine(0, 8);
            ImGui::SetCursorPosY(posY);

            // Project name
            const char* name = (m_project ? m_project->getName().c_str() : "Sin proyecto");
            ImGui::TextColored(TEXT0, "%s", name);

            if (m_hasUnsavedChanges) {
                ImGui::SameLine(0, 8);
                ImGui::SetCursorPosY(posY);
                ImGui::TextColored(TEXT2, "· sin guardar");
            }

            ImGui::EndChild();
            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor();
        }

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
    /**
     * @brief Execute any deferred native file dialogs
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Delegates to MenuBarRenderer::processPendingDialogs(). Called by App::run()
     * after render() (i.e. after SDL_RenderPresent) so the compositor has a clean
     * frame before the blocking NFD call freezes the main thread.
     *
     * @see MenuBarRenderer::processPendingDialogs()
     */
    void IDERenderer::processPendingDialogs()
    {
        m_menuBarRenderer->processPendingDialogs();
    }

    void IDERenderer::notifyWindowResized()
    {
        m_layoutManager->onWindowResized();
    }

    void IDERenderer::newProject()
    {
        m_inspectorPanel->clearSelection();

        delete m_project;
        m_project = new Core::Project("Nuevo proyecto");
        spdlog::info("IDERenderer: new project created — '{}'", m_project->getName());

        m_projectTreePanel->setProject(m_project);
        m_statusBarPanel->setProjectName(m_project->getName());
        m_statusBarPanel->setCounts(0, 0, 0, 0);
    }

    void IDERenderer::render()
    {
        // Render main dockspace window (with menu bar and toolbar)
        renderMainWindow();

        // Render status bar at the bottom
        m_statusBarPanel->render();

        // Render all dockable panels
        m_projectTreePanel->render();
        m_inspectorPanel->render();
        m_workingAreaPanel->render();
    }

    Panels::StatusBarPanel *IDERenderer::getStatusBar() const
    {
        return m_statusBarPanel;
    }

    Panels::ProjectTreePanel *IDERenderer::getProjectTreePanel() const
    {
        return m_projectTreePanel;
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
