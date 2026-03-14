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

#ifndef ADS_FLOAT_EDITOR_H
#define ADS_FLOAT_EDITOR_H

#include "IPropertyEditor.h"

namespace ADS::Inspector::Editors {
    /**
     * @brief Property editor for floating-point values
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Renders a slider or drag control for editing float properties.
     * Uses SliderFloat when min/max constraints are specified, otherwise
     * uses DragFloat for unconstrained values.
     */
    class FloatEditor : public IPropertyEditor {
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

#endif //ADS_FLOAT_EDITOR_H