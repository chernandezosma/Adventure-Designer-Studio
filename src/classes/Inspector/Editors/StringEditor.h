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

#ifndef ADS_STRING_EDITOR_H
#define ADS_STRING_EDITOR_H

#include "IPropertyEditor.h"

namespace ADS::Inspector::Editors {
    /**
     * @brief Property editor for string values
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Renders a text input field for editing string properties.
     * Respects maxLength constraints if specified.
     */
    class StringEditor : public IPropertyEditor {
    private:
        static constexpr size_t DEFAULT_BUFFER_SIZE = 1024;
        /// Scratch buffer backing the inline single-line/compact multiline
        /// InputText widget (not the expand-to-dialog popup — see m_dialogBuffer).
        char m_buffer[DEFAULT_BUFFER_SIZE];

        /// Scratch buffer for the multiline expand-to-dialog "…" popup
        /// (PropertyConstraints::multiline). Larger than m_buffer since
        /// long-form prose isn't bound by the compact field's maxLength
        /// the same way. Reused across properties — safe because only one
        /// modal popup can be interactively open at a time.
        static constexpr size_t DIALOG_BUFFER_SIZE = 8192;
        char m_dialogBuffer[DIALOG_BUFFER_SIZE];

        /// Scratch state for a translatable field's dialog
        /// (PropertyConstraints::translatable). Holds the full per-language
        /// map being edited and the target language (always the project
        /// default) m_dialogBuffer reflects. Reused across properties — safe
        /// for the same reason as m_dialogBuffer (only one modal popup open
        /// at a time).
        LocalizedText m_dialogLocalizedText;
        std::string m_dialogLanguage;

        /**
         * @brief Render a translatable String property
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Read-only inline preview of the default-language text, plus a
         * "…" button opening a dialog with a single multiline box. Editing
         * targets the project's default language only; multi-language
         * authoring lives in the Translations panel. See
         * PropertyConstraints::translatable.
         *
         * @param descriptor Property metadata
         * @param currentValue Current value — expected to hold LocalizedText
         * @param readOnly If true, the "…" button is disabled
         * @return EditResult New LocalizedText map on change (OK), unchanged on Cancel
         */
        EditResult renderTranslatable(
            const PropertyDescriptor& descriptor,
            const PropertyValue& currentValue,
            bool readOnly
        );

    public:
        /**
         * @brief Construct a new StringEditor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Initialises the internal character buffer to empty.
         */
        StringEditor();

        /**
         * @brief Get the property types supported by this editor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::vector<PropertyType> List containing PropertyType::String
         */
        std::vector<PropertyType> getSupportedTypes() const override;

        /**
         * @brief Render the string editor widget
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Renders an ImGui InputText field. Applies maxLength constraint
         * from the descriptor if present. Returns an EditResult containing
         * the new string if the user committed a change.
         *
         * @param descriptor Property metadata (label, constraints)
         * @param currentValue Current string value
         * @param readOnly If true, the widget is displayed but not editable
         * @return EditResult New value on change, empty result otherwise
         */
        EditResult render(
            const PropertyDescriptor& descriptor,
            const PropertyValue& currentValue,
            bool readOnly
        ) override;

        /**
         * @brief Get the unique identifier for this editor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::string Editor identifier string
         */
        std::string getEditorId() const override;
    };
}

#endif //ADS_STRING_EDITOR_H