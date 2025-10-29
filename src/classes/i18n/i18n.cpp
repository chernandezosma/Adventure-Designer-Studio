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

#include "i18n.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <regex>
#include <sstream>
#include <fstream>
#include <locale>
#include <map>


// Optional: Include nlohmann/json if available
#ifdef NLOHMANN_JSON_VERSION_MAJOR
#include <nlohmann/json.hpp>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

namespace ADS::i18n {
    /**
     * @brief Construct i18n system with base folder and fallback language
     *
     * @autor   Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Initializes the internationalization system with the specified base directory
     * for translation files and fallback language. Automatically detects system
     * locale and sets up default configurations.
     *
     * @param baseFolder Base directory path for translation files (relative to current dir)
     * @param fallback Fallback language code to use when translations are missing
     *
     * @throws locale_exception if fallback language is not supported
     * @throws translation_file_exception if base folder cannot be accessed
     *
     * @note The base folder path is resolved relative to current working directory
     * @see init(), extractSystemLocale()
     */
    i18n::i18n(const string& baseFolder, const string& fallback): baseFolder(std::filesystem::current_path() / baseFolder), fallbackLanguage(fallback)
    {
        // Validate base folder
        if (!std::filesystem::exists(this->baseFolder)) {
            throw translation_file_exception("Translation directory does not exist: " + this->baseFolder.string());
        }

        // Validate fallback language
        if (!Constants::Languages::isLanguageSupported(fallback)) {
            throw locale_exception("Fallback language not supported: " + fallback);
        }

        // Initialize system
        this->init();
    }

    /**
     * @brief Initialize the internationalization system
     *
     * @autor   Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Performs initial setup of the i18n system including locale detection,
     * default language configuration, and base folder validation. Called
     * automatically during construction.
     *
     * @throws locale_exception if system locale cannot be determined
     * @throws translation_file_exception if base folder is invalid
     */
    void i18n::init()
    {
        try {
            // Extract system locale information
            this->extractSystemLocale();

            // Set current locale to system locale (fallback to fallback language if system not supported)
            if (Constants::Languages::isLanguageSupported(systemLocale.locale)) {
                this->currentLocale = systemLocale;
            }
            else {
                this->currentLocale = createLocaleInfo(fallbackLanguage);
            }

            // Set default language to current locale
            this->defaultLanguage = currentLocale.locale;

            // Load fallback language
            this->addLanguage(fallbackLanguage);

            // Load current locale if different from fallback
            if (currentLocale.locale != fallbackLanguage) {
                try {
                    this->addLanguage(currentLocale.locale);
                }
                catch (const std::exception&) {
                    // If current locale can't be loaded, stick with fallback
                }
            }
        }
        catch (const std::exception& e) {
            throw i18n_exception("Failed to initialize i18n system: " + string(e.what()));
        }
    }

