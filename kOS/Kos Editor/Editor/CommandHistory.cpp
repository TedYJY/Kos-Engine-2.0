#include "CommandHistory.h"
#include "imgui.h"

std::stack<CommandHistory::CommandWrapper> CommandHistory::commandQueue;
std::stack<CommandHistory::CommandWrapper> CommandHistory::redoQueue;

#define CACHEDSCENE "CommandHistory"
void CommandHistory::Init() {
	m_ecs.AddScene(CACHEDSCENE, SceneData());
	m_ecs.sceneMap[CACHEDSCENE].isPrefab = true;
}

void CommandHistory::Update() {
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
		if (ImGui::IsKeyPressed(ImGuiKey_Z)){
			if (commandQueue.size() <= 0) return;

			redoQueue.push(commandQueue.top());
			commandQueue.top().Get()->Undo(m_ecs);
			commandQueue.pop();
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_Y)) {
			if (redoQueue.size() <= 0) return;
			
			commandQueue.push(redoQueue.top());
			redoQueue.top().Get()->Redo(m_ecs);
			redoQueue.pop();
		}
	}
}

CommandHistory::AddGameObject::AddGameObject(EntityID _id, std::string _scene) : Command(_id), sceneName(_scene) {}

void CommandHistory::AddGameObject::Undo(ecs::ECS& ecs) {
	EntityID newID = ecs.DuplicateEntity(id, CACHEDSCENE);
	ecs.DeleteEntity(id);
	id = newID;
}

void CommandHistory::AddGameObject::Redo(ecs::ECS& ecs) {
	EntityID newID = ecs.DuplicateEntity(id, sceneName);
	ecs.DeleteEntity(id);
	id = newID;
}

CommandHistory::DeleteGameObject::DeleteGameObject(EntityID _id, std::string _scene) : Command(_id), sceneName(_scene) {}

void CommandHistory::DeleteGameObject::Undo(ecs::ECS& ecs) {
	EntityID newID = ecs.DuplicateEntity(id, CACHEDSCENE);
	ecs.DeleteEntity(id);
	id = newID;
}

void CommandHistory::DeleteGameObject::Redo(ecs::ECS& ecs) {
	EntityID newID = ecs.DuplicateEntity(id, sceneName);
	ecs.DeleteEntity(id);
	id = newID;
}
