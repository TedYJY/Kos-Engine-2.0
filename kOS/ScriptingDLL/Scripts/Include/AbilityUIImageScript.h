#pragma once

#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ScriptAdapter/TemplateSC.h"

class AbilityUIImageScript : public TemplateSC {
public:
    std::string currAbility = "none"; // Tracks current power-up
    std::string lastAbility = "";     // Tracks last power-up to avoid redundant updates

    utility::GUID playerObject;       // Reference to the player object
    ecs::EntityID playerObjectID;

    // References to sprites for each power-up
    utility::GUID noneSprite;
    utility::GUID fireSprite;
    utility::GUID lightningSprite;
    utility::GUID acidSprite;
    utility::GUID fireLightningSprite;
    utility::GUID fireAcidSprite;
    utility::GUID lightningAcidSprite;

    void Start() override {
        // Resolve the entity ID from the player object GUID
        playerObjectID = ecsPtr->GetEntityIDFromGUID(playerObject);

        // Initialize the sprite on start
        UpdateAbilitySprite();
    }

    void Update() override {
        // Fetch the current power-up from PlayerManagerScript
        if (auto* playerMgr = ecsPtr->GetComponent<PlayerManagerScript>(playerObjectID)) {
            currAbility = playerMgr->currentPowerup;
        }

        // Only update the sprite if the ability changed
        if (currAbility != lastAbility) {
            UpdateAbilitySprite();
            lastAbility = currAbility;
        }
    }

private:
    void UpdateAbilitySprite() {
        if (auto* sc = ecsPtr->GetComponent<ecs::SpriteComponent>(entity)) {
            if (currAbility == "fire") sc->spriteGUID = fireSprite;
            else if (currAbility == "lightning") sc->spriteGUID = lightningSprite;
            else if (currAbility == "acid") sc->spriteGUID = acidSprite;
            else if (currAbility == "firelightning") sc->spriteGUID = fireLightningSprite;
            else if (currAbility == "fireacid") sc->spriteGUID = fireAcidSprite;
            else if (currAbility == "lightningacid") sc->spriteGUID = lightningAcidSprite;
            else sc->spriteGUID = noneSprite; // Default for "none" or unknown abilities
        }
    }

public:
    REFLECTABLE(AbilityUIImageScript, currAbility, lastAbility, playerObject, playerObjectID,
        noneSprite, fireSprite, lightningSprite, acidSprite,
        fireLightningSprite, fireAcidSprite, lightningAcidSprite);
};
