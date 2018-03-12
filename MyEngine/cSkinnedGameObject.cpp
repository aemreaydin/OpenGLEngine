#include "cSkinnedGameObject.h"
#include "cShader.h"

#include <stack>
#include <glm\gtc\matrix_transform.hpp>

cSkinnedGameObject::cSkinnedGameObject(std::string modelName, std::string modelDir)
{
	this->Model = new cSkinnedMesh(modelDir.c_str());

	this->Position = glm::vec3(0.0f);
	this->Scale = glm::vec3(1.0f);
	this->OrientationQuat = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
	this->OrientationEuler = glm::vec3(0.0f);

	this->animState = new sAnimationState();
	this->animState->defaultAnimation.name = modelDir;
	this->animState->defaultAnimation.frameStepTime = 0.005f;
	this->animState->defaultAnimation.totalTime = this->Model->GetDuration();
}
cSkinnedGameObject::cSkinnedGameObject(std::string modelName, std::string modelDir, glm::vec3 position, glm::vec3 scale, glm::vec3 orientationEuler)
{
	this->Model = new cSkinnedMesh(modelDir.c_str());

	this->Position = position;
	this->Scale = scale;
	this->OrientationQuat = glm::quat(orientationEuler);
	this->OrientationEuler = orientationEuler;

	this->animState = new sAnimationState();
	this->animState->defaultAnimation.name = modelDir;
	this->animState->defaultAnimation.frameStepTime = 0.005f;
	this->animState->defaultAnimation.totalTime = this->Model->GetDuration();
}
void cSkinnedGameObject::Draw(cShader Shader)
{
	std::string animToPlay = "";
	float curFrameTime = 0.0f;

	sAnimationState* curAnimState = this->animState;

	if (!curAnimState->stackAnimationsToPlay.empty())
	{
		animToPlay = curAnimState->stackAnimationsToPlay.top().name;
		curFrameTime = curAnimState->stackAnimationsToPlay.top().curTime;

		if (curAnimState->stackAnimationsToPlay.top().IncrementTime())
		{
			curAnimState->stackAnimationsToPlay.pop();
		}
	}
	else
	{
		curAnimState->defaultAnimation.IncrementTime();

		animToPlay = curAnimState->defaultAnimation.name;
		curFrameTime = curAnimState->defaultAnimation.curTime;
	}

	std::vector<glm::mat4> vecFinalTransformation;
	std::vector<glm::mat4> vecOffsets;

	this->Model->BoneTransform(curFrameTime, animToPlay, vecFinalTransformation, this->vecBoneTransformation, vecOffsets);
	
	GLuint numBonesUsed = static_cast<GLuint>(vecFinalTransformation.size());
	Shader.Use();
	Shader.SetInteger("numBonesUsed", numBonesUsed);
	glm::mat4* boneMatrixArray = &(vecFinalTransformation[0]);
	glUniformMatrix4fv(glGetUniformLocation(Shader.ID, "Bones"), numBonesUsed, GL_FALSE, (const GLfloat*) glm::value_ptr(*boneMatrixArray));
	//Shader.SetMatrix4("Bones", numBonesUsed, vecFinalTransformation[0]);

	//for (unsigned int i = 0; i != numBonesUsed; i++)
	//{
	//	glm::mat4 boneLocal = this->vecBoneTransformation[i];
	//	boneLocal = glm::scale(boneLocal, this->Scale);

	//	glm
	//}
	//

	this->Model->Draw(Shader);
}