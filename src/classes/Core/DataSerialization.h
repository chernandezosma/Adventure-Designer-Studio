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

#ifndef ADS_CORE_DATA_SERIALIZATION_H
#define ADS_CORE_DATA_SERIALIZATION_H

/**
 * @file DataSerialization.h
 * @brief nlohmann_json read/write helpers for the pure Data structs
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 *
 * Kept out of the Data headers so those stay free of any external
 * framework type — only ProjectSerializer.cpp and the serializer tests
 * include this. Every function works through the structs' public API.
 *
 * Convention: `applyJson(j, obj)` fills @p obj from JSON; `toJson(obj)` builds
 * the JSON. Object id and display name are written for a self-describing file
 * but are consumed by ProjectSerializer (it needs them before an entity
 * exists), so applyJson() for the BaseData-derived structs deliberately does
 * NOT touch id/name.
 */

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "Data/Affordance.h"
#include "Data/Capacities.h"
#include "Data/CharacterData.h"
#include "Data/Descriptions.h"
#include "Data/Effect.h"
#include "Data/GameData.h"
#include "Data/ItemData.h"
#include "Data/SceneData.h"
#include "Data/StateChainData.h"
#include "Data/StateData.h"
#include "Types/Color.h"
#include "Types/Id.h"

// ---------------------------------------------------------------------------
// ADS::Types::Id<Tag> — no default constructor, so a dedicated adl_serializer
// ---------------------------------------------------------------------------
namespace nlohmann {
    /// ADL customization point serializing an ADS::Types::Id<Tag> as its raw value.
    template<class Tag>
    struct adl_serializer<ADS::Types::Id<Tag>> {
        /// Write @p id's raw value.
        static void to_json(json& j, const ADS::Types::Id<Tag>& id) { j = id.value; }
        /// Read an Id<Tag> back from its raw value.
        static ADS::Types::Id<Tag> from_json(const json& j) {
            return ADS::Types::Id<Tag>(j.get<std::uint8_t>());
        }
    };
} // namespace nlohmann

namespace ADS::Types {

    /// Write @p c as a `[r, g, b, a]` JSON array.
    inline void to_json(nlohmann::json& j, const Color& c) {
        j = nlohmann::json::array({c.r, c.g, c.b, c.a});
    }

    /// Read @p c back from a `[r, g, b, a]` JSON array.
    inline void from_json(const nlohmann::json& j, Color& c) {
        c.r = j.at(0).get<float>();
        c.g = j.at(1).get<float>();
        c.b = j.at(2).get<float>();
        c.a = j.at(3).get<float>();
    }

} // namespace ADS::Types

namespace ADS::Data {

    // ----- small helpers for optional ids -------------------------------------

    /// Encode an optional typed id as its raw value, or JSON null when unset.
    template<class Tag>
    inline nlohmann::json optIdToJson(const std::optional<ADS::Types::Id<Tag>>& id) {
        return id ? nlohmann::json(id->value) : nlohmann::json(nullptr);
    }

    /// Decode an optional typed id from its raw value, or std::nullopt for JSON null.
    template<class Tag>
    inline std::optional<ADS::Types::Id<Tag>> optIdFromJson(const nlohmann::json& j) {
        if (j.is_null()) {
            return std::nullopt;
        }
        return ADS::Types::Id<Tag>(j.get<std::uint8_t>());
    }

    // ----- value structs (own ADL, symmetric) -------------------------------

    /// Write @p d's LexEngine text-id references.
    inline void to_json(nlohmann::json& j, const Descriptions& d) {
        j = nlohmann::json{{"normal", d.normal}, {"longText", d.longText}};
        j["odor"]  = d.odor  ? nlohmann::json(*d.odor)  : nlohmann::json(nullptr);
        j["sound"] = d.sound ? nlohmann::json(*d.sound) : nlohmann::json(nullptr);
    }

