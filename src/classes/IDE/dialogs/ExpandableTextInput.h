/**
 * Copyright (c) 2026 Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
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

#ifndef ADS_IDE_EXPANDABLE_TEXT_INPUT_H
#define ADS_IDE_EXPANDABLE_TEXT_INPUT_H

/**
 * @file ExpandableTextInput.h
 * @brief Inline single-line text field with a "…" button that pops a larger
 *        multiline editor over the same buffer
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Aug 2026
 */

#include <cstddef>
#include <string>

namespace ADS::IDE {

    /**
     * @brief Draw an inline InputText plus a "…" expand button.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Aug 2026
     *
     * The inline field edits @p buf directly. The "…" button opens a
     * resizable modal with a big InputTextMultiline over the same buffer;
     * its **Cancel** restores the text captured when the dialog opened, its
     * **OK** keeps the edit. Mirrors the look of the inspector's
     * `EditTextDialog`. Only one such modal can be open at a time, so the
     * open-snapshot is held in a function-local static.
     *
     * Call inside an active window, once per frame. Wrap in `ImGui::PushID`
     * when several instances share the same @p strId in one scope.
     *
     * @param strId       ImGui id for the inline field (also seeds the popup id)
     * @param buf         NUL-terminated, mutable text buffer
     * @param bufSize     Capacity of @p buf including the NUL
     * @param dialogTitle Caption shown at the top of the expand dialog
     * @param okLabel      Already-translated OK button caption
     * @param cancelLabel  Already-translated Cancel button caption
     * @param maxLen       Maximum characters the user may enter (0 = only the
     *                     @p bufSize limit applies)
     * @return bool         true when @p buf changed this frame
     */
    bool expandableTextInput(const char* strId, char* buf, std::size_t bufSize,
                             const std::string& dialogTitle,
                             const char* okLabel, const char* cancelLabel,
                             std::size_t maxLen = 0);

} // namespace ADS::IDE

#endif // ADS_IDE_EXPANDABLE_TEXT_INPUT_H
