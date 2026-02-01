#pragma once
#include "ScriptAdapter/TemplateSC.h"

// Forward Declaration (Crucial for compiling)
class BulletLogic;

class EnemyManagerScript : public TemplateSC {
public:
	R_AnimController* enemyController = nullptr;
	AnimatorComponent* anim = nullptr;
	AnimState currAnimationState{};

	// Set dis  to "Melee" or "Ranged" in the editor.
	std::string enemyType = "Melee";

	int agentid;
	int enemyHealth;
	float enemyMovementSpeed;

	bool enemyIsAttacking = false;

	float timeBeforeDamageByFlamethrowerAgain = 0.f;

	// Separate variables for ranges cuz if not the ranged will kiss u
	float enemyAttackRange = 2.5f;
	float enemyRangedAttackRange = 15.0f;

	float enemyChaseRange = 25.f;

	utility::GUID playerToChase;
	ecs::EntityID playerToChaseID;

	utility::GUID enemyHurtboxPrefab;
	utility::GUID enemyBulletPrefab;

	utility::GUID enemyHurtboxPosition;
	ecs::EntityID enemyHurtboxPositionID;

	ecs::EntityID enemyModelID;

	bool attackHurtboxIsSpawn = false;

	// Declarations Only
	void Start() override;
	void Update() override;

	REFLECTABLE(EnemyManagerScript, enemyHealth, enemyMovementSpeed, enemyType, enemyAttackRange, enemyRangedAttackRange, enemyChaseRange, playerToChase, enemyHurtboxPrefab, enemyBulletPrefab, enemyHurtboxPosition);
};

// --- IMPLEMENTATION ---
#include "BulletLogic.h"

inline void EnemyManagerScript::Start() {
	playerToChaseID = ecsPtr->GetEntityIDFromGUID(playerToChase);

	auto* trans = ecsPtr->GetComponent<TransformComponent>(entity);
	auto* capsule = ecsPtr->GetComponent<CapsuleColliderComponent>(entity);
	navMeshPtr->AddAgent(agentid, entity, trans->WorldTransformation.position, capsule->capsule.radius, capsule->capsule.height);

	std::vector<EntityID> children = ecsPtr->GetChild(entity).value();
	if (children.size() > 1)
	{
		enemyModelID = children[1];
		enemyHurtboxPositionID = children[0];
	}

	if (anim = ecsPtr->GetComponent<ecs::AnimatorComponent>(enemyModelID))
	{
		enemyController = resource->GetResource<R_AnimController>(anim->controllerGUID).get();
		if (enemyController)
		{
			currAnimationState = *enemyController->m_EnterState;
			anim->m_currentState = &currAnimationState;
			static_cast<AnimState*>(anim->m_currentState)->SetTrigger("ForcedEntry");
		}
	}
}

