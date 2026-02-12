/******************************************************************/
/*!
\file      imgui_preferences_window.cpp
\author    Mog Shi Feng
\par
\date
\brief

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "implot_internal.h"
#include "Editor.h"

std::string FormatTime(float time) {
    int min = static_cast<int>(time / 60.0f);
    int sec = static_cast<int>(round(time)) % 60;
    std::stringstream ss;
    ss << std::setw(1) << std::setfill('0') << min << ":" << std::setw(2) << std::setfill('0') << sec;
    return ss.str();
}

void gui::ImGuiHandler::DrawAnimationWindow() {

    if (ImGui::Begin("Animation Window")) {
        static float currentTime = 0.0f; // In minutes?
        static float endTime = 1.0f;
        static int sampleRate = 60;
        static bool isPlaying = false;
        static bool previewMode = false;
        static bool recordMode = false;
        static int draggingKeyframe = -1;
        static float pixelsPerSecond = 50.f; // Zooming will prolly edit this value

        float textHeight = ImGui::CalcTextSize("0:00").y;

        ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable | ImGuiTableFlags_ContextMenuInBody;

        if (ImGui::BeginTable("##AnimationWindow", 2, flags)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            //if (ImGui::Button("<<"));
            //ImGui::SameLine();
            //if (ImGui::Button("<"));
            //ImGui::SameLine();
            if (ImGui::Button(isPlaying ? "Pause" : "Play")) { isPlaying = !isPlaying; }
            //ImGui::SameLine();
            //if (ImGui::Button(">"));
            //ImGui::SameLine();
            //if (ImGui::Button(">>"));
            ImGui::SameLine();
            ImGui::Checkbox("Preview", &previewMode);
            ImGui::SameLine();
            ImGui::Checkbox("Record", &recordMode);

            if (ImGui::InputInt("Sample Rate", &sampleRate)) {
                if (sampleRate < 1) sampleRate = 1;
            }

            // Examples
            if (ImGui::TreeNode("Transform")) {
                ImGui::Selectable("X");
                ImGui::Selectable("Y");
                ImGui::Selectable("Z");
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Audio")) {
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Rigidbody")) {
                ImGui::TreePop();
            }

            //if (ImGui::Button("+"));

            ImGui::TableNextColumn();

            ImVec2 contentSpace = ImGui::GetContentRegionAvail();
            float secPerInterval = 5.f / sampleRate; //  Per Time Tick how much time has passed.

            // Timeline Slider
            ImGui::SetNextItemWidth(contentSpace.x);
            int timeInSeconds = static_cast<int>(currentTime * 60.f);
            int maxTime = static_cast<int>(contentSpace.x / pixelsPerSecond * secPerInterval * 60);
            if (ImGui::SliderInt("##timeline_slider", &timeInSeconds, 0, maxTime, FormatTime(static_cast<float>(timeInSeconds)).c_str())) {
                currentTime = static_cast<float>(timeInSeconds) / 60.f;
            }
            contentSpace.y = ImGui::GetContentRegionAvail().y;

            // Dopesheet
            if (ImGui::BeginChild("Dopesheet", contentSpace)) {
                float startX = ImGui::GetCursorScreenPos().x;
                float startY = ImGui::GetCursorScreenPos().y;

                ImColor defaultCol(ImGui::GetStyle().Colors[ImGuiCol_Text]);
                float lineEndY = startY + contentSpace.y - textHeight;
                // Draw Timeline
                for (int i = 0; i <= contentSpace.x / pixelsPerSecond; i++) {
                    float time = i * secPerInterval;
                    float x = startX + i * pixelsPerSecond;
                    ImGui::GetWindowDrawList()->AddLine(ImVec2(x, startY), ImVec2(x, lineEndY), defaultCol);
                    ImGui::SetCursorScreenPos(ImVec2(x, lineEndY));
                    ImGui::Text("%s", FormatTime(time * 60.f).c_str());
                }

                // Draw Playback Head (vertical white line)
                float playbackHeadX = startX + currentTime / secPerInterval * pixelsPerSecond;
                ImVec2 lineStart(playbackHeadX, startY);
                ImVec2 lineEnd(playbackHeadX, lineEndY);
                recordMode ?
                    ImGui::GetWindowDrawList()->AddLine(lineStart, lineEnd, IM_COL32(255, 0, 0, 255), 2.0f) :
                    ImGui::GetWindowDrawList()->AddLine(lineStart, lineEnd, IM_COL32(255, 255, 255, 255), 2.0f);

                // Create a small invisible button for the playback head
                ImGui::SetCursorScreenPos(ImVec2(playbackHeadX - 5, startY));
                ImGui::InvisibleButton("##playback_head", ImVec2(10, 200));

                // Handle playback head dragging
                static bool draggingPlaybackHead = false;

                if (ImGui::IsItemHovered() || draggingPlaybackHead) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                }

                if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                    // Pause playback while dragging
                    isPlaying = false;

                    // Convert mouse drag to time
                    float mouseDeltaX = ImGui::GetMouseDragDelta(0).x;
                    float newX = playbackHeadX + mouseDeltaX;
                    currentTime = (newX - startX) * secPerInterval / pixelsPerSecond;

                    // Clamp between 0-1
                    currentTime = std::clamp(currentTime, 0.0f, static_cast<float>(maxTime / 60.f));

                    // Reset drag delta
                    ImGui::ResetMouseDragDelta();
                    draggingPlaybackHead = true;
                }
                else {
                    draggingPlaybackHead = false;
                }

                // Handle dragging keyframes
                if (draggingKeyframe != -1 && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                    float mouseX = ImGui::GetMousePos().x - startX;
                    float newTime = mouseX / pixelsPerSecond;
                    //animation.GetKeyframes()[draggingKeyframe].time = newTime;
                    //std::sort(animation.GetKeyframes().begin(), animation.GetKeyframes().end(), [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
                }
                else {
                    draggingKeyframe = -1;
                }

                // Handle Keyframe Addition (Right-click to add)
                if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    float mouseX = ImGui::GetMousePos().x - startX;
                    float time = mouseX / pixelsPerSecond;
                    //animation.AddKeyframe(time, 0.0f); // Add a keyframe at the clicked time
                }
                ImGui::EndChild();
            }
            ImGui::EndTable();
        }

        // Sprite/ Model Preview Window
        //if (previewMode) {
        //    ImGui::Begin("Preview", 0, ImGuiWindowFlags_NoScrollbar);
        //    auto currentSprite = GetSpriteAtCurrentTime();
        //    if (currentSprite) {
        //        if (ImGui::GetContentRegionAvail().x < ImGui::GetContentRegionAvail().y) {
        //            ImGui::Image((ImTextureID)(currentSprite->RetrieveTexture()), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
        //        }
        //        else {
        //            ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.f - ImGui::GetContentRegionAvail().y / 2.f);
        //            ImGui::Image((ImTextureID)(currentSprite->RetrieveTexture()), ImVec2(ImGui::GetContentRegionAvail().y, ImGui::GetContentRegionAvail().y), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
        //        }
        //        ImGui::Text(currentSprite->RetrieveName().c_str());
        //    }
        //    ImGui::End();
        //}
    }
  
    ImGui::End();
}