    /// Read @p d back, decoding the optional odor/sound slots from JSON null.
    inline void from_json(const nlohmann::json& j, Descriptions& d) {
        d.normal   = j.at("normal").get<std::uint32_t>();
        d.longText = j.at("longText").get<std::uint32_t>();
        d.odor  = j.at("odor").is_null()  ? std::nullopt : std::optional<std::uint32_t>(j.at("odor").get<std::uint32_t>());
        d.sound = j.at("sound").is_null() ? std::nullopt : std::optional<std::uint32_t>(j.at("sound").get<std::uint32_t>());
    }

    /// Write @p t's per-language draft description text.
    inline void to_json(nlohmann::json& j, const DescriptionTexts& t) {
        j = nlohmann::json{
            {"normal", t.normal}, {"longText", t.longText},
            {"odor", t.odor}, {"sound", t.sound}};
    }

    /// Read @p t back.
    inline void from_json(const nlohmann::json& j, DescriptionTexts& t) {
        t.normal   = j.at("normal").get<LocalizedText>();
        t.longText = j.at("longText").get<LocalizedText>();
        t.odor     = j.at("odor").get<LocalizedText>();
        t.sound    = j.at("sound").get<LocalizedText>();
    }

    /// Write @p e's damage/heal fields.
    inline void to_json(nlohmann::json& j, const Effect& e) {
        j = nlohmann::json{
            {"appliedTo", e.appliedTo}, {"base", e.base},
            {"critChance", e.critChance}, {"critMultiplier", e.critMultiplier},
            {"rate", e.rate}, {"unit", e.unit}};
    }

    /// Read @p e back.
    inline void from_json(const nlohmann::json& j, Effect& e) {
        e.appliedTo      = j.at("appliedTo").get<std::uint8_t>();
        e.base           = j.at("base").get<std::uint8_t>();
        e.critChance     = j.at("critChance").get<std::uint8_t>();
        e.critMultiplier = j.at("critMultiplier").get<std::uint8_t>();
        e.rate           = j.at("rate").get<std::uint8_t>();
        e.unit           = j.at("unit").get<std::uint8_t>();
    }

    /// Write @p c's load/life/stamina/sanity fields.
    inline void to_json(nlohmann::json& j, const Capacities& c) {
        j = nlohmann::json{
            {"load", c.load}, {"life", c.life},
            {"stamina", c.stamina}, {"sanity", c.sanity}};
    }

    /// Read @p c back.
    inline void from_json(const nlohmann::json& j, Capacities& c) {
        c.load    = j.at("load").get<std::uint8_t>();
        c.life    = j.at("life").get<std::uint8_t>();
        c.stamina = j.at("stamina").get<std::uint8_t>();
        c.sanity  = j.at("sanity").get<std::uint8_t>();
    }

    /// Write @p a's name, trigger list, and preset key.
    inline void to_json(nlohmann::json& j, const Affordance& a) {
        j = nlohmann::json{
            {"name", a.name}, {"trigger", a.triggers}, {"presetKey", a.presetKey}};
    }

    /// Read @p a back; presetKey defaults to "" for files written before it existed.
    inline void from_json(const nlohmann::json& j, Affordance& a) {
        a.name      = j.at("name").get<std::string>();
        a.triggers  = j.at("trigger").get<std::vector<std::string>>();
        a.presetKey = j.value("presetKey", std::string{});
    }

    /// Write @p a's name and email.
    inline void to_json(nlohmann::json& j, const GameAuthor& a) {
        j = nlohmann::json{{"name", a.name}, {"email", a.email}};
    }

    /// Read @p a back; missing fields default to "".
    inline void from_json(const nlohmann::json& j, GameAuthor& a) {
        a.name  = j.value("name", std::string{});
        a.email = j.value("email", std::string{});
    }

