/*
 * Adventure Designer Studio
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is licensed under the GNU General Public License version 3 (GPLv3).
 * See LICENSE.md and COPYING for full license details.
 *
 * This software includes an additional requirement for visible attribution:
 * The original author's name must be displayed in any user interface or
 * promotional material
 */


#include "MenuBarRenderer.h"
#include "imgui.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include "spdlog/spdlog.h"
#include "languages.h"
#include "../themes/DarkTheme.h"
#include "../themes/LightTheme.h"

namespace ADS::IDE {
    /**
     * @brief Construct a new MenuBarRenderer object
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Initializes the menu bar renderer with references to required services.
     * The constructor sets up the layout manager reference, creates a new
     * NavigationService instance, and obtains the translation manager from
     * the IDEBase parent class for internationalization support.
     *
     * @param layoutManager Pointer to the layout manager for layout operations
     *
     * @note Locale management is handled by the IDEBase parent class
     * @see IDEBase::IDEBase()
     */
    MenuBarRenderer::MenuBarRenderer(LayoutManager *layoutManager) :
        IDEBase(),
        m_layoutManager(layoutManager),
        m_navigationService(std::make_unique<NavigationService>()),
        m_translationManager(this->getTranslationManager())
    {
        // Locale is now managed in IDEBase
        buildMenus();
    }

    /**
     * @brief Get the NavigationService owned by this menu bar
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     */
    NavigationService *MenuBarRenderer::getNavigationService() const
    {
        return m_navigationService.get();
    }

    /**
     * @brief Populate m_menus with the File / Edit / View / Options / Help table
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Called once from the constructor. This is the whole menu bar: adding an
     * item means adding one MenuEntry row here. Every handler calls through the
     * member callbacks / owned NavigationService, which are all valid for the
     * lifetime of this object, so building before setTranslationsToggle() etc.
     * have run is safe.
     */
    void MenuBarRenderer::buildMenus()
    {
        NavigationService* nav = m_navigationService.get();
        auto hasProject = [nav] { return nav->hasActiveProject(); };

        // One "Entities ▸ <Kind>" submenu: Create, then Duplicate ▸ / Delete ▸
        // lists rendered live from m_entityListProvider.
        auto entityKindMenu = [this, hasProject](const char* labelKey, EntityKind kind) {
            return MenuEntry{
                .kind     = MenuEntryKind::Submenu,
                .labelKey = labelKey,
                .enabled  = hasProject,
                .children = {
                    MenuEntry{.labelKey = "TREE.CONTEXT_CREATE",
                              .onClick = [this, kind] {
                                  if (m_onEntityCreate) m_onEntityCreate(kind);
                              }},
                    MenuEntry{.kind = MenuEntryKind::Custom,
                              .custom = [this, kind] {
                                  renderEntityListSubmenu(kind, "TREE.CONTEXT_DUPLICATE", false);
                              }},
                    MenuEntry{.kind = MenuEntryKind::Custom,
                              .custom = [this, kind] {
                                  renderEntityListSubmenu(kind, "TREE.CONTEXT_DELETE", true);
                              }},
                }};
        };

        m_menus = {
            // ---- File --------------------------------------------------------
            MenuDef{
                "MENU.FILE_HEADER", {
                MenuEntry{.labelKey = "MENU.FILE_NEW", .shortcut = "Ctrl+N", .onClick = [nav] { nav->fileNewHandler(); }},
                MenuEntry{.labelKey = "MENU.FILE_OPEN", .shortcut = "Ctrl+O", .onClick = [nav] { nav->fileOpenHandler(); }},
                // Save / Save As are project-scoped — greyed out on the empty
                // start screen.
                MenuEntry{.labelKey = "MENU.FILE_SAVE", .shortcut = "Ctrl+S", .onClick = [nav] { nav->fileSaveHandler(); },.enabled = hasProject},
                MenuEntry{.labelKey = "MENU.FILE_SAVE_AS", .shortcut = "Ctrl+Shift+S",.onClick = [nav] { nav->fileSaveAsHandler(); },.enabled = hasProject},
                MenuEntry{.kind = MenuEntryKind::Separator},
                MenuEntry{.labelKey = "MENU.FILE_EXIT", .shortcut = "Alt+F4",.onClick = [this] { handleExit(); }},
            }},

            // ---- Edit (placeholders) ---------------------------------------
            MenuDef{"MENU.EDIT_HEADER", {
                MenuEntry{.labelKey = "MENU.EDIT_UNDO", .shortcut = "Ctrl+Z"},
                MenuEntry{.labelKey = "MENU.EDIT_REDO", .shortcut = "Shift+Ctrl+Z"},
                MenuEntry{.kind = MenuEntryKind::Separator},
                MenuEntry{.labelKey = "MENU.EDIT_COPY", .shortcut = "Ctrl+C"},
                MenuEntry{.labelKey = "MENU.EDIT_CUT", .shortcut = "Ctrl+X"},
                MenuEntry{.labelKey = "MENU.EDIT_PASTE", .shortcut = "Ctrl+V"},
            }},

            // ---- Entities -------------------------------------------------
            // One submenu per entity kind, each with Create (direct) plus
            // Duplicate ▸ / Delete ▸ submenus that list the current entities.
            // Same handlers as ProjectTreePanel's context menu. The whole menu
            // is greyed out (cannot open) until a project is loaded.
            MenuDef{"MENU.ENTITIES_HEADER", {
                entityKindMenu("TREE.SECTION_SCENES",     EntityKind::Scene),
                entityKindMenu("TREE.SECTION_CHARACTERS", EntityKind::Character),
                entityKindMenu("TREE.SECTION_ITEMS",      EntityKind::Item),
                entityKindMenu("TREE.SECTION_STATES",     EntityKind::State),
            }, hasProject},

            // ---- View ------------------------------------------------------
            MenuDef{"MENU.VIEW_HEADER", {
                // MenuEntry{.labelKey = "MENU.VIEW_ZOOM_IN", .shortcut = "Ctrl++"},
                // MenuEntry{.labelKey = "MENU.VIEW_ZOOM_OUT", .shortcut = "Ctrl+-"},
                MenuEntry{.kind = MenuEntryKind::Separator},
                MenuEntry{.kind = MenuEntryKind::Toggle,.labelKey = "MENU.VIEW_TRANSLATIONS",
                    .onClick = [this] {
                        if (m_onToggleTranslations) m_onToggleTranslations();
                    }, .checked = [this] {
                       return m_translationsIsOpen && m_translationsIsOpen();
                    }
                },
                MenuEntry{.kind = MenuEntryKind::Separator},
                MenuEntry{.labelKey = "MENU.VIEW_RESET_LAYOUT", .onClick = [this] { m_layoutManager->resetLayout(); }},
            }},

            // ---- Options -------------------------------------------------
            MenuDef{"MENU.OPTIONS_HEADER", {
                // The language submenu is dynamic (loaded locales) with bespoke
                // switch logic — it renders itself.
                MenuEntry{.kind = MenuEntryKind::Custom,
                          .custom = [this] { renderLanguageMenu(); }},
                MenuEntry{.kind = MenuEntryKind::Separator},
                MenuEntry{.kind = MenuEntryKind::Submenu, .labelKey = "MENU.VIEW_THEME",
                  .children = {
                      MenuEntry{.labelKey = "MENU.VIEW_DARK_THEME", .onClick = [this] { handleThemeChange(true); }},
                      MenuEntry{.labelKey = "MENU.VIEW_LIGHT_THEME", .onClick = [this] { handleThemeChange(false); }},
                  }},
            }},

            // ---- Help ----------------------------------------------------
            MenuDef{"MENU.HELP_HEADER", {
                MenuEntry{.labelKey = "MENU.HELP_ABOUT"}, // About dialog: TODO
            }},
        };
    }

