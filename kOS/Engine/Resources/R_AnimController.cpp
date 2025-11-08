#include "Config/pch.h"
#include "R_AnimController.h"
#include "..\DeSerialization\json_handler.h"
void R_Material::Load()
{
	this->md = serialization::ReadJsonFile<AnimControllerData>(this->GetFilePath().string());
	std::cout << "MATERIAL PATH" << this->GetFilePath().string() << '\n';
	std::cout << "MATERIAL LOADED" << this->md.diffuseMaterialGUID.GetToString() << '\n';
}
void R_Material::Unload() {}