    /// Write @p l's default and supported language ids.
    inline void to_json(nlohmann::json& j, const GameLanguages& l) {
        j = nlohmann::json{{"default", l.defaultId}, {"supported", l.supportedIds}};
    }

    /// Read @p l back; missing fields default to {21} (en_US).
    inline void from_json(const nlohmann::json& j, GameLanguages& l) {
        l.defaultId    = j.value("default", static_cast<std::uint8_t>(21));
        l.supportedIds = j.value("supported", std::vector<std::uint8_t>{21});
    }

    /// Write @p g's project-wide game settings.
    inline void to_json(nlohmann::json& j, const GameData& g) {
        j = nlohmann::json{
            {"id", 0},
            {"title", g.getTitle()},
            {"synopsis", g.getSynopsis()},
            {"author", g.getAuthor()},
            {"version", {{"version", g.getVersion()}}},
            {"languages", g.getLanguages()},
            {"inventoryCapacity", g.getInventoryCapacity()}};
    }

    /// Read @p g back; every field defaults if absent (a fresh project has none yet).
    inline void from_json(const nlohmann::json& j, GameData& g) {
        g.setTitle(j.value("title", std::string{}));
        g.setSynopsis(j.value("synopsis", std::string{}));
        if (j.contains("author")) {
            g.setAuthor(j.at("author").get<GameAuthor>());
        }
        if (j.contains("version") && j.at("version").contains("version")) {
            g.setVersion(j.at("version").at("version").get<std::string>());
        }
        if (j.contains("languages")) {
            g.setLanguages(j.at("languages").get<GameLanguages>());
        }
        g.setInventoryCapacity(j.value("inventoryCapacity", static_cast<std::uint8_t>(20)));
    }

    // ----- SceneData::Exits -------------------------------------------------

    /// Encode @p e's ten directional exits, each an optional scene id.
    inline nlohmann::json toJson(const SceneData::Exits& e) {
        return nlohmann::json{
            {"north", optIdToJson(e.north)}, {"south", optIdToJson(e.south)},
            {"east", optIdToJson(e.east)}, {"west", optIdToJson(e.west)},
            {"northeast", optIdToJson(e.northeast)}, {"northwest", optIdToJson(e.northwest)},
            {"southeast", optIdToJson(e.southeast)}, {"southwest", optIdToJson(e.southwest)},
            {"up", optIdToJson(e.up)}, {"down", optIdToJson(e.down)}};
    }

    /// Decode a SceneData::Exits back from toJson()'s shape.
    inline SceneData::Exits exitsFromJson(const nlohmann::json& j) {
        SceneData::Exits e;
        e.north     = optIdFromJson<ADS::Types::SceneTag>(j.at("north"));
        e.south     = optIdFromJson<ADS::Types::SceneTag>(j.at("south"));
        e.east      = optIdFromJson<ADS::Types::SceneTag>(j.at("east"));
        e.west      = optIdFromJson<ADS::Types::SceneTag>(j.at("west"));
        e.northeast = optIdFromJson<ADS::Types::SceneTag>(j.at("northeast"));
        e.northwest = optIdFromJson<ADS::Types::SceneTag>(j.at("northwest"));
        e.southeast = optIdFromJson<ADS::Types::SceneTag>(j.at("southeast"));
        e.southwest = optIdFromJson<ADS::Types::SceneTag>(j.at("southwest"));
        e.up        = optIdFromJson<ADS::Types::SceneTag>(j.at("up"));
        e.down      = optIdFromJson<ADS::Types::SceneTag>(j.at("down"));
        return e;
    }

    // ----- trigger map (shared shape: Scene/Item/Character) -----------------

    /// Encode a trigger map as `{ "<eventId>": [handlerIds...] }`, key stringified.
    inline nlohmann::json triggersToJson(const std::map<std::uint8_t, std::vector<ADS::Types::EventId>>& triggers) {
        nlohmann::json j = nlohmann::json::object();
        for (const auto& [event, handlers] : triggers) {
            j[std::to_string(event)] = handlers;
        }
        return j;
    }

