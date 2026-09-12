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

#ifndef ADS_DATA_GAME_DATA_H
#define ADS_DATA_GAME_DATA_H

/**
 * @file GameData.h
 * @brief Pure data class for project/game-wide settings — no ImGui dependency
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <cstdint>
#include <string>
#include <vector>

namespace ADS::Data {

    /**
     * @brief Author / studio credit for the game (docs/core/schemas/game.md
     *        `game.author`).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * `name` maps to `game.author.name` (String, 128); `email` to
     * `game.author.email` (String, 255). Neither is a LexEngine entry —
     * a real-world name is not translated per language. Length limits are an
     * editor concern, not enforced here. Only standard C++ types are used.
     */
    struct GameAuthor {
        std::string name;  ///< game.author.name
        std::string email; ///< game.author.email

        /**
         * @brief Check equality with another GameAuthor
         * @param other The GameAuthor to compare against
         * @return bool True if every field is equal
         */
        bool operator==(const GameAuthor& other) const = default;
    };

    /**
     * @brief The languages a project's compiled builds can target
     *        (docs/core/schemas/game.md `game.languages`).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * `defaultId` is the Language catalog id (see src/constants/languages.h)
     * used when a compile target does not force a specific language.
     * `supportedIds` is the set of Language catalog ids the LexEngine holds
     * translations for; a build may compile for any one of them. The invariant
     * `defaultId ∈ supportedIds` is maintained by the editor, not here.
     */
    struct GameLanguages {
        std::uint8_t defaultId = 21;                    ///< game.languages.default (21 = en_US)
        std::vector<std::uint8_t> supportedIds{21};     ///< game.languages.supported

        /**
         * @brief Check equality with another GameLanguages
         * @param other The GameLanguages to compare against
         * @return bool True if every field is equal
         */
        bool operator==(const GameLanguages& other) const = default;
    };

    /**
     * @brief Project-wide game settings (docs/core/schemas/game.md `game` object).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * A project has exactly one game entry. Only the fields the IDE currently
     * edits live here — the rest of game.md (platforms/triggers/flags/…) is
     * added as those editors are built. Only standard C++ types are used —
     * no ImGui or UI framework types. Owned by Core::Project.
     */
    class GameData {
    public:
        /**
         * @brief Get the player inventory's maximum capacity, in slots.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Total number of inventory slots the player can carry. An item's
         * `slots` value (ItemData::getSlots()) can never exceed this — the
         * IDE clamps the item editor's range to it.
         *
         * @return uint8_t Inventory capacity in slots (1-255)
         */
        [[nodiscard]] uint8_t getInventoryCapacity() const { return m_inventoryCapacity; }

        /**
         * @brief Set the player inventory's maximum capacity, in slots.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param capacity New capacity in slots; values below 1 are treated as 1
         */
        void setInventoryCapacity(uint8_t capacity) {
            m_inventoryCapacity = capacity < 1 ? 1 : capacity;
        }

        /**
         * @brief Get the game's display title (game.md `game.title`).
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::string& Title text; empty until the author sets one
         */
        [[nodiscard]] const std::string& getTitle() const { return m_title; }

        /**
         * @brief Set the game's display title.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param title New title; the game.md 128-byte limit is an editor concern
         */
        void setTitle(const std::string& title) { m_title = title; }

        /**
         * @brief Get the game synopsis (game.md `game.synopsis`).
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const std::string& Free-form synopsis text, possibly empty
         */
        [[nodiscard]] const std::string& getSynopsis() const { return m_synopsis; }

        /**
         * @brief Set the game synopsis.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param synopsis New synopsis text
         */
        void setSynopsis(const std::string& synopsis) { m_synopsis = synopsis; }

        /**
         * @brief Get the author / studio credit (game.md `game.author`).
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const GameAuthor& Author name and email
         */
        [[nodiscard]] const GameAuthor& getAuthor() const { return m_author; }

        /**
         * @brief Set the author / studio credit.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param author New author record
         */
        void setAuthor(const GameAuthor& author) { m_author = author; }

        /**
         * @brief Get the overall game version string (game.md `game.version.version`).
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * A free-form semantic version string. Per-platform version overrides
         * (`game.version.platforms`) are added when that editor is built.
         *
         * @return const std::string& Version string, defaulting to "0.1.0"
         */
        [[nodiscard]] const std::string& getVersion() const { return m_version; }

        /**
         * @brief Set the overall game version string.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param version New version string; the game.md 16-byte limit is an
         *                editor concern
         */
        void setVersion(const std::string& version) { m_version = version; }

        /**
         * @brief Get the project's target-language set (game.md `game.languages`).
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return const GameLanguages& Default and supported Language catalog ids
         */
        [[nodiscard]] const GameLanguages& getLanguages() const { return m_languages; }

        /**
         * @brief Set the project's target-language set.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param languages New language set; the caller guarantees
         *                  `defaultId ∈ supportedIds`
         */
        void setLanguages(const GameLanguages& languages) { m_languages = languages; }

        /**
         * @brief Check equality with another GameData
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param other The GameData to compare against
         * @return bool True if every field is equal
         */
        bool operator==(const GameData& other) const = default;

    private:
        uint8_t m_inventoryCapacity = 20;   ///< Player inventory size, in slots
        std::string m_title;                ///< game.title — display name
        std::string m_synopsis;             ///< game.synopsis — free-form description
        GameAuthor m_author;                ///< game.author — name + email
        std::string m_version = "0.1.0";    ///< game.version.version — overall version
        GameLanguages m_languages;          ///< game.languages — default + supported ids
    };

} // namespace ADS::Data

#endif // ADS_DATA_GAME_DATA_H
