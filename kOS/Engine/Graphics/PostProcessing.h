#pragma once
#include "Graphics/GraphicsReferences.h"
#include "Graphics/Shader.h"
enum PostProcessType {
	Vigniette=0
};
struct PostProcessEffect {
	virtual Shader* GetShader()=0;
	virtual void UpdateShader() {};
	virtual PostProcessType GetType()=0;
};
struct Vigniette :public PostProcessEffect {
	 void UpdateShader();
	 float intensity,extent;
	 static Shader* currentShader;
	 Shader* GetShader() { return currentShader;; };
	 PostProcessType GetType() { return PostProcessType::Vigniette;;};
	 glm::vec2 resolution;

};
class PostProcessingProfile {
	public:
	std::string profileName;
	std::vector<std::unique_ptr<PostProcessEffect>>postProcessingEffects;
};