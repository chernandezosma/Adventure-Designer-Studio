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

#ifndef ADS_VECTOR2_EDITOR_H
#define ADS_VECTOR2_EDITOR_H

#include "IPropertyEditor.h"

namespace ADS::Inspector::Editors {
    /**
     * @brief Property editor for 2D vector values
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Mar 2026
     *
     * Renders two drag float controls for editing X and Y components
     * of a 2D vector (ImVec2).
     */
    class Vector2Editor : public IPropertyEditor {
    public:
        /**
         * @brief Get the property types supported by this editor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @return std::vector<PropertyType> List containing PropertyType::Vector2
         */
        std::vector<PropertyType> getSupportedTypes() const override;

        /**
         * @brief Render the 2D vector editor widget
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * Renders two side-by-side DragFloat controls for the X and Y
         * components. Returns an EditResult containing the new ImVec2
         * if either component changed.
         *
         * @param descriptor Property metadata (label, constraints)
         * @param currentValue Current vector value (ImVec2)
         * @param readOnly If true, the widgets are displayed but not editable
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

#endif //ADS_VECTOR2_EDITOR_H