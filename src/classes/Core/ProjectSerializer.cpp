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

#include "ProjectSerializer.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>

#include <nlohmann/json.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <spdlog/spdlog.h>

#include "DataSerialization.h"
#include "PathService.h"
#include "TranslationSerializer.h"
#include "LexEngine/LexEngineSerializer.h"
#include "../../exceptions/core/project_serialization_exception.h"
#include "../../exceptions/filesystem/file_not_found_exception.h"

namespace ADS::Core {

    using nlohmann::json;
    using Exceptions::project_serialization_exception;

    namespace {

        /**
         * @brief Serialise a DataObject collection to a JSON array.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @tparam DataVec  A range of `std::unique_ptr<T>` DataObjects
         * @tparam ToJsonFn Callable `json(const T&)`
         * @param  items    The owned DataObject collection
         * @param  toJson   Per-element serialiser
         * @return json      A JSON array, one element per DataObject
         */
        template<class DataVec, class ToJsonFn>
        json dumpCollection(const DataVec& items, ToJsonFn toJson)
        {
            json arr = json::array();
            for (const auto& item : items) {
                arr.push_back(toJson(*item));
            }
            return arr;
        }

        /**
         * @brief Compute the SHA-1 digest of @p data as a 40-char lowercase hex string.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Used only for accidental-modification / corruption detection of the
         * `.ads` file — not a security boundary, so SHA-1 (already available
         * via boost-uuid, no extra dependency) is sufficient.
         *
         * @param data Bytes to hash
         * @return std::string 40 lowercase hex characters
         */
        std::string sha1Hex(std::string_view data)
        {
            boost::uuids::detail::sha1 sha;
            sha.process_bytes(data.data(), data.size());

            boost::uuids::detail::sha1::digest_type digest{};
            sha.get_digest(digest);

            static constexpr char kHex[] = "0123456789abcdef";
            std::string out;
            out.reserve(sizeof(digest) * 2);
            for (unsigned char byte : digest) {
                out.push_back(kHex[byte >> 4]);
                out.push_back(kHex[byte & 0x0F]);
            }
            return out;
        }

    } // namespace

    /**
     * @brief Write @p project to @p path as pretty-printed JSON.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * @param project Project to serialise
     * @param path    Destination file (overwritten if it exists)
     *
     * @throws Exceptions::project_serialization_exception if the file cannot
     *         be opened for writing
     */
    void ProjectSerializer::save(const Project& project, const std::filesystem::path& path)
    {
        json doc;
        doc["format"] = "ads-project";
        doc["schemaVersion"] = kSchemaVersion;
        doc["game"] = project.getGameData();

        doc["scenes"] = dumpCollection(project.getSceneData(),
            [](const Data::SceneData& s) { return Data::toJson(s); });
        doc["characters"] = dumpCollection(project.getCharacterData(),
            [](const Data::CharacterData& c) { return Data::toJson(c); });
        doc["items"] = dumpCollection(project.getItemData(),
            [](const Data::ItemData& i) { return Data::toJson(i); });
        doc["states"] = dumpCollection(project.getStateData(),
            [](const Data::StateData& s) { return Data::toJson(s); });
        doc["chains"] = dumpCollection(project.getChainData(),
            [](const Data::StateChainData& c) { return Data::toJson(c); });

        doc["lexEngine"] = LexEngine::LexEngineSerializer::toJson(project.getLexEngine());

        // Integrity checksum over the canonical (compact, key-sorted — nlohmann::json
        // stores objects in a std::map) form of everything above. Added last so it
        // never covers itself; verified on load to catch accidental edits.
        doc["checksum"] = sha1Hex(doc.dump());

        // Create the per-project folder (<projects-root>/<slug>/) on first save.
        if (path.has_parent_path()) {
            std::error_code ec;
            std::filesystem::create_directories(path.parent_path(), ec);
        }

        // Binary mode: no CRLF translation, so the file is byte-identical on every
        // platform (matters for round-trip checks, diffs and the checksum).
        std::ofstream out(path, std::ios::trunc | std::ios::binary);
        if (!out.is_open()) {
            throw project_serialization_exception(
                "cannot open '" + PathService::toUtf8(path) + "' for writing");
        }
        out << doc.dump(2) << '\n';

        // Sibling <project>.trn — the standalone per-language translation store.
        // Its own format (no .ads checksum coverage); a write failure only warns.
        TranslationSerializer::saveToFile(
            project, std::filesystem::path(path).replace_extension(".trn"));
    }