    /**
     * @brief Render one menu entry according to its MenuEntryKind
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param e The entry to render
     */
    void MenuBarRenderer::renderEntry(const MenuEntry& e)
    {
        if (e.kind == MenuEntryKind::Separator) {
            ImGui::Separator();
            return;
        }
        if (e.kind == MenuEntryKind::Custom) {
            if (e.custom) {
                e.custom();
            }
            return;
        }

        const bool on = !e.enabled || e.enabled();
        if (!on) {
            ImGui::BeginDisabled();
        }

        switch (e.kind) {
            case MenuEntryKind::Action:
                if (ImGui::MenuItem(m_translationManager->_t(e.labelKey).data(), e.shortcut)) {
                    if (e.onClick) {
                        e.onClick();
                    }
                }
                break;

            case MenuEntryKind::Toggle: {
                const bool checked = e.checked && e.checked();
                if (ImGui::MenuItem(m_translationManager->_t(e.labelKey).data(),
                                    e.shortcut, checked)) {
                    if (e.onClick) {
                        e.onClick();
                    }
                }
                break;
            }

            case MenuEntryKind::Submenu:
                if (ImGui::BeginMenu(m_translationManager->_t(e.labelKey).data())) {
                    for (const MenuEntry& child : e.children) {
                        renderEntry(child);
                    }
                    ImGui::EndMenu();
                }
                break;

            default:
                break;
        }

        if (!on) {
            ImGui::EndDisabled();
        }
    }

