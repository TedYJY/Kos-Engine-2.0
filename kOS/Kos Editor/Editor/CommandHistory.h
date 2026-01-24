// Stack of Commands
// Commands: 
// Entities Related
//		- Instead of Instantly deleting the Entity ID, move it to editor side?
//		- Place it back to the entity map, order might matter
//		- Only after the command list gets filled up then completely delete the command
//		- ALL WILL STORE ENTITY ID AS A BASE
//		- Deleting Commands will just be removed

// Component Editing
//		- Might be able to involve templates

#include "ECS/ECS.h"

class CommandHistory {
	ecs::ECS& m_ecs;

public:
	CommandHistory(ecs::ECS& ecs) : m_ecs(ecs) {}

	void Init();
	void Update();

	struct Command {
		Command(EntityID _id) : id{ _id } {}

		EntityID id;
		virtual void Undo(ecs::ECS& ecs) = 0;
		virtual void Redo(ecs::ECS& ecs) = 0;
	};

	class CommandWrapper {
	public: 
		CommandWrapper(std::shared_ptr<Command> obj) : command(std::move(obj)) {}

		std::shared_ptr<Command> Get() const { return command; }
	private:
		std::shared_ptr<Command> command;
	};

	// Data
	static std::stack<CommandWrapper> commandQueue;
	static std::stack<CommandWrapper> redoQueue;
	
	template<typename TCommand, typename... Args>
	void AddCommand(Args&&... args) {
		static_assert(std::is_base_of_v<Command, TCommand>, "TCommand must derive from Command");
		auto cmdPtr = std::make_shared<TCommand>(std::forward<Args>(args)...);
		commandQueue.push(CommandWrapper{ cmdPtr });

		//Clear Redo Stack
		std::stack<CommandWrapper> empty;
		redoQueue.swap(empty);
	}

	struct AddGameObject : Command {
		AddGameObject(EntityID _id, std::string _scene);
		void Undo(ecs::ECS& ecs);
		void Redo(ecs::ECS& ecs);
		std::string sceneName;
	};

	//	- Delete
	//		- Undo -> Add Back or Duplicate Entity
	//		- Redo -> Call Delete Entity
	//		- Store Deleted Entity and Parent?
	//		- Destructor -> Delete Stored Entity for Command
	struct DeleteGameObject : Command {
		DeleteGameObject(EntityID _idm, std::string _scene);
		void Undo(ecs::ECS& ecs);
		void Redo(ecs::ECS& ecs);
		std::string sceneName;
	};

	//	- Parenting
	//		- Undo -> Unparent
	//		- Redo -> Parent
	//		- Store ParentID
	struct SetGameObjectParent : Command {
		SetGameObjectParent(EntityID _id, EntityID parentId);
		void Undo(ecs::ECS& ecs);
		void Redo(ecs::ECS& ecs);
	private:
		EntityID prevParent;
	};

	//	- Unparenting
	//		- Undo -> Parent
	//		- Redo -> Unparent
	//		- Store ParentID 
	struct UnparentGameObject : Command {
		UnparentGameObject(EntityID _id);
		void Undo(ecs::ECS& ecs);
		void Redo(ecs::ECS& ecs);
	private:
		EntityID prevParent;
	};

	//	- Set Actve
	//		- Store Status -> Hide or UnHide -> != currentStatus?
	struct SetGameObjectActive : Command {
		SetGameObjectActive(EntityID _id);
		void Undo(ecs::ECS& ecs);
		void Redo(ecs::ECS& ecs);
	};
};