#pragma once
#include "ScriptAdapter/TemplateSC.h"

// FORWARD DECLARATION: Fixes circular dependency
class PlayerManagerScript;

class EnemyBulletLogic : public TemplateSC {
public:
	int bulletDamage = 2;
	float bulletSpeed = 100.f;
	glm::vec3 direction;

	float timeBeforeDeath = 2.5f;
	float currentTimer = 0.f;

	ScoreManagerScript* scoreManager = nullptr;
	int scoreValue = 100;

	utility::GUID enemyDeathSfxGUID_1;
	utility::GUID enemyDeathSfxGUID_2;
	utility::GUID enemyDeathSfxGUID_3;
	std::vector<utility::GUID> enemyDeathSfxGUIDs;

	// Declarations Only (Implementation at the bottom)
	void Start() override;
	void Update() override;
	void PlayRandomEnemyDeathSFX();

	REFLECTABLE(BulletLogic, bulletDamage, bulletSpeed, enemyDeathSfxGUID_1, enemyDeathSfxGUID_2, enemyDeathSfxGUID_3)
};

// --- IMPLEMENTATION SECTION ---
#include "PlayerManagerScript.h"

inline void EnemyBulletLogic::Start() {
	enemyDeathSfxGUIDs.clear();
	if (!enemyDeathSfxGUID_1.Empty()) enemyDeathSfxGUIDs.push_back(enemyDeathSfxGUID_1);
	if (!enemyDeathSfxGUID_2.Empty()) enemyDeathSfxGUIDs.push_back(enemyDeathSfxGUID_2);
	if (!enemyDeathSfxGUID_3.Empty()) enemyDeathSfxGUIDs.push_back(enemyDeathSfxGUID_3); \

	physicsPtr->GetEventCallback()->OnTriggerEnter(entity, [this](const physics::Collision& col) {
		if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Player") {
			// ADD SFX OF ENEMY DEATH HERE - DONE
			PlayRandomEnemyDeathSFX();

			ecsPtr->GetComponent<PlayerManagerScript>(col.otherEntityID)->currPlayerHitPoints -= bulletDamage;
		}

		if (ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Ground" || ecsPtr->GetComponent<NameComponent>(col.otherEntityID)->entityTag == "Default") {
			ecsPtr->DeleteEntity(entity);
		}
	});
}

inline void EnemyBulletLogic::Update() {
	if (auto* tc = ecsPtr->GetComponent<ecs::TransformComponent>(entity)) {
		tc->LocalTransformation.position += direction * bulletSpeed * ecsPtr->m_GetDeltaTime();
	}

	if (currentTimer < timeBeforeDeath) {
		currentTimer += ecsPtr->m_GetDeltaTime();

		if (currentTimer >= timeBeforeDeath) {
			ecsPtr->DeleteEntity(entity);

		}
	}
}

inline void EnemyBulletLogic::PlayRandomEnemyDeathSFX()
{
	auto* ac = ecsPtr->GetComponent<ecs::AudioComponent>(entity);
	if (!ac) return;

	std::vector<ecs::AudioFile*> enemyDeathSfxPool;

	for (auto& af : ac->audioFiles) {
		if (!af.isSFX) continue;

		for (auto& g : enemyDeathSfxGUIDs) {
			if (!g.Empty() && af.audioGUID == g) {
				enemyDeathSfxPool.push_back(&af);
				break;
			}
		}
	}

	if (enemyDeathSfxPool.empty()) {
		std::cout << "[BulletLogic] No enemy death SFX found.\n";
		return;
	}

	int idx = rand() % static_cast<int>(enemyDeathSfxPool.size());
	enemyDeathSfxPool[idx]->requestPlay = true;

	std::cout << "[BulletLogic] Playing enemy death SFX index " << idx << "\n";
}