inline void EnemyManagerScript::Update() {
	auto* enemyTransform = ecsPtr->GetComponent<TransformComponent>(entity);
	auto* playerTransform = ecsPtr->GetComponent<TransformComponent>(playerToChaseID);
	auto* enemyHurtboxPositionTransform = ecsPtr->GetComponent<TransformComponent>(enemyHurtboxPositionID);

	if (!playerTransform || !enemyTransform || !enemyHurtboxPositionTransform) {
		LOGGING_WARN("Components not found");
		return;
	}

	//Copy state from controller
	if (enemyController)
	{
		//if (anim->m_currentState != &currAnimationState) {
		//	currAnimationState = *static_cast<AnimState*>(anim->m_currentState);
		//	anim->m_currentState = &currAnimationState;
		//}
	}

	// FUCK
	enemyHurtboxPositionTransform->LocalTransformation.position.z = 1.f;

	// CONSTANTLY LOOK AT PLAYER
	glm::vec3 direction = playerTransform->LocalTransformation.position - enemyTransform->LocalTransformation.position;
	direction = glm::normalize(direction);

	float yaw = std::atan2(direction.x, direction.z);
	float pitch = std::asin(-direction.y);
	glm::vec3 rotation(0.f, yaw, 0.f);
	glm::vec3 rotationDegrees = glm::degrees(rotation);

	enemyTransform->LocalTransformation.rotation = rotationDegrees;

	if (anim)
	{
		R_Animation* currAnim = resource->GetResource<R_Animation>(static_cast<AnimState*>(anim->m_currentState)->animationGUID).get();
		float animDuration = currAnim->GetDuration();

		if (anim->m_CurrentTime >= animDuration && !static_cast<AnimState*>(anim->m_currentState)->isLooping)
		{
			static_cast<AnimState*>(anim->m_currentState)->SetTrigger("AnimationFinished");
			anim->m_CurrentTime = 0.f;
			enemyIsAttacking = false;
			attackHurtboxIsSpawn = false;
		}
	}

	// SWITCH DISTANCE BASED ON STRING
	float currentActiveRange = (enemyType == "Ranged") ? enemyRangedAttackRange : enemyAttackRange;

	if (glm::distance(enemyTransform->LocalTransformation.position, playerTransform->LocalTransformation.position) <= currentActiveRange) {
		enemyIsAttacking = true;
		if (anim)
		{
			if (anim->m_currentState)
			{
				static_cast<AnimState*>(anim->m_currentState)->SetTrigger("AttackingPlayer");
			}
		}
	}

	if (enemyIsAttacking) {
		// NAVMESH STOP FOLLOWING (Implicit because we don't call MoveAgent in this block)
		// By setting the agent's destination to its own current position, I think it won't slide??
		if (enemyType == "Ranged") {
			navMeshPtr->MoveAgent(agentid, enemyTransform->WorldTransformation.position);
		}

		if (anim && !attackHurtboxIsSpawn)
		{
			if (anim->m_currentState)
			{
				R_Animation* currAnim = resource->GetResource<R_Animation>(static_cast<AnimState*>(anim->m_currentState)->animationGUID).get();
				float animDuration = currAnim->GetDuration();

				// Trigger attack at 50% animation
				if (anim->m_CurrentTime >= animDuration * 0.5f && static_cast<AnimState*>(anim->m_currentState)->name == "Attacking")
				{
					// SWITCH SPAWN BEHAVIOR BASED ON STRING
					if (enemyType == "Ranged")
					{
						// Ranged: Spawn Bullet
						std::shared_ptr<R_Scene> bullet = resource->GetResource<R_Scene>(enemyBulletPrefab);

						if (bullet) {
							std::string currentScene = ecsPtr->GetSceneByEntityID(entity);
							ecs::EntityID bulletID = DuplicatePrefabIntoScene<R_Scene>(currentScene, enemyBulletPrefab);

							if (auto* bulletTransform = ecsPtr->GetComponent<TransformComponent>(bulletID)) {
								// CHEESE WAY TO LOWER SHOT HEIGHT FOR NOW THE PLAYER TOO SHORTO
								bulletTransform->LocalTransformation.position = enemyHurtboxPositionTransform->WorldTransformation.position - glm::vec3(0.0f, 1.0f, 0.0f);
							}

							if (auto* bulletScript = ecsPtr->GetComponent<BulletLogic>(bulletID)) {
								bulletScript->direction = direction;
							}
						}
					}
					else
					{
						// Default/Melee: Spawn Hurtbox
						// Used 'else' here so if you typo the string, it at least does something (Melee)
						std::shared_ptr<R_Scene> enemyHurtbox = resource->GetResource<R_Scene>(enemyHurtboxPrefab);

						if (enemyHurtbox) {
							std::string currentScene = ecsPtr->GetSceneByEntityID(entity);
							ecs::EntityID enemyHurtboxID = DuplicatePrefabIntoScene<R_Scene>(currentScene, enemyHurtboxPrefab);

							if (auto* enemyHurtboxTransform = ecsPtr->GetComponent<TransformComponent>(enemyHurtboxID)) {
								enemyHurtboxTransform->LocalTransformation.position = enemyTransform->LocalTransformation.position + direction;
							}
						}
					}

					attackHurtboxIsSpawn = true;
				}
			}
		}
	}
	else if (glm::distance(enemyTransform->LocalTransformation.position, playerTransform->LocalTransformation.position) <= enemyChaseRange) {
		// NAVMESH FOLLOW TOWARDS PLAYER
		navMeshPtr->MoveAgent(agentid, playerTransform->LocalTransformation.position);

		if (anim)
		{
			if (anim->m_currentState)
			{
				static_cast<AnimState*>(anim->m_currentState)->SetTrigger("PlayerDetected");
			}

		}
	}

	if (timeBeforeDamageByFlamethrowerAgain >= 0.f) {
		timeBeforeDamageByFlamethrowerAgain -= ecsPtr->m_GetDeltaTime();
	}
}