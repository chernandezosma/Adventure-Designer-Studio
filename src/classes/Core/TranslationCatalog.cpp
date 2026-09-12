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

#include "TranslationCatalog.h"

#include <array>
#include <charconv>
#include <string>

#include "Data/Descriptions.h"
#include "languages.h"

namespace ADS::Core {

    namespace {

        /**
         * @brief The four description slots, paired with their id token and a
         *        pointer to the matching Data::DescriptionTexts member.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         */
        struct DescSlot {
            const char* token;                     ///< id field token, e.g. "desc.normal"
            TranslationField field;                ///< enum counterpart
            Data::LocalizedText Data::DescriptionTexts::* member; ///< which slot
        };

        constexpr std::array<DescSlot, 4> kDescSlots{{
            {"desc.normal", TranslationField::DescNormal, &Data::DescriptionTexts::normal},
            {"desc.long",   TranslationField::DescLong,   &Data::DescriptionTexts::longText},
            {"desc.odor",   TranslationField::DescOdor,   &Data::DescriptionTexts::odor},
            {"desc.sound",  TranslationField::DescSound,  &Data::DescriptionTexts::sound},
        }};

        /**
         * @brief The four entity groups, paired with their id token.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         */
        struct GroupInfo {
            const char*      token; ///< id type token, e.g. "scene"
            TranslationGroup group; ///< enum counterpart
        };

        constexpr std::array<GroupInfo, 4> kGroups{{
            {"scene",     TranslationGroup::Scenes},
            {"state",     TranslationGroup::States},
            {"character", TranslationGroup::Characters},
            {"item",      TranslationGroup::Items},
        }};

        /**
         * @brief Split a string id into `type`, `numericId`, `field`.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param id     A `<type>.<num>.<field>` id
         * @param type   [out] the type token
         * @param num    [out] the numeric id
         * @param field  [out] the field token (may itself contain a dot, e.g. "desc.normal")
         * @return bool   true when @p id parsed cleanly
         */
        bool parseId(std::string_view id, std::string& type, unsigned& num, std::string& field)
        {
            const std::size_t firstDot = id.find('.');
            if (firstDot == std::string_view::npos) {
                return false;
            }
            const std::size_t secondDot = id.find('.', firstDot + 1);
            if (secondDot == std::string_view::npos) {
                return false;
            }

            type.assign(id.substr(0, firstDot));
            field.assign(id.substr(secondDot + 1));

            const std::string_view numView = id.substr(firstDot + 1, secondDot - firstDot - 1);
            unsigned value = 0;
            const auto* begin = numView.data();
            const auto* end = numView.data() + numView.size();
            const auto [ptr, ec] = std::from_chars(begin, end, value);
            if (ec != std::errc{} || ptr != end) {
                return false;
            }
            num = value;
            return true;
        }

        /**
         * @brief Language code for the project's default language.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param project The project
         * @return std::string BCP-47/POSIX code (e.g. "en_US")
         */
        std::string defaultLangCode(const Project& project)
        {
            return ADS::Constants::Languages::getLanguageCodeById(
                project.getGameData().getLanguages().defaultId);
        }

        /**
         * @brief Read a value from a LocalizedText map, or "" if absent.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param map  The map
         * @param lang Key to look up
         * @return std::string The value or an empty string
         */
        std::string mapGet(const Data::LocalizedText& map, std::string_view lang)
        {
            const auto it = map.find(std::string(lang));
            return it != map.end() ? it->second : std::string{};
        }

        /**
         * @brief Set or (on empty text) erase a key in a LocalizedText map.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param map  The map (mutated)
         * @param lang Key
         * @param text Value ("" erases)
         */
        void mapSet(Data::LocalizedText& map, std::string_view lang, std::string text)
        {
            if (text.empty()) {
                map.erase(std::string(lang));
            } else {
                map[std::string(lang)] = std::move(text);
            }
        }

        /**
         * @brief Visit the DataObject for a parsed id, dispatched by group.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @tparam Fn        Callable `void(auto& dataObject)`
         * @param  project   The project
         * @param  type      Type token from parseId()
         * @param  num       Numeric id from parseId()
         * @param  visit     Invoked with the matching DataObject, if found
         * @return bool      true when an entity matched
         */
        template <class Fn>
        bool withDataObject(Project& project, std::string_view type, unsigned num, Fn&& visit)
        {
            const auto pick = [&](const auto& collection) -> bool {
                for (const auto& ptr : collection) {
                    if (ptr->getId().value == num) {
                        visit(*ptr);
                        return true;
                    }
                }
                return false;
            };

            if (type == "scene")     return pick(project.getSceneData());
            if (type == "state")     return pick(project.getStateData());
            if (type == "character") return pick(project.getCharacterData());
            if (type == "item")      return pick(project.getItemData());
            return false;
        }

