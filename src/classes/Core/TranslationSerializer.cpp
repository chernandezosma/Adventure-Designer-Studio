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

#include "TranslationSerializer.h"

#include <fstream>
#include <system_error>

#include <spdlog/spdlog.h>

#include "PathService.h"
#include "TranslationCatalog.h"
#include "languages.h"

namespace ADS::Core {

    namespace {
        /// Top-level JSON key for the per-language translation maps.
        constexpr const char* kLanguagesKey = "languages";
    } // namespace

    /**
     * @brief Gather every translatable string in @p project into a set.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Walks Core::TranslationCatalog::enumerate() for every project language
     * (`game.languages.supported`); a string with no text in a language is
     * omitted from that language's map.
     *
     * @param project The project to read
     * @return TranslationSet langCode -> (id -> text)
     */
    TranslationSet TranslationSerializer::collect(const Project& project)
    {
        TranslationSet set;
        const std::vector<TranslationEntry> entries = TranslationCatalog::enumerate(project);

        for (const std::uint8_t langId : project.getGameData().getLanguages().supportedIds) {
            const std::string code = ADS::Constants::Languages::getLanguageCodeById(langId);
            if (code.empty()) {
                continue;
            }
            for (const TranslationEntry& entry : entries) {
                std::string text = TranslationCatalog::get(project, entry.id, code);
                if (!text.empty()) {
                    set[code][entry.id] = std::move(text);
                }
            }
        }
        return set;
    }

    /**
     * @brief Push a set's values back onto @p project's DataObjects.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param project The project to mutate
     * @param set     langCode -> (id -> text)
     */
    void TranslationSerializer::apply(Project& project, const TranslationSet& set)
    {
        for (const auto& [code, byId] : set) {
            for (const auto& [id, text] : byId) {
                TranslationCatalog::set(project, id, code, text);
            }
        }
    }

    /**
     * @brief Serialise a set to the `.trn` JSON document shape.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param set The set
     * @return nlohmann::json `{ "format", "schemaVersion", "languages": { code: { id: text } } }`
     */
    nlohmann::json TranslationSerializer::toJson(const TranslationSet& set)
    {
        nlohmann::json languages = nlohmann::json::object();
        for (const auto& [code, byId] : set) {
            languages[code] = byId; // std::map<string,string> -> JSON object
        }

        return nlohmann::json{
            {"format", "ads-translations"},
            {"schemaVersion", kSchemaVersion},
            {kLanguagesKey, std::move(languages)},
        };
    }

    /**
     * @brief Parse a `.trn` JSON document into a set.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param doc The parsed JSON
     * @return TranslationSet The `languages` map, or empty on a version mismatch
     */
    TranslationSet TranslationSerializer::fromJson(const nlohmann::json& doc)
    {
        TranslationSet set;

        const int version = doc.value("schemaVersion", 0);
        if (version != kSchemaVersion) {
            spdlog::warn("TranslationSerializer: unsupported .trn schemaVersion {} (expected {})",
                         version, kSchemaVersion);
            return set;
        }

        const auto langsIt = doc.find(kLanguagesKey);
        if (langsIt == doc.end() || !langsIt->is_object()) {
            return set;
        }
        for (auto langIt = langsIt->begin(); langIt != langsIt->end(); ++langIt) {
            if (!langIt->is_object()) {
                continue;
            }
            std::map<std::string, std::string> byId;
            for (auto idIt = langIt->begin(); idIt != langIt->end(); ++idIt) {
                if (idIt->is_string()) {
                    byId[idIt.key()] = idIt->get<std::string>();
                }
            }
            if (!byId.empty()) {
                set[langIt.key()] = std::move(byId);
            }
        }
        return set;
    }

    /**
     * @brief Write @p project's translations to @p trnPath.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Creates the parent directory. Writes in binary mode (LF endings). If
     * @p project has no translatable text at all, still writes a valid empty
     * document so the file always mirrors the project.
     *
     * @param project The project
     * @param trnPath  Destination `.trn` path
     * @return bool     true on success
     */
    bool TranslationSerializer::saveToFile(const Project& project, const std::filesystem::path& trnPath)
    {
        if (trnPath.has_parent_path()) {
            std::error_code ec;
            std::filesystem::create_directories(trnPath.parent_path(), ec);
        }

        std::ofstream out(trnPath, std::ios::trunc | std::ios::binary);
        if (!out.is_open()) {
            spdlog::error("TranslationSerializer: cannot open '{}' for writing", PathService::toUtf8(trnPath));
            return false;
        }
        out << toJson(collect(project)).dump(2) << '\n';
        return true;
    }

    /**
     * @brief Load translations from @p trnPath into @p project.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * A missing file is not an error (a fresh project has no `.trn` yet) —
     * returns `true` and leaves @p project untouched. Malformed JSON or a
     * schema-version mismatch logs a warning and returns `false`.
     *
     * @param project The project to mutate
     * @param trnPath  Source `.trn` path
     * @return bool     true when the file was absent or loaded cleanly
     */
    bool TranslationSerializer::loadFromFile(Project& project, const std::filesystem::path& trnPath)
    {
        std::error_code ec;
        if (!std::filesystem::exists(trnPath, ec)) {
            return true; // a fresh project simply has no .trn yet
        }

        std::ifstream in(trnPath, std::ios::binary);
        if (!in.is_open()) {
            spdlog::error("TranslationSerializer: cannot open '{}' for reading", PathService::toUtf8(trnPath));
            return false;
        }

        nlohmann::json doc;
        try {
            in >> doc;
        } catch (const nlohmann::json::exception& e) {
            spdlog::warn("TranslationSerializer: malformed '{}' — {}", PathService::toUtf8(trnPath), e.what());
            return false;
        }

        apply(project, fromJson(doc));
        return true;
    }

} // namespace ADS::Core
