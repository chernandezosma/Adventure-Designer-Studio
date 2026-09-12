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

#ifndef ADS_CORE_TRANSLATION_CATALOG_H
#define ADS_CORE_TRANSLATION_CATALOG_H

/**
 * @file TranslationCatalog.h
 * @brief Enumerates the project's translatable game strings and routes reads /
 *        writes of a string id to the DataObject field that backs it
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <string>
#include <string_view>
#include <vector>

#include "Project.h"

namespace ADS::Core {

    /**
     * @brief Top-level grouping of a translatable string in the panel's list.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     */
    enum class TranslationGroup { Scenes, States, Characters, Items };

    /**
     * @brief Which field of an entity a translatable string comes from.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * `DescLong` maps to the `DescriptionTexts::longText` slot; the string id
     * spells it `desc.long` (the schema term).
     */
    enum class TranslationField { Name, DescNormal, DescLong, DescOdor, DescSound };

    /**
     * @brief One translatable string surfaced by the Translation panel.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     */
    struct TranslationEntry {
        std::string      id;          ///< "<type>.<numericId>.<field>", e.g. "scene.1.desc.normal"
        TranslationGroup group;       ///< list grouping
        std::string      entityLabel; ///< the entity's current (default-language) name
        TranslationField field;       ///< which field this string is
    };

    /**
     * @brief Static helpers over a Core::Project's translatable strings.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * The string id scheme is `<type>.<numericId>.<field>` where `<type>` is one
     * of `scene` / `state` / `character` / `item`, `<numericId>` is the entity's
     * `Id<Tag>::value`, and `<field>` is `name` / `desc.normal` / `desc.long` /
     * `desc.odor` / `desc.sound`. An id is stable for an entity's lifetime.
     */
    class TranslationCatalog {
    public:
        /**
         * @brief List every translatable string in @p project, in display order.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Order: Scenes, then States, then Characters, then Items; within each
         * group by ascending entity id; within each entity Name, then the four
         * description slots.
         *
         * @param project The project to walk
         * @return std::vector<TranslationEntry> One entry per translatable field
         */
        static std::vector<TranslationEntry> enumerate(const Project& project);

        /**
         * @brief Read one translatable string in one language.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param project The project
         * @param id      A `<type>.<id>.<field>` string id
         * @param lang    BCP-47/POSIX language code (e.g. "es_ES")
         * @return std::string The stored text, or "" when unset / id not found.
         *         For a `name` id in the project's default language, falls back
         *         to the entity's structural name (`BaseData::getName()`) when
         *         no per-language override exists.
         */
        static std::string get(const Project& project, std::string_view id,
                               std::string_view lang);

        /**
         * @brief Write one translatable string in one language.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Empty @p text clears the entry. A `desc.*` id writes into the matching
         * `Data::DescriptionTexts` slot. A `name` id in the project's default
         * language updates the entity's structural name (`setName()`, so the
         * tree / `.ads` fallback follow); a `name` id in any other language
         * writes into the DataObject's per-language `NameTexts` map. Neither
         * `NameTexts` nor `DescriptionTexts` is stored in `.ads` — only in
         * `.trn`.
         *
         * @param project The project (mutated)
         * @param id      A `<type>.<id>.<field>` string id
         * @param lang    BCP-47/POSIX language code
         * @param text    New text ("" clears)
         */
        static void set(Project& project, std::string_view id,
                        std::string_view lang, std::string text);

        /**
         * @brief Build the `<type>.<id>.<field>` id for an entity field.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param group Entity group
         * @param numericId Entity `Id<Tag>::value`
         * @param field Field
         * @return std::string The composed id
         */
        static std::string makeId(TranslationGroup group, unsigned numericId,
                                  TranslationField field);
    };

} // namespace ADS::Core

#endif // ADS_CORE_TRANSLATION_CATALOG_H
