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


#include "fonts.h"
#include <filesystem>
#include <spdlog/spdlog.h>

namespace ADS::UI {

    /**
     * @brief Constructor for Fonts class
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Initializes the Fonts manager with a reference to ImGui's I/O context.
     * Does NOT automatically load any fonts - call loadDefaultFonts() or
     * specific load methods after construction.
     *
     * @param io Pointer to ImGui I/O context (must not be nullptr)
     *
     * @note This constructor does not load fonts automatically to give
     *       the caller full control over which fonts to load and when
     */
    Fonts::Fonts(ImGuiIO *io) : io(io)
    {
        if (this->io == nullptr) {
            spdlog::error("Fonts::Fonts() - ImGuiIO pointer is null!");
            throw std::runtime_error("Cannot initialize Fonts with null ImGuiIO");
        }

        spdlog::debug("Fonts manager initialized");
    }

    /**
     * @brief Load ImGui's default font
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Loads the default ImGui font (ProggyClean) and stores it with the
     * name "default" in the loaded fonts map. This is typically the first
     * font loaded and serves as the fallback font.
     *
     * @note If a default font already exists, it will be replaced
     */
    void Fonts::loadDefaultFonts()
    {
        // Configure font for better quality on high DPI displays
        ImFontConfig config;
        config.OversampleH = 2;   // Horizontal oversampling for sharper fonts
        config.OversampleV = 2;   // Vertical oversampling for sharper fonts
        config.PixelSnapH = true; // Align to pixel boundaries
        // MergeMode is false by default - this is a standalone font

        ImFont* defaultFont = this->io->Fonts->AddFontDefault(&config);
        this->loadedFonts["default"] = defaultFont;

        spdlog::info("Default font loaded with high DPI configuration");
    }

    /**
     * @brief Load a custom font from a file
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Loads a TrueType font (.ttf) from the specified file path and registers
     * it with the given name. The font can later be retrieved using getFont().
     * Validates that the file exists before attempting to load.
     *
     * @param fontName  Unique identifier for this font (used for retrieval)
     * @param path      Filesystem path to the .ttf font file
     * @param size      Font size in pixels (default: 16.0f)
     *
     * @return ImFont* Pointer to the loaded font, or nullptr on failure
     *
     * @note Returns nullptr if the file doesn't exist or loading fails
     * @note If a font with the same name already exists, it will be replaced
     */
    ImFont *Fonts::loadFontFromFile(const std::string &fontName, const std::string &path, float size)
    {
        // Validate file exists
        if (!std::filesystem::exists(path)) {
            spdlog::error("Font file not found: {}", path);
            return nullptr;
        }

        // Configure font for high quality rendering
        ImFontConfig config;
        config.OversampleH = 2;   // Horizontal oversampling for sharper fonts
        config.OversampleV = 2;   // Vertical oversampling for sharper fonts
        config.PixelSnapH = true; // Align to pixel boundaries
        // MergeMode is false by default - this is a standalone font

        // Load the font with config
        ImFont* font = this->io->Fonts->AddFontFromFileTTF(path.c_str(), size, &config);

        if (font == nullptr) {
            spdlog::error("Failed to load font '{}' from: {}", fontName, path);
            return nullptr;
        }

        // Store in the map
        this->loadedFonts[fontName] = font;
        spdlog::info("Font '{}' loaded from: {} (size: {}px)", fontName, path, size);

        return font;
    }

    /**
     * @brief Load an icon font (e.g., FontAwesome) with merge mode
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Loads an icon font and merges it with the previously loaded font.
     * This allows icon glyphs to be used alongside regular text. The font
     * is loaded with specific configuration for optimal icon rendering:
     * - MergeMode enabled (icons merge with previous font)
     * - GlyphMinAdvanceX set for consistent icon spacing
     *
     * Typical usage: Load your text font first, then load icon fonts
     * to supplement it with icon glyphs.
     *
     * @param path Filesystem path to the icon font file (e.g., FontAwesome)
     * @param size Font size in pixels (recommended: 13.0f for FontAwesome)
     *
     * @return ImFont* Pointer to the loaded icon font, or nullptr on failure
     *
     * @note This font is stored with the name "icons" in the loaded fonts map
     * @note Merge mode requires at least one font to be loaded previously
     */
    ImFont *Fonts::loadIconFont(const std::string &path, float size)
    {
        // Validate file exists
        if (!std::filesystem::exists(path)) {
            spdlog::error("Icon font file not found: {}", path);
            return nullptr;
        }

        // Configure font for icon loading
        ImFontConfig config;
        config.MergeMode = true;            // Merge icons with previous font
        config.GlyphMinAdvanceX = size;     // Make icons monospace for consistency
        config.OversampleH = 2;             // High quality rendering
        config.OversampleV = 2;             // High quality rendering
        config.PixelSnapH = true;           // Align to pixel boundaries

        // FontAwesome 4 glyph range (0xf000 - 0xf2e0)
        static const ImWchar icons_ranges[] = { 0xf000, 0xf2e0, 0 };

        // Load the icon font with proper glyph ranges
        ImFont* iconFont = this->io->Fonts->AddFontFromFileTTF(path.c_str(), size, &config, icons_ranges);

        if (iconFont == nullptr) {
            spdlog::error("Failed to load icon font from: {}", path);
            return nullptr;
        }

        // Store in the map with 'icons' key
        this->loadedFonts["icons"] = iconFont;
        spdlog::info("Icon font loaded from: {} (size: {}px)", path, size);

        return iconFont;
    }

    /**
     * @brief Retrieve a previously loaded font by name
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Searches the loaded fonts collection for a font with the given name
     * and returns a pointer to it if found. Returns nullptr if the font
     * name is not registered.
     *
     * @param name The unique identifier of the font to retrieve
     *
     * @return ImFont* Pointer to the font if found, nullptr otherwise
     *
     * @note Use hasFont() to check existence before calling this method
     * @note Common font names: "default", "icons", "light", "medium", "regular"
     */
    ImFont *Fonts::getFont(const std::string &name)
    {
        auto it = this->loadedFonts.find(name);

        if (it != this->loadedFonts.end()) {
            return it->second;
        }

        spdlog::warn("Font '{}' not found in loaded fonts", name);
        return nullptr;
    }

    /**
     * @brief Check if a font with the given name has been loaded
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Dec 2025
     *
     * Queries the loaded fonts collection to determine if a font with
     * the specified name exists. Useful for conditional font usage.
     *
     * @param name The unique identifier of the font to check
     *
     * @return true if the font exists in the loaded fonts collection
     * @return false if the font has not been loaded or doesn't exist
     */
    bool Fonts::hasFont(const std::string &name)
    {
        return this->loadedFonts.find(name) != this->loadedFonts.end();
    }

} // namespace ADS::UI