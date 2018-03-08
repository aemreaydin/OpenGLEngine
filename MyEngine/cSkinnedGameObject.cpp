#include "cSkinnedGameObject.h"
#include "cShader.h"


cSkinnedGameObject::cSkinnedGameObject(std::string modelName, std::string modelDir)
{
	this->Model = new cSkinnedMesh(modelDir.c_str());

	this->Position = glm::vec3(0.0f);
	this->Scale = glm::vec3(1.0f);
	this->OrientationQuat = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
	this->OrientationEuler = glm::vec3(0.0f);
}
cSkinnedGameObject::cSkinnedGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler)
{
	this->Model = new cSkinnedMesh(modelDir.c_str());

	this->Position = position;
	this->Scale = scale;
	this->OrientationQuat = glm::quat(orientationEuler);
	this->OrientationEuler = orientationEuler;
}
void cSkinnedGameObject::Draw(cShader Shader)
{
	this->Model->Draw(Shader);
}