    /**
     * @brief Extract and normalize system locale information
     *
     * @autor   Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Detects the current system locale using environment variables (LANG, LC_ALL)
     * and normalizes it to POSIX format. Sets up systemLocale and rawLocaleString
     * member variables with the detected information.
     *
     * @note Falls back to fallback language if system locale cannot be detected
     * @see normalizePlatformLocale()
     */
    void i18n::extractSystemLocale()
    {
        try {
            // Get system locale string
            this->rawLocaleString = std::locale("").name();

            // Handle "C" or "POSIX" locale
            if (this->rawLocaleString == "C" || this->rawLocaleString == "POSIX") {
                this->systemLocale = createLocaleInfo(fallbackLanguage);
                return;
            }

            // Normalize platform-specific locale
            string normalizedLocale = Constants::Languages::normalizePlatformLocale(rawLocaleString);

            if (normalizedLocale.empty()) {
                // Try environment variables as fallback
                const char* envLocale = nullptr;

#ifdef _WIN32
                if ((envLocale = std::getenv("LANG")) && strlen(envLocale) > 0) {
                    normalizedLocale = Constants::Languages::normalizePlatformLocale(envLocale);
                }
#else
                const char* envVars[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
                for (const char* var: envVars) {
                    if ((envLocale = std::getenv(var)) && std::strlen(envLocale) > 0) {
                        normalizedLocale = Constants::Languages::normalizePlatformLocale(envLocale);
                        if (!normalizedLocale.empty()) break;
                    }
                }
#endif
            }

            // Use fallback if still empty
            if (normalizedLocale.empty()) {
                normalizedLocale = fallbackLanguage;
            }

            this->systemLocale = this->createLocaleInfo(normalizedLocale);
        }
        catch (const std::exception& e) {
            // Ultimate fallback
            this->systemLocale = this->createLocaleInfo(fallbackLanguage);
        }
    }

    /**
     * @brief Create LocaleInfo structure from language code
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Constructs a LocaleInfo object from a given locale code by looking up
     * the human-readable language name and validating the locale format.
     *
     * @param localeCode The locale code (e.g., "es_ES")
     * @return LocaleInfo structure with locale and language information
     *
     * @throws locale_exception if locale code is invalid or unsupported
     * @see LocaleInfo::isValid()
     */
    LocaleInfo i18n::createLocaleInfo(const string& localeCode) const
    {
        LocaleInfo locale;

        locale.locale = localeCode;
        locale.language = Constants::Languages::getLanguageName(localeCode);

        if (locale.language.empty()) {
            throw locale_exception("Unsupported locale: " + localeCode);
        }

        return locale;
    }

    /**
     * @brief Get the currently active locale information
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Returns the LocaleInfo object representing the currently active locale
     * including both the POSIX locale code and human-readable language name.
     *
     * @return Current LocaleInfo object
     */
    [[nodiscard]] LocaleInfo i18n::getCurrentLocale() const
    {
        return this->currentLocale;
    }

    /**
     * @brief Load translation file for a specific language
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Attempts to load translation data from various file formats in priority order.
     * Searches for files with extensions .json, .properties, .po, and .txt in the
     * base folder directory. Uses appropriate parser based on file extension.
     *
     * @param language The language code to load (e.g., "es_ES")
     * @return true if translation file was loaded successfully, false otherwise
     *
     * @note Updates the translations member variable with loaded data
     * @see parseJsonContent(), parsePropertiesContent(), parsePoContent()
     */
    bool i18n::loadTranslationFile(const string& language)
    {
        // Define file extensions with their corresponding parsers
        struct FileFormat {
            string extension;
            function<bool(const string&, unordered_map<string, string>&)> parser;
        };

        vector<FileFormat> formats = {
                {
                        ".json", [this](const string& content, unordered_map<string, string>& trans) {
                            return parseJsonContent(content, trans);
                        }
                },
                {
                        ".properties", [this](const string& content, unordered_map<string, string>& trans) {
                            return parsePropertiesContent(content, trans);
                        }
                },
                {
                        ".po", [this](const string& content, unordered_map<string, string>& trans) {
                            return parsePoContent(content, trans);
                        }
                },
                {
                        ".txt", [this](const string& content, unordered_map<string, string>& trans) {
                            return parsePropertiesContent(content, trans); // Same as properties
                        }
                }
        };

        for (const auto& format: formats) {
            filesystem::path filePath = baseFolder / (language + format.extension);

            if (std::filesystem::exists(filePath)) {
                try {
                    ifstream file(filePath);
                    if (!file.is_open()) continue;

                    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                    file.close();

                    unordered_map<string, string> languageTranslations;
                    if (format.parser(content, languageTranslations)) {
                        translations[language] = std::move(languageTranslations);
                        return true;
                    }
                }
                catch (const std::exception&) {
                    continue;
                }
            }
        }

        return false;
    }

    /**
     * @brief Parse JSON format translation file content
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Parses JSON content containing translation key-value pairs. Supports
     * nested objects using dot notation (e.g., "menu.file.open"). Handles
     * both flat and hierarchical JSON structures.
     *
     * @param content JSON file content as string
     * @param translations Output map to store parsed key-value pairs
     * @return true if parsing was successful, false on error
     *
     * @note Requires nlohmann/json library for JSON parsing
     * @see parseNestedJson() for nested object handling
     */
    bool i18n::parseJsonContent(const string& content, unordered_map<string, string>& translations)
    {
        try {
#ifdef NLOHMANN_JSON_VERSION_MAJOR
            nlohmann::json jsonData = nlohmann::json::parse(content);

            // Handle flat JSON object: {"key": "value", ...}
            if (jsonData.is_object()) {
                for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                    if (it.value().is_string()) {
                        translations[it.key()] = it.value().get<string>();
                    }
                    else if (it.value().is_object()) {
                        // Handle nested objects with dot notation: {"menu": {"file": "File"}} -> "menu.file": "File"
                        parseNestedJson(it.key(), it.value(), translations);
                    }
                }
                return !this->translations.empty();
            }
#else
            // Fallback to properties parser if nlohmann/json is not available
            return parsePropertiesContent(content, translations);
#endif
        }
        catch (const std::exception&) {
            // If JSON parsing fails, try properties format as fallback
            return parsePropertiesContent(content, translations);
        }

        return false;
    }

#ifdef NLOHMANN_JSON_VERSION_MAJOR
    /**
     * @brief Parse nested JSON objects recursively with dot notation
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Recursively traverses nested JSON objects and converts them to flat
     * key-value pairs using dot notation. For example, {"menu": {"file": "File"}}
     * becomes "menu.file" -> "File".
     *
     * @param prefix Current key prefix for nested objects
     * @param obj JSON object to parse recursively
     * @param translations Output map to store flattened key-value pairs
     *
     * @note Only available when nlohmann/json library is present
     * @see parseJsonContent()
     */
    void i18n::parseNestedJson(const string& prefix, const nlohmann::json& obj,
                               unordered_map<string, string>& translations)
    {
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            string key = prefix.empty() ? it.key() : prefix + "." + it.key();

            if (it.value().is_string()) {
                translations[key] = it.value().get<string>();
            }
            else if (it.value().is_object()) {
                parseNestedJson(key, it.value(), translations);
            }
        }
    }
