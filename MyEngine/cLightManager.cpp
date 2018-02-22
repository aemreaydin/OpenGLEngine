#include "cLightManager.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "cShader.h"

std::string createArrayStringForGLSL(int index, std::string type);

cLightManager::cLightManager()
{
	this->NumLights = 0;
}

void cLightManager::CreateLights()
{
	std::ifstream lightFile("assets/lighting/Lights.txt");
	std::string temp;
	

	lightFile >> this->NumLights;
	for (int i = 0; i < this->NumLights; i++)
	{
		sLight tempLight;
		lightFile >> temp;
		lightFile >> temp;
		if (temp == "DIRECTIONAL")
			tempLight.lightType = eLightType::DIRECTIONAL_LIGHT;
		else if (temp == "SPOT")
			tempLight.lightType = eLightType::SPOT_LIGHT;
		else if (temp == "POINT")
			tempLight.lightType = eLightType::POINT_LIGHT;
		else
			tempLight.lightType = eLightType::UNKNOWN;

		lightFile >> temp;
		lightFile >> tempLight.ambient.x;
		lightFile >> tempLight.ambient.y;
		lightFile >> tempLight.ambient.z;
		lightFile >> tempLight.ambient.w;

		lightFile >> temp;
		lightFile >> tempLight.diffuse.x;
		lightFile >> tempLight.diffuse.y;
		lightFile >> tempLight.diffuse.z;
		lightFile >> tempLight.diffuse.w;

		lightFile >> temp;
		lightFile >> tempLight.specular.x;
		lightFile >> tempLight.specular.y;
		lightFile >> tempLight.specular.z;
		lightFile >> tempLight.specular.w;

		lightFile >> temp;
		lightFile >> tempLight.attenuation.x;
		lightFile >> tempLight.attenuation.y;
		lightFile >> tempLight.attenuation.z;

		lightFile >> temp;
		lightFile >> tempLight.position.x;
		lightFile >> tempLight.position.y;
		lightFile >> tempLight.position.z;

		lightFile >> temp;
		lightFile >> tempLight.direction.x;
		lightFile >> tempLight.direction.y;
		lightFile >> tempLight.direction.z;

		lightFile >> temp;
		lightFile >> tempLight.cutoff.x;
		lightFile >> tempLight.cutoff.y;

		this->Lights.push_back(tempLight);
	}

}

void cLightManager::LoadLampsIntoShader(cShader Shader)
{
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	for (int i = 0; i < this->NumLights; i++)
	{
		sLightVAO tempVAO;

		GLuint lightVAO, lightVBO;
		glGenVertexArrays(1, &lightVAO);
		glGenBuffers(1, &lightVBO);

		glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(lightVAO);
		
		GLuint posLoc = glGetAttribLocation(Shader.ID, "lightPosition");
		glEnableVertexAttribArray(posLoc);
		glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindVertexArray(0);

		tempVAO.lightVAO = lightVAO;
		tempVAO.lightVBO = lightVBO;
		this->LightVAOs.push_back(tempVAO);
	}
}

void cLightManager::DrawLightsIntoScene(cShader Shader)
{
	for (int i = 0; i < this->NumLights; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, this->Lights[i].position);
		model = glm::scale(model, glm::vec3(0.2f));
		Shader.SetMatrix4("lightModel", model, true);

		glBindVertexArray(this->LightVAOs[i].lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void cLightManager::LoadLightsIntoShader(cShader Shader)
{
	for (int i = 0; i < this->Lights.size(); i++)
	{
		//std::cout << createArrayStringForGLSL(i, "AmbientColor").c_str() << std::endl;
		Shader.SetVector4f(createArrayStringForGLSL(i, "AmbientColor").c_str(), this->Lights[i].ambient, true);
		Shader.SetVector4f(createArrayStringForGLSL(i, "DiffuseColor").c_str(), this->Lights[i].diffuse, true);
		Shader.SetVector4f(createArrayStringForGLSL(i, "SpecularColor").c_str(), this->Lights[i].specular, true);
		Shader.SetVector3f(createArrayStringForGLSL(i, "Position").c_str(), this->Lights[i].position, true);
		Shader.SetVector3f(createArrayStringForGLSL(i, "Attenuation").c_str(), this->Lights[i].attenuation, true);
		Shader.SetVector3f(createArrayStringForGLSL(i, "LightDirection").c_str(), this->Lights[i].direction, true);
		Shader.SetVector2f(createArrayStringForGLSL(i, "Cutoff").c_str(), glm::radians(this->Lights[i].cutoff), true);

		if (this->Lights[i].lightType == eLightType::DIRECTIONAL_LIGHT)
			Shader.SetInteger(createArrayStringForGLSL(i, "LightType").c_str(), 0);
		else if (this->Lights[i].lightType == eLightType::POINT_LIGHT)
		{
			Shader.SetInteger(createArrayStringForGLSL(i, "LightType").c_str(), 1);
		}
		else if (this->Lights[i].lightType == eLightType::SPOT_LIGHT)
		{
			Shader.SetInteger(createArrayStringForGLSL(i, "LightType").c_str(), 2);
		}
	}
	Shader.SetInteger("NumLights", this->NumLights, true);
}
std::string createArrayStringForGLSL(int index, std::string type)
{
	//std::cout << "Lights[" + std::to_string(index) + std::string("].") + type << std::endl;
	return ("Lights[" + std::to_string(index) + std::string("].") + type);
}