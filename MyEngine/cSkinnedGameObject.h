#ifndef _SKINNED_GAME_OBJECT_HG_
#define _SKINNED_GAME_OBJECT_HG_

#include <glm\vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>
#include <string>

#include "cSkinnedMesh.h"


class cSkinnedGameObject
{
public:
	glm::vec3 Position, Scale;
	glm::quat OrientationQuat, OrientationEuler;

	cSkinnedMesh* Model;

	cSkinnedGameObject(std::string modelName, std::string modelDir);
	cSkinnedGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler);
	void Draw(cShader Shader);

};
#endif // !_GAME_OBJECT_