#endif
    /**
     * @brief Parse Java Properties format file content
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Parses standard Java Properties format with key=value pairs.
     * Handles comments (lines starting with # or !), escape sequences,
     * and multi-line values. Ignores empty lines and whitespace.
     *
     * @param content Properties file content as string
     * @param translations Output map to store parsed key-value pairs
     * @return true if parsing was successful, false on error
     *
     * @note Also used for .txt files with same format
     * @see unescapeString() for escape sequence handling
     */
    bool i18n::parsePropertiesContent(const string& content, unordered_map<string, string>& translations)
    {
        istringstream stream(content);
        string line;

        while (getline(stream, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';') continue;

            // Find key=value separator
            size_t equalPos = line.find('=');
            if (equalPos == string::npos) continue;

            string key = line.substr(0, equalPos);
            string value = line.substr(equalPos + 1);

            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            // Remove quotes if present
            if (!value.empty() &&
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.length() - 2);
            }

            // Handle escape sequences
            value = unescapeString(value);

            translations[key] = value;
        }

        return !this->translations.empty();
    }

    /**
     * Parses PO (Portable Object) file content and extracts msgid/msgstr pairs.
     * Handles multi-line entries, comments, and continuation lines.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Oct-2025
     *
     * @param string content PO file content to parse
     * @param unordered_map<string, string> translations Output map to store parsed translations
     *
     * @return bool True if parsing was successful, false otherwise
     */
    bool i18n::parsePoContent(const string& content, unordered_map<string, string>& translations)
    {
        // Basic PO file parser
        istringstream stream(content);
        string line;
        string currentMsgid;
        string currentMsgstr;
        bool inMsgid = false;
        bool inMsgstr = false;

        while (getline(stream, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.empty() || line[0] == '#') {
                // Save previous entry if complete
                if (!currentMsgid.empty() && !currentMsgstr.empty()) {
                    translations[currentMsgid] = currentMsgstr;
                }
                currentMsgid.clear();
                currentMsgstr.clear();
                inMsgid = inMsgstr = false;
                continue;
            }

            if (line.substr(0, 6) == "msgid ") {
                currentMsgid = extractQuotedString(line.substr(6));
                inMsgid = true;
                inMsgstr = false;
            }
            else if (line.substr(0, 7) == "msgstr ") {
                currentMsgstr = extractQuotedString(line.substr(7));
                inMsgid = false;
                inMsgstr = true;
            }
            else if (line[0] == '"' && (inMsgid || inMsgstr)) {
                // Continuation line
                string continuation = extractQuotedString(line);
                if (inMsgid) {
                    currentMsgid += continuation;
                }
                else if (inMsgstr) {
                    currentMsgstr += continuation;
                }
            }
        }

        // Don't forget the last entry
        if (!currentMsgid.empty() && !currentMsgstr.empty()) {
            translations[currentMsgid] = currentMsgstr;
        }

        return !this->translations.empty();
    }

    /**
     * @brief Extract and clean quoted strings from PO file format
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Removes surrounding quotes from PO file string values and handles
     * concatenated multi-line strings. Processes both single and double
     * quoted strings according to PO file specifications.
     *
     * @param str String potentially containing quotes and whitespace
     * @return Cleaned string with quotes removed
     *
     * @note Used internally by parsePoContent()
     */
    string i18n::extractQuotedString(const string& str)
    {
        string trimmed = str;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

        if (trimmed.length() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
            return unescapeString(trimmed.substr(1, trimmed.length() - 2));
        }

        return trimmed;
    }

    /**
     * @brief Unescape common string escape sequences
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Converts escape sequences like \n, \t, \r, \\, \" back to their
     * literal characters. Handles standard C-style escape sequences
     * commonly used in translation files.
     *
     * @param str String containing escape sequences
     * @return String with escape sequences converted to literal characters
     *
     * @note Used by parsePropertiesContent() and parsePoContent()
     */
    string i18n::unescapeString(const string& str)
    {
        string result;
        result.reserve(str.length());

        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '\\' && i + 1 < str.length()) {
                switch (str[i + 1]) {
                case 'n':
                    result += '\n';
                    ++i;
                    break;
                case 't':
                    result += '\t';
                    ++i;
                    break;
                case 'r':
                    result += '\r';
                    ++i;
                    break;
                case '\\':
                    result += '\\';
                    ++i;
                    break;
                case '"':
                    result += '"';
                    ++i;
                    break;
                case '\'':
                    result += '\'';
                    ++i;
                    break;
                default:
                    result += str[i];
                    break;
                }
            }
            else {
                result += str[i];
            }
        }

        return result;
    }

    /**
     * @brief Set the current active locale using LocaleInfo
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Changes the current active locale to the specified LocaleInfo object.
     * Validates the locale and loads translation data if not already loaded.
     *
     * @param locale LocaleInfo object containing locale and language information
     *
     * @throws std::invalid_argument if locale is invalid or not supported
     * @see LocaleInfo::isValid(), addLanguage()
     */
    void i18n::setLocale(const LocaleInfo& locale)
    {
        if (!locale.isValid()) {
            throw locale_exception("Invalid locale: " + locale.locale);
        }

        this->currentLocale = locale;

        // Ensure the locale is loaded
        if (!hasLanguage(locale.locale)) {
            this->addLanguage(locale.locale);
        }
    }

    /**
     * @brief Set the current active locale using language code
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Changes the current active locale to the specified language code.
     * Creates LocaleInfo from the code, validates it, and loads translation
     * data if not already available.
     *
     * @param localeCode POSIX locale code (e.g., "es_ES", "en_US")
     *
     * @throws std::invalid_argument if locale code is invalid or not supported
     * @see createLocaleInfo(), addLanguage()
     */
    void i18n::setLocale(const string& localeCode)
    {
        LocaleInfo locale = createLocaleInfo(localeCode);
        setLocale(locale);
    }

    /**
     * @brief Get all translation key-value pairs for a language
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Retrieves the complete translation map for the specified language.
     * If no language is specified, returns translations for the current locale.
     *
     * @param language Language code (empty string uses current locale)
     * @return Unordered map of translation key-value pairs
     *
     * @note Returns empty map if language is not loaded
     */
    unordered_map<string, string> i18n::getTranslations(const string& language) const
    {
        string targetLanguage = language.empty() ? currentLocale.locale : language;

        auto it = this->translations.find(targetLanguage);
        if (it != this->translations.end()) {
            return it->second;
        }

        return {};
    }

    /**
     * @brief Get pointer to fallback language translation data
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Returns a pointer to the fallback language's translation entry in the
     * translations map. Useful for direct access to fallback translations.
     *
     * @return Pointer to fallback language pair (language code, translations map) or nullptr
     *
     * @note The returned pointer is valid until translations are modified
     */
    const pair<const string, unordered_map<string, string> >* i18n::getFallbackLanguageTranslations() const
    {
        auto it = this->translations.find(fallbackLanguage);
        return (it != this->translations.end()) ? &(*it) : nullptr;
    }

    /**
     * @brief Get pointer to specific language translation data
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Returns a pointer to the specified language's translation entry in the
     * translations map. Provides direct access to language translation data.
     *
     * @param language The language code to retrieve
     * @return Pointer to language pair (language code, translations map) or nullptr
     *
     * @note The returned pointer is valid until translations are modified
     */
    const pair<const string, unordered_map<string, string> >* i18n::getLanguage(const string& language) const
    {
        auto it = this->translations.find(language);
        return (it != this->translations.end()) ? &(*it) : nullptr;
    }

    /**
     * @brief Check if a language is currently loaded
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Determines whether translation data for the specified language
     * has been loaded into the system and is available for use.
     *
     * @param language The language code to check
     * @return true if language is loaded, false otherwise
     */
    bool i18n::hasLanguage(const string& language) const
    {
        return this->translations.find(language) != this->translations.end();
    }

    /**
     * @brief Add or load a language into the translation system
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Loads translation data for the specified language from files or creates
     * an empty translation set if no file exists. Validates language support
     * and handles duplicate loading attempts.
     *
     * @param language The language code to add/load
     * @return Pointer to the language translation entry
     *
     * @throws std::invalid_argument if language is not supported
     * @throws std::runtime_error if language cannot be loaded
     *
     * @see loadTranslationFile(), Constants::Languages::isLanguageSupported()
     */
    pair<const string, unordered_map<string, string> >* i18n::addLanguage(const string& language)
    {
        // Validate language support
        if (!Constants::Languages::isLanguageSupported(language)) {
            throw locale_exception("Language not supported: " + language);
        }

        // Check if already loaded
        auto it = this->translations.find(language);
        if (it != this->translations.end()) {
            return &(*it);
        }

        // Try to load from file
        if (!loadTranslationFile(language)) {
            // Create empty translation map if file doesn't exist
            translations[language] = unordered_map<string, string>();
        }

        auto result = this->translations.find(language);
        return &(*result);
    }

    /**
     * @brief Add a translation key-value pair to the system
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Adds a new translation entry or updates an existing one. If no language
     * is specified, uses the current locale. Optionally adds a fallback
     * translation to the fallback language.
     *
     * @param key Translation key identifier
     * @param translation Translation text for the key
     * @param language Target language code (empty uses current locale)
     * @param fallbackTranslation Optional translation for fallback language
     *
     * @note Automatically loads the target language if not already loaded
     */
    void i18n::addTranslation(const string& key, const string& translation,
                              const string& language, const string& fallbackTranslation)
    {
        string targetLanguage = language.empty() ? currentLocale.locale : language;

        // Ensure language exists
        if (!hasLanguage(targetLanguage)) {
            addLanguage(targetLanguage);
        }

        // Add translation
        translations[targetLanguage][key] = translation;

        // Add fallback translation if provided
        if (!fallbackTranslation.empty() && targetLanguage != fallbackLanguage) {
            if (!hasLanguage(fallbackLanguage)) {
                addLanguage(fallbackLanguage);
            }
            translations[fallbackLanguage][key] = fallbackTranslation;
        }
    }

    /**
     * @brief Get translation for a specific key
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Retrieves the translation for the given key in the specified language.
     * Falls back to the fallback language if translation is not found.
     * Returns the key itself if no translation is available.
     *
     * @param translationKey The key to translate
     * @param language Specific language code (empty uses current locale)
     * @return Translated string, fallback translation, or the key itself
     *
     * @note Implements automatic fallback chain: specified language → fallback language → key
     */
    string i18n::translate(const string& translationKey, const string& language) const
    {
        string targetLanguage = language.empty() ? currentLocale.locale : language;

        // Try target language first
        auto langIt = this->translations.find(targetLanguage);
        if (langIt != this->translations.end()) {
            auto transIt = langIt->second.find(translationKey);
            if (transIt != langIt->second.end()) {
                return transIt->second;
            }
        }

        // Try fallback language
        if (targetLanguage != fallbackLanguage) {
            auto fallbackIt = this->translations.find(fallbackLanguage);
            if (fallbackIt != this->translations.end()) {
                auto transIt = fallbackIt->second.find(translationKey);
                if (transIt != fallbackIt->second.end()) {
                    return transIt->second;
                }
            }
        }

        // Return key as last resort
        return translationKey;
    }

    /**
     * @brief Get translation with pluralization support
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Returns appropriate singular or plural translation based on the count value.
     * Uses language-specific pluralization rules where available, otherwise
     * uses simple English pluralization (count == 1 for singular).
     *
     * @param singularKey Translation key for singular form
     * @param pluralKey Translation key for plural form
     * @param count Number used to determine singular/plural form
     * @param language Specific language code (empty uses current locale)
     * @return Translated string in appropriate singular/plural form
     *
     * @note Falls back through the same chain as translate() for each form
     */
    string i18n::translatePlural(const string& singularKey,
                                 const string& pluralKey,
                                 int count,
                                 const string& language) const
    {
        // Simple English pluralization rules (can be extended for other languages)
        string key = (count == 1) ? singularKey : pluralKey;
        return translate(key, language);
    }

    /**
     * @brief Get translation with parameter substitution
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Retrieves translation for the key and performs parameter substitution
     * using the provided parameter map. Parameters in the translation text
     * should be formatted as {paramName} and will be replaced with values
     * from the parameters map.
     *
     * @param translationKey The key to translate
     * @param parameters Map of parameter names to replacement values
     * @param language Specific language code (empty uses current locale)
     * @return Translated string with parameters substituted
     *
     * @note Unmatched parameters in the translation are left unchanged
     * @example translate("hello.user", {{"name", "John"}}) → "Hello, John!"
     */
    string i18n::translateWithParams(const string& translationKey,
                                     const unordered_map<string, string>& parameters,
                                     const string& language) const
    {
        string result = translate(translationKey, language);

        // Replace parameters in format {param_name}
        for (const pair<const string, string>& param: parameters) {
            string placeholder = "{" + param.first + "}";
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != string::npos) {
                result.replace(pos, placeholder.length(), param.second);
                pos += param.second.length();
            }
        }

        return result;
    }

    /**
     * @brief Get all currently loaded/available language codes
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Returns a vector containing the language codes for all translation
     * sets that have been loaded into the system and are available for use.
     *
     * @return Vector of loaded language codes
     * @note This returns only loaded languages, not all supported languages
     * @see getSupportedLanguages() for all supported languages
     */
    vector<string> i18n::getAvailableLanguages() const
    {
        vector<string> languages;
        languages.reserve(this->translations.size());

        for (const auto& pair: translations) {
            languages.push_back(pair.first);
        }

        sort(languages.begin(), languages.end());

        return languages;
    }

    /**
     * @brief Get all supported language codes from the system
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Returns a complete list of all language codes that are supported
     * by the internationalization system, regardless of whether they
     * are currently loaded.
     *
     * @return Vector of all supported locale codes
     * @see Constants::Languages for language support definitions
     */
    vector<string> i18n::getSupportedLanguages()
    {
        vector<string> languages;
        auto supportedLocales = Constants::Languages::getSupportedLocales();

        languages.reserve(supportedLocales.size());
        for (const auto& locale: supportedLocales) {
            languages.emplace_back(locale);
        }

        sort(languages.begin(), languages.end());

        return languages;
    }

    /**
     * @brief Reload all translation files from disk
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Reloads translation data for all currently loaded languages from
     * their respective files. Useful for picking up changes made to
     * translation files during runtime.
     *
     * @return Number of languages successfully reloaded
     * @note Failed reloads are silently ignored and the old data is retained
     */
    size_t i18n::reloadTranslations()
    {
        size_t reloadedCount = 0;
        vector<string> languagesToReload = getAvailableLanguages();

        for (const string& lang: languagesToReload) {
            this->translations.erase(lang); // Remove existing
            if (loadTranslationFile(lang)) {
                reloadedCount++;
            }
            else {
                // Re-add empty map if file doesn't exist
                translations[lang] = unordered_map<string, string>();
            }
        }

        return reloadedCount;
    }

    /**
     * @brief Save translation data to file in appropriate format
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Saves the translation data for the specified language to a file.
     * Automatically detects the existing file format or defaults to
     * Properties format if no existing file is found.
     *
     * @param language      Language code to save
     * @param fileFormat    Format to save the file. By default the format is JSON
     *
     * @return true if saved successfully, false on error
     *
     * @note Supports saving to JSON, Properties, PO, and TXT formats
     * @see serializeJsonContent(), serializePropertiesContent(), serializePoContent()
     */
    bool i18n::saveTranslations(const string& language, const string& fileFormat) const
    {
        auto it = this->translations.find(language);
        if (it == this->translations.end()) {
            return false;
        }

        // Define file formats with their corresponding serializers
        struct FileFormat {
            string extension;
            function<bool(const unordered_map<string, string>&, const string&, ofstream&)> serializer;
        };

        vector<FileFormat> formats = {
                {
                    ".json",
                    [this](const unordered_map<string, string>& trans, const string& lang, ofstream& file) {
                        return serializeJsonContent(trans, lang, file);
                    }
                },
                {
                    ".properties",
                    [this](const unordered_map<string, string>& trans, const string& lang, ofstream& file) {
                        return serializePropertiesContent(trans, lang, file);
                    }
                },
                {
                    ".po",
                    [this](const unordered_map<string, string>& trans, const string& lang, ofstream& file) {
                        return serializePoContent(trans, lang, file);
                    }
                },
                {
                    ".txt",
                    [this](const unordered_map<string, string>& trans, const string& lang, ofstream& file) {
                        return serializePropertiesContent(trans, lang, file); // Same as properties
                    }
                }
        };

        // Try to find existing file format first
        for (const auto& format: formats) {
            filesystem::path filePath = baseFolder / (language + format.extension);

            if (filesystem::exists(filePath)) {
                try {
                    ofstream file(filePath);
                    if (!file.is_open()) continue;

                    if (format.serializer(it->second, language, file)) {
                        file.close();
                        return true;
                    }
                    file.close();
                }
                catch (const std::exception&) {
                    continue;
                }
            }
        }

        // If no existing file found, default to .properties format
        try {
            filesystem::path filePath = baseFolder / (language + fileFormat);
            ofstream file(filePath);

            if (!file.is_open()) {
                return false;
            }

            // Use properties serializer as default
            bool success = serializePropertiesContent(it->second, language, file);
            file.close();
            return success;
        }
        catch (const std::exception&) {
            return false;
        }
    }

    /**
     * @brief Serialize translation data to Java Properties file format
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Writes translations to an output stream in the standard Java Properties format
     * (key=value pairs). Keys are sorted alphabetically and a header comment is included
     * with language information and generation metadata.
     *
     * @param trans The translation map containing key-value pairs to serialize
     * @param language The language code for the translations (used in header comment)
     * @param file Output file stream to write the serialized data to
     * @return true if serialization was successful, false on error
     *
     * @note The file stream should be opened and ready for writing before calling
     * @see loadTranslationFile() for the corresponding parser
     */
    bool i18n::serializePropertiesContent(const unordered_map<string, string>& trans,
                                          const string& language, ofstream& file)
    {
        try {
            // Write header
            file << "# Translation file for " << language << "\n";
            file << "# Generated by Adventure Designer Studio i18n system\n\n";

            // Write translations sorted by key
            vector<pair<string, string> > sortedTranslations(trans.begin(), trans.end());
            sort(sortedTranslations.begin(), sortedTranslations.end());

            for (const auto& translation: sortedTranslations) {
                // Escape special characters if needed
                file << translation.first << "=" << translation.second << "\n";
            }
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }

    /**
     * @brief Serialize translation data to JSON file format
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Writes translations to an output stream as a JSON object with metadata.
     * The JSON structure includes a _metadata section with language and generation
     * information, followed by sorted key-value translation pairs. Special characters
     * are properly escaped according to JSON standards.
     *
     * @param trans The translation map containing key-value pairs to serialize
     * @param language The language code for the translations (included in metadata)
     * @param file Output file stream to write the serialized JSON data to
     * @return true if serialization was successful, false on error
     *
     * @note The file stream should be opened and ready for writing before calling
     * @see parseJsonContent() for the corresponding parser
     * @see escapeJsonString() for string escaping implementation
     */
    bool i18n::serializeJsonContent(const unordered_map<string, string>& trans,
                                    const string& language, ofstream& file) const
    {
        try {
            file << "{\n";
            file << "  \"_metadata\": {\n";
            file << "    \"language\": \"" << language << "\",\n";
            file << "    \"generated_by\": \"Adventure Designer Studio i18n system\"\n";
            file << "  },\n";

            // Write translations sorted by key
            vector<pair<string, string> > sortedTranslations(trans.begin(), trans.end());
            sort(sortedTranslations.begin(), sortedTranslations.end());

            for (size_t i = 0; i < sortedTranslations.size(); ++i) {
                const auto& translation = sortedTranslations[i];
                file << "  \"" << escapeJsonString(translation.first) << "\": \""
                        << escapeJsonString(translation.second) << "\"";

                if (i < sortedTranslations.size() - 1) {
                    file << ",";
                }
                file << "\n";
            }

            file << "}\n";
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }

    /**
     * @brief Serialize translation data to GNU gettext PO file format
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Writes translations to an output stream in the standard GNU gettext PO format.
     * Includes proper PO file headers with language and encoding information,
     * followed by msgid/msgstr pairs for each translation. Keys are sorted
     * alphabetically and strings are properly escaped for PO format.
     *
     * @param trans The translation map containing key-value pairs to serialize
     * @param language The language code for the translations (used in PO headers)
     * @param file Output file stream to write the serialized PO data to
     * @return true if serialization was successful, false on error
     *
     * @note The file stream should be opened and ready for writing before calling
     * @see parsePoContent() for the corresponding parser
     * @see escapePoString() for string escaping implementation
     */
    bool i18n::serializePoContent(const unordered_map<string, string>& trans,
                                  const string& language, ofstream& file) const
    {
        try {
            // Write PO header
            file << "# Translation file for " << language << "\n";
            file << "# Generated by Adventure Designer Studio i18n system\n";
            file << "msgid \"\"\n";
            file << "msgstr \"\"\n";
            file << "\"Language: " << language << "\\n\"\n";
            file << "\"Content-Type: text/plain; charset=UTF-8\\n\"\n\n";

            // Write translations sorted by key
            vector<pair<string, string> > sortedTranslations(trans.begin(), trans.end());
            sort(sortedTranslations.begin(), sortedTranslations.end());

            for (const auto& translation: sortedTranslations) {
                file << "msgid \"" << escapePoString(translation.first) << "\"\n";
                file << "msgstr \"" << escapePoString(translation.second) << "\"\n\n";
            }
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }

    /**
     * @brief Escape special characters in a string for JSON format
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Converts special characters to their JSON-escaped equivalents to ensure
     * the string can be safely included in a JSON document. Handles common
     * escape sequences like quotes, backslashes, newlines, carriage returns,
     * and tabs.
     *
     * @param str The input string to escape
     * @return A new string with JSON-escaped special characters
     *
     * @note Escaped characters: " → \", \ → \\, \n → \\n, \r → \\r, \t → \\t
     * @see serializeJsonContent() for usage context
     */
    string i18n::escapeJsonString(const string& str) const
    {
        string escaped;
        for (char c: str) {
            switch (c) {
            case '"':
                escaped += "\\\"";
                break;
            case '\\':
                escaped += "\\\\";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            default:
                escaped += c;
                break;
            }
        }
        return escaped;
    }

    /**
     * @brief Escape special characters in a string for PO file format
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Converts special characters to their PO-escaped equivalents to ensure
     * the string conforms to GNU gettext PO file standards. Handles the same
     * escape sequences as JSON format since PO files use similar escaping rules
     * for msgid and msgstr values.
     *
     * @param str The input string to escape
     * @return A new string with PO-escaped special characters
     *
     * @note Escaped characters: " → \", \ → \\, \n → \\n, \r → \\r, \t → \\t
     * @see serializePoContent() for usage context
     */
    string i18n::escapePoString(const string& str) const
    {
        string escaped;
        for (char c: str) {
            switch (c) {
            case '"':
                escaped += "\\\"";
                break;
            case '\\':
                escaped += "\\\\";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            default:
                escaped += c;
                break;
            }
        }
        return escaped;
    }

    /**
     * @brief Get translation statistics for all loaded languages
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Returns a map containing the number of translation entries for
     * each loaded language. Useful for analyzing translation coverage
     * and completeness.
     *
     * @return Map of language codes to translation counts
     */
    unordered_map<string, size_t> i18n::getTranslationStats() const
    {
        unordered_map<string, size_t> stats;

        for (const auto& lang: translations) {
            stats[lang.first] = lang.second.size();
        }

        return stats;
    }

    /**
     * @brief Find missing translation keys compared to fallback language
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Compares the specified language's translations with the fallback
     * language and returns a list of keys that exist in fallback but
     * are missing in the target language.
     *
     * @param language Language code to check for missing translations
     * @return Vector of translation keys missing in the specified language
     *
     * @note Useful for identifying incomplete translations during development
     */
    vector<string> i18n::findMissingTranslations(const string& language) const
    {
        vector<string> missing;

        auto fallbackIt = this->translations.find(fallbackLanguage);
        auto targetIt = this->translations.find(language);

        if (fallbackIt == this->translations.end() || targetIt == this->translations.end()) {
            return missing;
        }

        for (const auto& fallbackTrans: fallbackIt->second) {
            if (targetIt->second.find(fallbackTrans.first) == targetIt->second.end()) {
                missing.push_back(fallbackTrans.first);
            }
        }

        sort(missing.begin(), missing.end());
        return missing;
    }
}