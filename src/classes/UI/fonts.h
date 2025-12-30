/*
 * Adventure Designer Studio
 * Copyright (c) 2025 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 *
 * This file is licensed under the GNU General Public License version 3 (GPLv3).
 * See LICENSE.md and COPYING for full license details.
 *
 * This software includes an additional requirement for visible attribution:
 * The original author's name must be displayed in any user interface or
 * promotional material.
 */


#ifndef ADS_FONTS_H
#define ADS_FONTS_H
#include <string>
#include <unordered_map>

#include "imgui.h"

namespace ADS::UI {
    class Fonts
    {
    private:
        ImGuiIO *io;

        std::unordered_map<std::string, ImFont *> loadedFonts;

    public:
        /**
         * @brief Construct a new Fonts manager
         *
         * @param io Pointer to ImGui I/O context (must not be nullptr)
         * @throws std::runtime_error if io is nullptr
         */
        explicit Fonts(ImGuiIO *io);

        /**
         * @brief Load ImGui's default font
         *
         * Loads the default ImGui font (ProggyClean) and registers it
         * as "default" for later retrieval.
         */
        void loadDefaultFonts();

        /**
         * @brief Load a custom TrueType font from file
         *
         * @param fontName Unique identifier for this font
         * @param path Filesystem path to the .ttf font file
         * @param size Font size in pixels (default: 16.0f)
         * @return ImFont* Pointer to loaded font, or nullptr on failure
         */
        ImFont *loadFontFromFile(const std::string &fontName, const std::string &path, float size = 16.0f);

        /**
         * @brief Load an icon font with merge mode enabled
         *
         * Loads an icon font (e.g., FontAwesome) and merges it with the
         * previously loaded font, allowing icons to be used alongside text.
         *
         * @param path Filesystem path to the icon font file
         * @param size Font size in pixels (default: 13.0f)
         * @return ImFont* Pointer to loaded icon font, or nullptr on failure
         */
        ImFont *loadIconFont(const std::string &path, float size = 13.0f);

        /**
         * @brief Retrieve a previously loaded font by name
         *
         * @param name The unique identifier of the font to retrieve
         * @return ImFont* Pointer to the font if found, nullptr otherwise
         */
        ImFont *getFont(const std::string &name);

        /**
         * @brief Check if a font with the given name has been loaded
         *
         * @param name The unique identifier of the font to check
         * @return true if the font exists, false otherwise
         */
        bool hasFont(const std::string &name);


    };
} // ADS

#endif //_FONTS_H
