#pragma once
#include "Config/pch.h"
#include "Resource.h"
#include "ResourceHeader.h"

struct AnimPin {
	int id;
	enum PinKind
	{
		Input,
		Output
	} kind;
	std::string name;
};

struct AnimTransition {
	int id;
	int fromPinId;
	int toPinId;
	std::string condition;
	REFLECTABLE(AnimTransition);
};

struct AnimState {
	int id;
	std::vector<AnimPin> inputs;
	std::vector<AnimPin> outputs;

	std::string name;
	utility::GUID animationGUID;
	float playSpeed = 1.0f;
	bool isLooping = true;
	bool isDefault = false;
	std::vector<AnimTransition> transitions;

	REFLECTABLE(AnimState, name, playSpeed, isLooping);
};


struct AnimControllerData
{
	std::string name;
	std::unordered_map<int, AnimState> states;
	//std::vector<AnimTransition> transitions;
	int currentStateId;

	REFLECTABLE(AnimControllerData);
};

class R_AnimController :public Resource
{
public:
	using Resource::Resource;
	void Load() override;
	void Unload() override;

	AnimControllerData m_AnimControllerData;

	REFLECTABLE(R_AnimController);
};