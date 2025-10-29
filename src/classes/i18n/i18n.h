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
#ifndef I18N_H
#define I18N_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "base_exception.h"
#include "languages.h"

namespace ADS::i18n {
    using namespace std;

    /**
     * @struct LocaleInfo
     * @brief Contains locale information in POSIX format with language details
     *
     * @autor   Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Stores locale data including POSIX format string (e.g., "es_ES") and
     * human-readable language name (e.g., "Spanish (Spain)"). Provides utility
     * methods for extracting language and country codes.
     */
    struct LocaleInfo {
        string locale; // POSIX format (e.g., "es_ES")
        string language; // Full name (e.g., "Spanish (Spain)")

        /**
         * @brief Extract the language code from locale string
         *
         * @autor   Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * Extracts the language portion from a POSIX locale string by finding
         * the underscore separator and returning the substring before it.
         *
         * @return Language code (e.g., "es" from "es_ES")
         * @note Returns the entire locale string if no underscore is found
         */
        [[nodiscard]] string getLanguageCode() const {
            size_t underscorePos = locale.find('_');
            return (underscorePos != string::npos) ? locale.substr(0, underscorePos) : locale;
        }

        /**
         * @brief Extract the language code from locale string
         *
         * @autor   Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * Extracts the language portion from a POSIX locale string by finding
         * the underscore separator and returning the substring before it.
         *
         * @return Language code (e.g., "es" from "es_ES")
         * @note Returns the entire locale string if no underscore is found
         */
        [[nodiscard]] string getCountryCode() const {
            size_t underscorePos = locale.find('_');
            if (underscorePos != string::npos && underscorePos + 1 < locale.length()) {
                return locale.substr(underscorePos + 1);
            }
            return "";
        }

        /**
         * @brief Validate the locale information
         *
         * @autor   Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * Checks if the locale data is valid by verifying that both locale
         * and language strings are non-empty and the locale is supported
         * by the system.
         *
         * @return true if locale is valid and supported, false otherwise
         * @see Constants::Languages::isLanguageSupported()
         */
        [[nodiscard]] bool isValid() const {
            return !locale.empty() &&
                   !language.empty() &&
                   Constants::Languages::isLanguageSupported(locale);
        }
    };

    /**
     * @class i18n
     * @brief Main internationalization class for translation management
     *
     * @autor   Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Provides comprehensive internationalization functionality including:
     * - Multiple file format support (JSON, Properties, PO files)
     * - Automatic locale detection and normalization
     * - Translation loading, saving, and management
     * - Pluralization and parameter substitution
     * - Fallback language support
     * - Translation statistics and validation
     */
    class i18n {
    private:
        /**
         * Translations storage.
         * Stores translations under the language identifier like es_ES
         */
        unordered_map<string, unordered_map<string, string> > translations;

        /**
         * Current system locale information
         */
        LocaleInfo systemLocale;

        /**
         * Currently active locale
         */
        LocaleInfo currentLocale;

        /**
         * Raw locale string from system
         */
        string rawLocaleString;

        /**
         * Base directory path for translation files
         */
        filesystem::path baseFolder;

        /**
         * Default language for new translations
         */
        string defaultLanguage;

        /**
         * Fallback language when translation is not found
         */
        string fallbackLanguage;

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
        void init();

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
        void extractSystemLocale();

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
        bool loadTranslationFile(const string &language);

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
        bool parseJsonContent(const string &content,
                              unordered_map<string, string> &translations);

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
        bool parsePropertiesContent(const string &content,
                                    unordered_map<string, string> &translations);

        /**
         * @brief Parse GNU gettext PO (Portable Object) format file content
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version 2025
         *
         * Parses standard PO file format used by GNU gettext. Extracts msgid
         * and msgstr pairs, handles multi-line strings, and processes escape
         * sequences. Skips header entries and comments.
         *
         * @param content PO file content as string
         * @param translations Output map to store parsed msgid-msgstr pairs
         * @return true if parsing was successful, false on error
         *
         * @see extractQuotedString(), unescapeString()
         */
        bool parsePoContent(const string &content,
                            unordered_map<string, string> &translations);

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
        void parseNestedJson(const string& prefix, const nlohmann::json& obj,
                           unordered_map<string, string>& translations);
#endif

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
        string extractQuotedString(const string &str);

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
        static string unescapeString(const string &str);

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
        LocaleInfo createLocaleInfo(const string &localeCode) const;

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
        static bool serializePropertiesContent(const unordered_map<string, string> &trans,
                                        const string &language, ofstream &file) ;

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
        bool serializeJsonContent(const unordered_map<string, string> &trans,
                                    const string &language, ofstream &file) const;

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
        bool serializePoContent(const unordered_map<string, string> &trans,
                                          const string &language, ofstream &file) const;

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
        string escapeJsonString(const string &str) const;

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
        string escapePoString(const string &str) const;

    public:
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
        explicit i18n(const string &baseFolder,
                      const string &fallback = string(Constants::Languages::DEFAULT_FALLBACK));

        /**
         * @brief Destructor - cleanup resources
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * Default destructor that automatically cleans up translation data
         * and releases any resources used by the i18n system.
         */
        ~i18n() = default;

        // Copy/Move constructors and assignment operators
        i18n(const i18n &) = delete;
        i18n &operator=(const i18n &) = delete;
        i18n(i18n &&) = default;
        i18n &operator=(i18n &&) = default;

