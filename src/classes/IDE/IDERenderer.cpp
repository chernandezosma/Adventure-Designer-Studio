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
#include <memory>
#include <optional>
#include <string>

#include "IconsFontAwesome4.h"
#include "app.h"
#include "imgui.h"
#include "spdlog/spdlog.h"
#include "Core/ProjectSerializer.h"
#include "Core/PathService.h"
#include "exceptions/base_exception.h"

namespace ADS::IDE {
    IDERenderer::IDERenderer() : IDEBase(),
        m_layoutManager(nullptr),
        m_menuBarRenderer(nullptr),
        m_toolBarRenderer(nullptr),
        m_statusBarPanel(nullptr),
        m_projectTreePanel(nullptr),
        m_inspectorPanel(nullptr),
        m_workingAreaPanel(nullptr),
        m_translationPanel(nullptr),
        m_newProjectDialog(nullptr),
        m_loadWarningsDialog(nullptr),
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
        delete m_translationPanel;
        delete m_newProjectDialog;
        delete m_loadWarningsDialog;
        delete m_toolBarRenderer;
        delete m_menuBarRenderer;
        delete m_layoutManager;
        delete m_project;
    }

    /**
     * @brief Initialize all panels
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Creates and initializes instances of all panel objects including the
     * layout manager, menu bar renderer, toolbar renderer, and all IDE panels
     * (status bar, entities, properties, inspector, and working area).
     * This method is called automatically during construction.
     *
     * @note This is a private method called by the constructor
     * @see IDERenderer()
     */
    void IDERenderer::initializePanels()
    {
        // Create layout manager and renderers
        m_layoutManager = new LayoutManager();
        m_menuBarRenderer = new MenuBarRenderer(m_layoutManager);
        m_toolBarRenderer = new ToolBarRenderer(m_layoutManager);
        m_toolBarRenderer->setNavigationService(m_menuBarRenderer->getNavigationService());

        // Create all panels
        m_statusBarPanel   = new Panels::StatusBarPanel();
        m_projectTreePanel = new Panels::ProjectTreePanel();
        m_inspectorPanel   = new Panels::InspectorPanel();
        m_workingAreaPanel = new Panels::WorkingAreaPanel();
        m_translationPanel = new Panels::TranslationPanel();
        m_newProjectDialog = new NewProjectDialog();
        m_loadWarningsDialog = new LoadWarningsDialog();

        // Start with no project — the IDE opens on a clear interface and the
        // user creates or opens one via File ▸ New / File ▸ Open.
        m_project = nullptr;
        m_hasUnsavedChanges = false;
        m_projectTreePanel->setProject(nullptr);
        m_translationPanel->setProject(nullptr);
        m_statusBarPanel->setProjectName(getTranslationManager()->_t("PROJECT.NO_PROJECT"));
        m_statusBarPanel->setCounts(0, 0, 0, 0);

        m_projectTreePanel->onNodeSelected = [this](const std::string& id, Panels::NodeType type) {
            if (m_project == nullptr) return;
            // Forward selection to inspector — search only the collection
            // matching the node's type. SceneId/CharacterId/ObjectId are
            // independent per-tag namespaces (not globally unique), so a
            // scene and an item can share the same raw id string — the
            // type tells us which collection to search, avoiding a
            // same-id collision selecting the wrong entity.
            auto findById = [&id](const auto& vec) -> Inspector::IInspectable* {
                for (const auto& e : vec)
                    if (e->getId() == id) return e.get();
                return nullptr;
            };
            Inspector::IInspectable* found = nullptr;
            switch (type) {
                case Panels::NodeType::Scene:
                    found = findById(m_project->getScenes());
                    break;
                case Panels::NodeType::NPC:
                    found = findById(m_project->getCharacters());
                    break;
                case Panels::NodeType::Item:
                    found = findById(m_project->getItems());
                    break;
                case Panels::NodeType::State:
                    found = findById(m_project->getStates());
                    break;
                case Panels::NodeType::Chain:
                    found = findById(m_project->getChains());
                    break;
                default:
                    break;
            }
            if (found) m_inspectorPanel->setSelectedObject(found);
        };

        m_projectTreePanel->onAddNode = [this](Panels::NodeType type) {
            if (nodeTypeIsEntity(type)) {
                createEntity(nodeTypeToEntityKind(type));
            } else if (type == Panels::NodeType::Chain) {
                createNewChain();
            }
            // Variable / Audio have no DataObject yet — nothing to create.
        };

        m_projectTreePanel->onDuplicateNode =
            [this](const std::string& id, Panels::NodeType type) {
                if (nodeTypeIsEntity(type)) {
                    duplicateEntity(nodeTypeToEntityKind(type), id);
                }
            };

        m_projectTreePanel->onDeleteNode =
            [this](const std::string& id, Panels::NodeType type) {
                if (nodeTypeIsEntity(type)) {
                    deleteEntity(nodeTypeToEntityKind(type), id);
                }
            };

        // The Inspector's "+" button next to a State-referencing dropdown
        // (Scene's "state", State's own "next", StateChain's own "head")
        // fires the exact same creation as the tree's "Add > State" — no
        // separate Name/Next dialog. See createNewState().
        m_inspectorPanel->onCreateState = [this]() {
            Entities::State* state = createNewState();
            m_statusBarPanel->setCounts(
                static_cast<int>(m_project->getScenes().size()),
                static_cast<int>(m_project->getCharacters().size()),
                0, 0);
            return state;
        };

        // When the inspector edits any property, refresh badges and mark dirty
        m_inspectorPanel->onPropertyChanged = [this]() {
            m_projectTreePanel->rebuildFromProject();
            m_hasUnsavedChanges = true;
        };

        // Wire navigation callbacks. File > New opens the New Project modal
        // (directly, or after the discard-confirm dialog); the project is only
        // actually replaced when the modal's Create fires newProjectFromSpec().
        m_menuBarRenderer->setNavigationCallbacks(
            [this]() { return m_project != nullptr; },
            [this]() { m_newProjectDialog->open(); }
        );
        m_newProjectDialog->onCreate = [this](const NewProjectSpec& spec) -> std::string {
            return this->newProjectFromSpec(spec); // "" ok, else the failure reason
        };

        // Wire file I/O: receive paths selected by the native OS dialogs
        m_menuBarRenderer->setFileCallbacks(
            [this](const std::string& path) {
                m_lastOpenError.clear();
                try {
                    auto loaded = Core::ProjectSerializer::load(path);
                    m_inspectorPanel->clearSelection();
                    delete m_project;
                    m_project = loaded.project.release();
                    m_project->setFilePath(path);
                    m_projectTreePanel->setProject(m_project);
                    m_projectTreePanel->rebuildFromProject();
                    m_translationPanel->setProject(m_project);
                    m_statusBarPanel->setProjectName(m_project->getName());
                    m_statusBarPanel->setCounts(
                        static_cast<int>(m_project->getScenes().size()),
                        static_cast<int>(m_project->getCharacters().size()),
                        static_cast<int>(m_project->getItems().size()),
                        static_cast<int>(m_project->getStates().size()));
                    m_hasUnsavedChanges = false;
                    spdlog::info("IDERenderer: opened project '{}' from {}", m_project->getName(), path);
                    if (!loaded.warnings.empty()) {
                        spdlog::warn("IDERenderer: opened '{}' with {} entity(ies) skipped",
                                     path, loaded.warnings.size());
                        m_loadWarningsDialog->open(loaded.warnings);
                    }
                } catch (const std::exception& e) {
                    // Keep the current project untouched and tell the user —
                    // an open must never fail silently.
                    m_lastOpenError = e.what();
                    spdlog::error("IDERenderer: open failed ({}) — keeping current project", e.what());
                }
            },
            [this](const std::string& path) {
                if (m_project == nullptr) return;
                m_lastSaveError.clear();
                try {
                    Core::ProjectSerializer::save(*m_project, path);
                    m_project->setFilePath(path);
                    m_hasUnsavedChanges = false;
                    m_statusBarPanel->setProjectName(m_project->getName());
                    spdlog::info("IDERenderer: saved project to {}", path);
                } catch (const std::exception& e) {
                    // Keep the project dirty and tell the user — a save must
                    // never fail silently.
                    m_lastSaveError = e.what();
                    spdlog::error("IDERenderer: save failed — {}", e.what());
                }
            }
        );
        m_menuBarRenderer->setProjectPathProvider([this]() -> std::optional<std::string> {
            if (m_project && m_project->isSaved()) {
                return Core::PathService::toUtf8(m_project->getFilePath());
            }
            return std::nullopt;
        });

        // Native Open / Save-As pickers start in the projects root
        // (<home>/ADS-Projects, or $PROJECTS_DIR).
        m_menuBarRenderer->setDefaultDirProvider([this]() -> std::string {
            const std::string overrideDir =
                getEnvironment() ? getEnvironment()->getOrDefault("PROJECTS_DIR", "") : "";
            return Core::PathService::toUtf8(Core::PathService::projectsRoot(overrideDir));
        });

        // Pre-fill for the Save picker: the real path once saved, otherwise a
        // suggested <projects-root>/<slug>/<slug>.ads from the project name.
        m_menuBarRenderer->setSavePrefillProvider([this]() -> std::string {
            if (m_project && m_project->isSaved()) {
                return Core::PathService::toUtf8(m_project->getFilePath());
            }
            if (!m_project) {
                return "";
            }
            const std::string overrideDir =
                getEnvironment() ? getEnvironment()->getOrDefault("PROJECTS_DIR", "") : "";
            return Core::PathService::toUtf8(Core::PathService::projectFile(
                Core::PathService::projectsRoot(overrideDir), m_project->getName()));
        });

        // Refresh locale-derived caches after a live UI-language switch.
        // Most on-screen text re-reads _t() every frame, but the project tree
        // labels and the inspector's category cache are built once and need a
        // nudge.
        m_menuBarRenderer->setLanguageChangedCallback([this]() {
            this->updateLocale();
            m_menuBarRenderer->updateLocale();
            m_projectTreePanel->rebuildFromProject();
            m_inspectorPanel->refresh();
        });

        // Translations panel: mark the project dirty on any edit; a name edit
        // may change a tree label, so refresh the tree too.
        m_translationPanel->onEdited = [this]() {
            m_hasUnsavedChanges = true;
            m_projectTreePanel->rebuildFromProject();
        };

        // View ▸ Translations + toolbar 🌐 toggle the panel.
        auto toggleTranslations = [this]() {
            m_translationPanel->setVisible(!m_translationPanel->isVisible());
        };
        m_menuBarRenderer->setTranslationsToggle(
            toggleTranslations,
            [this]() { return m_translationPanel->isVisible(); });
        m_toolBarRenderer->setTranslationsToggle(toggleTranslations);

        // "Entities" menu — same handlers the tree context menu uses.
        EntityMenuCallbacks entityCb;
        entityCb.onCreate      = [this](EntityKind k) { createEntity(k); };
        entityCb.onDuplicate   = [this](EntityKind k, const std::string& id) { duplicateEntity(k, id); };
        entityCb.onDelete      = [this](EntityKind k, const std::string& id) { deleteEntity(k, id); };
        entityCb.listEntities  = [this](EntityKind k) { return listEntities(k); };
        m_menuBarRenderer->setEntityMenuCallbacks(std::move(entityCb));
    }

    /**
     * @brief Render the main dockspace window
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders the main dockspace window that serves as the container for all
     * IDE panels. Calculates viewport dimensions accounting for status bar height,
     * configures window flags for proper docking behavior, creates the ImGui
     * dockspace, sets up the docking layout, and renders the menu bar.
     *
     * The window is positioned to fill the entire viewport except for the status
     * bar at the bottom, and uses passthrough central node flags to allow the
     * dockspace background to be transparent.
     *
     * @note This is a private method called by render()
     * @see render(), LayoutManager::setupDockingLayout()
     */
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
        m_newProjectDialog->render();
        m_loadWarningsDialog->render();

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

            auto* t = Core::App::getTranslationsManager();

            if (m_project == nullptr) {
                // No project open — just a muted placeholder, no dot and no
                // saved / unsaved indicator (there is nothing to save).
                ImGui::TextColored(TEXT2, "%s", t->_t("PROJECT.NO_PROJECT").c_str());
            } else {
                // Saved/unsaved dot indicator (ICON_FA_CIRCLE guaranteed in the loaded icon font)
                ImGui::TextColored(m_hasUnsavedChanges ? C_WARN : C_OK, ICON_FA_CIRCLE);
                ImGui::SameLine(0, 8);
                ImGui::SetCursorPosY(posY);

                ImGui::TextColored(TEXT0, "%s", m_project->getName().c_str());

                // Save state, translated — orange "unsaved" while there are
                // pending changes, muted "saved" otherwise.
                ImGui::SameLine(0, 8);
                ImGui::SetCursorPosY(posY);
                const std::string state = " | " + t->_t(m_hasUnsavedChanges ? "PROJECT.UNSAVED" : "PROJECT.SAVED");
                ImGui::TextColored(m_hasUnsavedChanges ? C_WARN : TEXT2, "%s", state.c_str());

                // A failed save is loud: red text + hover for the full reason.
                if (!m_lastSaveError.empty()) {
                    ImGui::SameLine(0, 8);
                    ImGui::SetCursorPosY(posY);
                    ImGui::TextColored(C_ERROR, "%s", ("· " + t->_t("PROJECT.SAVE_FAILED")).c_str());
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("%s", m_lastSaveError.c_str());
                    }
                }
            }

            // A failed open is loud too, independent of whether a project is
            // currently loaded (the failed one never replaced it).
            if (!m_lastOpenError.empty()) {
                ImGui::SameLine(0, 8);
                ImGui::SetCursorPosY(posY);
                ImGui::TextColored(C_ERROR, "%s", ("· " + t->_t("PROJECT.OPEN_FAILED")).c_str());
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s", m_lastOpenError.c_str());
                }
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
     * @brief Execute any deferred native file dialogs
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Delegates to MenuBarRenderer::processPendingDialogs() (File → Open/Save
     * project) and InspectorPanel::processPendingFileDialog() (a property's
     * "Browse…" button). Called by App::run() after render() (i.e. after
     * SDL_RenderPresent) so the compositor has a clean frame before the
     * blocking NFD call freezes the main thread.
     *
     * @see MenuBarRenderer::processPendingDialogs()
     * @see Panels::InspectorPanel::processPendingFileDialog()
     */
    void IDERenderer::processPendingDialogs()
    {
        m_menuBarRenderer->processPendingDialogs();
        m_inspectorPanel->processPendingFileDialog();
        m_newProjectDialog->processPendingDialogs();
    }

    /**
     * @brief Whether any native file dialog is currently running
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     */
    bool IDERenderer::isDialogInProgress() const
    {
        return m_menuBarRenderer->getNavigationService()->isDialogInProgress()
            || m_inspectorPanel->isFileDialogInProgress()
            || m_newProjectDialog->isFileDialogInProgress();
    }

    /**
     * @brief Notify the IDE that the main window was resized
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Forwards to LayoutManager::onWindowResized() so the Project/Inspector/
     * Working Area panels are re-split at their default width ratios against
     * the new window size on the next frame.
     *
     * @see LayoutManager::onWindowResized()
     */
    void IDERenderer::notifyWindowResized()
    {
        m_layoutManager->onWindowResized();
    }

    /**
     * @brief Replace the active project with one built from a NewProjectSpec
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Builds a fresh Core::Project from @p spec (title, synopsis, author,
     * version, languages) and **writes its `.ads` file to
     * @c spec.projectPath**. Only if that write succeeds does it delete the
     * previous project, install the new one and refresh the tree / status
     * bar. On a write failure nothing changes — the caller (NewProjectDialog)
     * keeps its modal open and shows the returned message.
     *
     * @param spec Collected settings from the New Project modal
     * @return std::string Empty on success; the failure reason otherwise
     */
    std::string IDERenderer::newProjectFromSpec(const NewProjectSpec& spec)
    {
        if (spec.projectPath.empty()) {
            return "no location given";
        }

        // Build in a local first — the current project and the tree are only
        // replaced once the .ads file is on disk, so a bad Location leaves the
        // app exactly as it was and the New Project modal can stay open.
        auto project = std::make_unique<Core::Project>(spec.name); // ctor sets game.title == name
        Data::GameData& game = project->getGameData();
        game.setSynopsis(spec.synopsis);
        game.setAuthor({spec.authorName, spec.authorEmail});
        game.setVersion(spec.version);
        game.setLanguages({spec.defaultLangId, spec.supportedLangIds});

        try {
            Core::ProjectSerializer::save(*project, spec.projectPath);
        } catch (const ADS::Exceptions::BaseException& e) {
            // detail() = the reason without the "[file:line] - " prefix; the
            // full form still goes to the log.
            spdlog::error("IDERenderer: could not create project at {} — {}",
                          spec.projectPath.string(), e.what());
            return e.detail();
        } catch (const std::exception& e) {
            spdlog::error("IDERenderer: could not create project at {} — {}",
                          spec.projectPath.string(), e.what());
            return e.what();
        }
        project->setFilePath(spec.projectPath);

        // Success — commit.
        m_inspectorPanel->clearSelection();
        delete m_project;
        m_project = project.release();
        m_lastSaveError.clear();
        m_lastOpenError.clear();
        m_hasUnsavedChanges = false;
        spdlog::info("IDERenderer: new project '{}' created at {}",
                     m_project->getName(), spec.projectPath.string());

        m_projectTreePanel->setProject(m_project);
        m_projectTreePanel->rebuildFromProject();
        m_translationPanel->setProject(m_project);
        m_statusBarPanel->setProjectName(m_project->getName());
        m_statusBarPanel->setCounts(0, 0, 0, 0);
        return {};
    }

    /**
     * @brief Create a new bare State, mirroring the tree's "Add > State"
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Auto-names it "Nuevo estado N" the same way onAddNode does, links
     * it to nothing, rebuilds the project tree, and marks unsaved
     * changes. Shared by onAddNode's State case and
     * InspectorPanel::onCreateState so both paths behave identically.
     *
     * @return Entities::State* The newly created state, or nullptr on failure
     */
    Entities::State* IDERenderer::createNewState()
    {
        if (m_project == nullptr) return nullptr;
        const ADS::Types::StateId id = m_project->nextStateId();
        const std::string label = getTranslationManager()->_t("ENTITY.NEW_STATE");
        Entities::State* state = m_project->addState(
            id, label + " " + std::to_string(id.value));
        m_projectTreePanel->rebuildFromProject();
        m_hasUnsavedChanges = true;
        return state;
    }

    /**
     * @brief Create a new bare StateChain, mirroring "Add > Chain"
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @return Entities::StateChain* The newly created chain, or nullptr on failure
     */
    Entities::StateChain* IDERenderer::createNewChain()
    {
        if (m_project == nullptr) return nullptr;
        const ADS::Types::ChainId id = m_project->nextChainId();
        const std::string label = getTranslationManager()->_t("ENTITY.NEW_CHAIN");
        Entities::StateChain* chain = m_project->addChain(
            id, label + " " + std::to_string(id.value));
        m_projectTreePanel->rebuildFromProject();
        m_hasUnsavedChanges = true;
        return chain;
    }

    // ---------------------------------------------------------------------
    // Entity commands (shared by the tree "+"/context menu and the
    // "Entities" menu)
    // ---------------------------------------------------------------------

    namespace {

        /**
         * @brief Find the entity in @p vec whose BaseEntity::getId() equals @p id.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @tparam Vec Range of `std::unique_ptr<Entity>`
         * @param vec Collection to search
         * @param id  String id to match
         * @return Raw entity pointer, or nullptr when not found
         */
        template <typename Vec>
        auto entityByStringId(const Vec& vec, const std::string& id)
            -> decltype(vec.front().get())
        {
            for (const auto& e : vec) {
                if (e->getId() == id) {
                    return e.get();
                }
            }
            return nullptr;
        }

    } // namespace

    /**
     * @brief Whether @p type is one of the four full CRUD entity kinds.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @param type Tree node type
     * @return bool true for Scene / NPC / Item / State
     */
    bool IDERenderer::nodeTypeIsEntity(Panels::NodeType type)
    {
        switch (type) {
            case Panels::NodeType::Scene:
            case Panels::NodeType::NPC:
            case Panels::NodeType::Item:
            case Panels::NodeType::State:
                return true;
            default:
                return false;
        }
    }

    /**
     * @brief Map a tree NodeType to its EntityKind.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @param type Tree node type (must satisfy nodeTypeIsEntity())
     * @return EntityKind The matching kind (defaults to Scene)
     */
    EntityKind IDERenderer::nodeTypeToEntityKind(Panels::NodeType type)
    {
        switch (type) {
            case Panels::NodeType::NPC:   return EntityKind::Character;
            case Panels::NodeType::Item:  return EntityKind::Item;
            case Panels::NodeType::State: return EntityKind::State;
            case Panels::NodeType::Scene:
            default:                      return EntityKind::Scene;
        }
    }

    /**
     * @brief Refresh tree + status bar + dirty flag after an entity change.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     */
    void IDERenderer::refreshAfterEntityChange()
    {
        if (m_project == nullptr) return;
        m_projectTreePanel->rebuildFromProject();
        // setCounts(scenes, npcs, warnings, errors) — warning/error counts are
        // not tracked yet, so keep them at 0 as the other call sites do.
        m_statusBarPanel->setCounts(
            static_cast<int>(m_project->getScenes().size()),
            static_cast<int>(m_project->getCharacters().size()),
            0, 0);
        m_hasUnsavedChanges = true;
    }

    /**
     * @brief Create a fresh entity of the given kind.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Auto-names it ("New scene 12", …) with a free id from
     * Core::Project::nextXId(), then refreshes the tree and status bar and
     * marks the project dirty. Shared by the tree's "+" / context menu and
     * the "Entities" menu.
     *
     * @param kind Entity type to create
     */
    void IDERenderer::createEntity(EntityKind kind)
    {
        if (m_project == nullptr) return;
        auto* tm = getTranslationManager();
        Entities::BaseEntity* created = nullptr;

        switch (kind) {
            case EntityKind::Scene: {
                const ADS::Types::SceneId id = m_project->nextSceneId();
                created = m_project->addScene(
                    id, tm->_t("ENTITY.NEW_SCENE") + " " + std::to_string(id.value));
                break;
            }
            case EntityKind::Character: {
                const ADS::Types::CharacterId id = m_project->nextCharacterId();
                created = m_project->addCharacter(
                    id, tm->_t("ENTITY.NEW_CHARACTER") + " " + std::to_string(id.value));
                break;
            }
            case EntityKind::Item: {
                const ADS::Types::ObjectId id = m_project->nextItemId();
                created = m_project->addItem(
                    id, tm->_t("ENTITY.NEW_ITEM") + " " + std::to_string(id.value));
                break;
            }
            case EntityKind::State:
                created = createNewState();  // already rebuilds the tree + marks dirty
                break;
        }

        refreshAfterEntityChange();
        if (created != nullptr) {
            m_inspectorPanel->setSelectedObject(created);
            Panels::NodeType nodeType = Panels::NodeType::Scene;
            switch (kind) {
                case EntityKind::Character: nodeType = Panels::NodeType::NPC;   break;
                case EntityKind::Item:      nodeType = Panels::NodeType::Item;  break;
                case EntityKind::State:     nodeType = Panels::NodeType::State; break;
                case EntityKind::Scene:     nodeType = Panels::NodeType::Scene; break;
            }
            m_projectTreePanel->setSelectedNode(created->getId(), nodeType);
        }
    }

    /**
     * @brief Deep-copy an existing entity (new id + " (copy)" name).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Resolves @p id within the @p kind collection, calls the matching
     * Core::Project::duplicateX(), then selects the copy in the Inspector.
     * No-op when @p id is unknown.
     *
     * @param kind Entity type
     * @param id   String id of the entity to copy (BaseEntity::getId())
     */
    void IDERenderer::duplicateEntity(EntityKind kind, const std::string& id)
    {
        if (m_project == nullptr) return;
        const std::string suffix =
            " " + getTranslationManager()->_t("ENTITY.COPY_SUFFIX");
        Inspector::IInspectable* copy = nullptr;

        switch (kind) {
            case EntityKind::Scene:
                if (auto* e = entityByStringId(m_project->getScenes(), id)) {
                    copy = m_project->duplicateScene(
                        e->getSceneId(), e->getDisplayName() + suffix);
                }
                break;
            case EntityKind::Character:
                if (auto* e = entityByStringId(m_project->getCharacters(), id)) {
                    copy = m_project->duplicateCharacter(
                        e->getCharacterId(), e->getDisplayName() + suffix);
                }
                break;
            case EntityKind::Item:
                if (auto* e = entityByStringId(m_project->getItems(), id)) {
                    copy = m_project->duplicateItem(
                        e->getItemId(), e->getDisplayName() + suffix);
                }
                break;
            case EntityKind::State:
                if (auto* e = entityByStringId(m_project->getStates(), id)) {
                    copy = m_project->duplicateState(
                        e->getStateId(), e->getDisplayName() + suffix);
                }
                break;
        }

        refreshAfterEntityChange();
        if (copy != nullptr) {
            m_inspectorPanel->setSelectedObject(copy);
        }
    }

    /**
     * @brief Delete the entity of @p kind with the given string id.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Clears the Inspector selection first when it points at that entity,
     * then calls the matching Core::Project::removeX(). No-op when @p id is
     * unknown.
     *
     * @param kind Entity type
     * @param id   String id of the entity to delete
     */
    void IDERenderer::deleteEntity(EntityKind kind, const std::string& id)
    {
        if (m_project == nullptr) return;

        // Drop the Inspector selection first if it points at the victim.
        auto clearIfSelected = [this](Inspector::IInspectable* target) {
            if (target != nullptr && m_inspectorPanel->getSelectedObject() == target) {
                m_inspectorPanel->clearSelection();
            }
        };

        switch (kind) {
            case EntityKind::Scene:
                if (auto* e = entityByStringId(m_project->getScenes(), id)) {
                    clearIfSelected(e);
                    m_project->removeScene(e->getSceneId());
                }
                break;
            case EntityKind::Character:
                if (auto* e = entityByStringId(m_project->getCharacters(), id)) {
                    clearIfSelected(e);
                    m_project->removeCharacter(e->getCharacterId());
                }
                break;
            case EntityKind::Item:
                if (auto* e = entityByStringId(m_project->getItems(), id)) {
                    clearIfSelected(e);
                    m_project->removeItem(e->getItemId());
                }
                break;
            case EntityKind::State:
                if (auto* e = entityByStringId(m_project->getStates(), id)) {
                    clearIfSelected(e);
                    m_project->removeState(e->getStateId());
                }
                break;
        }

        refreshAfterEntityChange();
    }

    std::vector<std::pair<std::string, std::string>>
    IDERenderer::listEntities(EntityKind kind) const
    {
        std::vector<std::pair<std::string, std::string>> out;
        if (m_project == nullptr) return out;

        auto fill = [&out](const auto& vec) {
            for (const auto& e : vec) {
                out.emplace_back(e->getId(), e->getDisplayName());
            }
        };

        switch (kind) {
            case EntityKind::Scene:     fill(m_project->getScenes());     break;
            case EntityKind::Character: fill(m_project->getCharacters()); break;
            case EntityKind::Item:      fill(m_project->getItems());      break;
            case EntityKind::State:     fill(m_project->getStates());     break;
        }
        return out;
    }

    /**
     * @brief Render the complete IDE
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Main rendering method that orchestrates the rendering of all IDE components
     * in the correct order. Renders the main dockspace window with menu bar first,
     * followed by the toolbar, status bar at the bottom, and finally all dockable
     * panels (entities, properties, inspector, and working area).
     *
     * This method should be called once per frame from the main application render
     * loop. It replaces the monolithic RenderIDE() function from main.cpp with a
     * more modular and maintainable object-oriented approach.
     *
     * @note Must be called within an ImGui frame context (after ImGui_ImplSDL2_NewFrame)
     * @see renderMainWindow(), MenuBarRenderer::render(), ToolBarRenderer::render()
     */
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
        m_translationPanel->render();
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
