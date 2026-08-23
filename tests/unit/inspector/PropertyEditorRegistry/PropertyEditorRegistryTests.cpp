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

#include <gtest/gtest.h>

#include "Inspector/PropertyEditorRegistry.h"

using namespace ADS::Inspector;
using namespace ADS::Inspector::Editors;

namespace {
    // Minimal stub editor — never calls into ImGui, so it's safe to use
    // without a live ImGui context. Registration/lookup logic is what's
    // under test here, not actual widget rendering.
    class StubEditor : public IPropertyEditor {
    public:
        explicit StubEditor(std::string id) : m_id(std::move(id)) {}

        std::vector<PropertyType> getSupportedTypes() const override { return {PropertyType::String}; }

        EditResult render(const PropertyDescriptor&, const PropertyValue&, bool) override
        {
            return EditResult::unchanged();
        }

        std::string getEditorId() const override { return m_id; }

    private:
        std::string m_id;
    };
}

TEST(PropertyEditorRegistry, DefaultConstruction_RegistersAllDefaultEditors)
{
    PropertyEditorRegistry registry;

    EXPECT_TRUE(registry.hasEditor(PropertyType::String));
    EXPECT_TRUE(registry.hasEditor(PropertyType::Int));
    EXPECT_TRUE(registry.hasEditor(PropertyType::Float));
    EXPECT_TRUE(registry.hasEditor(PropertyType::Bool));
    EXPECT_TRUE(registry.hasEditor(PropertyType::Enum));
    EXPECT_TRUE(registry.hasEditor(PropertyType::Color));
    EXPECT_TRUE(registry.hasEditor(PropertyType::Vector2));
    EXPECT_TRUE(registry.hasEditor(PropertyType::Select));
}

TEST(PropertyEditorRegistry, HasEditor_UnknownType_ReturnsFalseUntilRegistered)
{
    PropertyEditorRegistry registry;

    EXPECT_FALSE(registry.hasEditor(PropertyType::Unknown));
}

TEST(PropertyEditorRegistry, GetEditor_ReturnsNonNullForRegisteredType)
{
    PropertyEditorRegistry registry;

    EXPECT_NE(registry.getEditor(PropertyType::Int), nullptr);
}

TEST(PropertyEditorRegistry, GetEditor_UnregisteredType_ReturnsNullptr)
{
    PropertyEditorRegistry registry;

    EXPECT_EQ(registry.getEditor(PropertyType::Unknown), nullptr);
}

TEST(PropertyEditorRegistry, RegisterEditor_OverridesTypeEditor)
{
    PropertyEditorRegistry registry;

    registry.registerEditor(PropertyType::Unknown, std::make_unique<StubEditor>("stub"));

    ASSERT_NE(registry.getEditor(PropertyType::Unknown), nullptr);
    EXPECT_EQ(registry.getEditor(PropertyType::Unknown)->getEditorId(), "stub");
}

TEST(PropertyEditorRegistry, GetEditorForProperty_NoOverride_FallsBackToTypeEditor)
{
    PropertyEditorRegistry registry;
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);

    IPropertyEditor* editor = registry.getEditorForProperty(descriptor);

    EXPECT_EQ(editor, registry.getEditor(PropertyType::Int));
}

TEST(PropertyEditorRegistry, RegisterEditorForProperty_TakesPrecedenceOverType)
{
    PropertyEditorRegistry registry;
    PropertyDescriptor descriptor("width", "Width", PropertyType::Int);

    registry.registerEditorForProperty("width", std::make_unique<StubEditor>("custom-width"));

    IPropertyEditor* editor = registry.getEditorForProperty(descriptor);

    ASSERT_NE(editor, nullptr);
    EXPECT_EQ(editor->getEditorId(), "custom-width");
    EXPECT_NE(editor, registry.getEditor(PropertyType::Int));
}

TEST(PropertyEditorRegistry, GetEditorForProperty_UnknownTypeNoOverride_ReturnsNullptr)
{
    PropertyEditorRegistry registry;
    PropertyDescriptor descriptor("mystery", "Mystery", PropertyType::Unknown);

    EXPECT_EQ(registry.getEditorForProperty(descriptor), nullptr);
}

TEST(PropertyEditorRegistry, StubEditor_SupportedTypesAndUnchangedResult)
{
    StubEditor editor("stub");
    PropertyDescriptor descriptor("x", "X", PropertyType::String);

    EXPECT_EQ(editor.getSupportedTypes(), std::vector<PropertyType>{PropertyType::String});

    EditResult result = editor.render(descriptor, std::string("value"), false);

    EXPECT_FALSE(result.changed);
}

TEST(EditResult, Unchanged_HasChangedFalse)
{
    EditResult result = EditResult::unchanged();

    EXPECT_FALSE(result.changed);
}

TEST(EditResult, Modified_HasChangedTrueAndCarriesValue)
{
    EditResult result = EditResult::modified(std::string("new value"));

    EXPECT_TRUE(result.changed);
    EXPECT_EQ(std::get<std::string>(result.newValue), "new value");
}

TEST(EditResult, RequestFileDialog_SetsFlagAndExtensions_LeavesChangedFalse)
{
    EditResult result = EditResult::requestFileDialog({"png", "jpg"});

    EXPECT_TRUE(result.fileDialogRequested);
    EXPECT_EQ(result.fileDialogExtensions, (std::vector<std::string>{"png", "jpg"}));
    EXPECT_FALSE(result.changed);
}

TEST(EditResult, Unchanged_DoesNotRequestFileDialog)
{
    EditResult result = EditResult::unchanged();

    EXPECT_FALSE(result.fileDialogRequested);
    EXPECT_TRUE(result.fileDialogExtensions.empty());
}
