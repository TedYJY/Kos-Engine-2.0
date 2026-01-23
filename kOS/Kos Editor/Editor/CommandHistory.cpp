#include "CommandHistory.h"
 
std::stack<CommandHistory::CommandWrapper> CommandHistory::commandQueue;
std::stack<CommandHistory::CommandWrapper> CommandHistory::redoQueue;
std::map<EntityID, ComponentSignature> CommandHistory::cachedEntity;

void CommandHistory::Init() {

}

void CommandHistory::Update() {

}

void CommandHistory::AddCommand(CommandWrapper cmd) {
	commandQueue.push(cmd);
	//Clear Redo Stack
	std::stack<CommandWrapper> empty;
	redoQueue.swap(empty);
}

CommandHistory::AddGameObject::AddGameObject(EntityID _id) : Command(_id) {}

void CommandHistory::AddGameObject::Undo() {
	//m_ecs.DeleteEntity(id);
}

void CommandHistory::AddGameObject::Redo() {
	
}
