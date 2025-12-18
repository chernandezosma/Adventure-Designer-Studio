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
#include <fstream>
#include <iostream>
#include <SDL.h>
#include <random>
#include <unistd.h>
#include <X11/X.h>

#include "System.h"
#include "src/classes/env/env.h"
#include "i18nUtils.h"
#include "adsString.h"
#include "app.h"
#include "UI/Window.h"
#include "imgui/window_intialization_exception.h"

#ifdef BUILD_TESTING_ENABLED
#include "lib/googletest/googletest/include/gtest/internal/gtest-string.h"
#endif

#ifdef _WIN32
#include <windows.h>        // SetProcessDPIAware()
#endif

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_internal.h"
#include "languages.h"
#include "i18n/i18n.h"
#include "IconsFontAwesome4.h"

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

using namespace std;
using namespace ADS::Constants; // ADS::Constants::System::SystemConst;

static bool isDockingSetup = false;

ImVec4 RGB(int r, int g, int b, int a = 255)
{
    return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

void SetupDockingLayout(ImGuiID dockSpaceId)
{
    // Only setup once
    if (isDockingSetup)
        return;

    isDockingSetup = true;

    // Check if there's an existing layout in the .ini file
    // If the dock node exists, it means we have a saved layout - don't override it!
    ImGuiDockNode *existingNode = ImGui::DockBuilderGetNode(dockSpaceId);
    if (existingNode != nullptr && existingNode->IsSplitNode()) {
        // Layout already exists from saved settings, don't override it
        std::cout << "Found saved layout, using it..." << std::endl;
        return;
    }

    // No saved layout found, create default layout
    std::cout << "No saved layout found, creating default layout..." << std::endl;

    // Clear any existing layout
    ImGui::DockBuilderRemoveNode(dockSpaceId);
    ImGui::DockBuilderAddNode(dockSpaceId, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockSpaceId, ImGui::GetMainViewport()->Size);

    // Split the dockspace into left and right
    ImGuiID dock_main_id = dockSpaceId;
    ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, nullptr, &dock_main_id);
    ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
    // 0.25 because it's 20% of remaining 80%

    // Split the right side into top (Properties) and bottom (Inspector)
    ImGuiID dock_right_top_id = dock_right_id;
    ImGuiID dock_right_bottom_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.50f, nullptr, &dock_right_top_id);

    // Dock windows to their respective areas
    ImGui::DockBuilderDockWindow("Entities", dock_left_id);
    ImGui::DockBuilderDockWindow("Properties", dock_right_top_id);
    ImGui::DockBuilderDockWindow("Inspector", dock_right_bottom_id);
    ImGui::DockBuilderDockWindow("Working Area", dock_main_id);

    // Finalize the docking layout
    ImGui::DockBuilderFinish(dockSpaceId);
}

