#pragma once
#include "ScriptAdapter/TemplateSC.h"

class EnemyHurtboxScript : public TemplateSC {
public:
	int enemyDamage = 1;

	float timeBeforeDeath = 0.5f;
	float currentTimer = 0.f;

	void Start() override {
		physicsPtr->GetEventCallback()->OnTriggerEnter(entity, [this](const physics::Collision& col) {
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Player") {
				// ADD SFX OF PLAYER GETTING HURT HERE

				ecsPtr->GetComponent<PlayerManagerScript>(col.otherEntityID)->currPlayerHitPoints -= enemyDamage;

				if (ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)->enemyHealth <= 0) {
					// GAME OVER
				}
			}

			});
	}

	void Update() override {
		if (currentTimer < timeBeforeDeath) {
			currentTimer += ecsPtr->m_GetDeltaTime();

			if (currentTimer >= timeBeforeDeath) {
				ecsPtr->DeleteEntity(entity);
			}
		}
	}

	REFLECTABLE(EnemyHurtboxScript, enemyDamage)
};