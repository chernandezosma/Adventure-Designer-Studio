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

/**
 * @file LexEngineSerializer.cpp
 * @brief Implementation of LexEngineSerializer
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "LexEngineSerializer.h"

#include <fstream>
#include <unordered_map>
#include <utility>

#include "spdlog/spdlog.h"

#include "Core/PathService.h"

namespace ADS::LexEngine {

    namespace {
        constexpr int kCurrentVersion = 1;
    } // namespace

    /**
     * @brief Serialise one LexEntry to its canonical JSON representation
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param entry Entry to serialise
     * @return nlohmann::json Object with id, lang, canonical, role, types,
     *         synonyms, frequency, raw_count, and synonym_meta
     */
    nlohmann::json LexEngineSerializer::toJson(const LexEntry& entry) {
        nlohmann::json j;
        j["id"]        = entry.id;
        j["lang"]      = entry.lang;
        j["canonical"] = entry.canonical;
        j["role"]      = static_cast<uint8_t>(entry.role);
        j["types"]     = entry.types();
        j["frequency"] = entry.frequency();
        j["raw_count"] = entry.rawCount();

        nlohmann::json synonyms      = nlohmann::json::array();
        nlohmann::json synonymMeta   = nlohmann::json::array();
        for (const SynonymLink& link : entry.synonyms) {
            synonyms.push_back(link.target);
            synonymMeta.push_back({
                {"target",     link.target},
                {"confidence", link.confidence},
                {"confirmed",  link.confirmed},
            });
        }
        j["synonyms"]     = std::move(synonyms);
        j["synonym_meta"] = std::move(synonymMeta);

        return j;
    }

    /**
     * @brief Deserialise one LexEntry from its canonical JSON representation
     *
     * An absent synonym_meta block is treated per the documented default:
     * every id listed in synonyms is restored as
     * {confidence: 1.0, confirmed: true}.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param json Canonical JSON object previously produced by toJson()
     * @return LexEntry Reconstructed entry — compiled_token is
     *         Token::UNASSIGNED until LexEngine::index() runs again
     */
    LexEntry LexEngineSerializer::fromJson(const nlohmann::json& json) {
        LexEntry entry;
        entry.id        = json.at("id").get<LexEntryId>();
        entry.lang       = json.at("lang").get<LanguageCode>();
        entry.canonical  = json.at("canonical").get<std::string>();
        entry.role       = static_cast<EntryRole>(json.at("role").get<uint8_t>());

        entry.restoreTypes(json.at("types").get<WordType>());
        entry.restoreCounts(json.at("raw_count").get<uint32_t>(), json.at("frequency").get<float>());

        std::unordered_map<LexEntryId, std::pair<float, bool>> metaByTarget;
        if (const auto metaIt = json.find("synonym_meta"); metaIt != json.end()) {
            for (const auto& meta : *metaIt) {
                metaByTarget.emplace(meta.at("target").get<LexEntryId>(),
                                      std::make_pair(meta.at("confidence").get<float>(),
                                                      meta.at("confirmed").get<bool>()));
            }
        }

        if (const auto synIt = json.find("synonyms"); synIt != json.end()) {
            for (const auto& targetJson : *synIt) {
                const auto target = targetJson.get<LexEntryId>();
                SynonymLink link;
                link.target = target;
                if (const auto metaEntry = metaByTarget.find(target); metaEntry != metaByTarget.end()) {
                    link.confidence = metaEntry->second.first;
                    link.confirmed  = metaEntry->second.second;
                } else {
                    // Absent synonym_meta: treat as manually confirmed, per the documented default.
                    link.confidence = 1.0f;
                    link.confirmed  = true;
                }
                entry.synonyms.push_back(link);
            }
        }

        return entry;
    }

    /**
     * @brief Serialise an entire LexEngine (every language) to one JSON document
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param engine Engine to serialise
     * @return nlohmann::json Object with "version" (currently 1) and a flat
     *         "entries" array covering every language — each element is the
     *         same shape produced by toJson(const LexEntry&)
     */
    nlohmann::json LexEngineSerializer::toJson(const LexEngine& engine) {
        nlohmann::json entries = nlohmann::json::array();

        for (const LanguageCode& lang : engine.getLanguages()) {
            for (const std::unique_ptr<LexEntry>& entry : engine.getEntries(lang)) {
                entries.push_back(toJson(*entry));
            }
        }

        nlohmann::json document;
        document["version"] = kCurrentVersion;
        document["entries"] = std::move(entries);
        return document;
    }

    /**
     * @brief Restore every entry from a whole-engine JSON document into engine
     *
     * Reconstructs each entry via fromJson() and inserts it through
     * LexEngine::restoreEntry(), preserving ids. Only "version": 1
     * documents are currently supported.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param engine Engine to populate — existing entries are left untouched
     * @param document Document previously produced by toJson(const LexEngine&)
     */
    void LexEngineSerializer::fromJson(LexEngine& engine, const nlohmann::json& document) {
        if (document.at("version").get<int>() != kCurrentVersion) {
            spdlog::warn("LexEngineSerializer: unsupported document version {}, expected {}",
                         document.at("version").get<int>(), kCurrentVersion);
            return;
        }

        for (const auto& entryJson : document.at("entries")) {
            LexEntry entry = fromJson(entryJson);
            const LanguageCode lang = entry.lang;
            engine.restoreEntry(std::move(entry), lang);
        }
    }

    /**
     * @brief Write an engine's whole vocabulary to a JSON file on disk
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param engine Engine to serialise
     * @param path Destination file path — overwritten if it already exists
     * @return bool True on success, false if the file could not be written
     */
    bool LexEngineSerializer::saveToFile(const LexEngine& engine, const std::filesystem::path& path) {
        try {
            std::ofstream file(path, std::ios::trunc);
            if (!file.is_open()) {
                spdlog::error("LexEngineSerializer: failed to open '{}' for writing", Core::PathService::toUtf8(path));
                return false;
            }

            file << toJson(engine).dump(2) << std::endl;
            file.close();

            return true;
        } catch (const std::ios_base::failure& e) {
            spdlog::error("LexEngineSerializer: failed to write '{}': {}", Core::PathService::toUtf8(path), e.what());
            return false;
        }
    }

    /**
     * @brief Load a whole vocabulary from a JSON file on disk into engine
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * @param engine Engine to populate
     * @param path Source file path
     * @return bool True on success, false if the file is missing, unreadable, or malformed
     */
    bool LexEngineSerializer::loadFromFile(LexEngine& engine, const std::filesystem::path& path) {
        if (!std::filesystem::exists(path)) {
            spdlog::error("LexEngineSerializer: file '{}' does not exist", Core::PathService::toUtf8(path));
            return false;
        }

        try {
            std::ifstream file(path);
            const std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            const nlohmann::json document = nlohmann::json::parse(content);
            fromJson(engine, document);

            return true;
        } catch (const nlohmann::json::exception& e) {
            spdlog::error("LexEngineSerializer: malformed JSON in '{}': {}", Core::PathService::toUtf8(path), e.what());
            return false;
        } catch (const std::ios_base::failure& e) {
            spdlog::error("LexEngineSerializer: failed to read '{}': {}", Core::PathService::toUtf8(path), e.what());
            return false;
        }
    }

} // namespace ADS::LexEngine