    /**
     * @brief Render one top-level menu from its MenuDef
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param menu The menu to render
     */
    void MenuBarRenderer::renderMenu(const MenuDef& menu)
    {
        const bool on = !menu.enabled || menu.enabled();
        if (!on) {
            ImGui::BeginDisabled();
        }
        if (ImGui::BeginMenu(m_translationManager->_t(menu.headerKey).data())) {
            for (const MenuEntry& entry : menu.entries) {
                renderEntry(entry);
            }
            ImGui::EndMenu();
        }
        if (!on) {
            ImGui::EndDisabled();
        }
    }

    /**
     * @brief Render the Options > Language submenu
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Offers only the languages that are actually loaded
     * (i18n::getAvailableLanguages()); picking one switches the IDE UI live and
     * writes UI_LANGUAGE to .env so the choice survives a restart.
     */
    void MenuBarRenderer::renderLanguageMenu()
    {
        if (!ImGui::BeginMenu(m_translationManager->_t("MENU.OPTIONS_LANGUAGE_SELECTOR").data())) {
            return;
        }

        const std::string current = m_translationManager->getCurrentLocale().locale;

        // Sort by display name for a stable, readable list.
        std::vector<std::string> codes = m_translationManager->getAvailableLanguages();
        std::sort(codes.begin(), codes.end(),
                  [](const std::string& a, const std::string& b) {
                      return ADS::Constants::Languages::getLanguageName(a)
                           < ADS::Constants::Languages::getLanguageName(b);
                  });

        for (const std::string& code : codes) {
            std::string label = ADS::Constants::Languages::getLanguageName(code);
            if (label.empty()) {
                label = code;
            }

            if (ImGui::MenuItem(label.c_str(), nullptr, code == current)) {
                if (code == current) {
                    continue;
                }
                try {
                    m_translationManager->setLocale(code);
                } catch (const std::exception& e) {
                    spdlog::warn("Language switch to '{}' failed: {}", code, e.what());
                    continue;
                }
                m_translationManager->reloadTranslations();

                if (Environment* env = getEnvironment()) {
                    env->set("UI_LANGUAGE", code);
                }
                if (m_onLanguageChanged) {
                    m_onLanguageChanged();
                }
            }
        }

        ImGui::EndMenu();
    }

    /**
     * @brief Render a "Duplicate ▸" / "Delete ▸" submenu listing entities
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @param kind     Entity type the submenu operates on
     * @param labelKey i18n key for the submenu label
     * @param isDelete  true → delete route, false → duplicate route
     */
    void MenuBarRenderer::renderEntityListSubmenu(EntityKind kind, const char* labelKey,
                                                  bool isDelete)
    {
        std::vector<std::pair<std::string, std::string>> items;
        if (m_entityListProvider) {
            items = m_entityListProvider(kind);
        }

        const bool empty = items.empty();
        if (empty) {
            ImGui::BeginDisabled();
        }
        if (ImGui::BeginMenu(m_translationManager->_t(labelKey).data())) {
            for (const auto& [id, label] : items) {
                // "##id" keeps the MenuItem id unique when two entities share
                // a display name.
                const std::string shown = label + "##" + id;
                if (ImGui::MenuItem(shown.c_str())) {
                    if (isDelete) {
                        if (m_onEntityDelete) m_onEntityDelete(kind, id);
                    } else {
                        if (m_onEntityDuplicate) m_onEntityDuplicate(kind, id);
                    }
                }
            }
            ImGui::EndMenu();
        }
        if (empty) {
            ImGui::EndDisabled();
        }
    }

    /**
     * @brief Handle exit action
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Initiates application shutdown by creating and pushing an SDL_QUIT event
     * to the SDL event queue. This signals the main application loop to begin
     * its graceful shutdown sequence.
     *
     * The function creates an SDL_Event structure, sets its type to SDL_QUIT,
     * and pushes it to the event queue using SDL_PushEvent().
     *
     * @note This method only signals shutdown; it does not perform cleanup
     * @see SDL_PushEvent()
     */
    void MenuBarRenderer::handleExit()
    {
        SDL_Event quit_event = {};
        quit_event.type = SDL_EVENT_QUIT;
        SDL_PushEvent(&quit_event);
    }

    /**
     * @brief Handle theme change
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Applies the specified theme to the ImGui interface. Creates an instance of
     * either DarkTheme or LightTheme based on the parameter and calls its apply()
     * method to update the ImGui style colors.
     *
     * @param darkTheme True to apply dark theme, false to apply light theme
     *
     * @see DarkTheme::apply()
     * @see LightTheme::apply()
     */
    void MenuBarRenderer::handleThemeChange(bool darkTheme)
    {
        if (darkTheme) {
            DarkTheme theme;
            theme.apply();
        } else {
            LightTheme theme;
            theme.apply();
        }
    }