    /// Decode a trigger map back from triggersToJson()'s shape.
    inline std::map<std::uint8_t, std::vector<ADS::Types::EventId>> triggersFromJson(const nlohmann::json& j) {
        std::map<std::uint8_t, std::vector<ADS::Types::EventId>> triggers;
        for (auto it = j.begin(); it != j.end(); ++it) {
            triggers[static_cast<std::uint8_t>(std::stoi(it.key()))] =
                it.value().get<std::vector<ADS::Types::EventId>>();
        }
        return triggers;
    }

    // ----- per-entity DataObjects (getters/setters only) -------------------
    // id + name are emitted for readability but read back by ProjectSerializer;
    // applyJson() sets every OTHER field on an already-constructed object.

    /// Serialize @p s, including id/name for readability (ProjectSerializer reads those back).
    inline nlohmann::json toJson(const SceneData& s) {
        return nlohmann::json{
            {"id", s.getId().value},
            {"name", s.getName()},
            {"descriptions", s.getDescriptions()},
            {"image", s.getImage()},
            {"state", optIdToJson(s.getState())},
            {"exits", toJson(s.getExits())},
            {"presentItemIds", s.getPresentItemIds()},
            {"triggers", triggersToJson(s.getTriggers())},
            {"affordance", s.getAffordances()},
            {"isStartScene", s.isStartScene()}};
    }

    /// Apply every field but id/name onto an already-constructed @p s.
    inline void applyJson(const nlohmann::json& j, SceneData& s) {
        s.setDescriptions(j.at("descriptions").get<Descriptions>());
        s.setImage(j.at("image").get<std::string>());
        s.setState(optIdFromJson<ADS::Types::StateTag>(j.at("state")));
        s.setExits(exitsFromJson(j.at("exits")));
        s.setPresentItemIds(j.at("presentItemIds").get<std::vector<ADS::Types::ObjectId>>());
        s.setTriggers(triggersFromJson(j.at("triggers")));
        s.setAffordances(j.at("affordance").get<std::vector<Affordance>>());
        s.setStartScene(j.at("isStartScene").get<bool>());
    }

    /// Serialize @p c, including id/name for readability (ProjectSerializer reads those back).
    inline nlohmann::json toJson(const CharacterData& c) {
        return nlohmann::json{
            {"id", c.getId().value},
            {"name", c.getName()},
            {"descriptions", c.getDescriptions()},
            {"isPlayer", c.isPlayer()},
            {"capacities", c.getCapacities()},
            {"state", optIdToJson(c.getState())},
            {"dialogColor", c.getDialogColor()},
            {"imagePath", c.getImagePath()},
            {"avatarPath", c.getAvatarPath()},
            {"initialSceneId", optIdToJson(c.getInitialSceneId())},
            {"affordance", c.getAffordances()},
            {"triggers", triggersToJson(c.getTriggers())}};
    }

    /// Apply every field but id/name onto an already-constructed @p c.
    inline void applyJson(const nlohmann::json& j, CharacterData& c) {
        c.setDescriptions(j.at("descriptions").get<Descriptions>());
        c.setPlayer(j.at("isPlayer").get<bool>());
        c.setCapacities(j.at("capacities").get<Capacities>());
        c.setState(optIdFromJson<ADS::Types::StateTag>(j.at("state")));
        c.setDialogColor(j.at("dialogColor").get<ADS::Types::Color>());
        c.setImagePath(j.at("imagePath").get<std::string>());
        c.setAvatarPath(j.at("avatarPath").get<std::string>());
        c.setInitialSceneId(optIdFromJson<ADS::Types::SceneTag>(j.at("initialSceneId")));
        c.setAffordances(j.at("affordance").get<std::vector<Affordance>>());
        c.setTriggers(triggersFromJson(j.at("triggers")));
    }

