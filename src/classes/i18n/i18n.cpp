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
    // Constructor
    i18n::i18n(const string &baseFolder, const string &fallback)
        : baseFolder(std::filesystem::current_path() / baseFolder)
          , fallbackLanguage(fallback) {
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

    void i18n::init() {
        try {
            // Extract system locale information
            this->extractSystemLocale();

            // Set current locale to system locale (fallback to fallback language if system not supported)
            if (Constants::Languages::isLanguageSupported(systemLocale.locale)) {
                this->currentLocale = systemLocale;
            } else {
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
                } catch (const std::exception &) {
                    // If current locale can't be loaded, stick with fallback
                }
            }
        } catch (const std::exception &e) {
            throw i18n_exception("Failed to initialize i18n system: " + string(e.what()));
        }
    }

    void i18n::extractSystemLocale() {
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
                const char *envLocale = nullptr;

#ifdef _WIN32
                if ((envLocale = std::getenv("LANG")) && strlen(envLocale) > 0) {
                    normalizedLocale = Constants::Languages::normalizePlatformLocale(envLocale);
                }
#else
                const char *envVars[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
                for (const char *var: envVars) {
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
        } catch (const std::exception &e) {
            // Ultimate fallback
            this->systemLocale = this->createLocaleInfo(fallbackLanguage);
        }
    }

    LocaleInfo i18n::createLocaleInfo(const string &localeCode) const {
        LocaleInfo locale;

        locale.locale = localeCode;
        locale.language = Constants::Languages::getLanguageName(localeCode);

        if (locale.language.empty()) {
            throw locale_exception("Unsupported locale: " + localeCode);
        }

        return locale;
    }

    [[nodiscard]] LocaleInfo i18n::getCurrentLocale() const {
        return this->currentLocale;
    }

    bool i18n::loadTranslationFile(const string &language) {
        // Define file extensions with their corresponding parsers
        struct FileFormat {
            string extension;
            function<bool(const string &, unordered_map<string, string> &)> parser;
        };

        vector<FileFormat> formats = {
            {
                ".json", [this](const string &content, unordered_map<string, string> &trans) {
                    return parseJsonContent(content, trans);
                }
            },
            {
                ".properties", [this](const string &content, unordered_map<string, string> &trans) {
                    return parsePropertiesContent(content, trans);
                }
            },
            {
                ".po", [this](const string &content, unordered_map<string, string> &trans) {
                    return parsePoContent(content, trans);
                }
            },
            {
                ".txt", [this](const string &content, unordered_map<string, string> &trans) {
                    return parsePropertiesContent(content, trans); // Same as properties
                }
            }
        };

        for (const auto &format: formats) {
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
                } catch (const std::exception &) {
                    continue;
                }
            }
        }

        return false;
    }

    bool i18n::parseJsonContent(const string &content, unordered_map<string, string> &translations) {
        try {
#ifdef NLOHMANN_JSON_VERSION_MAJOR
            nlohmann::json jsonData = nlohmann::json::parse(content);

            // Handle flat JSON object: {"key": "value", ...}
            if (jsonData.is_object()) {
                for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                    if (it.value().is_string()) {
                        translations[it.key()] = it.value().get<string>();
                    } else if (it.value().is_object()) {
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
        } catch (const std::exception &) {
            // If JSON parsing fails, try properties format as fallback
            return parsePropertiesContent(content, translations);
        }

        return false;
    }

#ifdef NLOHMANN_JSON_VERSION_MAJOR
    void i18n::parseNestedJson(const string& prefix, const nlohmann::json& obj,
                              unordered_map<string, string>& translations) {
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            string key = prefix.empty() ? it.key() : prefix + "." + it.key();

            if (it.value().is_string()) {
                translations[key] = it.value().get<string>();
            } else if (it.value().is_object()) {
                parseNestedJson(key, it.value(), translations);
            }
        }
    }
#endif

    bool i18n::parsePropertiesContent(const string &content, unordered_map<string, string> &translations) {
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

    bool i18n::parsePoContent(const string &content, unordered_map<string, string> &translations) {
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
            } else if (line.substr(0, 7) == "msgstr ") {
                currentMsgstr = extractQuotedString(line.substr(7));
                inMsgid = false;
                inMsgstr = true;
            } else if (line[0] == '"' && (inMsgid || inMsgstr)) {
                // Continuation line
                string continuation = extractQuotedString(line);
                if (inMsgid) {
                    currentMsgid += continuation;
                } else if (inMsgstr) {
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

    string i18n::extractQuotedString(const string &str) {
        string trimmed = str;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

        if (trimmed.length() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
            return unescapeString(trimmed.substr(1, trimmed.length() - 2));
        }

        return trimmed;
    }

    string i18n::unescapeString(const string &str) {
        string result;
        result.reserve(str.length());

        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '\\' && i + 1 < str.length()) {
                switch (str[i + 1]) {
                    case 'n': result += '\n';
                        ++i;
                        break;
                    case 't': result += '\t';
                        ++i;
                        break;
                    case 'r': result += '\r';
                        ++i;
                        break;
                    case '\\': result += '\\';
                        ++i;
                        break;
                    case '"': result += '"';
                        ++i;
                        break;
                    case '\'': result += '\'';
                        ++i;
                        break;
                    default: result += str[i];
                        break;
                }
            } else {
                result += str[i];
            }
        }

        return result;
    }

    void i18n::setLocale(const LocaleInfo &locale) {
        if (!locale.isValid()) {
            throw locale_exception("Invalid locale: " + locale.locale);
        }

        this->currentLocale = locale;

        // Ensure the locale is loaded
        if (!hasLanguage(locale.locale)) {
            this->addLanguage(locale.locale);
        }
    }

    void i18n::setLocale(const string &localeCode) {
        LocaleInfo locale = createLocaleInfo(localeCode);
        setLocale(locale);
    }

    unordered_map<string, string> i18n::getTranslations(const string &language) const {
        string targetLanguage = language.empty() ? currentLocale.locale : language;

        auto it = this->translations.find(targetLanguage);
        if (it != this->translations.end()) {
            return it->second;
        }

        return {};
    }

    const pair<const string, unordered_map<string, string> > *i18n::getFallbackLanguageTranslations() const {
        auto it = this->translations.find(fallbackLanguage);
        return (it != this->translations.end()) ? &(*it) : nullptr;
    }

    const pair<const string, unordered_map<string, string> > *i18n::getLanguage(const string &language) const {
        auto it = this->translations.find(language);
        return (it != this->translations.end()) ? &(*it) : nullptr;
    }

    bool i18n::hasLanguage(const string &language) const {
        return this->translations.find(language) != this->translations.end();
    }

    pair<const string, unordered_map<string, string> > *i18n::addLanguage(const string &language) {
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

    void i18n::addTranslation(const string &key, const string &translation,
                              const string &language, const string &fallbackTranslation) {
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

    string i18n::translate(const string &translationKey, const string &language) const {
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

    string i18n::translatePlural(const string &singularKey, const string &pluralKey,
                                 int count, const string &language) const {
        // Simple English pluralization rules (can be extended for other languages)
        string key = (count == 1) ? singularKey : pluralKey;
        return translate(key, language);
    }

    string i18n::translateWithParams(const string &translationKey,
                                     const unordered_map<string, string> &parameters,
                                     const string &language) const {
        string result = translate(translationKey, language);

        // Replace parameters in format {param_name}
        for (const pair<const string, string> &param: parameters) {
            string placeholder = "{" + param.first + "}";
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != string::npos) {
                result.replace(pos, placeholder.length(), param.second);
                pos += param.second.length();
            }
        }

        return result;
    }

    vector<string> i18n::getAvailableLanguages() const {
        vector<string> languages;
        languages.reserve(this->translations.size());

        for (const auto &pair: translations) {
            languages.push_back(pair.first);
        }

        sort(languages.begin(), languages.end());

        return languages;
    }

    vector<string> i18n::getSupportedLanguages() {
        vector<string> languages;
        auto supportedLocales = Constants::Languages::getSupportedLocales();

        languages.reserve(supportedLocales.size());
        for (const auto &locale: supportedLocales) {
            languages.emplace_back(locale);
        }

        sort(languages.begin(), languages.end());

        return languages;
    }

    size_t i18n::reloadTranslations() {
        size_t reloadedCount = 0;
        vector<string> languagesToReload = getAvailableLanguages();

        for (const string &lang: languagesToReload) {
            this->translations.erase(lang); // Remove existing
            if (loadTranslationFile(lang)) {
                reloadedCount++;
            } else {
                // Re-add empty map if file doesn't exist
                translations[lang] = unordered_map<string, string>();
            }
        }

        return reloadedCount;
    }

    bool i18n::saveTranslations(const string &language) const {
        auto it = this->translations.find(language);
        if (it == this->translations.end()) {
            return false;
        }

        // Define file formats with their corresponding serializers
        struct FileFormat {
            string extension;
            function<bool(const unordered_map<string, string> &, const string &, ofstream &)> serializer;
        };

        vector<FileFormat> formats = {
            {
                ".json", [this](const unordered_map<string, string> &trans, const string &lang, ofstream &file) {
                    return serializeJsonContent(trans, lang, file);
                }
            },
            {
                ".properties", [this](const unordered_map<string, string> &trans, const string &lang, ofstream &file) {
                    return serializePropertiesContent(trans, lang, file);
                }
            },
            {
                ".po", [this](const unordered_map<string, string> &trans, const string &lang, ofstream &file) {
                    return serializePoContent(trans, lang, file);
                }
            },
            {
                ".txt", [this](const unordered_map<string, string> &trans, const string &lang, ofstream &file) {
                    return serializePropertiesContent(trans, lang, file); // Same as properties
                }
            }
        };

        // Try to find existing file format first
        for (const auto &format: formats) {
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
                } catch (const std::exception &) {
                    continue;
                }
            }
        }

        // If no existing file found, default to .properties format
        try {
            filesystem::path filePath = baseFolder / (language + ".properties");
            ofstream file(filePath);

            if (!file.is_open()) {
                return false;
            }

            // Use properties serializer as default
            bool success = serializePropertiesContent(it->second, language, file);
            file.close();
            return success;
        } catch (const std::exception &) {
            return false;
        }
    }

    // Helper serializer functions (you'll need to implement these)
    bool i18n::serializePropertiesContent(const unordered_map<string, string> &trans,
                                          const string &language, ofstream &file) const {
        try {
            // Write header
            file << "# Translation file for " << language << "\n";
            file << "# Generated by Adventure Designer Studio i18n system\n\n";

            // Write translations sorted by key
            vector<pair<string, string> > sortedTranslations(trans.begin(), trans.end());
            sort(sortedTranslations.begin(), sortedTranslations.end());

            for (const auto &translation: sortedTranslations) {
                // Escape special characters if needed
                file << translation.first << "=" << translation.second << "\n";
            }
            return true;
        } catch (const std::exception &) {
            return false;
        }
    }

    bool i18n::serializeJsonContent(const unordered_map<string, string> &trans,
                                    const string &language, ofstream &file) const {
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
                const auto &translation = sortedTranslations[i];
                file << "  \"" << escapeJsonString(translation.first) << "\": \""
                        << escapeJsonString(translation.second) << "\"";

                if (i < sortedTranslations.size() - 1) {
                    file << ",";
                }
                file << "\n";
            }

            file << "}\n";
            return true;
        } catch (const std::exception &) {
            return false;
        }
    }

    bool i18n::serializePoContent(const unordered_map<string, string> &trans,
                                  const string &language, ofstream &file) const {
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

            for (const auto &translation: sortedTranslations) {
                file << "msgid \"" << escapePoString(translation.first) << "\"\n";
                file << "msgstr \"" << escapePoString(translation.second) << "\"\n\n";
            }
            return true;
        } catch (const std::exception &) {
            return false;
        }
    }

    // May we need to refactor these two functions in only one?
    // Helper function to escape JSON strings
    string i18n::escapeJsonString(const string &str) const {
        string escaped;
        for (char c: str) {
            switch (c) {
                case '"': escaped += "\\\"";
                    break;
                case '\\': escaped += "\\\\";
                    break;
                case '\n': escaped += "\\n";
                    break;
                case '\r': escaped += "\\r";
                    break;
                case '\t': escaped += "\\t";
                    break;
                default: escaped += c;
                    break;
            }
        }
        return escaped;
    }

    // Helper function to escape PO strings
    string i18n::escapePoString(const string &str) const {
        string escaped;
        for (char c: str) {
            switch (c) {
                case '"': escaped += "\\\"";
                    break;
                case '\\': escaped += "\\\\";
                    break;
                case '\n': escaped += "\\n";
                    break;
                case '\r': escaped += "\\r";
                    break;
                case '\t': escaped += "\\t";
                    break;
                default: escaped += c;
                    break;
            }
        }
        return escaped;
    }

    unordered_map<string, size_t> i18n::getTranslationStats() const {
        unordered_map<string, size_t> stats;

        for (const auto &lang: translations) {
            stats[lang.first] = lang.second.size();
        }

        return stats;
    }

    vector<string> i18n::findMissingTranslations(const string &language) const {
        vector<string> missing;

        auto fallbackIt = this->translations.find(fallbackLanguage);
        auto targetIt = this->translations.find(language);

        if (fallbackIt == this->translations.end() || targetIt == this->translations.end()) {
            return missing;
        }

        for (const auto &fallbackTrans: fallbackIt->second) {
            if (targetIt->second.find(fallbackTrans.first) == targetIt->second.end()) {
                missing.push_back(fallbackTrans.first);
            }
        }

        sort(missing.begin(), missing.end());
        return missing;
    }
}