void RenderMainWindow(float &statusBarHeight)
{
    // Setup main window to fill the viewport (except for status bar)
    ImGuiViewport *viewport = ImGui::GetMainViewport();

    // Calculate the main window size (95% of viewport height)
    ImVec2 mainWindowPos = viewport->Pos;
    ImVec2 mainWindowSize = ImVec2(viewport->Size.x, viewport->Size.y * 0.95f);

    // Status bar height (5% of viewport)
    // float statusBarHeight = viewport->Size.y * 0.05f;

    // Calculate status bar height based on text line height
    // statusBarHeight = ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().WindowPadding.y * 2.0f;
    statusBarHeight = ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y;

    // Setup the main dockSpace window
    ImGui::SetNextWindowPos(mainWindowPos);
    ImGui::SetNextWindowSize(mainWindowSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    // ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    // window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    // window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    // window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    windowFlags |= ImGuiWindowFlags_NoBackground; // Remove background to prevent color issues

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("MainDockSpace", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    // Create the dockspace
    ImGuiID dockSpaceId = ImGui::GetID("MyDockSpace");
    ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), dockSpaceFlags);

    // Setup docking layout on first frame
    SetupDockingLayout(dockSpaceId);

    // Menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                /* Handle new */
            }
            if (ImGui::MenuItem("Open")) {
                /* Handle open */
            }
            if (ImGui::MenuItem("Save")) {
                /* Handle save */
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Reset Layout")) {
                isDockingSetup = false;
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Theme")) {
                if (ImGui::MenuItem("Dark Theme")) {
                    ImGui::StyleColorsDark();
                }
                if (ImGui::MenuItem("Light Theme")) {
                    ImGui::StyleColorsLight();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                /* Show about dialog */
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();
}

void RenderStatusBar(float statusBarHeight)
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();

    // Status Bar (fixed at bottom, 5% height)
    // ImVec2 statusBarPos = ImVec2(viewport->Pos.x, viewport->Pos.y + mainWindowSize.y);
    ImVec2 statusBarPos = ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - statusBarHeight);
    ImVec2 statusBarSize = ImVec2(viewport->Size.x, statusBarHeight);

    ImGui::SetNextWindowPos(statusBarPos);
    ImGui::SetNextWindowSize(statusBarSize);

    // ImGuiWindowFlags status_bar_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    // status_bar_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
    // status_bar_flags |= ImGuiWindowFlags_NoSavedSettings;

    ImGuiWindowFlags statusBarFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    statusBarFlags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
    statusBarFlags |= ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking;

    // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 5.0f));
    // ImGui::Begin("Status Bar", nullptr, statusBarFlags);
    // ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 3.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("Status Bar", nullptr, statusBarFlags);
    ImGui::PopStyleVar(2); // Pop 2 style vars instead of 1

    ImGui::Text("Ready | Line: 1, Col: 1 | FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::SameLine(ImGui::GetWindowWidth() - 150);
    ImGui::Text("Text Adventure IDE");

    ImGui::End();
}

void RenderEntitiesWindow()
{
    // No restrictive flags - window can be freely moved and docked
    ImGui::Begin("Entities");

    ImGui::Text("Entities List");
    ImGui::Separator();

    // Your entities content here
    if (ImGui::TreeNode("Scenes")) {
        ImGui::BulletText("Scene 1");
        ImGui::BulletText("Scene 2");
        ImGui::BulletText("Scene 3");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Characters")) {
        ImGui::BulletText("Hero");
        ImGui::BulletText("Villain");
        ImGui::BulletText("NPC 1");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Items")) {
        ImGui::BulletText("Sword");
        ImGui::BulletText("Key");
        ImGui::BulletText("Potion");
        ImGui::TreePop();
    }

    ImGui::Separator();
    if (ImGui::Button("Add New Entity")) {
        // Handle add entity
    }

    ImGui::End();
}

void RenderPropertiesWindow()
{
    // No restrictive flags - window can be freely moved and docked
    ImGui::Begin("Properties");

    ImGui::Text("Properties Panel");
    ImGui::Separator();

    // Your properties content here
    static char name[128] = "Object Name";
    ImGui::InputText("Name", name, IM_ARRAYSIZE(name));

    static char description[256] = "Object description...";
    ImGui::InputTextMultiline("Description", description, IM_ARRAYSIZE(description), ImVec2(-1, 100));

    static int property_value = 50;
    ImGui::SliderInt("Value", &property_value, 0, 100);

    static bool is_visible = true;
    ImGui::Checkbox("Visible", &is_visible);

    static bool is_interactive = true;
    ImGui::Checkbox("Interactive", &is_interactive);

    ImGui::Separator();

    if (ImGui::Button("Apply")) {
        // Handle apply
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        // Handle reset
    }

    ImGui::End();
}

void RenderInspectorWindow()
{
    // No restrictive flags - window can be freely moved and docked
    ImGui::Begin("Inspector");

    ImGui::Text("Inspector Panel");
    ImGui::Separator();

    // Your inspector content here
    ImGui::TextWrapped("Details about the selected item will appear here.");

    ImGui::Spacing();
    ImGui::Text("Selected: None");
    ImGui::Text("Type: -");
    ImGui::Text("Location: -");

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Select an entity to view details");

    ImGui::End();
}

