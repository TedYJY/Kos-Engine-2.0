#pragma once

#include "ECS/Component/Component.h"
#include "Config/pch.h"
#include "ScriptAdapter/TemplateSC.h"


class AbilityUIScript : public TemplateSC {
public:
	std::string currAbility = "none";
	utility::GUID playerObject;
	ecs::EntityID playerObjectID;

	void Start() {
		playerObjectID = ecsPtr->GetEntityIDFromGUID(playerObject);
	}

	void Update() {
		if (auto* tc = ecsPtr->GetComponent<ecs::TextComponent>(entity)) {
			//currAbility = ecsPtr->GetComponent<PlayerManagerScript>(playerObjectID)->currentPowerup;
			tc->text = currAbility;
		}
	}

	REFLECTABLE(AbilityUIScript, currAbility, playerObject, playerObjectID);
};