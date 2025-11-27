#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "EnemyManagerScript.h"

class FireAcidPowerupManagerScript : public TemplateSC {
public:
	int flamethrowerDamage = 1;
	float lingerTime;
	float timeBeforeDamageAgain;

	float currentTimer = 0.f;

	void Start() override {
		// ADD SFX OF FLAMETHROWER HERE

		physicsPtr->GetEventCallback()->OnTriggerStay(entity, [this](const physics::Collision& col) {
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
					if (enemyScript->timeBeforeDamageByFlamethrowerAgain <= 0.f) {
						enemyScript->enemyHealth -= flamethrowerDamage;

						enemyScript->timeBeforeDamageByFlamethrowerAgain = timeBeforeDamageAgain;
					}

					if (enemyScript->enemyHealth <= 0) {
						// ADD SFX OF ENEMY DEATH HERE

						ecsPtr->DeleteEntity(col.otherEntityID);
					}
				}
			}
		});
	}

	void Update() override {
		if (currentTimer <= lingerTime) {
			currentTimer += ecsPtr->m_GetDeltaTime();

			if (currentTimer >= lingerTime) {
				ecsPtr->DeleteEntity(entity);
			}
		}
	}


	REFLECTABLE(FireAcidPowerupManagerScript, flamethrowerDamage, lingerTime, timeBeforeDamageAgain)
};