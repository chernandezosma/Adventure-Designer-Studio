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

/**
 * @file TextureCache.cpp
 * @brief Implementation of ADS::IDE::TextureCache
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include "TextureCache.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "spdlog/spdlog.h"

#include "app.h"
#include "UI/Window.h"

namespace ADS::IDE {

    /**
     * @brief Destroy the cache and every texture it still holds
     */
    TextureCache::~TextureCache()
    {
        clear();
    }

    /**
     * @brief Get the texture for an image file, decoding it on first use
     *
     * @param absPath Absolute path to an image file
     * @return SDL_Texture* Cached texture, or nullptr on load/decode failure
     */
    SDL_Texture* TextureCache::get(const std::string& absPath)
    {
        if (const auto it = m_textures.find(absPath); it != m_textures.end()) {
            return it->second;
        }

        SDL_Texture* texture = nullptr;

        UI::Window* window     = Core::App::getMainWindow();
        SDL_Renderer* renderer = window != nullptr ? window->getRenderer() : nullptr;

        if (renderer != nullptr) {
            if (SDL_Surface* surface = IMG_Load(absPath.c_str())) {
                texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_DestroySurface(surface);
                if (texture == nullptr) {
                    spdlog::warn("TextureCache: could not create texture from '{}' — {}",
                                 absPath, SDL_GetError());
                }
            } else {
                spdlog::debug("TextureCache: could not load image '{}' — {}",
                              absPath, SDL_GetError());
            }
        }

        // Cache the result — including nullptr — so a bad path is not retried
        // on every frame.
        m_textures.emplace(absPath, texture);
        return texture;
    }

    /**
     * @brief Destroy every cached texture and empty the cache
     */
    void TextureCache::clear()
    {
        for (auto& [path, texture] : m_textures) {
            if (texture != nullptr) {
                SDL_DestroyTexture(texture);
            }
        }
        m_textures.clear();
    }

} // namespace ADS::IDE