void RenderWorkingArea()
{
    // No restrictive flags - window can be freely moved and docked
    ImGui::Begin("Working Area");

    ImGui::Text("Main Content Area");
    ImGui::Separator();

    // Your main content here
    // Example: Add tabs for different documents
    if (ImGui::BeginTabBar("DocumentTabs")) {
        std::string tabLabel = std::string(ICON_FA_CARET_UP) + " Script 1";
        if (ImGui::BeginTabItem(tabLabel.c_str())) {
            static char script_text[4096] =
                    ICON_FA_TREE " Forest Entrance\n"
                    ICON_FA_BOOK " Description: You stand at the edge of a dark forest...\n"
                    ICON_FA_COMMENT " Dialog: 'Welcome, traveler...'\n";

            // "// Your text adventure script here\n\nScene: Forest Entrance\nDescription: You stand at the edge of a dark forest...\n";
            ImGui::InputTextMultiline("##script", script_text, IM_ARRAYSIZE(script_text), ImVec2(-1, -1));
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Script 2")) {
            ImGui::Text("Content of script 2 - %s", ICON_FA_CHECK);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("+")) {
            ImGui::Text("Click to add new script");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void RenderIDE()
{
    static float statusBarHeight = 0.0f;

    // Render main dockspace (this also calculates status bar height)
    RenderMainWindow(statusBarHeight);

    // Render status bar
    RenderStatusBar(statusBarHeight);

    // Render all dockable windows
    RenderEntitiesWindow();
    RenderPropertiesWindow();
    RenderInspectorWindow();
    RenderWorkingArea();
}

int main()
{
    ADS::Core::App *app = new ADS::Core::App();

    spdlog::info("Start of Main");
    spdlog::info(format("Using the C++ version: {0}", __cplusplus));
    spdlog::info("Read the .env file");

    // Create window with SDL_Renderer graphics context
    std::pair<UUIDv4::UUID, ADS::UI::Window *> WindowInfo;
    ADS::UI::SDL_WINDOW_INFO *sdlWindowInformation = new ADS::UI::SDL_WINDOW_INFO(
            {
                    app->_t("WIN_TITLE", std::string(lang::RUSSIAN_RUSSIA)),
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                    System::DEFAULT_X_WIN_SIZE,
                    System::DEFAULT_Y_WIN_SIZE,
            }
            );
    ADS::UI::SDL_FLAGS *flags = new ADS::UI::SDL_FLAGS();
    ADS::UI::ImGuiManager &imguiObject = app->getImGuiObject();
    pair<UUIDv4::UUID, ADS::UI::Window *> windowInfo = imguiObject.newWindow(sdlWindowInformation, flags);
    ADS::UI::Window *mainWindow = windowInfo.second;
    SDL_Window *window = mainWindow->getWindow();
    ADS::Environment *env = app->getEnv();

    // float mainScale = mainWindow->getMainScale();
    SDL_Renderer *renderer = mainWindow->getRenderer();
    ImGuiIO *io = imguiObject.getIO();
    ADS::UI::Fonts *fm = imguiObject.getFontManager();
    fm->loadDefaultFonts();
    fm->loadIconFont("public/fonts/FontAwesome/fontawesome-webfont.ttf", 13.0f);
    fm->loadFontFromFile("lightFont", env->get("LIGHT_FONT")->data());
    fm->loadFontFromFile("mediumFont", env->get("MEDIUM_FONT")->data());
    fm->loadFontFromFile("regularFont", env->get("REGULAR_FONT")->data());

    ImFont *font = fm->getFont("lightFont");

    // Setup Platform/Renderer backends (MUST be called before main loop)
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // Apply window style for viewports and DPI scaling
    mainWindow->setStyle();

    // Main loop
    bool done = false;

    while (!done) {
        // Poll and handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID ==
                SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Render the IDE
        RenderIDE();

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io->DisplayFramebufferScale.x, io->DisplayFramebufferScale.y);
        // SDL_SetRenderDrawColor(renderer, 114, 144, 154, 255);
        SDL_SetRenderDrawColor(renderer, 45, 45, 48, 255); // Dark gray background
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

        // Update and Render additional Platform Windows
        if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        SDL_RenderPresent(renderer);
    }

    // Save the configuration
    ImGui::SaveIniSettingsToDisk(System::CONFIG_FILE);

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "End of Main" << std::endl;
    return 0;
}
