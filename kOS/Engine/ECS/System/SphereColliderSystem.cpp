#include "Config/pch.h"
#include "SphereColliderSystem.h"
#include "Physics/PhysicsManager.h"

namespace {
	constexpr float MINSIZE = 0.01f;
}

namespace ecs {
	void SphereColliderSystem::Init() {
		onDeregister.Add([&](EntityID id) {
			SphereColliderComponent* sphere = m_ecs.GetComponent<SphereColliderComponent>(id);
			if (!sphere || !sphere->shape) { return; }

			RigidbodyComponent* rb = m_ecs.GetComponent<RigidbodyComponent>(id);
			if (rb) { sphere->shape = nullptr; }
			else {
				PxRigidStatic* actor = sphere->actor ? static_cast<PxRigidStatic*>(sphere->actor) : nullptr;
				PxShape* shape = static_cast<PxShape*>(sphere->shape);
				if (actor) {
					actor->detachShape(*shape);
					if (actor->getNbShapes() == 0) {
						PxScene* scene = actor->getScene();
						if (scene) { scene->removeActor(*actor); }
						actor->release();
					}
				}
				shape->release();
				sphere->shape = nullptr;
				sphere->actor = nullptr;
			}
		});
	}

	void SphereColliderSystem::Update() {
		const auto& entities = m_entities.Data();
		for (EntityID id : entities) {
			TransformComponent* trans = m_ecs.GetComponent<TransformComponent>(id);
			NameComponent* name = m_ecs.GetComponent<NameComponent>(id);
			SphereColliderComponent* sphere = m_ecs.GetComponent<SphereColliderComponent>(id);

			if (name->hide || !sphere) { continue; }

			PxFilterData filter;
			filter.word0 = name->Layer;

			glm::vec3& scale = trans->LocalTransformation.scale;
			scale.x = glm::max(scale.x, MINSIZE);
			scale.y = glm::max(scale.y, MINSIZE);
			scale.z = glm::max(scale.z, MINSIZE);

			float radius = sphere->sphere.radius * glm::max(scale.x, glm::max(scale.y, scale.z));
			PxShape* shape = static_cast<PxShape*>(sphere->shape);
			PxSphereGeometry geometry{ radius };

			if (!shape) {
				shape = m_physicsManager.GetPhysics()->createShape(geometry, *m_physicsManager.GetDefaultMaterial(), true);
				sphere->shape = shape;
			}

			shape->setGeometry(geometry);
			glm::vec3 scaledCenter = sphere->sphere.center * scale;
			shape->setLocalPose(PxTransform{ PxVec3{ scaledCenter.x, scaledCenter.y, scaledCenter.z } });
			ToPhysxIsTrigger(shape, sphere->isTrigger);
			shape->setSimulationFilterData(filter);
			shape->setQueryFilterData(filter);

			RigidbodyComponent* rb = m_ecs.GetComponent<RigidbodyComponent>(id);
			if (!rb) {
				PxRigidStatic* actor = sphere->actor ? static_cast<PxRigidStatic*>(sphere->actor) : nullptr;
				glm::vec3 pos = trans->WorldTransformation.position;
				glm::quat rot{ glm::radians(trans->WorldTransformation.rotation) };
				PxTransform pxTrans{ PxVec3{ pos.x, pos.y, pos.z }, PxQuat{ rot.x, rot.y, rot.z, rot.w } };
				if (!actor) {
					actor = m_physicsManager.GetPhysics()->createRigidStatic(pxTrans);
					actor->userData = reinterpret_cast<void*>(static_cast<uintptr_t>(id));
					m_physicsManager.GetScene()->addActor(*actor);
					sphere->actor = actor;
				} else {
					actor->setGlobalPose(pxTrans);
				}
				if (!IsShapeAttachedToActor(actor, shape)) {
					actor->attachShape(*shape);
				}
			} else {
				sphere->actor = rb->actor;
			}
		}
	}

	bool SphereColliderSystem::IsShapeAttachedToActor(PxRigidActor* actor, PxShape* shape) {
		if (!shape) { return false; }
		PxU32 nbShapes = actor->getNbShapes();
		std::vector<PxShape*> shapes(nbShapes);
		actor->getShapes(shapes.data(), nbShapes);
		for (PxU32 i = 0; i < nbShapes; ++i) {
			if (shapes[i] == shape) {
				return true;
			}
		}
		return false;
	}
}