    /**
     * @brief Register project-awareness callbacks on the NavigationService
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Forwards the callbacks directly to the owned NavigationService so it can
     * detect an active project and trigger new-project creation without holding
     * a direct reference to IDERenderer.
     *
     * @param hasProject   Predicate returning true when a project is open
     * @param onNewProject Callable invoked to create a fresh project
     * @see NavigationService::setProjectCallbacks()
     */
    void MenuBarRenderer::setNavigationCallbacks(
        std::function<bool()> hasProject,
        std::function<void()> onNewProject)
    {
        m_navigationService->setProjectCallbacks(
            std::move(hasProject),
            std::move(onNewProject)
        );
    }

    /**
     * @brief Register file I/O callbacks forwarded to the NavigationService
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Delegates directly to NavigationService::setFileCallbacks(). See that
     * method's documentation for the callback semantics.
     *
     * @param onOpen  Callable receiving the absolute path from the Open dialog
     * @param onSave  Callable receiving the absolute path from the Save dialog
     * @see NavigationService::setFileCallbacks()
     */
    void MenuBarRenderer::setFileCallbacks(
        std::function<void(const std::string&)> onOpen,
        std::function<void(const std::string&)> onSave)
    {
        m_navigationService->setFileCallbacks(
            std::move(onOpen),
            std::move(onSave)
        );
    }

    /**
     * @brief Register a callback fired when the IDE UI language changes
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param onLanguageChanged Callable invoked after a successful language switch
     */
    void MenuBarRenderer::setLanguageChangedCallback(std::function<void()> onLanguageChanged)
    {
        m_onLanguageChanged = std::move(onLanguageChanged);
    }

    /**
     * @brief Register the Translations-panel toggle and its open-state query
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param toggle Flip the Translations panel's visibility
     * @param isOpen Return whether the panel is currently visible
     */
    void MenuBarRenderer::setTranslationsToggle(std::function<void()> toggle,
                                               std::function<bool()> isOpen)
    {
        m_onToggleTranslations = std::move(toggle);
        m_translationsIsOpen = std::move(isOpen);
    }

    /**
     * @brief Register the entity create / duplicate / delete handlers
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * @param callbacks The handler set (moved from)
     */
    void MenuBarRenderer::setEntityMenuCallbacks(EntityMenuCallbacks callbacks)
    {
        m_onEntityCreate     = std::move(callbacks.onCreate);
        m_onEntityDuplicate  = std::move(callbacks.onDuplicate);
        m_onEntityDelete     = std::move(callbacks.onDelete);
        m_entityListProvider = std::move(callbacks.listEntities);
    }

    /**
     * @brief Forward the project-file-path provider to the NavigationService
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param getPath Callable returning the saved path, or std::nullopt when
     *                the project has never been saved
     * @see NavigationService::setProjectPathProvider()
     */
    void MenuBarRenderer::setProjectPathProvider(std::function<std::optional<std::string>()> getPath)
    {
        m_navigationService->setProjectPathProvider(std::move(getPath));
    }

    /**
     * @brief Forward the default-directory provider to the NavigationService
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param getDir Callable returning the folder the native pickers open in
     */
    void MenuBarRenderer::setDefaultDirProvider(std::function<std::string()> getDir)
    {
        m_navigationService->setDefaultDirProvider(std::move(getDir));
    }

    /**
     * @brief Forward the save-picker pre-fill provider to the NavigationService
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param getPrefill Callable returning a full `.ads` path to seed the picker
     */
    void MenuBarRenderer::setSavePrefillProvider(std::function<std::string()> getPrefill)
    {
        m_navigationService->setSavePrefillProvider(std::move(getPrefill));
    }

    /**
     * @brief Render any pending modal dialogs from the NavigationService
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Delegates to NavigationService::renderDialogs(). Called by IDERenderer
     * once per frame after EndMenuBar but still within the MainDockSpace window,
     * ensuring the correct ImGui window context for popup rendering.
     *
     * @see NavigationService::renderDialogs()
     */
    void MenuBarRenderer::renderDialogs()
    {
        m_navigationService->renderDialogs();
    }

    /**
     * @brief Execute any deferred native file dialogs
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Feb 2026
     *
     * Delegates to NavigationService::processPendingDialogs(). Called by
     * IDERenderer after SDL_RenderPresent so that blocking NFD calls do not
     * freeze the render loop mid-frame.
     *
     * @see NavigationService::processPendingDialogs()
     */
    void MenuBarRenderer::processPendingDialogs()
    {
        m_navigationService->processPendingDialogs();
    }

    /**
     * @brief Render the menu bar
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Walks the data-driven menu table built by buildMenus(), rendering each
     * top-level menu in order.
     *
     * @note Should be called within an active ImGui::BeginMenuBar() context
     * @see buildMenus()
     * @see renderMenu()
     * @see renderEntry()
     */
    void MenuBarRenderer::render()
    {
        for (const MenuDef& menu : m_menus) {
            renderMenu(menu);
        }
    }
}
