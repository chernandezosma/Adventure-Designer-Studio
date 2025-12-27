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


#ifndef ADS_STRING_H
#define ADS_STRING_H

#include <string>
#include <vector>

#include "uuid_v4.h"

/**
 * @brief Split a string by delimiter (PHP explode equivalent)
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Nov 2025
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
                                 char delimiter = ',',
                                 bool trimEmpty = false);

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
std::string unescapeString(const std::string& str);

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
                    const std::string& glue=", ");

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
std::string extractQuotedString(const std::string& str);

/**
 * @brief Trim specified characters from both ends of a string
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Nov 2025
 *
 * Removes all occurrences of the specified characters from the
 * beginning and end of the input string. The original string is
 * not modified; a new trimmed string is returned.
 *
 * @param str The string to be trimmed
 * @param charsToTrim String containing all characters to remove (default: whitespace)
 *
 * @return A new string with leading and trailing characters removed
 *
 * @note If the string contains only characters to trim, an empty string is returned
 * @note Default whitespace characters include space, tab, newline, and carriage return
 */
std::string trim(const std::string& str, const std::string& charsToTrim = " \t\n\r");

/**
 * @brief Convert string representation to boolean value
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Converts a string to a boolean value by checking against common
 * representations of "true". The comparison is case-insensitive and
 * automatically trims whitespace from the input string.
 *
 * Recognized true values: "TRUE", "1", "YES", "ON"
 * All other values (including empty string): false
 *
 * @param value String value to convert to boolean
 *
 * @return true if value matches a recognized true representation
 * @return false for all other values
 *
 * @note Comparison is case-insensitive (e.g., "true", "True", "TRUE" all return true)
 * @note Whitespace is automatically trimmed before comparison
 *
 * @example
 * stringToBool("true")   // returns true
 * stringToBool("TRUE")   // returns true
 * stringToBool("1")      // returns true
 * stringToBool("yes")    // returns true
 * stringToBool("on")     // returns true
 * stringToBool("false")  // returns false
 * stringToBool("0")      // returns false
 * stringToBool("")       // returns false
 */
bool stringToBool(const std::string& value);

/**
 * @brief Generate hash value for a string
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Computes a hash value for the given string using std::hash. This is
 * commonly used for creating unique identifiers or keys in hash-based
 * containers like unordered_map. Handles empty strings correctly by
 * returning their valid hash value.
 *
 * @param str The string to hash
 *
 * @return std::size_t Hash value of the input string
 *
 * @note Empty strings return a valid hash (implementation-defined but consistent)
 */
std::size_t makeHash(const std::string& str);

/**
 * @brief Generate a random UUIDv4
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Computes a random UUIDv4
 *
 * @return A new UUIDv4
 */
UUIDv4::UUID getRandomUuid();

#endif //ADS_STRING_H