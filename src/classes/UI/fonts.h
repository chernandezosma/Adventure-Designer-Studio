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

#ifndef ADS_FONTS_H
#define ADS_FONTS_H

#include <string>
#include <unordered_map>

#include "imgui.h"

namespace ADS::UI {
    /**
     * @brief Font manager for the application
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Manages loading and retrieval of ImGui fonts. Fonts are stored
     * by a unique string name and are owned by the ImGui font atlas.
     */
    class Fonts
    {
    private:
        ImGuiIO* io;                                          ///< ImGui I/O context (non-owning)
        std::unordered_map<std::string, ImFont*> loadedFonts; ///< Name → font pointer map

    public:
        /**
         * @brief Construct a new Fonts manager
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param io Pointer to ImGui I/O context (must not be nullptr)
         */
        explicit Fonts(ImGuiIO* io);

        /**
         * @brief Load ImGui's default font
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Loads the default ImGui font (ProggyClean) and registers it
         * as "default" for later retrieval.
         */
        void loadDefaultFonts();

        /**
         * @brief Load a custom TrueType font from file
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param fontName Unique identifier for this font
         * @param path Filesystem path to the .ttf font file
         * @param size Font size in pixels (default: 16.0f)
         * @return ImFont* Pointer to loaded font, or nullptr on failure
         */
        ImFont* loadFontFromFile(const std::string& fontName, const std::string& path, float size = 16.0f);

        /**
         * @brief Load an icon font with merge mode enabled
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Loads an icon font (e.g., FontAwesome) and merges it with the
         * previously loaded font, allowing icons to be used alongside text.
         *
         * @param path Filesystem path to the icon font file
         * @param size Font size in pixels (default: 13.0f)
         * @return ImFont* Pointer to loaded icon font, or nullptr on failure
         */
        ImFont* loadIconFont(const std::string& path, float size = 13.0f);

        /**
         * @brief Retrieve a previously loaded font by name
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param name The unique identifier of the font to retrieve
         * @return ImFont* Pointer to the font if found, nullptr otherwise
         */
        ImFont* getFont(const std::string& name);

        /**
         * @brief Check if a font with the given name has been loaded
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param name The unique identifier of the font to check
         * @return bool True if the font exists, false otherwise
         */
        bool hasFont(const std::string& name);
    };
} // namespace ADS::UI

#endif // ADS_FONTS_H