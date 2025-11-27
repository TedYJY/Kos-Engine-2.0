#pragma once
#include "ScriptAdapter/TemplateSC.h"
#include "EnemyManagerScript.h"

class LightningAcidPowerupManagerScript : public TemplateSC {
public:
	float starfallDamage = 5.f;
	float starfallForce;
	float lingerTime;

	glm::vec3 direction;

	float currentTimer = 0.f;

	void Start() override {
		// ADD SFX OF STARFALL HERE

		physicsPtr->GetEventCallback()->OnTriggerEnter(entity, [this](const physics::Collision& col) {
			if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Enemy") {
				if (auto* enemyScript = ecsPtr->GetComponent<EnemyManagerScript>(col.otherEntityID)) {
					enemyScript->enemyHealth -= static_cast<int>(starfallDamage);

					if (enemyScript->enemyHealth <= 0) {
						// ADD SFX OF ENEMY DEATH HERE

						ecsPtr->DeleteEntity(col.otherEntityID);
					}
				}
			}
		});

		// TODO: READJUST THE FORCE OF THE STARFALL TO BE MORE LIKE A CATAPULT, LIKE IN THE UNITY GAME
		direction.y += 1.f;
		direction = glm::normalize(direction) * starfallForce;
		physicsPtr->AddForce(ecsPtr->GetComponent<RigidbodyComponent>(entity)->actor, direction, ForceMode::Impulse);
	}

	void Update() override {
		if (currentTimer <= lingerTime) {
			currentTimer += ecsPtr->m_GetDeltaTime();

			if (currentTimer >= lingerTime) {
				ecsPtr->DeleteEntity(entity);
			}
		}
	}


	REFLECTABLE(LightningAcidPowerupManagerScript, starfallDamage, starfallForce, lingerTime)
};