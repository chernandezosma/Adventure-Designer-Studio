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
 * @file LexiconSerializer.cpp
 * @brief Implementation of LexiconSerializer
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 */

#include "LexiconSerializer.h"

#include <unordered_map>
#include <utility>

namespace ADS::Lexicon {

    nlohmann::json LexiconSerializer::toJson(const LexEntry& entry) {
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

    LexEntry LexiconSerializer::fromJson(const nlohmann::json& json) {
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

} // namespace ADS::Lexicon
