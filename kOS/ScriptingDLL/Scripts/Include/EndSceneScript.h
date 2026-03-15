#pragma once

#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ScriptAdapter/TemplateSC.h"


class EndSceneScript : public TemplateSC {
public:

    utility::GUID video_GUID;
    utility::GUID next_Scene;
    utility::GUID current_Scene;
    utility::GUID quit_Button;
    utility::GUID audio;
    utility::GUID audio_2;
    utility::GUID button_canva;
    float timer = 0.0f;
    void Start() override;
    void Update() override;
    EntityID v_ID, quit_ButtonID, canva;


    REFLECTABLE(EndSceneScript, video_GUID, current_Scene, next_Scene, quit_Button, audio, audio_2, button_canva);
};


inline void EndSceneScript::Start() {
    ecsPtr->SetTimeScale(1.0f);
    ecsPtr->SetState(RUNNING);
    v_ID = ecsPtr->GetEntityIDFromGUID(video_GUID);
    quit_ButtonID = ecsPtr->GetEntityIDFromGUID(quit_Button);
    canva = ecsPtr->GetEntityIDFromGUID(button_canva);

    if (auto* ac = ecsPtr->GetComponent<ecs::AudioComponent>(entity)) {
        for (auto& af : ac->audioFiles) {
            if (af.audioGUID == audio && af.isSFX) {
                af.requestPlay = true;
                break;
            }
        }
    }

    // Turn off mouse
    Input->HideCursor(true);
}

inline void EndSceneScript::Update() {

    timer += ecsPtr->m_GetDeltaTime();

    auto* vc = ecsPtr->GetComponent<ecs::VideoComponent>(v_ID);
    auto* quit_bc = ecsPtr->GetComponent<ecs::ButtonComponent>(quit_ButtonID);

    bool quit_isClicked = quit_bc->isPressed;

    // Show quit button and pause video after 10 seconds
    if (timer > 10.f) {
        ecsPtr->SetActive(canva, true);
        vc->pause = true;
        Input->HideCursor(false);
    }

    if (quit_isClicked) {
        Input->InputExitWindow();
    }

    if ((!vc->playing) || Input->IsKeyTriggered(keys::K)) {
        Scenes->ClearAllScene();
        Scenes->LoadScene(next_Scene);
    }
}