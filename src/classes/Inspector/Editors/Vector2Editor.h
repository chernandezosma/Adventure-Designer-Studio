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
