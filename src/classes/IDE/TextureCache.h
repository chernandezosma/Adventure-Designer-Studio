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

#ifndef ADS_IDE_TEXTURE_CACHE_H
#define ADS_IDE_TEXTURE_CACHE_H

/**
 * @file TextureCache.h
 * @brief Path-keyed cache of decoded SDL textures for inspector image previews
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <string>
#include <unordered_map>

struct SDL_Texture;

namespace ADS::IDE {

    /**
     * @class TextureCache
     * @brief Loads and retains GPU textures for on-disk image files, one per path
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * The inspector shows a small thumbnail under every image / avatar field.
     * Decoding and uploading an image every frame would be wasteful, so this
     * cache keeps the resulting SDL_Texture alive, keyed by the absolute file
     * path it was loaded from. A path that fails to load caches a null entry
     * so a broken reference is not retried on every frame.
     *
     * Not copyable: it owns SDL_Texture handles. The owning panel calls
     * clear() whenever the selection changes and the destructor frees the
     * rest. All access happens on the render thread.
     */
    class TextureCache {
    public:
        /**
         * @brief Construct an empty cache
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         */
        TextureCache() = default;

        /**
         * @brief Destroy the cache and every texture it still holds
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         */
        ~TextureCache();

        TextureCache(const TextureCache&) = delete;
        TextureCache& operator=(const TextureCache&) = delete;

        /**
         * @brief Get the texture for an image file, decoding it on first use
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param absPath Absolute path to a PNG / JPEG / BMP (or any format
         *                the linked SDL_image build supports)
         * @return SDL_Texture* Cached texture, or nullptr when the file cannot
         *                      be read or decoded (the null result is cached)
         */
        SDL_Texture* get(const std::string& absPath);

        /**
         * @brief Destroy every cached texture and empty the cache
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         */
        void clear();

    private:
        /// Absolute path -> texture (nullptr means "known to fail to load").
        std::unordered_map<std::string, SDL_Texture*> m_textures;
    };

} // namespace ADS::IDE

#endif // ADS_IDE_TEXTURE_CACHE_H
