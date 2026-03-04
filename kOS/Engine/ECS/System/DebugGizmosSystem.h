#ifndef DEBUGGIZMOSSYSTEM_H
#define DEBUGGIZMOSSYSTEM_H

#include "ECS/ECS.h"
#include "System.h"

namespace ecs {
	class DebugGizmosSystem : public ISystem {
	public:
		using ISystem::ISystem;
		void Init() override;
		void Update() override;

		REFLECTABLE(DebugGizmosSystem)
	};
}

#endif