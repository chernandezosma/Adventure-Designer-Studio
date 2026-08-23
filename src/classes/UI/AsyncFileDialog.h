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

#ifndef ADS_ASYNC_FILE_DIALOG_H
#define ADS_ASYNC_FILE_DIALOG_H

#include <atomic>
#include <functional>
#include <optional>
#include <string>
#include <thread>

namespace ADS::UI {

    /**
     * @brief Runs a blocking native-file-dialog call on a background thread
     *
     * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
     * @version May 2026
     *
     * NFD::OpenDialog()/SaveDialog() are synchronous, blocking calls. Calling
     * them directly from the main thread — even after SDL_RenderPresent, per
     * CLAUDE.md's deferred dialog pattern — freezes the app's own render loop
     * for the entire lifetime of the dialog: no processEvents()/render() runs,
     * so SDL_RenderPresent never fires again until the dialog closes. Under
     * some window managers/compositors this makes the parent window fail to
     * repaint while the dialog is dragged, since the app never produces a new
     * frame for the areas the drag uncovers.
     *
     * AsyncFileDialog runs the NFD call on a dedicated std::thread instead, so
     * the main loop keeps pumping events and presenting frames while the
     * dialog is open. Call start() once when the dialog should open, then
     * poll() once per frame — from the same thread, at the same point in the
     * frame the synchronous NFD call used to happen — until it returns true.
     */
    class AsyncFileDialog {
    public:
        /**
         * @brief Join the background thread, if one is still running
         *
         * Blocks until any in-flight NFD call returns. Safe to destroy even
         * while a dialog is open.
         */
        ~AsyncFileDialog()
        {
            if (m_thread.joinable()) {
                m_thread.join();
            }
        }

        /**
         * @brief Start running `task` on a background thread
         *
         * No-op if a dialog is already running. `task` must perform the NFD
         * call itself (including its own NFD::Guard) and return the selected
         * path, or std::nullopt on cancel/error.
         */
        void start(std::function<std::optional<std::string>()> task)
        {
            if (m_running) return;
            if (m_thread.joinable()) m_thread.join();

            m_running = true;
            m_ready   = false;
            m_thread  = std::thread([this, task = std::move(task)]() mutable {
                m_result = task();
                m_ready  = true;
            });
        }

        /**
         * @brief Check whether the running dialog has finished
         *
         * Must be called once per frame. Returns true exactly once, the first
         * frame after the background task completes, and moves its result
         * into `outResult` (std::nullopt if the dialog was cancelled or
         * errored). Returns false every other frame, including while no
         * dialog is running.
         */
        bool poll(std::optional<std::string>& outResult)
        {
            if (!m_running || !m_ready) return false;

            if (m_thread.joinable()) m_thread.join();
            outResult = std::move(m_result);
            m_result.reset();
            m_running = false;
            m_ready   = false;
            return true;
        }

        /**
         * @brief Check whether a background dialog task is currently in flight
         * @return bool True from start() until poll() consumes the result
         */
        bool isRunning() const { return m_running; }

    private:
        std::atomic<bool> m_running{false};        ///< True from start() until poll() consumes the result
        std::atomic<bool> m_ready{false};          ///< True once the background task has stored a result
        std::thread m_thread;                      ///< Background thread running the current NFD call, if any
        std::optional<std::string> m_result;       ///< Result stashed by the background task, consumed by poll()
    };

} // namespace ADS::UI

#endif //ADS_ASYNC_FILE_DIALOG_H
