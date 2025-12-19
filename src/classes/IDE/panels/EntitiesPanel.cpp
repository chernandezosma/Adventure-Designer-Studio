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


#include "EntitiesPanel.h"
#include "imgui.h"

namespace ADS::IDE::Panels {
    EntitiesPanel::EntitiesPanel()
        : BasePanel("Entities") {
    }

    void EntitiesPanel::renderSceneTree() {
        if (ImGui::TreeNode("Scenes")) {
            ImGui::BulletText("Scene 1");
            ImGui::BulletText("Scene 2");
            ImGui::BulletText("Scene 3");
            ImGui::TreePop();
        }
    }

    void EntitiesPanel::renderCharacterTree() {
        if (ImGui::TreeNode("Characters")) {
            ImGui::BulletText("Hero");
            ImGui::BulletText("Villain");
            ImGui::BulletText("NPC 1");
            ImGui::TreePop();
        }
    }

    void EntitiesPanel::renderItemTree() {
        if (ImGui::TreeNode("Items")) {
            ImGui::BulletText("Sword");
            ImGui::BulletText("Key");
            ImGui::BulletText("Potion");
            ImGui::TreePop();
        }
    }

    void EntitiesPanel::handleAddEntity() {
        // Handle add entity
    }

    void EntitiesPanel::render() {
        if (!m_isVisible) {
            return;
        }

        ImGui::Begin(m_windowName.c_str());

        ImGui::Text("Entities List");
        ImGui::Separator();

        renderSceneTree();
        renderCharacterTree();
        renderItemTree();

        ImGui::Separator();
        if (ImGui::Button("Add New Entity")) {
            handleAddEntity();
        }

        ImGui::End();
    }
}
