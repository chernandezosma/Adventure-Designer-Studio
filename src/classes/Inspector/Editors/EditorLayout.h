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

#ifndef ADS_EDITOR_LAYOUT_H
#define ADS_EDITOR_LAYOUT_H

#include <algorithm>
#include "imgui.h"
#include "../PropertyDescriptor.h"

namespace ADS::Inspector::Editors {
    namespace detail {
        /// Category-scoped label column width override, set by
        /// InspectorPanel::renderCategory() via CategoryColumnWidthScope so
        /// every row in the same open category shares one column split
        /// (sized to that category's widest visible label) instead of each
        /// row measuring only its own label. -1.0f means "no override —
        /// measure this row's own label" (the pre-May-2026 behaviour).
        /// Plain (non-thread_local): ImGui only ever renders on the main
        /// thread in this app.
        inline float g_columnWidthOverride = -1.0f;
    }

    /**
     * @brief RAII scope that overrides beginPropertyColumns()'s label width
     *        for every row rendered while it's alive
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Used by InspectorPanel::renderCategory() to align every property row
     * in a category to one shared column split, sized to the category's
     * widest visible label, instead of each row picking its own width.
     */
    class CategoryColumnWidthScope {
    public:
        explicit CategoryColumnWidthScope(float width) {
            detail::g_columnWidthOverride = width;
        }

        ~CategoryColumnWidthScope() {
            detail::g_columnWidthOverride = -1.0f;
        }

        CategoryColumnWidthScope(const CategoryColumnWidthScope&) = delete;
        CategoryColumnWidthScope& operator=(const CategoryColumnWidthScope&) = delete;
    };

    /**
     * @brief Begin the shared two-column (label | widget) layout for a property row
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * Every editor renders a label column followed by a widget column. When
     * a CategoryColumnWidthScope is active (set by InspectorPanel around a
     * category's rows), that shared width is used so every row in the
     * category aligns; otherwise the column is sized to fit this row's own
     * label text (measured via ImGui::CalcTextSize), which previously
     * clipped longer labels (e.g. "Affordances" rendering as "Afforda").
     * Clamped to [80px, 50% of available width] so very long labels don't
     * crowd out the widget column entirely.
     *
     * @param descriptor Property metadata (only the display name is used)
     * @return float The label column width that was applied
     */
    inline float beginPropertyColumns(const PropertyDescriptor& descriptor) {
        float labelWidth = detail::g_columnWidthOverride > 0.0f
            ? detail::g_columnWidthOverride
            : ImGui::CalcTextSize(descriptor.getDisplayName().c_str()).x + 16.0f;
        labelWidth = std::clamp(labelWidth, 80.0f, ImGui::GetContentRegionAvail().x * 0.5f);

        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, labelWidth);
        return labelWidth;
    }

    /**
     * @brief Draw a trailing "…" over the last item when its text is clipped
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * Call immediately after an inline single-line ImGui::InputText whose full
     * content may be wider than the widget. When the widget is not being
     * edited and @p fullText does not fit, an ellipsis glyph is painted at the
     * widget's right inner edge (in the current text colour) as a hint that
     * more text exists and the expand dialog is needed to see it. A no-op
     * while the field is active so it never covers the caret.
     *
     * @param fullText The complete, untruncated field text
     */
    inline void drawInputOverflowHint(const char* fullText) {
        if (ImGui::IsItemActive() || fullText == nullptr || fullText[0] == '\0') {
            return;
        }

        const ImVec2 rectMin = ImGui::GetItemRectMin();
        const ImVec2 rectMax = ImGui::GetItemRectMax();
        const float padX     = ImGui::GetStyle().FramePadding.x;
        const float innerW   = (rectMax.x - rectMin.x) - padX * 2.0f;

        if (ImGui::CalcTextSize(fullText).x <= innerW) {
            return;
        }

        const ImVec2 dotsSize = ImGui::CalcTextSize("…");
        const ImVec2 pos(rectMax.x - padX - dotsSize.x,
                         rectMin.y + (rectMax.y - rectMin.y - dotsSize.y) * 0.5f);
        ImGui::GetWindowDrawList()->AddText(
            pos, ImGui::GetColorU32(ImGuiCol_Text), "…");
    }
}

#endif // ADS_EDITOR_LAYOUT_H
