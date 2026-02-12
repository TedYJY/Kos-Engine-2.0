/******************************************************************/
/*!
\file      imgui_logs_window.cpp
\author    Rayner Tan
\par       raynerweichen.tan@digipen.edu
\date      Sept 28, 2024
\brief     Logging Panel for ImGUI

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Editor.h"
#include "Debugging/Logging.h"

#define ImGuiBlack ImVec4(0.0, 0.0, 0.0, 1.0)
#define ImGuiWhite ImVec4(1.0, 1.0, 1.0, 1.0)
#define ImGuiOrange ImVec4(1.0, 0.75, 0.0, 1.0)
#define ImGuiRed ImVec4(1.0, 0.0, 0.0, 1.0)

void gui::ImGuiHandler::DrawLogsWindow() {

    //static ExampleAppLog log;
    bool open = true;
    // For the demo: add a debug button _BEFORE_ the normal log window contents
    // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
    // Most of the contents of the window will be added by the log.Draw() call.
    //ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Log", &open)) {

        std::vector<std::string> log_Entries = logging::Logger::m_GetInstance().m_GetLogList();
        //for (size_t i = 0; i < log_Entries.size(); ++i) {
        //    ImGui::Text(log_Entries[i].c_str());
        //}
        for (size_t i = log_Entries.size(); i > 0; --i) {
            if (log_Entries[i - 1][1] == 'I') {
                //ImGui::TextColored(ImGuiBlack, log_Entries[i - 1].c_str());
                ImGui::Text(log_Entries[i - 1].c_str());
            }
            else if (log_Entries[i - 1][1] == 'W') {
                ImGui::TextColored(ImGuiOrange, log_Entries[i - 1].c_str());
            }
            else if (log_Entries[i - 1][1] == 'E') {
                ImGui::TextColored(ImGuiRed, log_Entries[i - 1].c_str());
            }
        }
    }


    ImGui::End();

    // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
    //log.Draw("Example: Log", &open);
}