    /**
     * @brief Rebuild a Project from the `.ads` file at @p path.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * The new Project is fully built locally and only returned on success —
     * a throw leaves any project the caller currently holds untouched.
     *
     * @param path Source `.ads` file
     * @return std::unique_ptr<Project> The rebuilt project
     *
     * @throws Exceptions::file_not_found_exception       if @p path does not exist
     * @throws Exceptions::project_serialization_exception on malformed JSON,
     *         an unsupported schema version, a missing required key, or a
     *         duplicate / dangling id
     */
    std::unique_ptr<Project> ProjectSerializer::load(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path)) {
            throw Exceptions::file_not_found_exception("File not found: " + PathService::toUtf8(path));
        }

        // Binary mode so a LF-only file read on Windows is not silently altered.
        std::ifstream in(path, std::ios::binary);
        if (!in.is_open()) {
            throw project_serialization_exception("cannot open '" + PathService::toUtf8(path) + "' for reading");
        }

        json doc;
        try {
            in >> doc;
        } catch (const json::exception& e) {
            throw project_serialization_exception(std::string("malformed JSON — ") + e.what());
        }

        try {
            const int version = doc.at("schemaVersion").get<int>();
            if (version != kSchemaVersion) {
                throw project_serialization_exception(
                    "unsupported schema version " + std::to_string(version));
            }

            // Verify the integrity checksum: strip it, recompute over the same
            // canonical form save() hashed, compare. A mismatch means the file
            // was edited outside the editor.
            const std::string storedChecksum = doc.value("checksum", std::string{});
            doc.erase("checksum");
            if (storedChecksum.empty()) {
                spdlog::warn("ProjectSerializer: '{}' has no checksum — integrity not verified",
                             PathService::toUtf8(path));
            } else if (sha1Hex(doc.dump()) != storedChecksum) {
                throw project_serialization_exception(
                    "checksum mismatch — '" + PathService::toUtf8(path) +
                    "' was modified outside the editor");
            }

            auto project = std::make_unique<Project>(std::string{});

            if (doc.contains("game")) {
                doc.at("game").get_to(project->getGameData());
            }

            for (const auto& js : doc.at("scenes")) {
                const auto id = ADS::Types::SceneId(js.at("id").get<std::uint8_t>());
                if (!project->addScene(id, js.at("name").get<std::string>())) {
                    throw project_serialization_exception(
                        "duplicate scene id " + std::to_string(id.value));
                }
                Data::applyJson(js, *project->getSceneData().back());
            }

            for (const auto& jc : doc.at("characters")) {
                const auto id = ADS::Types::CharacterId(jc.at("id").get<std::uint8_t>());
                if (!project->addCharacter(id, jc.at("name").get<std::string>())) {
                    throw project_serialization_exception(
                        "duplicate character id " + std::to_string(id.value));
                }
                Data::applyJson(jc, *project->getCharacterData().back());
            }

            for (const auto& ji : doc.at("items")) {
                const auto id = ADS::Types::ObjectId(ji.at("id").get<std::uint8_t>());
                if (!project->addItem(id, ji.at("name").get<std::string>())) {
                    throw project_serialization_exception(
                        "duplicate item id " + std::to_string(id.value));
                }
                Data::applyJson(ji, *project->getItemData().back());
            }

            for (const auto& js : doc.at("states")) {
                const auto id = ADS::Types::StateId(js.at("id").get<std::uint8_t>());
                if (!project->addState(id, js.at("name").get<std::string>())) {
                    throw project_serialization_exception(
                        "duplicate state id " + std::to_string(id.value));
                }
                Data::applyJson(js, *project->getStateData().back());
            }

            for (const auto& jc : doc.at("chains")) {
                const auto id = ADS::Types::ChainId(jc.at("id").get<std::uint8_t>());
                if (!project->addChain(id, jc.at("name").get<std::string>())) {
                    throw project_serialization_exception(
                        "duplicate chain id " + std::to_string(id.value));
                }
                Data::applyJson(jc, *project->getChainData().back());
            }

            if (doc.contains("lexEngine")) {
                LexEngine::LexEngineSerializer::fromJson(project->getLexEngine(), doc.at("lexEngine"));
            }

            // Sibling <project>.trn — absent is fine (fresh project); a bad file
            // only warns and leaves translations empty.
            TranslationSerializer::loadFromFile(
                *project, std::filesystem::path(path).replace_extension(".trn"));

            return project;
        } catch (const project_serialization_exception&) {
            throw;
        } catch (const json::exception& e) {
            throw project_serialization_exception(
                std::string("could not rebuild project — ") + e.what());
        }
    }

} // namespace ADS::Core
