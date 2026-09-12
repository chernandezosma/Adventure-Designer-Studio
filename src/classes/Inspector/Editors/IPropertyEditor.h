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

#ifndef ADS_IPROPERTY_EDITOR_H
#define ADS_IPROPERTY_EDITOR_H

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
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

        /// Set by an editor that needs an action performed after this ImGui
        /// frame ends (e.g. a native file dialog — see CLAUDE.md's deferred
        /// dialog pattern). When true, @c changed/@c newValue are not used;
        /// InspectorPanel queues the request instead of applying a value
        /// this frame. See PropertyConstraints::filePath() / StringEditor.
        bool fileDialogRequested = false;
        std::vector<std::string> fileDialogExtensions; ///< Valid only if fileDialogRequested

        /// Set by SelectEditor when the user just checked ON an option
        /// marked via PropertyDescriptor::setUserDefinedOptionBits() (e.g.
        /// a Scene "(User Defined N)" affordance/flag bit). When true,
        /// @c changed/@c newValue are not used — the checkbox is not
        /// committed yet; InspectorPanel should prompt for a label first,
        /// then commit @c labelDialogPendingValue via setPropertyValue()
        /// only if the user confirms. Canceling requires no revert: the
        /// bit was never written into the property's persisted value.
        bool labelDialogRequested = false;
        uint8_t labelDialogBitPosition = 0;   ///< Valid only if labelDialogRequested
        PropertyValue labelDialogPendingValue; ///< Valid only if labelDialogRequested — the SelectValue to commit on confirm

        /// Set by SelectEditor when the user clicked the "+" button shown
        /// next to a single-select dropdown marked
        /// PropertyDescriptor::setAllowCreateNew() (e.g. Scene's "state"
        /// property). When true, @c changed/@c newValue are not used —
        /// InspectorPanel should prompt for the new option's details, then
        /// create it and select it only if the user confirms.
        bool createNewRequested = false;

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

        /**
         * @brief Create a result requesting a deferred file dialog
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param extensions Allowed file extensions, no dots (e.g. "png")
         * @return EditResult Result with fileDialogRequested set
         */
        static EditResult requestFileDialog(std::vector<std::string> extensions) {
            EditResult result;
            result.fileDialogRequested = true;
            result.fileDialogExtensions = std::move(extensions);
            return result;
        }

        /**
         * @brief Create a result requesting a "type a label" dialog
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @param bitPosition Bit position of the user-defined option that was checked
         * @param pendingValue The SelectValue to commit if the dialog is confirmed
         * @return EditResult Result with labelDialogRequested set
         */
        static EditResult requestLabelDialog(uint8_t bitPosition, const PropertyValue& pendingValue) {
            EditResult result;
            result.labelDialogRequested = true;
            result.labelDialogBitPosition = bitPosition;
            result.labelDialogPendingValue = pendingValue;
            return result;
        }

        /**
         * @brief Create a result requesting a "create a new option" dialog
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version May 2026
         *
         * @return EditResult Result with createNewRequested set
         */
        static EditResult requestCreateNew() {
            EditResult result;
            result.createNewRequested = true;
            return result;
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
         * @brief Signature of the UI-string translator injected into the editors
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Maps an i18n key to its localized text in the active UI language.
         */
        using Translator = std::function<std::string(std::string_view)>;

        /**
         * @brief Install the process-wide UI-string translator for all editors
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * Called once by InspectorPanel at construction so editors — which
         * link only against Dear ImGui and have no access to the translations
         * manager — can still localize the few literals they own (empty-option
         * notices, dialog buttons). When left unset (e.g. in unit tests that
         * build editors directly) tr() returns the key unchanged.
         *
         * @param translator Callable mapping an i18n key to localized text
         */
        static void setTranslator(Translator translator) {
            s_translate = std::move(translator);
        }

        /**
         * @brief Translate a UI-string key through the installed translator
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @param key i18n key to resolve
         * @return std::string Localized text, or @p key itself when no
         *                     translator is installed
         */
        static std::string tr(std::string_view key) {
            return s_translate ? s_translate(key) : std::string(key);
        }

        /**
         * @brief Install the accent colour for editor-owned popup title bars
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * The expand-to-dialog popups (`EditTextDialog` / `EditTranslatableDialog`)
         * are plain `BeginPopupModal`s, not `ModalScaffold` modals, so they get
         * this pushed onto `ImGuiCol_TitleBg*` to keep the modal-dialog blue
         * (distinct from the deeper docked-panel caption). Set once by
         * InspectorPanel from `Colors::C_DIALOG_TITLE`.
         *
         * @param color RGBA fill for the popup title bar
         */
        static void setDialogAccent(const ImVec4& color) {
            s_dialogAccent = color;
        }

        /**
         * @brief Install the text colour for editor-owned popup title bars
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * The caption fill is always a dark blue, so its text stays near-white
         * in both the light and the dark theme. Set once by InspectorPanel
         * from Colors::C_CAPTION_TEXT.
         *
         * @param color RGBA text colour for the popup title bar
         */
        static void setDialogTextColor(const ImVec4& color) {
            s_dialogTextColor = color;
        }

        /**
         * @brief The text colour for editor-owned popup title bars
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return ImVec4 The installed colour, or a near-white default
         */
        static ImVec4 dialogTextColor() {
            return s_dialogTextColor;
        }

        /**
         * @brief The accent colour for editor-owned popup title bars
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Aug 2026
         *
         * @return ImVec4 The installed colour, or a sensible blue default
         */
        static ImVec4 dialogAccent() {
            return s_dialogAccent;
        }

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

    private:
        /// Process-wide UI-string translator, installed via setTranslator().
        /// Header-inline so the interface stays link-dependency-free.
        inline static Translator s_translate{};

        /// Accent for editor-owned popup title bars (see setDialogAccent()).
        /// Defaults to the modal blue #2f5fb0 until InspectorPanel installs the
        /// live Colors::C_DIALOG_TITLE.
        inline static ImVec4 s_dialogAccent{0.184f, 0.373f, 0.690f, 1.0f};

        /// Text colour on editor-owned popup title bars (see setDialogTextColor()).
        inline static ImVec4 s_dialogTextColor{0.941f, 0.953f, 0.965f, 1.0f};
    };
}

#endif //ADS_IPROPERTY_EDITOR_H