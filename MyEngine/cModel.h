#ifndef _MODEL_
#define _MODEL_

#include <vector>
#include <string>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "cMesh.h"

class cShader;

class cModel
{
public:
	cModel(const char* path);
	void Draw(cShader shader);
private:
	std::vector<cMesh> vecMeshes;
	std::vector<sTexture> vecTexturesLoaded;
	std::string directory;
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	cMesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<sTexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif // !_MODEL_
