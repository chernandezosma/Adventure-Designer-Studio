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

/**
 * @file PropertyEditorRegistry.cpp
 * @brief Implementation of the PropertyEditorRegistry class
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Jan 2026
 */

#include "PropertyEditorRegistry.h"
#include "Editors/StringEditor.h"
#include "Editors/IntEditor.h"
#include "Editors/FloatEditor.h"
#include "Editors/BoolEditor.h"
#include "Editors/EnumEditor.h"
#include "Editors/ColorEditor.h"
#include "Editors/Vector2Editor.h"

namespace ADS::Inspector {
    /**
     * @brief Construct a new PropertyEditorRegistry
     *
     * Automatically registers all default editors.
     */
    PropertyEditorRegistry::PropertyEditorRegistry() {
        registerDefaultEditors();
    }

    /**
     * @brief Register an editor for a property type
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * @param type The property type to handle
     * @param editor The editor instance (takes ownership)
     */
    void PropertyEditorRegistry::registerEditor(
        PropertyType type,
        std::unique_ptr<Editors::IPropertyEditor> editor
    ) {
        m_typeEditors[type] = std::move(editor);
    }

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
    void PropertyEditorRegistry::registerEditorForProperty(
        const std::string& propertyId,
        std::unique_ptr<Editors::IPropertyEditor> editor
    ) {
        m_propertyEditors[propertyId] = std::move(editor);
    }

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
    void PropertyEditorRegistry::registerEditorForProperty(
        std::string&& propertyId,
        std::unique_ptr<Editors::IPropertyEditor> editor
    ) {
        m_propertyEditors[std::move(propertyId)] = std::move(editor);
    }

    /**
     * @brief Get the editor for a property type
     *
     * @param type The property type
     * @return Editors::IPropertyEditor* Pointer to editor, or nullptr if not found
     */
    Editors::IPropertyEditor* PropertyEditorRegistry::getEditor(PropertyType type) const {
        auto it = m_typeEditors.find(type);
        if (it != m_typeEditors.end()) {
            return it->second.get();
        }
        return nullptr;
    }

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
    Editors::IPropertyEditor* PropertyEditorRegistry::getEditorForProperty(
        const PropertyDescriptor& descriptor
    ) const {
        // First check for custom property-specific editor
        auto propIt = m_propertyEditors.find(descriptor.getId());
        if (propIt != m_propertyEditors.end()) {
            return propIt->second.get();
        }

        // Fall back to type-based editor
        return getEditor(descriptor.getType());
    }

    /**
     * @brief Register all default editors
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Registers editors for: String, Int, Float, Bool, Enum, Color, Vector2
     */
    void PropertyEditorRegistry::registerDefaultEditors() {
        registerEditor(PropertyType::String, std::make_unique<Editors::StringEditor>());
        registerEditor(PropertyType::Int, std::make_unique<Editors::IntEditor>());
        registerEditor(PropertyType::Float, std::make_unique<Editors::FloatEditor>());
        registerEditor(PropertyType::Bool, std::make_unique<Editors::BoolEditor>());
        registerEditor(PropertyType::Enum, std::make_unique<Editors::EnumEditor>());
        registerEditor(PropertyType::Color, std::make_unique<Editors::ColorEditor>());
        registerEditor(PropertyType::Vector2, std::make_unique<Editors::Vector2Editor>());
    }

    /**
     * @brief Check if an editor exists for a property type
     *
     * @param type The property type
     * @return true if an editor is registered
     */
    bool PropertyEditorRegistry::hasEditor(PropertyType type) const {
        return m_typeEditors.find(type) != m_typeEditors.end();
    }
}
