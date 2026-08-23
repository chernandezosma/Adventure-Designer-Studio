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

#ifndef ADS_SELECT_EDITOR_H
#define ADS_SELECT_EDITOR_H

#include "IPropertyEditor.h"

namespace ADS::Inspector::Editors {
    /**
     * @brief Property editor for single- or multi-select values
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Renders a nullable single-select dropdown (descriptor.isMultiSelect()
     * == false) or a checklist-style multi-select box (== true), backed by
     * either a dynamic PropertyDescriptor::getOptionsProvider() callback or
     * a static PropertyConstraints::enumValues list. Used for fields whose
     * choices reference other entities in the project — e.g. a scene's
     * exits (single-select, nullable) or present items (multi-select).
     */
    class SelectEditor : public IPropertyEditor {
    public:
        /**
         * @brief Get the property types supported by this editor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return std::vector<PropertyType> List containing PropertyType::Select
         */
        std::vector<PropertyType> getSupportedTypes() const override;

        /**
         * @brief Render the select editor widget
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param descriptor Property metadata (label, multiSelect flag, options provider)
         * @param currentValue Current value (SelectValue)
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
         * @version May 2026
         *
         * @return std::string Editor identifier string
         */
        std::string getEditorId() const override;
    };
}

#endif //ADS_SELECT_EDITOR_H
