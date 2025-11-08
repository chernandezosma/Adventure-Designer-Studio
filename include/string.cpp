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

#include <string>
#include <vector>
#include <sstream>

/**
 * @brief Split a string by delimiter (PHP explode equivalent)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2025
 *
 * Splits a string into an array of substrings using the specified
 * delimiter character. Similar to PHP's explode() function.
 *
 * @param str String to split
 * @param delimiter Character to use as separator
 * @param trimEmpty Remove empty strings from result
 * @return Vector of string parts
 */
std::vector<std::string> explode(const std::string& str,
                                 const char delimiter,
                                 const bool trimEmpty)
{
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;

    if (str.empty()) {
        return result;
    }

    while (std::getline(ss, item, delimiter)) {
        if (!trimEmpty || !item.empty()) {
            result.push_back(item);
        }
    }

    return result;
}

/**
 * @brief Join vector of strings with delimiter (PHP implode equivalent)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2025
 *
 * Concatenates all elements of a vector into a single string,
 * separated by the specified glue string. Similar to PHP's implode()
 * or join() function.
 *
 * @param parts Vector of strings to join
 * @param glue Separator string to insert between elements
 * @return Joined string with all parts separated by glue
 *
 * @note Returns empty string if vector is empty
 */
std::string implode(const std::vector<std::string>& parts,
                    const std::string& glue)
{
    if (parts.empty()) {
        return "";
    }

    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += glue + parts[i];
    }

    return result;
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
 *
 * @return String with escape sequences converted to literal characters
 *
 * @note Used by parsePropertiesContent() and parsePoContent()
 */
std::string unescapeString(const std::string& str)
{
    std::string result;
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
std::string extractQuotedString(const std::string& str)
{
    std::string trimmed = str;

    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

    if (trimmed.length() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
        return unescapeString(trimmed.substr(1, trimmed.length() - 2));
    }

    return trimmed;
}
