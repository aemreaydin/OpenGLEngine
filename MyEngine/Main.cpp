#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include "cGLCalls.h"
#include "Camera.h"
#include "cShader.h"
#include "cModel.h"
#include "cGameObject.h"
#include "cLightManager.h"
#include "eDepthType.h"


#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#include <gtest\gtest.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

cGLCalls* GLCalls;
cShader * Shader, * LampShader, * StencilShader;
cGameObject * Nanosuit, * SanFran;
cLightManager * LightManager;

std::vector< cGameObject * > GOVec;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

const unsigned int width = 1600;
const unsigned int height = 900;

Camera camera(glm::vec3(0.0f, 10.0f, 10.0f));
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

int currentLight = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GL_DEPTH_TYPE depthType;
int depthIndex = 0;

TEST(TC_INIT, InitializeGLFW)
{
	ASSERT_TRUE(GLCalls->Initialize() == GL_TRUE);
}
TEST(TC_CREATE_WINDOW, CreateGLWindow)
{
	ASSERT_TRUE(GLCalls->CreateGLWindow() == GL_TRUE);
}

int main(int argc, char **argv)
{
	GLCalls = new cGLCalls(width, height, "AssimpImport");

	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();	

	glfwSetFramebufferSizeCallback(GLCalls->GetWindow(), framebuffer_size_callback);
	glfwSetCursorPosCallback(GLCalls->GetWindow(), mouse_callback);
	glfwSetScrollCallback(GLCalls->GetWindow(), scroll_callback);
	glfwSetKeyCallback(GLCalls->GetWindow(), keyCallback);
	//glfwSetInputMode(GLCalls->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	

	Shader = new cShader("assets/shaders/simpleVertex.glsl", "assets/shaders/simpleFragment.glsl");
	LampShader = new cShader("assets/shaders/lampShader.glsl", "assets/shaders/lampFragment.glsl");
	StencilShader = new cShader("assets/shaders/simpleVertex.glsl", "assets/shaders/stencilFragment.glsl");

	Nanosuit = new cGameObject("Nanosuit", "assets/models/nanosuit/nanosuit.obj", glm::vec3(7.0f, 0.0f, 0.0f), glm::vec3(0.2), glm::vec3(0.0f));	
	SanFran = new cGameObject("Tree", "assets/models/sanfrancisco/houseSF.obj", glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.5f), glm::vec3(90.0f, 90.0f, 0.0f));


	LightManager = new cLightManager();
	LightManager->CreateLights();
	LightManager->LoadLampsIntoShader(*LampShader);

	//LightManager->LoadLampsIntoShader(*Shader);

	//GOVec.push_back(Nanosuit);
	GOVec.push_back(SanFran);



	while (!glfwWindowShouldClose(GLCalls->GetWindow()))
	{
		switch (depthIndex)
		{
		case 0:
			glDepthFunc(GL_LESS);
			break;
		case 1:
			glDepthFunc(GL_ALWAYS);
			break;
		case 2:
			glDepthFunc(GL_NEVER);
			break;
		case 3:
			glDepthFunc(GL_EQUAL);
			break;
		case 4:
			glDepthFunc(GL_LEQUAL);
			break;
		case 5:
			glDepthFunc(GL_GREATER);
			break;
		case 6:
			glDepthFunc(GL_NOTEQUAL);
			break;
		case 7:
			glDepthFunc(GL_GEQUAL);
			break;
		default:
			break;
		}
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		processInput(GLCalls->GetWindow());

		glClearColor(0.5f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


		glStencilMask(0x00);
		Shader->Use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)width / (float)height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		Shader->SetMatrix4("projection", projection, true);
		Shader->SetMatrix4("view", view, true);
		Shader->SetVector3f("eyePos", camera.GetPosition());

		LampShader->Use();
		LampShader->SetMatrix4("projection", projection, true);
		LampShader->SetMatrix4("view", view, true);
		LightManager->LoadLightsIntoShader(*Shader);

		StencilShader->Use();
		StencilShader->SetMatrix4("projection", projection, true);
		StencilShader->SetMatrix4("view", view, true);
		StencilShader->SetVector3f("eyePos", camera.GetPosition());


		for (int i = 0; i < GOVec.size(); i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, GOVec[i]->Position);
			model = glm::rotate(model, glm::radians(GOVec[i]->OrientationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(GOVec[i]->OrientationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(GOVec[i]->OrientationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, GOVec[i]->Scale);
			Shader->SetMatrix4("model", model, true);
			GOVec[i]->Draw(*Shader);
		}


		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		Shader->Use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Nanosuit->Position);
		model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, Nanosuit->Scale);
		Shader->SetMatrix4("model", model, true);
		Nanosuit->Draw(*Shader);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
		StencilShader->Use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(Nanosuit->Position.x, Nanosuit->Position.y - 0.05f, Nanosuit->Position.z));
		model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, Nanosuit->Scale + 0.005f);
		StencilShader->SetMatrix4("model", model, true);
		Nanosuit->Draw(*StencilShader);

		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);

		for (int i = 0; i < LightManager->NumLights; i++)
		{
			glm::mat4 lightModel = glm::mat4(1.0f);
			lightModel = glm::translate(lightModel, LightManager->Lights[i].position);
			lightModel = glm::scale(lightModel, glm::vec3(0.2f));
			Shader->SetMatrix4("lightModel", lightModel, true);
		}
		LightManager->DrawLightsIntoScene(*LampShader);

		

		glfwPollEvents();
		glfwSwapBuffers(GLCalls->GetWindow());
	}

	glfwTerminate();
	return 0;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		depthIndex++;
		std::cout << depthIndex << std::endl;
		if (depthIndex == 8)
			depthIndex = 0;
		switch (depthIndex)
		{
		case 0:
			std::cout << "Current depth function is GL_LESS" << std::endl;
			break;
		case 1:
			std::cout << "Current depth function is GL_ALWAYS" << std::endl;
			break;
		case 2:
			std::cout << "Current depth function is GL_NEVER" << std::endl;
			break;
		case 3:
			std::cout << "Current depth function is GL_EQUAL" << std::endl;
			break;
		case 4:
			std::cout << "Current depth function is GL_LEQUAL" << std::endl;
			break;
		case 5:
			std::cout << "Current depth function is GL_GREATER" << std::endl;
			break;
		case 6:
			std::cout << "Current depth function is GL_NOTEQUAL" << std::endl;
			break;
		case 7:
			std::cout << "Current depth function is GL_GEQUAL" << std::endl;
			break;
		default:
			break;
		}
	}
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		LightManager->Lights[currentLight].position.x += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		LightManager->Lights[currentLight].position.x -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		LightManager->Lights[currentLight].position.z += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		LightManager->Lights[currentLight].position.z -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		LightManager->Lights[currentLight].position.y += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		LightManager->Lights[currentLight].position.y -= 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		currentLight++;
		if (currentLight == LightManager->NumLights)
			currentLight = 0;		
	}


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}