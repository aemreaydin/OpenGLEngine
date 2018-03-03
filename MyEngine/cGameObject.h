#ifndef _GAME_OBJECT_
#define _GAME_OBJECT_

#include <glm\vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>
#include <string>

#include "cModel.h"
#include "cSkybox.h"


class cSkybox;

class cGameObject
{
public:
	glm::vec3 Position, Scale;
	glm::quat OrientationQuat, OrientationEuler;

	cModel* Model;

	cGameObject(std::string modelName, std::string modelDir, bool skybox = false, std::string folder = "");
	cGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler, bool skybox = false, std::string folder = "");
	void Draw(cShader Shader);

	bool isSkybox;
	cSkybox* Skybox;
};
#endif // !_GAME_OBJECT_