        /// const overload of withDataObject().
        template <class Fn>
        bool withDataObject(const Project& project, std::string_view type, unsigned num, Fn&& visit)
        {
            const auto pick = [&](const auto& collection) -> bool {
                for (const auto& ptr : collection) {
                    if (ptr->getId().value == num) {
                        visit(*ptr);
                        return true;
                    }
                }
                return false;
            };

            if (type == "scene")     return pick(project.getSceneData());
            if (type == "state")     return pick(project.getStateData());
            if (type == "character") return pick(project.getCharacterData());
            if (type == "item")      return pick(project.getItemData());
            return false;
        }

    } // namespace

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
    std::string TranslationCatalog::makeId(TranslationGroup group, unsigned numericId,
                                           TranslationField field)
    {
        const char* type = "scene";
        for (const auto& g : kGroups) {
            if (g.group == group) { type = g.token; break; }
        }

        std::string fieldToken = "name";
        if (field != TranslationField::Name) {
            for (const auto& s : kDescSlots) {
                if (s.field == field) { fieldToken = s.token; break; }
            }
        }

        return std::string(type) + "." + std::to_string(numericId) + "." + fieldToken;
    }

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
    std::vector<TranslationEntry> TranslationCatalog::enumerate(const Project& project)
    {
        std::vector<TranslationEntry> out;

        const auto addEntity = [&](TranslationGroup group, unsigned numericId,
                                   const std::string& label) {
            out.push_back({makeId(group, numericId, TranslationField::Name), group, label,
                           TranslationField::Name});
            for (const auto& slot : kDescSlots) {
                out.push_back({makeId(group, numericId, slot.field), group, label, slot.field});
            }
        };

        for (const auto& s : project.getSceneData()) {
            addEntity(TranslationGroup::Scenes, s->getId().value, s->getName());
        }
        for (const auto& s : project.getStateData()) {
            addEntity(TranslationGroup::States, s->getId().value, s->getName());
        }
        for (const auto& c : project.getCharacterData()) {
            addEntity(TranslationGroup::Characters, c->getId().value, c->getName());
        }
        for (const auto& i : project.getItemData()) {
            addEntity(TranslationGroup::Items, i->getId().value, i->getName());
        }
        return out;
    }

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
    std::string TranslationCatalog::get(const Project& project, std::string_view id,
                                        std::string_view lang)
    {
        std::string type;
        std::string field;
        unsigned num = 0;
        if (!parseId(id, type, num, field)) {
            return {};
        }

        const std::string defaultCode = defaultLangCode(project);
        std::string result;

        withDataObject(project, type, num, [&](const auto& obj) {
            if (field == "name") {
                const std::string override = mapGet(obj.getNameTexts(), lang);
                if (!override.empty()) {
                    result = override;
                } else if (lang == defaultCode) {
                    result = obj.getName();
                }
                return;
            }
            for (const auto& slot : kDescSlots) {
                if (field == slot.token) {
                    result = mapGet(obj.getDescriptionTexts().*(slot.member), lang);
                    return;
                }
            }
        });

        return result;
    }

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
    void TranslationCatalog::set(Project& project, std::string_view id,
                                 std::string_view lang, std::string text)
    {
        std::string type;
        std::string field;
        unsigned num = 0;
        if (!parseId(id, type, num, field)) {
            return;
        }

        const std::string defaultCode = defaultLangCode(project);

        withDataObject(project, type, num, [&](auto& obj) {
            if (field == "name") {
                if (lang == defaultCode) {
                    obj.setName(text);
                } else {
                    Data::LocalizedText names = obj.getNameTexts();
                    mapSet(names, lang, std::move(text));
                    obj.setNameTexts(names);
                }
                return;
            }
            for (const auto& slot : kDescSlots) {
                if (field == slot.token) {
                    Data::DescriptionTexts texts = obj.getDescriptionTexts();
                    mapSet(texts.*(slot.member), lang, std::move(text));
                    obj.setDescriptionTexts(texts);
                    return;
                }
            }
        });
    }

} // namespace ADS::Core