    /// Serialize @p i, including id/name for readability (ProjectSerializer reads those back).
    inline nlohmann::json toJson(const ItemData& i) {
        return nlohmann::json{
            {"id", i.getId().value},
            {"name", i.getName()},
            {"descriptions", i.getDescriptions()},
            {"itemType", i.getItemType()},
            {"affordance", i.getAffordances()},
            {"isContainer", i.isContainer()},
            {"state", optIdToJson(i.getState())},
            {"weight", i.getWeight()},
            {"slots", i.getSlots()},
            {"serviceLife", i.getServiceLife()},
            {"imagePath", i.getImagePath()},
            {"containerItemIds", i.getContainerItemIds()},
            {"combinableWithIds", i.getCombinableWithIds()},
            {"synonyms", i.getSynonyms()},
            {"abbreviatures", i.getAbbreviatures()},
            {"damage", i.getDamageEffect()},
            {"heal", i.getHealEffect()},
            {"initialSceneId", optIdToJson(i.getInitialSceneId())},
            {"triggers", triggersToJson(i.getTriggers())}};
    }

    /// Apply every field but id/name onto an already-constructed @p i.
    inline void applyJson(const nlohmann::json& j, ItemData& i) {
        i.setDescriptions(j.at("descriptions").get<Descriptions>());
        i.setItemType(j.at("itemType").get<int>());
        i.setAffordances(j.at("affordance").get<std::vector<Affordance>>());
        i.setContainer(j.at("isContainer").get<bool>());
        i.setState(optIdFromJson<ADS::Types::StateTag>(j.at("state")));
        i.setWeight(j.at("weight").get<std::uint8_t>());
        i.setSlots(j.at("slots").get<std::uint8_t>());
        i.setServiceLife(j.at("serviceLife").get<std::uint8_t>());
        i.setImagePath(j.at("imagePath").get<std::string>());
        i.setContainerItemIds(j.at("containerItemIds").get<std::vector<ADS::Types::ObjectId>>());
        i.setCombinableWithIds(j.at("combinableWithIds").get<std::vector<ADS::Types::ObjectId>>());
        i.setSynonyms(j.at("synonyms").get<std::vector<std::string>>());
        i.setAbbreviatures(j.at("abbreviatures").get<std::vector<std::string>>());
        i.setDamageEffect(j.at("damage").get<Effect>());
        i.setHealEffect(j.at("heal").get<Effect>());
        i.setInitialSceneId(optIdFromJson<ADS::Types::SceneTag>(j.at("initialSceneId")));
        i.setTriggers(triggersFromJson(j.at("triggers")));
    }

    /// Serialize @p s, including id/name for readability (ProjectSerializer reads those back).
    inline nlohmann::json toJson(const StateData& s) {
        return nlohmann::json{
            {"id", s.getId().value},
            {"name", s.getName()},
            {"descriptions", s.getDescriptions()},
            {"next", optIdToJson(s.getNext())}};
    }

    /// Apply every field but id/name onto an already-constructed @p s.
    inline void applyJson(const nlohmann::json& j, StateData& s) {
        s.setDescriptions(j.at("descriptions").get<Descriptions>());
        s.setNext(optIdFromJson<ADS::Types::StateTag>(j.at("next")));
    }

    /// Serialize @p c, including id/name for readability (ProjectSerializer reads those back).
    inline nlohmann::json toJson(const StateChainData& c) {
        return nlohmann::json{
            {"id", c.getId().value},
            {"name", c.getName()},
            {"head", optIdToJson(c.getHead())}};
    }

    /// Apply every field but id/name onto an already-constructed @p c.
    inline void applyJson(const nlohmann::json& j, StateChainData& c) {
        c.setHead(optIdFromJson<ADS::Types::StateTag>(j.at("head")));
    }

} // namespace ADS::Data

#endif // ADS_CORE_DATA_SERIALIZATION_H
