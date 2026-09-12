/**
 * Copyright (c) 2026 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
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

#ifndef ADS_IDE_MENU_MODEL_H
#define ADS_IDE_MENU_MODEL_H

/**
 * @file MenuModel.h
 * @brief Plain-data description of the IDE menu bar
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 *
 * The menu bar used to be five hand-written render methods. It is now a table of
 * these structs walked by MenuBarRenderer::renderMenu() / renderEntry(). Adding
 * an item is one MenuEntry row in MenuBarRenderer::buildMenus().
 */

#include <functional>
#include <vector>

namespace ADS::IDE {

    /**
     * @brief The shapes a single menu entry can take
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * One value per pattern that already occurs in the menu bar — nothing
     * speculative.
     */
    enum class MenuEntryKind {
        Action,     ///< clickable ImGui::MenuItem; runs @c onClick
        Toggle,     ///< ImGui::MenuItem with a check mark from @c checked(); runs @c onClick
        Separator,  ///< ImGui::Separator()
        Submenu,    ///< ImGui::BeginMenu wrapping @c children
        Custom      ///< @c custom() emits its own ImGui calls (e.g. the Language submenu)
    };

    /**
     * @brief One entry inside a menu
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Which fields matter depends on @c kind:
     * - @c Action  : @c labelKey, @c shortcut (optional), @c onClick, @c enabled (optional)
     * - @c Toggle  : as Action plus @c checked
     * - @c Submenu : @c labelKey, @c children, @c enabled (optional)
     * - @c Separator: nothing
     * - @c Custom  : @c custom
     *
     * @c labelKey / @c shortcut are @c const @c char* string literals; the label
     * is resolved through i18n every frame, never cached, so a live language
     * switch re-localises the bar.
     */
    // Every member carries a default initializer so a designated-initializer
    // that names only a few fields does not trip -Wmissing-field-initializers.
    struct MenuEntry {
        MenuEntryKind          kind     = MenuEntryKind::Action;
        const char*            labelKey = nullptr;  ///< i18n key (Action / Toggle / Submenu)
        const char*            shortcut = nullptr;  ///< accelerator hint, e.g. "Ctrl+S" (optional)
        std::function<void()>  onClick  = {};       ///< Action / Toggle handler
        std::function<bool()>  checked  = {};       ///< Toggle: current check state
        std::function<bool()>  enabled  = {};       ///< optional; entry greyed out when it returns false
        std::vector<MenuEntry> children = {};       ///< Submenu contents
        std::function<void()>  custom   = {};       ///< Custom: emits its own ImGui calls
    };

    /**
     * @brief One top-level menu in the bar (File, Edit, …)
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     */
    struct MenuDef {
        const char*            headerKey = nullptr; ///< i18n key for the menu title
        std::vector<MenuEntry> entries   = {};      ///< entries, in display order
        std::function<bool()>  enabled   = {};      ///< optional; the whole menu is
                                                    ///< greyed and cannot open when
                                                    ///< this returns false
    };

} // namespace ADS::IDE

#endif // ADS_IDE_MENU_MODEL_H
