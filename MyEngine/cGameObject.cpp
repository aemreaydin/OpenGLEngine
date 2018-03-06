#include "cGameObject.h"
#include "cShader.h"
#include "cSkybox.h"

cShader * Shader;

cGameObject::cGameObject(std::string modelName, std::string modelDir, bool skybox, std::string folder)
{
	this->Model = new cModel(modelDir.c_str());

	this->Position = glm::vec3(0.0f);
	this->Scale = glm::vec3(1.0f);
	this->OrientationQuat = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
	this->OrientationEuler = glm::vec3(0.0f);

	this->isSkybox = skybox;
	if (this->isSkybox)
	{
		this->Skybox = new cSkybox(folder, *Shader);
		this->Skybox->LoadCubeMap();
	}
}
cGameObject::cGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler, bool skybox, std::string folder)
{
	this->Model = new cModel(modelDir.c_str());

	this->Position = position;
	this->Scale = scale;
	this->OrientationQuat = glm::quat(orientationEuler);
	this->OrientationEuler = orientationEuler;

	this->isSkybox = skybox;
	if (this->isSkybox)
	{
		this->Skybox = new cSkybox(folder, *Shader);
		this->Skybox->LoadCubeMap();
	}
}
void cGameObject::Draw(cShader Shader)
{
	if(!isSkybox)
		this->Model->Draw(Shader);
	else
		this->Skybox->Draw(Shader);
}