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


#ifndef ADS_MENU_BAR_RENDERER_H
#define ADS_MENU_BAR_RENDERER_H

#include "NavigationService.h"
#include "MenuModel.h"
#include "../EntityKind.h"
#include "../LayoutManager.h"
#include "i18n/i18n.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace ADS::IDE {

    /**
     * @brief Handlers backing the "Entities" menu and the tree context menu.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Every field is optional (a null std::function is simply ignored). The
     * same four handlers serve the menu bar's "Entities" submenus and
     * ProjectTreePanel's per-node context menu so both routes end up in one
     * place (IDERenderer).
     */
    struct EntityMenuCallbacks {
        /// Create a fresh entity of the given kind.
        std::function<void(EntityKind)> onCreate;
        /// Deep-copy the entity with the given string id (new id + name).
        std::function<void(EntityKind, const std::string& id)> onDuplicate;
        /// Delete the entity with the given string id.
        std::function<void(EntityKind, const std::string& id)> onDelete;
        /// Current entities of a kind as {stringId, displayName} pairs.
        std::function<std::vector<std::pair<std::string, std::string>>(EntityKind)>
            listEntities;
    };

    /**
     * @brief Renders the main menu bar for the IDE
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Provides menu functionality for File, Edit, View, Options, and Help menus
     * including theme switching, layout management, and language selection.
     * Integrates with NavigationService for handling menu actions.
     */
    class MenuBarRenderer: public IDEBase {
    private:
        /**
         * @brief Reference to the layout manager
         *
         * Provides access to layout management functionality for operations
         * like resetting the IDE layout to default settings.
         */
        LayoutManager* m_layoutManager;

        /**
         * @brief Service for handling navigation actions
         *
         * Unique pointer to NavigationService that handles file operations
         * and other navigation-related actions triggered from menu items.
         */
        std::unique_ptr<NavigationService> m_navigationService;

        /**
         * @brief Translation manager for internationalization
         *
         * Provides access to localized strings for menu items and labels,
         * enabling multi-language support in the menu bar.
         */
        i18n::i18n* m_translationManager;

        /**
         * @brief Invoked after the user picks a new IDE UI language
         *
         * Set via setLanguageChangedCallback(). Lets the owning renderer
         * refresh caches (IDEBase::updateLocale(), tree rebuild, inspector
         * refresh) that read the locale once rather than every frame.
         */
        std::function<void()> m_onLanguageChanged;

        /**
         * @brief Toggle / query the Translations panel
         *
         * Set via setTranslationsToggle(). `m_onToggleTranslations` flips the
         * panel's visibility; `m_translationsIsOpen` feeds the menu item's check.
         */
        std::function<void()> m_onToggleTranslations;
        std::function<bool()> m_translationsIsOpen;

        /**
         * @brief Entity create / duplicate / delete handlers + list provider
         *
         * Set via setEntityMenuCallbacks(); consumed by the "Entities" menu
         * built in buildMenus() and by renderEntityListSubmenu(). Null until
         * the owning IDERenderer wires them, so every call site guards.
         */
        std::function<void(EntityKind)> m_onEntityCreate;
        std::function<void(EntityKind, const std::string&)> m_onEntityDuplicate;
        std::function<void(EntityKind, const std::string&)> m_onEntityDelete;
        std::function<std::vector<std::pair<std::string, std::string>>(EntityKind)>
            m_entityListProvider;

        /**
         * @brief The whole menu bar as data, built once by buildMenus()
         *
         * Walked every frame by render() → renderMenu() → renderEntry().
         */
        std::vector<MenuDef> m_menus;

        /**
         * @brief Populate m_menus with the File / Edit / View / Options / Help table
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Called once from the constructor. Entry handlers capture @c this and
         * call through the member std::function callbacks (m_on…, the owned
         * NavigationService), so building before setNavigationCallbacks() /
         * setTranslationsToggle() / … run is safe.
         */
        void buildMenus();

        /**
         * @brief Render one top-level menu from its MenuDef
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param menu The menu to render
         * @note Must be called within an active ImGui::BeginMenuBar() context
         */
        void renderMenu(const MenuDef& menu);

        /**
         * @brief Render one menu entry according to its MenuEntryKind
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Recurses into @c children for a Submenu entry. Labels are resolved
         * through the translation manager on every call, never cached.
         *
         * @param entry The entry to render
         * @note Must be called within an active ImGui menu context
         */
        void renderEntry(const MenuEntry& entry);

        /**
         * @brief Render the Options > Language submenu
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Lists every currently loaded UI language (i18n::getAvailableLanguages()
         * — the codes that actually have a translation file). Selecting one
         * applies it live via i18n::setLocale() + reloadTranslations(), persists
         * it to .env as UI_LANGUAGE, and fires the language-changed callback.
         *
         * @note Must be called within an active ImGui menu context
         */
        void renderLanguageMenu();

        /**
         * @brief Render a "Duplicate ▸" / "Delete ▸" submenu listing entities.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Pulls the current entities of @p kind from @c m_entityListProvider
         * and shows one @c MenuItem per entity; picking one calls
         * @c m_onEntityDelete (when @p isDelete) or @c m_onEntityDuplicate
         * with that entity's string id. The submenu is greyed when the kind
         * has no entities.
         *
         * @param kind     Entity type this submenu operates on
         * @param labelKey i18n key for the submenu label
         * @param isDelete  true → delete route, false → duplicate route
         * @note Must be called within an active ImGui menu context
         */
        void renderEntityListSubmenu(EntityKind kind, const char* labelKey, bool isDelete);

        /**
         * @brief Handle exit action
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initiates application shutdown by pushing an SDL_QUIT event to
         * the SDL event queue. This triggers the main application loop
         * to exit gracefully.
         *
         * @note This does not perform any cleanup; it only signals the
         *       application to begin its shutdown sequence
         */
        void handleExit();

        /**
         * @brief Handle theme change
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Applies the specified theme to the ImGui interface by creating
         * and applying either a DarkTheme or LightTheme instance based on
         * the parameter value.
         *
         * @param darkTheme True to apply dark theme, false to apply light theme
         *
         * @see DarkTheme::apply()
         * @see LightTheme::apply()
         */
        void handleThemeChange(bool darkTheme);

    public:
        /**
         * @brief Construct a new MenuBarRenderer object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the menu bar renderer with references to required services.
         * Sets up the layout manager, creates the navigation service, and obtains
         * the translation manager from the IDEBase parent class.
         *
         * @param layoutManager Pointer to the layout manager for layout operations
         *
         * @note The translation manager is obtained through IDEBase inheritance
         * @see IDEBase::getTranslationManager()
         */
        explicit MenuBarRenderer(LayoutManager* layoutManager);

        /**
         * @brief Destroy the MenuBarRenderer object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Default destructor. The NavigationService is automatically cleaned up
         * through the unique_ptr.
         */
        ~MenuBarRenderer() = default;

        /**
         * @brief Get the NavigationService owned by this menu bar
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * Exposes the single NavigationService instance so other renderers
         * (e.g. ToolBarRenderer) can share it instead of owning a second,
         * separately-wired instance.
         *
         * @return NavigationService* Non-owning pointer to the owned instance
         */
        NavigationService *getNavigationService() const;

        /**
         * @brief Register project-awareness callbacks on the NavigationService
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Forwards the two callbacks to the owned NavigationService so it can
         * query whether a project is active and trigger new-project creation.
         * Must be called after construction and before the first File > New action.
         *
         * @param hasProject   Predicate returning true when a project is open
         * @param onNewProject Callable invoked to create a fresh project
         * @see NavigationService::setProjectCallbacks()
         */
        void setNavigationCallbacks(
            std::function<bool()> hasProject,
            std::function<void()> onNewProject
        );

        /**
         * @brief Register file I/O callbacks forwarded to the NavigationService
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Forwards the open and save path callbacks to the owned NavigationService
         * so that native file dialog results reach the application layer. Must be
         * called before the user can trigger File > Open or the Save action in the
         * "New project" confirmation dialog.
         *
         * @param onOpen  Callable receiving the absolute path from the Open dialog
         * @param onSave  Callable receiving the absolute path from the Save dialog
         * @see NavigationService::setFileCallbacks()
         */
        void setFileCallbacks(
            std::function<void(const std::string&)> onOpen,
            std::function<void(const std::string&)> onSave
        );

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
        void setProjectPathProvider(std::function<std::optional<std::string>()> getPath);

        /**
         * @brief Forward the default-directory provider to the NavigationService
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param getDir Callable returning the folder the native Open / Save-As
         *               pickers should open in (the projects root), or an empty
         *               string to let the OS decide
         * @see NavigationService::setDefaultDirProvider()
         */
        void setDefaultDirProvider(std::function<std::string()> getDir);

        /**
         * @brief Forward the save-picker pre-fill provider to the NavigationService
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param getPrefill Callable returning a full `.ads` path to seed the
         *                   Save / Save As dialog, or an empty string for no hint
         * @see NavigationService::setSavePrefillProvider()
         */
        void setSavePrefillProvider(std::function<std::string()> getPrefill);

        /**
         * @brief Register a callback fired when the IDE UI language changes
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * The Options > Language submenu applies the new locale immediately and
         * persists it to .env; this callback lets the owning renderer refresh
         * whatever it caches from the locale (menu/inspector LocaleInfo, project
         * tree labels).
         *
         * @param onLanguageChanged Callable invoked after a successful switch
         */
        void setLanguageChangedCallback(std::function<void()> onLanguageChanged);

        /**
         * @brief Register the Translations-panel toggle and its open-state query
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param toggle Flip the Translations panel's visibility
         * @param isOpen Return whether the panel is currently visible (menu check)
         */
        void setTranslationsToggle(std::function<void()> toggle, std::function<bool()> isOpen);

        /**
         * @brief Register the entity create / duplicate / delete handlers.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * Backs the "Entities" menu. IDERenderer passes the same callables it
         * uses for the tree context menu, so both routes share one code path.
         *
         * @param callbacks The handler set (moved from)
         */
        void setEntityMenuCallbacks(EntityMenuCallbacks callbacks);

        /**
         * @brief Render any pending modal dialogs from the NavigationService
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Delegates to NavigationService::renderDialogs(). Must be called once
         * per frame from within an active ImGui window, outside any
         * BeginMenu / EndMenu scope (typically after EndMenuBar).
         *
         * @see NavigationService::renderDialogs()
         */
        void renderDialogs();

        /**
         * @brief Execute any deferred native file dialogs
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Feb 2026
         *
         * Delegates to NavigationService::processPendingDialogs(). Must be
         * called after SDL_RenderPresent and before the next ImGui::NewFrame()
         * to ensure the compositor has a clean frame while the blocking NFD
         * call is in progress.
         *
         * @see NavigationService::processPendingDialogs()
         */
        void processPendingDialogs();

        /**
         * @brief Render the menu bar
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Walks the data-driven menu table (see buildMenus()) rendering each
         * top-level menu in order.
         *
         * @note Should be called within an active ImGui::BeginMenuBar() context
         * @see buildMenus()
         * @see renderMenu()
         * @see renderEntry()
         */
        void render();
    };
}

#endif //ADS_MENU_BAR_RENDERER_H
