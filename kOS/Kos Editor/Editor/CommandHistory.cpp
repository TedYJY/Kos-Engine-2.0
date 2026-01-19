#include "CommandHistory.h"

using namespace CommandHistory;
using namespace ecs;

//std::stack<Command> CommandHistory::commandQueue;

void CommandHistory::AddCommand(Command *cmd) {

}


AddGameObject::AddGameObject(EntityID _id) : Command(_id) {}

void AddGameObject::Redo() {

}

void AddGameObject::Undo() {

}