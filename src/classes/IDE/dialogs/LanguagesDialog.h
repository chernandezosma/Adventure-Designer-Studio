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

#ifndef ADS_IDE_LANGUAGES_DIALOG_H
#define ADS_IDE_LANGUAGES_DIALOG_H

/**
 * @file LanguagesDialog.h
 * @brief Modal to edit a project's translatable-language set
 *        (`game.languages.supported`) after the project has been created
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <array>
#include <cstdint>
#include <functional>

#include "../IDEBase.h"
#include "Data/GameData.h"
#include "languages.h"

namespace ADS::IDE {

    /**
     * @brief Pure-ImGui modal listing every Language-catalog entry with a
     *        checkbox, seeded from the project's current supported set.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * open() arms the modal; render() must be called every frame from within
     * an active ImGui window. The project's default language is shown checked
     * and disabled — it can never be removed. On OK the resulting
     * Data::GameLanguages is handed to onApply.
     */
    class LanguagesDialog : public IDEBase {
    public:
        /// Invoked with the new language set when the user confirms.
        std::function<void(const Data::GameLanguages&)> onApply;

        /**
         * @brief Arm the modal, seeded from @p current.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param current The project's present language set
         */
        void open(const Data::GameLanguages& current);

        /**
         * @brief Render the modal for the current frame.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * A no-op frame unless the modal is armed or already open.
         */
        void render();

    private:
        /// Number of entries in the language catalog (fixes the checkbox array size).
        static constexpr std::size_t kCatalogSize =
            ADS::Constants::Languages::languageCatalog.size();

        bool m_pending = false;                    ///< set by open(), consumed on the next render()
        std::uint8_t m_defaultId = 21;             ///< the (locked) default language id
        std::array<bool, kCatalogSize> m_checked{}; ///< checkbox state per catalog entry
    };

} // namespace ADS::IDE

#endif // ADS_IDE_LANGUAGES_DIALOG_H
