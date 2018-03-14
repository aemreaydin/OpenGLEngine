#ifndef _SKINNED_GAME_OBJECT_HG_
#define _SKINNED_GAME_OBJECT_HG_

#include <glm\vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>
#include <string>

#include "cSkinnedMesh.h"
#include "sAnimationState.h"


class cSkinnedGameObject
{
public:
	glm::vec3 Position, Scale;
	glm::quat OrientationQuat;
	glm::vec3 OrientationEuler;

	cSkinnedGameObject(std::string modelName, std::string modelDir);
	cSkinnedGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler);
	cSkinnedGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler, std::vector<std::string> charAnimations);
	cSkinnedGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler, std::map<int, std::string> charAnimations);
	cSkinnedGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler, float speed, std::map<int, std::string> charAnimations);
	void Draw(cShader Shader);

	std::vector<std::string> vecCharacterAnimations;
	std::map<int, std::string> mapCharacterAnimations;
	sAnimationState* defaultAnimState, *curAnimState;
	std::string animToPlay;
	float Speed;
private:
	cSkinnedMesh* Model;
	std::vector<glm::mat4> vecBoneTransformation;
};
#endif // !_GAME_OBJECT_
