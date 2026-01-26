#include "Editor.h"
#include "config/pch.h"
#include "AssetManager/AssetManager.h"
#include "Resources/ResourceManager.h"
#include "Graphics/GraphicsManager.h"


PostProcessingProfile tempProfile;
std::string lastGUID;

void gui::ImGuiHandler::DrawPostProcessWindow() {
	//Display info first by double clicking click click
	if (lastGUID != selectedAsset.GUID.GetToString() && selectedAsset.Type == "R_PostProcessingProfile") {
		tempProfile = m_resourceManager.GetResource<R_PostProcessingProfile>(selectedAsset.GUID)->profile;

;	}
	//Profile em rip sean
	ImGui::Begin("Post processing");
	

}