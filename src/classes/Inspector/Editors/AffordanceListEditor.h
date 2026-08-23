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

#ifndef ADS_AFFORDANCE_LIST_EDITOR_H
#define ADS_AFFORDANCE_LIST_EDITOR_H

#include "IPropertyEditor.h"

namespace ADS::Inspector::Editors {
    /**
     * @brief Property editor for PropertyType::AffordanceList
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Renders an addable/removable list of rows, each row holding an
     * editable affordance name and its own addable/removable list of
     * trigger-name strings. descriptor.getOptionsProvider() (if set)
     * supplies a suggested/known affordance-name catalog offered as a
     * quick-add preset; typing a custom name is always available.
     */
    class AffordanceListEditor : public IPropertyEditor {
    public:
        /**
         * @brief Get the property types supported by this editor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @return std::vector<PropertyType> List containing PropertyType::AffordanceList
         */
        std::vector<PropertyType> getSupportedTypes() const override;

        /**
         * @brief Render the affordance-list editor widget
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @param descriptor Property metadata (label, options provider for presets)
         * @param currentValue Current value (AffordanceListValue)
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
         * @version Sep 2026
         *
         * @return std::string Editor identifier string
         */
        std::string getEditorId() const override;
    };
}

#endif //ADS_AFFORDANCE_LIST_EDITOR_H
