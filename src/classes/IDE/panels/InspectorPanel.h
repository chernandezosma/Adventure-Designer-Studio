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


#ifndef ADS_INSPECTOR_PANEL_H
#define ADS_INSPECTOR_PANEL_H

#include <map>
#include <vector>
#include "BasePanel.h"
#include "Inspector/IInspectable.h"
#include "Inspector/PropertyDescriptor.h"
#include "Inspector/PropertyEditorRegistry.h"

namespace ADS::IDE::Panels {
    /**
     * @brief Inspector panel for displaying and editing entity properties
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Jan 2026
     *
     * Shows detailed information about the currently selected entity
     * including its type, and all editable properties organized by
     * category. Uses a Visual Basic-like property grid interface.
     */
    class InspectorPanel : public BasePanel {
    private:
        /// Currently selected inspectable object
        Inspector::IInspectable* m_selectedObject;

        /// Registry of property editors
        Inspector::PropertyEditorRegistry m_editorRegistry;

        /// Cached property descriptors grouped by category
        std::map<std::string, std::vector<Inspector::PropertyDescriptor>> m_categorizedProperties;

        /// Flag indicating if category cache needs refresh
        bool m_needsRefresh;

        /**
         * @brief Render the object header (type and name)
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         */
        void renderObjectHeader();

        /**
         * @brief Render properties for a category
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param category Category name
         * @param properties Properties in this category
         */
        void renderCategory(
            const std::string& category,
            const std::vector<Inspector::PropertyDescriptor>& properties
        );

        /**
         * @brief Render a single property using the appropriate editor
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param descriptor Property metadata
         */
        void renderProperty(const Inspector::PropertyDescriptor& descriptor);

        /**
         * @brief Refresh the category cache from the selected object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         */
        void refreshCategoryCache();

        /**
         * @brief Render placeholder when no object is selected
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         */
        void renderNoSelection();

    public:
        /**
         * @brief Construct a new InspectorPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Initializes the inspector panel with name "Inspector".
         */
        InspectorPanel();

        /**
         * @brief Destroy the InspectorPanel object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Default destructor. No special cleanup required for this class.
         */
        ~InspectorPanel() override = default;

        /**
         * @brief Render the inspector panel
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Displays a property grid for the selected entity with
         * properties organized by category. Each property uses
         * an appropriate editor control based on its type.
         *
         * @note Returns early if panel is not visible
         */
        void render() override;

        /**
         * @brief Set the currently selected object
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * @param object The inspectable object to display (can be nullptr)
         */
        void setSelectedObject(Inspector::IInspectable* object);

        /**
         * @brief Get the currently selected object
         *
         * @return Inspector::IInspectable* Pointer to selected object, or nullptr
         */
        Inspector::IInspectable* getSelectedObject() const;

        /**
         * @brief Clear the current selection
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         */
        void clearSelection();

        /**
         * @brief Force a refresh of the property display
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Jan 2026
         *
         * Call this after the selected object's properties have changed
         * externally (e.g., via undo/redo).
         */
        void refresh();
    };
}

#endif //ADS_INSPECTOR_PANEL_H
