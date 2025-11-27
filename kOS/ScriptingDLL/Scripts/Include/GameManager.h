#pragma once
#include "TemplateSC.h"

class GameManager : public TemplateSC {
public:
	utility::GUID enemyObjectPrefab;

	utility::GUID enemySpawnpoint;
	ecs::EntityID enemySpawnpointID;

	void Start() override {
		// ADD SFX OF BGM HERE

		enemySpawnpointID = ecsPtr->GetEntityIDFromGUID(enemySpawnpoint);
	}

	void Update() override {
		// SPAWN ENEMIES
		if (Input->IsKeyTriggered(keys::X)) {
			std::shared_ptr<R_Scene> enemyObject = resource->GetResource<R_Scene>(enemyObjectPrefab);

			if (enemyObject) {
				std::string currentScene = ecsPtr->GetSceneByEntityID(entity);
				ecs::EntityID enemyObjectID = DuplicatePrefabIntoScene<R_Scene>(currentScene, enemyObjectPrefab);

				if (auto* enemyObjectTransform = ecsPtr->GetComponent<TransformComponent>(enemyObjectID)) {
					enemyObjectTransform->LocalTransformation.position = ecsPtr->GetComponent<TransformComponent>(enemySpawnpointID)->WorldTransformation.position;
				}
			}
		}
	}

	REFLECTABLE(GameManager, enemyObjectPrefab, enemySpawnpoint)
};