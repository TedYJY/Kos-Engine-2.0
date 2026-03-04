#include "Config/pch.h"
#include "DebugGizmosSystem.h"

namespace ecs {
	void DebugGizmosSystem::Init() {

	}

	void DebugGizmosSystem::Update() {
        for (const auto& line : m_physicsManager.m_debugRays) {
            m_graphicsManager.gm_PushLineDebugData(DebugLineData{
                line.start,
                line.end,
                line.color
            });
        }
        m_physicsManager.m_debugRays.clear();
        for (const auto& sphere : m_physicsManager.m_debugSpheres) {
            glm::mat4 model = glm::translate(glm::mat4{ 1.0f }, sphere.center) * glm::scale(glm::mat4{ 1.0f }, glm::vec3{ sphere.radius });
            m_graphicsManager.gm_PushSphereDebugData(BasicDebugData{ model, sphere.color });
        }
        m_physicsManager.m_debugSpheres.clear();
	}
}