        /**
         * @brief Get the configured fallback language code
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * Returns the language code used as fallback when requested translations
         * are not found in the current or specified language.
         *
         * @return Fallback language code string
         */
        [[nodiscard]] string getFallbackLanguage() const;

        /**
         * @brief Get the configured default language code
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * Returns the default language code used for new translations when
         * no specific language is specified.
         *
         * @return Default language code string
         */
        [[nodiscard]] string getDefaultLanguage() const;

        /**
         * @brief Get the base folder path for translation files
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * Returns the absolute path to the directory where translation files
         * are stored and loaded from.
         *
         * @return Base folder path as string
         */
        [[nodiscard]] string getBaseFolder() const;

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
        void setLocale(const LocaleInfo &locale);

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
        void setLocale(const string &localeCode);

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
        [[nodiscard]] LocaleInfo getCurrentLocale() const;

        /**
         * @brief Get the detected system locale information
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * Returns the LocaleInfo object representing the system's default locale
         * as detected during initialization from environment variables.
         *
         * @return System LocaleInfo object
         * @see extractSystemLocale()
         */
        [[nodiscard]] LocaleInfo getSystemLocale() const;

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
        [[nodiscard]] unordered_map<string, string> getTranslations(const string &language = "") const;

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
        [[nodiscard]] const pair<const string, unordered_map<string, string> > *
        getFallbackLanguageTranslations() const;

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
        [[nodiscard]] const pair<const string, unordered_map<string, string> > *
        getLanguage(const string &language) const;

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
        [[nodiscard]] bool hasLanguage(const string &language) const;

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
        pair<const string, unordered_map<string, string> > *addLanguage(const string &language);

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
        void addTranslation(const string &key,
                            const string &translation,
                            const string &language = "",
                            const string &fallbackTranslation = "");

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
        [[nodiscard]] string translate(const string &translationKey,
                                       const string &language = "") const;

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
        [[nodiscard]] string translatePlural(const string &singularKey,
                                             const string &pluralKey,
                                             int count,
                                             const string &language = "") const;

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
        [[nodiscard]] string translateWithParams(const string &translationKey,
                                                 const unordered_map<string, string> &parameters,
                                                 const string &language = "") const;

        /**
         * @brief Get the system locale's language code
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * Convenience method that returns just the language code portion
         * of the detected system locale (e.g., "es" from "es_ES").
         *
         * @return System locale language code
         * @see getSystemLocale(), LocaleInfo::getLanguageCode()
         */
        [[nodiscard]] string getSystemLocaleLanguage() const;

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
        [[nodiscard]] vector<string> getAvailableLanguages() const;

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
        [[nodiscard]] static vector<string> getSupportedLanguages();

        /**
         * @brief Check if a locale code is supported by the system
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * Validates whether the given locale code is recognized and supported
         * by the internationalization system for loading and translation.
         *
         * @param localeCode The locale code to validate
         * @return true if the locale is supported, false otherwise
         * @see Constants::Languages::isLanguageSupported()
         */
        [[nodiscard]] static bool isLocaleSupported(const string &localeCode);

        /**
         * @brief Normalize platform-specific locale to POSIX format
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * Converts platform-specific locale strings (e.g., Windows "English_United States")
         * to standardized POSIX format (e.g., "en_US"). Handles various platform
         * locale formats and naming conventions.
         *
         * @param platformLocale Platform-specific locale string
         * @return Normalized POSIX locale code
         * @see Constants::Languages::normalizePlatformLocale()
         */
        [[nodiscard]] static string normalizePlatformLocale(const string &platformLocale);

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
        size_t reloadTranslations();

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
        bool saveTranslations(const string& language, const string& fileFormat = ".json") const;

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
        [[nodiscard]] unordered_map<string, size_t> getTranslationStats() const;

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
        [[nodiscard]] vector<string> findMissingTranslations(const string &language) const;
    };

    /**
     * @class i18n_exception
     * @brief Base exception class for internationalization-related errors
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * General exception type for errors that occur within the i18n system.
     * Serves as the base class for more specific i18n exception types.
     *
     * @see locale_exception, translation_file_exception
     */
    class i18n_exception : public Exceptions::BaseException {
    public:
        /**
         * @brief Construct i18n exception with error message
         * @param message Descriptive error message
         */
        explicit i18n_exception(const string &message) : BaseException(message) {}
    };

    /**
     * @class locale_exception
     * @brief Exception for locale-related errors
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Thrown when locale detection, validation, or configuration operations
     * fail. Includes automatic "Locale error:" prefix for clear error identification.
     */
    class locale_exception : public i18n_exception {
    public:
        /**
         * @brief Construct locale exception with error message
         * @param message Descriptive error message (automatically prefixed)
         */
        explicit locale_exception(const string &message) : i18n_exception("Locale error: " + message) {}
    };

    /**
     * @class translation_file_exception
     * @brief Exception for translation file operation errors
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jul 2025
     *
     * Thrown when translation file loading, parsing, or saving operations
     * fail. Includes automatic "Translation file error:" prefix for clear
     * error identification.
     */
    class translation_file_exception : public i18n_exception {
    public:
        /**
         * @brief Construct translation file exception with error message
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jul 2025
         *
         * @param message Descriptive error message (automatically prefixed)
         */
        explicit translation_file_exception(const string &message)
            : i18n_exception("Translation file error: " + message) {}
    };
}

#endif // I18N_H
