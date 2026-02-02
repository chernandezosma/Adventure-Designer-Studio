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

#ifndef ADS_IPROPERTY_EDITOR_H
#define ADS_IPROPERTY_EDITOR_H

#include <vector>
#include <string>
#include "../PropertyType.h"
#include "../PropertyValue.h"
#include "../PropertyDescriptor.h"

namespace ADS::Inspector::Editors {
    /**
     * @brief Result of a property edit operation
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     */
    struct EditResult {
        bool changed;           ///< Whether the value was modified
        PropertyValue newValue; ///< The new value (valid only if changed is true)

        EditResult() : changed(false) {}
        EditResult(bool wasChanged, const PropertyValue& value)
            : changed(wasChanged), newValue(value) {}

        /**
         * @brief Create an unchanged result
         */
        static EditResult unchanged() {
            return EditResult();
        }

        /**
         * @brief Create a changed result with the new value
         */
        static EditResult modified(const PropertyValue& value) {
            return EditResult(true, value);
        }
    };

    /**
     * @brief Base interface for property editors
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Property editors are responsible for rendering the UI controls
     * that allow users to view and edit property values. Each editor
     * handles one or more property types.
     *
     * Editors work with ImGui's immediate mode paradigm, meaning they
     * render their controls every frame and return whether the value
     * was changed.
     */
    class IPropertyEditor {
    public:
        /**
         * @brief Virtual destructor
         */
        virtual ~IPropertyEditor() = default;

        /**
         * @brief Get the property types this editor can handle
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @return std::vector<PropertyType> List of supported types
         */
        virtual std::vector<PropertyType> getSupportedTypes() const = 0;

        /**
         * @brief Render the editor for a property
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Renders the ImGui controls for editing the property value.
         * Returns an EditResult indicating whether the value was changed
         * and what the new value is.
         *
         * @param descriptor Property metadata
         * @param currentValue Current property value
         * @param readOnly True if the property cannot be edited
         * @return EditResult Result of the edit operation
         */
        virtual EditResult render(
            const PropertyDescriptor& descriptor,
            const PropertyValue& currentValue,
            bool readOnly
        ) = 0;

        /**
         * @brief Get a unique identifier for this editor type
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @return std::string Unique editor identifier
         */
        virtual std::string getEditorId() const = 0;
    };
}

#endif //ADS_IPROPERTY_EDITOR_H