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

#ifndef ADS_ENUM_EDITOR_H
#define ADS_ENUM_EDITOR_H

#include "IPropertyEditor.h"

namespace ADS::Inspector::Editors {
    /**
     * @brief Property editor for enumeration values
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Renders a combo box (dropdown) for selecting from predefined
     * enum values. The available options are stored in the EnumValue
     * or taken from the property constraints.
     */
    class EnumEditor : public IPropertyEditor {
    public:
        /**
         * @brief Get the property types supported by this editor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::vector<PropertyType> List containing PropertyType::Enum
         */
        std::vector<PropertyType> getSupportedTypes() const override;

        /**
         * @brief Render the enum editor widget
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Renders an ImGui Combo (dropdown) populated from the EnumValue's
         * option list. Returns an EditResult containing the new EnumValue
         * if the user selected a different option.
         *
         * @param descriptor Property metadata (label, constraints)
         * @param currentValue Current enum value (EnumValue)
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

#endif //ADS_ENUM_EDITOR_H