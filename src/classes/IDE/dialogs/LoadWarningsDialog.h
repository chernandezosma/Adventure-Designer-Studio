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

#ifndef ADS_IDE_LOAD_WARNINGS_DIALOG_H
#define ADS_IDE_LOAD_WARNINGS_DIALOG_H

/**
 * @file LoadWarningsDialog.h
 * @brief Modal listing entities skipped during a partial `.ads` project load
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Sep 2026
 */

#include <vector>

#include "../IDEBase.h"
#include "Core/ProjectSerializer.h"

namespace ADS::IDE {

    /**
     * @brief Pure-ImGui modal showing every Core::LoadWarning from the most
     *        recent project open.
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version Sep 2026
     *
     * open() arms the modal with the warning list; render() must be called
     * every frame from within an active ImGui window. Purely informational —
     * the caller has already loaded every entity that parsed successfully,
     * so the only action here is to acknowledge and close.
     */
    class LoadWarningsDialog : public IDEBase {
    public:
        /**
         * @brief Arm the modal with @p warnings.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * @param warnings Entities skipped by ProjectSerializer::load()
         */
        void open(std::vector<Core::LoadWarning> warnings);

        /**
         * @brief Render the modal for the current frame.
         *
         * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
         * @version Sep 2026
         *
         * A no-op frame unless the modal is armed or already open.
         */
        void render();

    private:
        bool m_pending = false;                        ///< set by open(), consumed on the next render()
        std::vector<Core::LoadWarning> m_warnings;      ///< entities skipped on the last open()
    };

} // namespace ADS::IDE

#endif // ADS_IDE_LOAD_WARNINGS_DIALOG_H
