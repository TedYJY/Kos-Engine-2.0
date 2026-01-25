#pragma once
#include "Graphics/GraphicsReferences.h"
#include "Graphics/Shader.h"
struct PostProcessEffect {
	Shader* currentShader;
	virtual void UpdateShader() {};
};
struct Vigniette :public PostProcessEffect {
	 void UpdateShader();
	 float intensity,extent;
	 glm::vec2 resolution;
};
class PostProcessingProfile {
	public:
	std::string profileName;
	std::vector<std::unique_ptr<PostProcessEffect>>postProcessingEffects;
};