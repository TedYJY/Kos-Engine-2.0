#pragma once
#include "ScriptAdapter/TemplateSC.h"

class EnemyManagerScript : public TemplateSC {
public:
	int enemyHealth;
	float enemyMovementSpeed;

	bool enemyIsAttacking = false;

	float timeBeforeDamageByFlamethrowerAgain = 0.f;

	void Start() override {

	}

	void Update() override {
		if (enemyIsAttacking) {
			// NAVMESH STOP FOLLOWING

			// ADD ENEMY ATTACKING ANIMATION

			// if (CHECK IF ANIMATION IS DONE) {
			//		enemyIsAttacking = false;
			// }
		}
		else {
			// NAVMESH FOLLOW TOWARDS PLAYER

			// ADD ENEMY RUNNING ANIMATION
		}

		if (timeBeforeDamageByFlamethrowerAgain >= 0.f) {
			timeBeforeDamageByFlamethrowerAgain -= ecsPtr->m_GetDeltaTime();
		}
	}

	REFLECTABLE(EnemyManagerScript, enemyHealth, enemyMovementSpeed);
};