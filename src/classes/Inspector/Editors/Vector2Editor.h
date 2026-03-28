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
     * @version Jan 2026
     *
     * Renders two drag float controls for editing X and Y components
     * of a 2D vector (ImVec2).
     */
    class Vector2Editor : public IPropertyEditor {
    public:
        std::vector<PropertyType> getSupportedTypes() const override;
        EditResult render(
            const PropertyDescriptor& descriptor,
            const PropertyValue& currentValue,
            bool readOnly
        ) override;
        std::string getEditorId() const override;
    };
}

#endif //ADS_VECTOR2_EDITOR_H
