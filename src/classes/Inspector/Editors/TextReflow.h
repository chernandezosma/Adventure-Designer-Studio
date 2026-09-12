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

#ifndef ADS_INSPECTOR_TEXT_REFLOW_H
#define ADS_INSPECTOR_TEXT_REFLOW_H

/**
 * @file TextReflow.h
 * @brief Word-wrap a plain-text buffer to a pixel width for the expand dialogs
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Sep 2026
 *
 * Dear ImGui's InputTextMultiline has no word-wrap — long lines scroll
 * horizontally. For prose fields (scene / item / character descriptions) that
 * is unusable, so the expand dialogs re-flow the buffer to the current widget
 * width. reflowWrap() rebuilds the whole buffer (used on blur / resize and,
 * via unwrapSoft(), before storage); wrapInPlace() / wrapCallback() flip only
 * a lone inter-word space<->newline without changing the byte length, which is
 * safe to run every frame from an InputText CallbackAlways hook so the text
 * folds *while* the user types. Wrapping is soft — a blank line is kept as a
 * hard paragraph break, every other newline is recomputed — so nothing the
 * user must keep is lost.
 */

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <string>

#include "imgui.h"

namespace ADS::Inspector::Editors {

    /**
     * @brief Rewrap @p buf so no line exceeds @p wrapPx at the current font.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Paragraphs (runs separated by a blank line) are preserved; inside a
     * paragraph every run of spaces / tabs / single newlines collapses to one
     * space and the words are greedily packed into lines. A single word wider
     * than @p wrapPx is left on its own line.
     *
     * @param buf     NUL-terminated, mutable text buffer
     * @param bufSize Capacity of @p buf including the NUL
     * @param wrapPx  Target line width in pixels (<= 0 disables, returns false)
     * @return bool true when @p buf was changed
     */
    inline bool reflowWrap(char* buf, std::size_t bufSize, float wrapPx)
    {
        if (buf == nullptr || bufSize == 0 || wrapPx <= 1.0f) {
            return false;
        }

        const std::string src(buf);
        const float spaceW = ImGui::CalcTextSize(" ").x;

        std::string out;
        out.reserve(src.size() + src.size() / 8 + 16);

        std::size_t i = 0;
        const std::size_t n = src.size();
        bool firstParagraph = true;

        while (i < n) {
            // Skip a run of blank lines between paragraphs.
            std::size_t blanks = 0;
            while (i < n && (src[i] == '\n' || src[i] == '\r')) {
                if (src[i] == '\n') {
                    ++blanks;
                }
                ++i;
            }
            if (i >= n) {
                break;
            }
            if (!firstParagraph) {
                out += (blanks >= 2) ? "\n\n" : "\n"; // >=1 blank line -> paragraph break
            }
            firstParagraph = false;

            // Collect this paragraph's text up to the next blank line.
            std::string para;
            while (i < n) {
                if (src[i] == '\n') {
                    // A single newline is a soft break; two in a row ends the paragraph.
                    if (i + 1 < n && src[i + 1] == '\n') {
                        break;
                    }
                    para.push_back(' ');
                    ++i;
                    continue;
                }
                if (src[i] == '\r' || src[i] == '\t') {
                    para.push_back(' ');
                    ++i;
                    continue;
                }
                para.push_back(src[i]);
                ++i;
            }

            // Greedily wrap the paragraph's words.
            float lineW = 0.0f;
            bool lineEmpty = true;
            std::size_t w = 0;
            const std::size_t pn = para.size();
            while (w < pn) {
                while (w < pn && para[w] == ' ') {
                    ++w;
                }
                if (w >= pn) {
                    break;
                }
                std::size_t we = w;
                while (we < pn && para[we] != ' ') {
                    ++we;
                }
                const std::string word = para.substr(w, we - w);
                const float wordW = ImGui::CalcTextSize(word.c_str()).x;

                if (lineEmpty) {
                    out += word;
                    lineW = wordW;
                    lineEmpty = false;
                } else if (lineW + spaceW + wordW <= wrapPx) {
                    out += ' ';
                    out += word;
                    lineW += spaceW + wordW;
                } else {
                    out += '\n';
                    out += word;
                    lineW = wordW;
                }
                w = we;
            }
        }

        if (out == src || out.size() + 1 > bufSize) {
            return false;
        }
        std::snprintf(buf, bufSize, "%s", out.c_str());
        return true;
    }

    /**
     * @brief Reflow only when it is worth it (blur, or the width changed).
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Call once per frame after an InputTextMultiline while it is NOT active.
     * Skips the (mildly expensive) rewrap on idle frames where neither the
     * width nor the content changed.
     *
     * @param buf        NUL-terminated, mutable text buffer
     * @param bufSize    Capacity of @p buf including the NUL
     * @param wrapPx     Target line width in pixels
     * @param force      Reflow even if the width did not change (e.g. on blur)
     * @param lastWrapPx In/out: the width used by the previous reflow
     * @return bool true when @p buf was changed
     */
    inline bool reflowWrapIfNeeded(char* buf, std::size_t bufSize, float wrapPx,
                                   bool force, float& lastWrapPx)
    {
        if (!force && std::fabs(wrapPx - lastWrapPx) < 1.0f) {
            return false;
        }
        lastWrapPx = wrapPx;
        return reflowWrap(buf, bufSize, wrapPx);
    }

