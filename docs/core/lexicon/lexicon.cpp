#pragma once

// =============================================================================
// ads/lexicon/types.hpp
//
// Base types for the ADS Lexicon system.
//
// NLP backend: UDPipe 1.x (C++ native)
//   udpipe::word  →  NLPToken  (our view of a UDPipe-analysed word)
//   udpipe::model →  INLPBackend implementation (UDPipeBackend)
//
// The Lexicon never depends on udpipe.h directly — it consumes NLPToken,
// which is populated by the backend adapter. This keeps UDPipe swappable.
// =============================================================================

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ads::lexicon {

// -----------------------------------------------------------------------------
// LanguageCode  —  BCP-47 with region: "es_ES", "en_US", "de_DE"
// -----------------------------------------------------------------------------

using LanguageCode = std::string;

namespace lang {
    inline constexpr std::string_view ES_ES = "es_ES";
    inline constexpr std::string_view EN_US = "en_US";
    inline constexpr std::string_view EN_GB = "en_GB";
    inline constexpr std::string_view DE_DE = "de_DE";
    inline constexpr std::string_view FR_FR = "fr_FR";
    inline constexpr std::string_view PT_PT = "pt_PT";
    inline constexpr std::string_view PT_BR = "pt_BR";
}

[[nodiscard]] inline std::string base_lang(std::string_view code) noexcept {
    if (auto pos = code.find('_'); pos != std::string_view::npos)
        return std::string(code.substr(0, pos));
    return std::string(code);
}

// -----------------------------------------------------------------------------
// TokenIndex  —  compiled 8-bit target token
//
//   0x0000              reserved: EOL (end of message)
//   0x0001 – 0x00FE     1-byte token  (top-254 by frequency)
//   0x00FF              reserved: escape byte
//   0x0100 – 0xFFFE     3-byte token  [0xFF][hi][lo]
//   0xFFFF              sentinel: UNASSIGNED
// -----------------------------------------------------------------------------

using TokenIndex = uint16_t;

namespace token {
    inline constexpr TokenIndex EOL             = 0x0000;
    inline constexpr TokenIndex ESCAPE          = 0x00FF;
    inline constexpr TokenIndex UNASSIGNED      = 0xFFFF;
    inline constexpr TokenIndex SINGLE_BYTE_MIN = 0x0001;
    inline constexpr TokenIndex SINGLE_BYTE_MAX = 0x00FE;
    inline constexpr TokenIndex EXTENDED_MIN    = 0x0100;
    inline constexpr TokenIndex EXTENDED_MAX    = 0xFFFE;

    [[nodiscard]] constexpr bool is_single_byte(TokenIndex t) noexcept {
        return t >= SINGLE_BYTE_MIN && t <= SINGLE_BYTE_MAX;
    }
    [[nodiscard]] constexpr bool is_extended(TokenIndex t) noexcept {
        return t >= EXTENDED_MIN && t <= EXTENDED_MAX;
    }
    [[nodiscard]] constexpr uint8_t encoded_size(TokenIndex t) noexcept {
        if (t == EOL)          return 1;
        if (is_single_byte(t)) return 1;
        if (is_extended(t))    return 3;
        return 0;
    }
}

// -----------------------------------------------------------------------------
// WordType  —  bitmask over uint16_t
//
// Each bit represents one grammatical category independently.
// A single LexEntry may carry multiple bits simultaneously, reflecting
// genuine lexical ambiguity observed in the corpus ("abierto": Verb|Adjective).
//
// Bit layout (11 categories used, 5 spare for future extension):
//
//   bit  0  —  Verb          UD: VERB, AUX
//   bit  1  —  Noun          UD: NOUN, PROPN
//   bit  2  —  Adjective     UD: ADJ
//   bit  3  —  Adverb        UD: ADV
//   bit  4  —  Preposition   UD: ADP
//   bit  5  —  Article       UD: DET
//   bit  6  —  Conjunction   UD: CCONJ, SCONJ
//   bit  7  —  Pronoun       UD: PRON
//   bit  8  —  Interjection  UD: INTJ
//   bit  9  —  Punctuation   UD: PUNCT  (filtered — never inserted in Lexicon)
//   bit 10  —  Other         UD: NUM, SYM, X, PART
//   bits 11-15  spare
//
// BIT_COUNT is the number of defined bit positions (including spare).
// TYPE_COUNT is the number of meaningful categories (excluding spare).
// -----------------------------------------------------------------------------

using WordType = uint16_t;

namespace word_type {
    inline constexpr WordType None         = 0x0000;
    inline constexpr WordType Verb         = 1 << 0;
    inline constexpr WordType Noun         = 1 << 1;
    inline constexpr WordType Adjective    = 1 << 2;
    inline constexpr WordType Adverb       = 1 << 3;
    inline constexpr WordType Preposition  = 1 << 4;
    inline constexpr WordType Article      = 1 << 5;
    inline constexpr WordType Conjunction  = 1 << 6;
    inline constexpr WordType Pronoun      = 1 << 7;
    inline constexpr WordType Interjection = 1 << 8;
    inline constexpr WordType Punctuation  = 1 << 9;
    inline constexpr WordType Other        = 1 << 10;

    // Total number of defined bit positions (used to size type_counts array).
    inline constexpr std::size_t BIT_COUNT = 11;

    // String representation indexed by bit position (0–10).
    // Index must match the bit positions above.
    inline constexpr std::array<std::string_view, BIT_COUNT> NAMES = {
        "verb",         // bit  0
        "noun",         // bit  1
        "adjective",    // bit  2
        "adverb",       // bit  3
        "preposition",  // bit  4
        "article",      // bit  5
        "conjunction",  // bit  6
        "pronoun",      // bit  7
        "interjection", // bit  8
        "punctuation",  // bit  9
        "other",        // bit 10
    };

    // Returns the bit position of a single-bit WordType value.
    // Undefined behaviour if wt is not a power of two.
    [[nodiscard]] constexpr std::size_t bit_pos(WordType wt) noexcept {
        std::size_t pos = 0;
        while (wt > 1) { wt >>= 1; ++pos; }
        return pos;
    }

    // Human-readable name for a single-bit WordType.
    [[nodiscard]] constexpr std::string_view name(WordType wt) noexcept {
        const auto pos = bit_pos(wt);
        return pos < BIT_COUNT ? NAMES[pos] : "unknown";
    }

    // Returns true if the entry carries more than one category bit.
    [[nodiscard]] constexpr bool is_ambiguous(WordType wt) noexcept {
        return wt != None && (wt & (wt - 1)) != 0;
    }

    // Returns true if wt has the given category bit set.
    [[nodiscard]] constexpr bool has(WordType wt, WordType category) noexcept {
        return (wt & category) != 0;
    }

    // Maps a UD UPOS string to a single-bit WordType.
    [[nodiscard]] inline WordType from_upos(std::string_view upos) noexcept {
        if (upos == "VERB" || upos == "AUX")    return Verb;
        if (upos == "NOUN" || upos == "PROPN")  return Noun;
        if (upos == "ADJ")                      return Adjective;
        if (upos == "ADV")                      return Adverb;
        if (upos == "ADP")                      return Preposition;
        if (upos == "DET")                      return Article;
        if (upos == "CCONJ" || upos == "SCONJ") return Conjunction;
        if (upos == "PRON")                     return Pronoun;
        if (upos == "INTJ")                     return Interjection;
        if (upos == "PUNCT")                    return Punctuation;
        return Other;
    }

} // namespace word_type

// -----------------------------------------------------------------------------
// EntryRole  —  participation in output text and/or parser input
// -----------------------------------------------------------------------------

enum class EntryRole : uint8_t {
    Output = 0b01,
    Input  = 0b10,
    Both   = 0b11,
};

[[nodiscard]] constexpr bool has_output(EntryRole r) noexcept {
    return (static_cast<uint8_t>(r) & 0b01) != 0;
}
[[nodiscard]] constexpr bool has_input(EntryRole r) noexcept {
    return (static_cast<uint8_t>(r) & 0b10) != 0;
}

// -----------------------------------------------------------------------------
// LexEntryId  —  stable design-time handle
// -----------------------------------------------------------------------------

using LexEntryId = uint32_t;
inline constexpr LexEntryId INVALID_ENTRY_ID = std::numeric_limits<uint32_t>::max();

// -----------------------------------------------------------------------------
// NLPToken  —  plain data carrier from the UDPipe adapter
//
// Mapping from udpipe::word fields:
//   word.form      →  form
//   word.lemma     →  lemma     (Lexicon lookup key)
//   word.upostag   →  upos      (mapped to WordType via word_type::from_upos)
//   word.feats     →  feats
//   word.head      →  head
//   word.deprel    →  deprel
//   word.xpostag   →  not used
//   word.misc      →  not used
//
// NLPToken is a struct: it is a plain data carrier with no invariants.
// The adapter fills it; the Lexicon consumes it; it is then discarded.
// -----------------------------------------------------------------------------

struct NLPToken {
    std::string  form;
    std::string  lemma;
    std::string  upos;
    std::string  feats;
    std::string  deprel;
    int          head       = 0;
    float        confidence = 1.0f;   // 1.0: full model  0.6: fallback tokenizer

    // Derived by adapter — not from UDPipe directly.
    WordType     word_type  = word_type::None;
    LanguageCode lang;

    [[nodiscard]] bool is_lexical() const noexcept {
        return word_type != word_type::Punctuation
            && word_type != word_type::Other
            && !lemma.empty();
    }
};

// -----------------------------------------------------------------------------
// SynonymLink  —  directed synonym relationship
//
// Confidence by origin:
//   1.0  manually confirmed by author
//   0.9  affix match (Capa 1) + lemma exists in Lexicon
//   0.7  affix match (Capa 1) only
//   0.6  shared Snowball stem (Capa 2 fallback, no UDPipe model)
//   0.4  semantic similarity candidate (Capa 3, pending author review)
// -----------------------------------------------------------------------------

struct SynonymLink {
    LexEntryId target     = INVALID_ENTRY_ID;
    float      confidence = 0.0f;
    bool       confirmed  = false;
};

// -----------------------------------------------------------------------------
// SynonymRef  —  forward reference for incremental construction
// -----------------------------------------------------------------------------

struct SynonymRef {
    LexEntryId from;
    LexEntryId to;
    float      confidence;
};

// -----------------------------------------------------------------------------
// LexEntry  —  atomic unit of the Lexicon
//
// A class (not struct) because type_counts and the types bitmask must stay
// in sync — observe_type() is the single controlled mutation point for both.
//
// Lifecycle:
//   1. Created when UDPipe returns a lemma not yet in the Lexicon.
//   2. On each subsequent occurrence, observe_type() updates the bitmask,
//      the per-bit counts, and recalculates dominant_type().
//   3. Author may add synonyms and adjust role via the IDE.
//   4. compiled_token assigned by Lexicon::index() at compile time.
// -----------------------------------------------------------------------------

class LexEntry {
public:

    // -- Identity (set at construction, immutable) ----------------------------

    LexEntryId   id   = INVALID_ENTRY_ID;
    LanguageCode lang;
    std::string  canonical;     // max 32 chars, source: NLPToken::lemma
    EntryRole    role = EntryRole::Both;

    // -- Linguistic metadata (public read, controlled write) ------------------

    // Raw UPOS string from UDPipe — kept for debugging.
    std::string upos_raw;

    // UD morphological features — used by Capa 2 fallback synonym detection.
    std::string feats;

    // Snowball stem — populated ONLY when no UDPipe model is available.
    std::string stem;

    // Synonym relationships.
    std::vector<SynonymLink> synonyms;

    // -- Type bitmask — use observe_type() to mutate -------------------------

    // Returns the combined bitmask of all observed WordTypes.
    // Example: "abierto" observed as Verb and Adjective → Verb | Adjective
    [[nodiscard]] WordType types() const noexcept { return types_; }

    // Returns the single WordType with the highest occurrence count.
    // Recalculated automatically by observe_type().
    [[nodiscard]] WordType dominant_type() const noexcept { return dominant_type_; }

    // Returns the occurrence count for a specific single-bit WordType.
    [[nodiscard]] uint32_t type_count(WordType single_bit) const noexcept {
        const auto pos = word_type::bit_pos(single_bit);
        return pos < word_type::BIT_COUNT ? type_counts_[pos] : 0;
    }

    // True if more than one category bit is set.
    [[nodiscard]] bool is_ambiguous() const noexcept {
        return word_type::is_ambiguous(types_);
    }

    // Records one new occurrence of a given single-bit WordType.
    // Updates the bitmask, the per-bit count, and recalculates dominant_type_.
    void observe_type(WordType single_bit) noexcept {
        assert(single_bit != word_type::None);
        assert((single_bit & (single_bit - 1)) == 0); // must be single-bit

        const auto pos = word_type::bit_pos(single_bit);
        assert(pos < word_type::BIT_COUNT);

        types_ |= single_bit;
        ++type_counts_[pos];

        // Recalculate dominant type: the bit with the highest count.
        const auto max_pos = static_cast<std::size_t>(
            std::max_element(type_counts_.begin(), type_counts_.end())
            - type_counts_.begin());
        dominant_type_ = static_cast<WordType>(1 << max_pos);
    }

    // -- Frequency (raw_count is private — mutated only via accumulate()) -----

    [[nodiscard]] float    frequency() const noexcept { return frequency_; }
    [[nodiscard]] uint32_t raw_count() const noexcept { return raw_count_; }

    // Called by the Lexicon on every token occurrence.
    // total_tokens: current total corpus token count after this occurrence.
    void accumulate(uint32_t total_tokens) noexcept {
        ++raw_count_;
        frequency_ = total_tokens > 0
            ? static_cast<float>(raw_count_) / static_cast<float>(total_tokens)
            : 0.0f;
    }

    // -- Compiled token (compile time only) -----------------------------------

    [[nodiscard]] TokenIndex compiled_token() const noexcept { return compiled_token_; }
    [[nodiscard]] bool       is_indexed()     const noexcept {
        return compiled_token_ != token::UNASSIGNED;
    }
    [[nodiscard]] uint8_t encoded_size() const noexcept {
        return token::encoded_size(compiled_token_);
    }

    void set_compiled_token(TokenIndex t) noexcept { compiled_token_ = t; }

    // -- Synonym helpers ------------------------------------------------------

    [[nodiscard]] const SynonymLink* find_synonym(LexEntryId target_id) const noexcept {
        for (const auto& s : synonyms)
            if (s.target == target_id) return &s;
        return nullptr;
    }

    // -- Ordering: descending frequency, alphabetical tiebreak ---------------

    [[nodiscard]] std::partial_ordering operator<=>(const LexEntry& o) const noexcept {
        if (frequency_ != o.frequency_) return o.frequency_ <=> frequency_;
        return canonical <=> o.canonical;
    }
    [[nodiscard]] bool operator==(const LexEntry& o) const noexcept {
        return id == o.id;
    }

private:
    WordType  types_        = word_type::None;
    WordType  dominant_type_ = word_type::None;

    // Occurrence count per bit position, parallel to the bitmask.
    // type_counts_[bit_pos(X)] = number of times WordType X was observed.
    std::array<uint32_t, word_type::BIT_COUNT> type_counts_{};

    float    frequency_      = 0.0f;
    uint32_t raw_count_      = 0;
    TokenIndex compiled_token_ = token::UNASSIGNED;
};

} // namespace ads::lexicon