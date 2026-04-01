
#include <Windows.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <RAPIDJSON/rapidjson.h>
#include <RAPIDJSON/document.h>
#include <RAPIDJSON/error/en.h>

std::string getShortPath(const std::string& longPath) {
	char shortPath[MAX_PATH];
	DWORD result = GetShortPathNameA(longPath.c_str(), shortPath, MAX_PATH);
	if (result > 0 && result < MAX_PATH) {
		return std::string(shortPath);
	}
	return longPath; // fallback
}
bool GetPNGDimensions(const std::string& filepath, uint32_t& outWidth, uint32_t& outHeight) {
	std::ifstream file(filepath, std::ios::binary);
	if (!file) return false;

	unsigned char header[24];
	if (!file.read(reinterpret_cast<char*>(header), 24)) return false;

	// Verify it's actually a PNG (Magic Number check)
	if (header[0] != 0x89 || header[1] != 0x50 || header[2] != 0x4E || header[3] != 0x47) {
		return false;
	}

	// Extract width and height from the IHDR chunk (Big Endian)
	outWidth = (header[16] << 24) | (header[17] << 16) | (header[18] << 8) | header[19];
	outHeight = (header[20] << 24) | (header[21] << 16) | (header[22] << 8) | header[23];
	return true;
}
int main(int argc, char* argv[])
{
	std::string texturePath{ std::filesystem::absolute(argv[1]).string() };
	std::string metaPath{ std::filesystem::absolute(argv[2]).string() };
	std::string outputPath{ std::filesystem::absolute(argv[3]).string()};
	//std::string tempBatch = "temp_run_texturecompiler.bat";
	// Write the batch file
	/*std::ofstream batchFile(tempBatch);
	if (!batchFile) {
		std::cerr << "Failed to create batch file!" << std::endl;
		return 0;
	}*/
	//Manipulate output file path
	//Remove file name 
	//Iterate backwards
	auto endStringIterator{ outputPath.end() };
	endStringIterator--;
	while (*endStringIterator != '\\') {
		endStringIterator--;
	}
	std::string outputFilePath=std::string(outputPath.begin(), endStringIterator);

	//Get file name
	std::filesystem::path p(texturePath);
	std::string fileName = p.stem().string();

	//std::cout<<"Test output: " << outputFilePath<<fileName<<".dds" << '\n';
	//std::string shortPath{ getShortPath(texturePath) };
	//batchFile << "@echo off\n";
	//batchFile << "set EXE=\"" << ".\\Compilers\\Executable\\Texture Compiler\\texconv.exe" << "\"\n";
	//batchFile << "%EXE% -ft DDS -f BC3_UNORM -y -o ";
	//batchFile << "\"" << outputFilePath << "\" ";
	//batchFile << "\"" << texturePath << "\"";
	//batchFile << std::endl;
	//batchFile.close();
	std::cout << "Output file path basrgaregret test" << outputFilePath<< '\n';
	std::cout << "Texture file path" << texturePath << '\n';
	//Open texture META file path settings
	std::ifstream ifs(metaPath);
	if (!ifs.is_open()) {
		std::cerr << "Could not open file: " << metaPath << std::endl;
		return 1;
	}
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	std::string jsonStr = buffer.str();

	rapidjson::Document doc;
	doc.Parse(jsonStr.c_str());	

	bool hf,vf;
	double width, height;
	int mipLevel, wrapSetting, colorSpace;

	const auto& arrObj = doc[1];
	const auto& assetData = arrObj["TextureCompilerData"];
	hf = assetData["HoriztonalFlip"].GetBool();
	vf=assetData["VerticalFlip"].GetBool();
	width = assetData["Width"].GetDouble();
	height = assetData["Height"].GetDouble();
	mipLevel = assetData["MipLevels"].GetInt();
	wrapSetting = assetData["Wrap"].GetInt();
	colorSpace = assetData["ColorSpace"].GetInt();

	std::cout << "Horizontal Flip " << hf << std::endl;
	std::cout << "Vertical Flip " << vf << std::endl;
	std::cout << "Width " << width << std::endl;
	std::cout << "Height " << height << std::endl;
	std::cout << "mipLevel " << mipLevel << std::endl;
	std::cout << "wrapSetting " << wrapSetting << std::endl;
	std::cout << "colorSpace " << colorSpace << std::endl;

	std::string srgbSetting;
	switch (colorSpace) {
	case 0:
		srgbSetting = "-srgb ";
		break;;
	case 1:
		srgbSetting = "-srgbi ";
		break;;
	case 2:
		srgbSetting = "-srgbo ";
		break;;
	}
	std::stringstream flags{};
	flags<<"\" -ft DDS -f BC3_UNORM ";
	if(hf)flags << "-hflip ";
	if (vf)flags << "-vflip ";
	if (width > 0)  flags << "-w " << (int)width << " ";
	if (height > 0) flags << "-h " << (int)height << " ";
	if (mipLevel > 0) flags << "-m " << (int)mipLevel << " ";
	flags << "-m " << mipLevel << " ";
	flags << srgbSetting << " ";
	if (wrapSetting) {
		flags <<"-mirror " << " ";
	}
	//End flag
	flags << "-y -o \"";
	std::cout << "FLAGS " << flags.str()<<'\n';
	// Run the batch file
	std::string exePath = std::filesystem::absolute(".\\Kos Editor\\Compilers\\Executable\\Texture Compiler\\texconv.exe").string();
	std::string command = "\"\"" + exePath + flags.str() + outputFilePath + "\" \"" + texturePath + "\"\"";
	/*command += outputFilePath + "\" \"" + texturePath + "\"";*/
	std::cout <<"COMMAND: " << command << '\n';

	int result = std::system(command.c_str());
	if (result != 0) {
		std::cerr << "Command failed with code: " << result << std::endl;
	}

	// Construct the expected output path
	outputFilePath += '\\';
	outputFilePath += fileName;
	outputFilePath += ".dds";
	std::cout << "Expected output file path: " << outputFilePath << '\n';
	std::cout << "New file path: " << outputPath << '\n';

	// 1. Check if texconv actually created the file
	if (!std::filesystem::exists(outputFilePath)) {
		std::cerr << "ERROR: Texture compiler failed to generate " << fileName << ".dds! (Check image dimensions or format)" << std::endl;
		return -1; // Exit gracefully instead of crashing
	}

	// 2. Use std::error_code to prevent unhandled exception crashes
	std::error_code ec;
	std::filesystem::rename(outputFilePath, outputPath, ec);

	if (ec) {
		std::cerr << "ERROR: Failed to rename file: " << ec.message() << std::endl;
		return -1;
	}

	return 0;
}