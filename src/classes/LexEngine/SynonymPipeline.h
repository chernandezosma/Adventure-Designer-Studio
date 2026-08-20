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

#ifndef ADS_LEXENGINE_SYNONYM_PIPELINE_H
#define ADS_LEXENGINE_SYNONYM_PIPELINE_H

/**
 * @file SynonymPipeline.h
 * @brief Three-layer synonym detection pipeline
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Mar 2026
 *
 * Runs automatically on every new LexEngine insertion, in order, with
 * short-circuit propagation: if a layer produces a match, the reduced
 * form (not the original) is passed to the next layer.
 *
 *   Layer 1 - affix trie (automatic, per-language)
 *   Layer 2 - shared-stem grouping (fallback mode)
 *   Layer 3 - semantic similarity (deferred — requires an embedding model)
 *
 * Confidence by origin:
 *   1.0  manually confirmed by the author (not produced by this pipeline)
 *   0.9  Layer 1: affix detected and the reduced root exists in the LexEngine
 *   0.6  Layer 2: shared stem
 *   0.4  Layer 3: semantic-similarity candidate (currently never produced)
 *
 * If an affix is detected but the reduced root does NOT exist in the
 * LexEngine, the match is discarded entirely rather than proposed at lower
 * confidence — this is the explicit rule in both design documents
 * (docs/core/lexengine/LexEngine.md §2.3, docs/core/lexengine/vocabulary.md
 * "Layer 1"), stated to prevent false positives such as Spanish
 * "recabar" -> "cabar" where the candidate root is not itself a word.
 *
 * @see ADS::LexEngine::ILexEngineLookup
 */

#include <string_view>
#include <vector>

#include "ILexEngineLookup.h"
#include "types.h"

namespace ADS::LexEngine {

    /**
     * @brief Runs the three-layer synonym detection pipeline for one entry
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     */
    class SynonymPipeline {
    public:
        /**
         * @brief Run all three layers for a newly-inserted or updated entry
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param canonical Canonical form of the entry being processed
         * @param entryId Id of the entry being processed — excluded from its own candidate list
         * @param lang Language of the entry
         * @param lookup Read-only LexEngine view used to validate candidate roots and find stem siblings
         * @return std::vector<SynonymLink> Proposed (unconfirmed) synonym links, possibly empty
         */
        [[nodiscard]] static std::vector<SynonymLink> run(std::string_view canonical,
                                                            LexEntryId entryId,
                                                            const LanguageCode& lang,
                                                            const ILexEngineLookup& lookup);

        /// Confidence assigned to a Layer 1 match (affix detected, root exists in the LexEngine).
        static constexpr float LAYER1_CONFIDENCE = 0.9f;

        /// Confidence assigned to a Layer 2 match (shared stem, fallback mode).
        static constexpr float LAYER2_CONFIDENCE = 0.6f;

        /// Confidence assigned to a Layer 3 match (semantic similarity, pending author review).
        static constexpr float LAYER3_CONFIDENCE = 0.4f;

    private:
        /**
         * @brief Layer 1 — strip a productive prefix/suffix and check the root exists
         * @param canonical Canonical form to reduce
         * @param lang Language selecting the affix table
         * @param lookup Read-only LexEngine view used to validate the candidate root
         * @return LexEntryId Root entry id if a valid reduction was found, INVALID_ENTRY_ID otherwise
         */
        [[nodiscard]] static LexEntryId affixLayer(std::string_view canonical,
                                                     const LanguageCode& lang,
                                                     const ILexEngineLookup& lookup);

        /**
         * @brief Layer 3 — semantic-similarity candidates
         * @return std::vector<SynonymLink> Always empty in the current implementation
         *
         * Deferred: proposing candidates by semantic distance requires an
         * embedding model that is not part of this pass. This layer exists
         * so the pipeline shape matches the design document and future work
         * has a single place to add the real implementation.
         */
        [[nodiscard]] static std::vector<SynonymLink> semanticLayer();
    };

} // namespace ADS::LexEngine

#endif // ADS_LEXENGINE_SYNONYM_PIPELINE_H