    /**
     * @brief Drop the soft line breaks reflowWrap() added, keeping paragraphs.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Call before persisting the field so the stored text is the canonical
     * prose (each paragraph one line, paragraphs separated by a blank line) and
     * does not carry width-dependent newlines. Re-opening the dialog re-wraps
     * it for display.
     *
     * @param buf     NUL-terminated, mutable text buffer
     * @param bufSize Capacity of @p buf including the NUL
     */
    inline void unwrapSoft(char* buf, std::size_t bufSize)
    {
        // A width no real paragraph can exceed collapses every soft break.
        reflowWrap(buf, bufSize, 1.0e9f);
    }

    /**
     * @brief Rewrap @p buf to @p wrapPx without changing its byte length.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Unlike reflowWrap() this never inserts or deletes characters: it only
     * flips a lone inter-word whitespace byte between a space and a newline so
     * that greedy word-wrapping holds at the given width. Because the length
     * is preserved it is safe to run from an InputText @c CallbackAlways hook
     * while the field is being edited — the caret offset stays valid with no
     * remapping. Runs of two or more whitespace bytes (blank-line paragraph
     * breaks, indented lines, CR/LF pairs) are treated as hard breaks and
     * left untouched.
     *
     * @param buf    Mutable text buffer (need not be NUL-terminated at @p len)
     * @param len    Number of bytes in @p buf to consider
     * @param wrapPx Target line width in pixels (<= 1 disables, returns false)
     * @return bool true when at least one byte in @p buf was changed
     */
    inline bool wrapInPlace(char* buf, int len, float wrapPx)
    {
        if (buf == nullptr || len <= 0 || wrapPx <= 1.0f) {
            return false;
        }

        const auto isWs = [](char c) {
            return c == ' ' || c == '\n' || c == '\t' || c == '\r';
        };

        const float spaceW = ImGui::CalcTextSize(" ").x;
        bool  changed = false;
        float lineW   = 0.0f;
        int   i       = 0;

        while (i < len) {
            // Measure the word that starts at the cursor.
            const int wordStart = i;
            while (i < len && !isWs(buf[i])) {
                ++i;
            }
            if (i > wordStart) {
                const float wordW = ImGui::CalcTextSize(buf + wordStart, buf + i).x;
                lineW = (lineW <= 0.0f) ? wordW : (lineW + spaceW + wordW);
            }
            if (i >= len) {
                break;
            }

            // Span the whitespace run that follows the word.
            const int runStart = i;
            while (i < len && isWs(buf[i])) {
                ++i;
            }
            const int runLen = i - runStart;

            if (runLen == 1 && (buf[runStart] == ' ' || buf[runStart] == '\n')) {
                // A soft gap we own: keep it a space while the next word still
                // fits on the line, make it a newline once it does not.
                const int nextStart = i;
                int       nextEnd   = nextStart;
                while (nextEnd < len && !isWs(buf[nextEnd])) {
                    ++nextEnd;
                }
                const float nextW = (nextEnd > nextStart)
                    ? ImGui::CalcTextSize(buf + nextStart, buf + nextEnd).x : 0.0f;
                const bool fits = (lineW + spaceW + nextW) <= wrapPx;
                const char want = fits ? ' ' : '\n';
                if (buf[runStart] != want) {
                    buf[runStart] = want;
                    changed = true;
                }
                if (want == '\n') {
                    lineW = 0.0f;
                }
            } else {
                // Blank line, indent or CR/LF pair — a hard break, left as-is.
                lineW = 0.0f;
            }
        }

        return changed;
    }

    /**
     * @brief Dear ImGui @c InputTextCallbackData hook that live-wraps the text.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * Install on an @c InputTextMultiline with @c ImGuiInputTextFlags_CallbackAlways
     * (pair it with @c ImGuiInputTextFlags_NoHorizontalScroll so a too-long
     * single word clips instead of panning the view). @c UserData must point
     * to a @c float holding the target wrap width in pixels. Each frame the
     * buffer is rewrapped in place via wrapInPlace(); the length never changes
     * so the caret is left where ImGui put it.
     *
     * @param data Callback payload supplied by Dear ImGui
     * @return int Always 0 (the return value is unused for this event)
     */
    inline int wrapCallback(ImGuiInputTextCallbackData* data)
    {
        if (data == nullptr || data->EventFlag != ImGuiInputTextFlags_CallbackAlways) {
            return 0;
        }
        const float wrapPx = (data->UserData != nullptr)
            ? *static_cast<const float*>(data->UserData) : 0.0f;
        if (wrapInPlace(data->Buf, data->BufTextLen, wrapPx)) {
            // Only ' ' <-> '\n' swaps happened, so BufTextLen and CursorPos
            // are still correct; just tell ImGui the bytes moved.
            data->BufDirty = true;
        }
        return 0;
    }

} // namespace ADS::Inspector::Editors

#endif // ADS_INSPECTOR_TEXT_REFLOW_H
