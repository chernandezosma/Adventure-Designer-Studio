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


#include "MenuBarRenderer.h"
#include "imgui.h"
#include <SDL.h>

namespace ADS::IDE {
    MenuBarRenderer::MenuBarRenderer(LayoutManager* layoutManager)
        : m_layoutManager(layoutManager) {
    }

    void MenuBarRenderer::renderFileMenu() {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                // Handle new
            }
            if (ImGui::MenuItem("Open")) {
                // Handle open
            }
            if (ImGui::MenuItem("Save")) {
                // Handle save
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                handleExit();
            }
            ImGui::EndMenu();
        }
    }

    void MenuBarRenderer::renderViewMenu() {
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Reset Layout")) {
                m_layoutManager->resetLayout();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Theme")) {
                if (ImGui::MenuItem("Dark Theme")) {
                    handleThemeChange(true);
                }
                if (ImGui::MenuItem("Light Theme")) {
                    handleThemeChange(false);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
    }

    void MenuBarRenderer::renderHelpMenu() {
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                // Show about dialog
            }
            ImGui::EndMenu();
        }
    }

    void MenuBarRenderer::handleExit() {
        SDL_Event quit_event;
        quit_event.type = SDL_QUIT;
        SDL_PushEvent(&quit_event);
    }

    void MenuBarRenderer::handleThemeChange(bool darkTheme) {
        if (darkTheme) {
            ImGui::StyleColorsDark();
        } else {
            ImGui::StyleColorsLight();
        }
    }

    void MenuBarRenderer::render() {
        renderFileMenu();
        renderViewMenu();
        renderHelpMenu();
    }
}
