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

/**
 * @brief Convert RGB color values to ImGui color format
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Converts standard RGB color values (0-255 range) to ImGui's ImVec4
 * format (0.0-1.0 range) by dividing each component by 255.0.
 *
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @param a Alpha/opacity component (0-255), default is 255 (fully opaque)
 *
 * @return ImVec4 Color vector in ImGui format with normalized values (0.0-1.0)
 */
ImVec4 RGB(int r, int g, int b, int a = 255)
{
    return ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

/**
 * @brief Setup the default docking layout for the IDE
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Configures the default layout for IDE windows if no saved layout exists.
 * Creates a split layout with Entities panel on the left (20%), Working Area
 * in the center, and Properties/Inspector panels on the right (20%).
 * If a saved layout exists in the .ini file, it is preserved and this function
 * returns without making changes.
 *
 * @param dockSpaceId The ImGui dock space identifier for the main window
 *
 * @note Only runs once per application session (controlled by isDockingSetup flag)
 * @note Saved layouts in imgui.ini take precedence over the default layout
 */
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

/**
 * @brief Render the main window with docking support and menu bar
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Creates and renders the main application window that fills 95% of the viewport
 * height (leaving 5% for the status bar). Sets up the dockspace for window docking
 * and renders the main menu bar with File, View, and Help menus. The status bar
 * height is calculated and returned via the output parameter.
 *
 * @param statusBarHeight Output parameter that receives the calculated status bar height
 *
 * @note This window has no title bar, is not movable/resizable, and has no background
 * @see SetupDockingLayout(), RenderStatusBar()
 */
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

/**
 * @brief Render the status bar at the bottom of the application window
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Creates a fixed status bar at the bottom of the viewport displaying current
 * application state information including line/column position, FPS counter,
 * and application name. The status bar occupies 5% of the viewport height
 * and cannot be moved, resized, or docked.
 *
 * @param statusBarHeight Height of the status bar in pixels
 *
 * @note The status bar is always positioned at the bottom of the viewport
 * @see RenderMainWindow()
 */
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

/**
 * @brief Render the Entities panel showing game objects hierarchy
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Displays a dockable panel listing all game entities organized in a tree
 * structure. Currently shows placeholder categories for Scenes, Characters,
 * and Items. The panel can be freely moved and docked within the IDE.
 *
 * @note This window is dockable and can be rearranged by the user
 * @see SetupDockingLayout() for default position
 */
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

/**
 * @brief Render the Properties panel for editing selected object properties
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Displays a dockable panel with editable properties for the currently selected
 * entity. Includes text inputs, multi-line text areas, sliders, and checkboxes
 * for common object properties like name, description, value, visibility, and
 * interactivity. Changes can be applied or reset using the buttons at the bottom.
 *
 * @note This window is dockable and can be rearranged by the user
 * @see SetupDockingLayout() for default position
 */
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

/**
 * @brief Render the Inspector panel showing detailed information
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Displays a dockable panel with detailed information about the currently
 * selected entity. Shows metadata such as selection status, type, and location.
 * Currently displays placeholder text prompting the user to select an entity.
 *
 * @note This window is dockable and can be rearranged by the user
 * @see SetupDockingLayout() for default position
 */
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

/**
 * @brief Render the main Working Area with tabbed document interface
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Displays the central dockable panel containing tabbed editors for scripts
 * and other documents. Supports multiple tabs for different files and includes
 * a multi-line text editor for each tab. Uses icon fonts (FontAwesome) for
 * visual elements in the content.
 *
 * @note This window is dockable and can be rearranged by the user
 * @note The "+" tab allows users to create new script documents
 * @see SetupDockingLayout() for default position (center)
 */
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

/**
 * @brief Render all IDE windows and panels
 *
 * @author Cayetano H. Osma <cayetano.hernandez.osma@gmail.com>
 * @version Dec 2025
 *
 * Orchestrates the rendering of all IDE components in the correct order:
 * main window with dockspace, status bar, and all dockable panels (Entities,
 * Properties, Inspector, Working Area). This function is called once per frame
 * during the main application render loop.
 *
 * @note Must be called between ImGui::NewFrame() and ImGui::Render()
 * @see App::render() for usage in main loop
 */
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

    // Create window
    ADS::UI::SDL_WINDOW_INFO *sdlWindowInformation = new ADS::UI::SDL_WINDOW_INFO({
            app->_t("WIN_TITLE", std::string(lang::RUSSIAN_RUSSIA)),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            System::DEFAULT_X_WIN_SIZE,
            System::DEFAULT_Y_WIN_SIZE,
    });

    ADS::UI::SDL_FLAGS *flags = new ADS::UI::SDL_FLAGS();
    ADS::UI::ImGuiManager &imguiObject = app->getImGuiObject();
    pair<UUIDv4::UUID, ADS::UI::Window *> windowInfo = imguiObject.newWindow(sdlWindowInformation, flags);
    ADS::UI::Window *mainWindow = windowInfo.second;
    app->setMainWindow(mainWindow);

    // Load fonts
    ADS::Environment *env = app->getEnv();
    ADS::UI::Fonts *fm = imguiObject.getFontManager();
    fm->loadDefaultFonts();
    fm->loadIconFont("public/fonts/FontAwesome/fontawesome-webfont.ttf", 13.0f);
    fm->loadFontFromFile("lightFont", env->get("LIGHT_FONT")->data());
    fm->loadFontFromFile("mediumFont", env->get("MEDIUM_FONT")->data());
    fm->loadFontFromFile("regularFont", env->get("REGULAR_FONT")->data());

    // Setup backends
    ImGui_ImplSDL2_InitForSDLRenderer(mainWindow->getWindow(), mainWindow->getRenderer());
    ImGui_ImplSDLRenderer2_Init(mainWindow->getRenderer());
    mainWindow->setStyle();

    // Run the application
    app->run();

    // Cleanup
    app->shutdown();
    delete app;

    return 0;
}
