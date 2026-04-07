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

#ifndef ADS_PROPERTY_EDITOR_REGISTRY_H
#define ADS_PROPERTY_EDITOR_REGISTRY_H

#include <unordered_map>
#include <memory>
#include <string>
#include "PropertyType.h"
#include "PropertyDescriptor.h"
#include "Editors/IPropertyEditor.h"

namespace ADS::Inspector {
    /**
     * @brief Registry and factory for property editors
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Manages the mapping between property types and their editors.
     * Supports both type-based and property-ID-based editor lookup
     * for custom editor overrides.
     */
    class PropertyEditorRegistry {
    private:
        /// Map of property type to editor
        std::unordered_map<PropertyType, std::unique_ptr<Editors::IPropertyEditor>> m_typeEditors;

        /// Map of property ID to custom editor (for overrides)
        std::unordered_map<std::string, std::unique_ptr<Editors::IPropertyEditor>> m_propertyEditors;

    public:
        /**
         * @brief Construct a new PropertyEditorRegistry
         *
         * Automatically registers all default editors.
         */
        PropertyEditorRegistry();

        /**
         * @brief Destroy the PropertyEditorRegistry
         */
        ~PropertyEditorRegistry() = default;

        // Non-copyable
        PropertyEditorRegistry(const PropertyEditorRegistry&) = delete;
        PropertyEditorRegistry& operator=(const PropertyEditorRegistry&) = delete;

        // Movable
        PropertyEditorRegistry(PropertyEditorRegistry&&) = default;
        PropertyEditorRegistry& operator=(PropertyEditorRegistry&&) = default;

        /**
         * @brief Register an editor for a property type
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param type The property type to handle
         * @param editor The editor instance (takes ownership)
         */
        void registerEditor(
            PropertyType type,
            std::unique_ptr<Editors::IPropertyEditor> editor
        );

        /**
         * @brief Register a custom editor for a specific property ID - lvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Custom editors take precedence over type-based editors.
         *
         * @param propertyId The property ID to override
         * @param editor The custom editor instance (takes ownership)
         */
        void registerEditorForProperty(
            const std::string& propertyId,
            std::unique_ptr<Editors::IPropertyEditor> editor
        );

        /**
         * @brief Register a custom editor for a specific property ID - rvalue version
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Custom editors take precedence over type-based editors.
         *
         * @param propertyId The property ID to override
         * @param editor The custom editor instance (takes ownership)
         */
        void registerEditorForProperty(
            std::string&& propertyId,
            std::unique_ptr<Editors::IPropertyEditor> editor
        );

        /**
         * @brief Get the editor for a property type
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param type The property type
         * @return Editors::IPropertyEditor* Pointer to editor, or nullptr if not found
         */
        Editors::IPropertyEditor* getEditor(PropertyType type) const;

        /**
         * @brief Get the editor for a property descriptor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * First checks for a custom property-ID-based editor,
         * then falls back to the type-based editor.
         *
         * @param descriptor The property descriptor
         * @return Editors::IPropertyEditor* Pointer to editor, or nullptr if not found
         */
        Editors::IPropertyEditor* getEditorForProperty(
            const PropertyDescriptor& descriptor
        ) const;

        /**
         * @brief Register all default editors
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Registers editors for: String, Int, Float, Bool, Enum, Color, Vector2
         */
        void registerDefaultEditors();

        /**
         * @brief Check if an editor exists for a property type
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Mar 2026
         *
         * @param type The property type
         * @return bool True if an editor is registered for this type
         */
        bool hasEditor(PropertyType type) const;
    };
}

#endif //ADS_PROPERTY_EDITOR_REGISTRY_H
