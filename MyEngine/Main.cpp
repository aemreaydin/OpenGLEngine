#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include "cGLCalls.h"
#include "Camera.h"
#include "cShader.h"
#include "cModel.h"
#include "cGameObject.h"
#include "cLightManager.h"
#include "eDepthType.h"
#include "cFBO.h"


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
cShader * Shader, * LampShader, * StencilShader, * SkyboxShader, * FBOShader;
cGameObject * Nanosuit, * SanFran;
cLightManager * LightManager;
cFBO * FBO, * DeferredRender;

std::vector< cGameObject * > GOVec;
std::vector< cGameObject * > GOSkybox;
int currentSkybox = 0;
cGameObject * FBOPlane;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void RenderScene();
void RenderFboScene();
void RenderSkybox();

int width = 1600;
int height = 900;

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
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	//glFrontFace(GL_CW);
	//glCullFace(GL_FRONT);
	//glCullFace(GL_BACK);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int maxColorAttach;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttach);
	int maxDrawBuffers;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
	std::cout << "GL_MAX_COLOR_ATTACHMENTS: " << maxColorAttach << "\nGL_MAX_DRAW_BUFFERS: " << maxDrawBuffers << std::endl;

	Shader = new cShader("assets/shaders/simpleVertex.glsl", "assets/shaders/simpleFragment.glsl");
	LampShader = new cShader("assets/shaders/lampShader.glsl", "assets/shaders/lampFragment.glsl");
	StencilShader = new cShader("assets/shaders/simpleVertex.glsl", "assets/shaders/stencilFragment.glsl");
	SkyboxShader = new cShader("assets/shaders/skyboxVert.glsl", "assets/shaders/skyboxFrag.glsl");
	FBOShader = new cShader("assets/shaders/fboVert.glsl", "assets/shaders/fboFrag.glsl");

	FBO = new cFBO(width, height);
	DeferredRender = new cFBO(width, height);

	Nanosuit = new cGameObject("Nanosuit", "assets/models/nanosuit/nanosuit.obj", glm::vec3(7.0f, 0.0f, 0.0f), glm::vec3(0.2), glm::vec3(0.0f));	
	SanFran = new cGameObject("Tree", "assets/models/sanfrancisco/houseSF.obj", glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.5f), glm::vec3(90.0f, 90.0f, 0.0f));
	FBOPlane = new cGameObject("FBOPlane", "assets/models/FBOPlane/FboPlane.obj", glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 90.0f));
	
	GOSkybox.push_back(new cGameObject("SkyboxSea", "assets/models/skybox/cube.obj", true, "assets/skybox/ocean"));
	GOSkybox.push_back(new cGameObject("SkyboxSpace", "assets/models/skybox/cube.obj", true, "assets/skybox/space"));

	LightManager = new cLightManager();
	LightManager->CreateLights();
	LightManager->LoadLampsIntoShader(*LampShader);

	//LightManager->LoadLampsIntoShader(*Shader);

	//GOVec.push_back(Nanosuit);
	GOVec.push_back(SanFran);



	while (!glfwWindowShouldClose(GLCalls->GetWindow()))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(GLCalls->GetWindow());

		//FBO->BindFBO();
		//FBO->ClearBuffers();
		//glEnable(GL_DEPTH_TEST);

		//RenderScene();
		RenderSkybox();

		//FBO->UnbindFBO();
		//glDisable(GL_DEPTH_TEST);
		//glClearColor(1.f, 1.f, 1.f, 1.f);
		//glClear(GL_COLOR_BUFFER_BIT);

		//FBOShader->Use();
		//FBOShader->SetInteger("screenTexture", 0, true);
		//FBO->Draw(*FBOShader);


		glfwPollEvents();
		glfwSwapBuffers(GLCalls->GetWindow());
	}

	glfwTerminate();
	return 0;
}

void RenderScene()
{
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

	//StencilShader->Use();
	//StencilShader->SetMatrix4("projection", projection, true);
	//StencilShader->SetMatrix4("view", view, true);
	//StencilShader->SetVector3f("eyePos", camera.GetPosition());

	//glStencilFunc(GL_ALWAYS, 1, 0xFF);
	//glStencilMask(0xFF);
	////Shader->Use();
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



	Shader->Use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, Nanosuit->Position);
	model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, Nanosuit->Scale);
	Shader->SetMatrix4("model", model, true);
	Nanosuit->Draw(*Shader);

	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	//glStencilMask(0x00);
	//glDisable(GL_DEPTH_TEST);
	//StencilShader->Use();
	//model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(Nanosuit->Position.x, Nanosuit->Position.y - 0.05f, Nanosuit->Position.z));
	//model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::scale(model, Nanosuit->Scale + 0.005f);
	//StencilShader->SetMatrix4("model", model, true);
	//Nanosuit->Draw(*StencilShader);

	//glStencilMask(0xFF);
	//glEnable(GL_DEPTH_TEST);
}
void RenderFboScene()
{
	Shader->Use();
	glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	//glm::mat4 view = glm::lookAt(camera.GetPosition(), FBOPlane->Position, glm::vec3(0.0f, 1.0f, 0.0f));

	Shader->SetMatrix4("projection", projection, true);
	Shader->SetMatrix4("view", view, true);
	Shader->SetVector3f("eyePos", camera.GetPosition());

	LampShader->Use();
	LampShader->SetMatrix4("projection", projection, true);
	LampShader->SetMatrix4("view", view, true);
	LightManager->LoadLightsIntoShader(*Shader);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, FBOPlane->Position);
	model = glm::rotate(model, glm::radians(FBOPlane->OrientationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(FBOPlane->OrientationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(FBOPlane->OrientationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, FBOPlane->Scale);
	Shader->SetMatrix4("model", model, true);
	FBOPlane->Draw(*Shader);

	for (int i = 0; i < LightManager->NumLights; i++)
	{
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, LightManager->Lights[i].position);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f));
		Shader->SetMatrix4("lightModel", lightModel, true);
	}
	LightManager->DrawLightsIntoScene(*LampShader);
}

void RenderSkybox()
{
	glDepthFunc(GL_LEQUAL);
	

	SkyboxShader->Use();
	SkyboxShader->SetInteger("skybox", 0);
	glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	SkyboxShader->SetMatrix4("projection", projection, true);
	SkyboxShader->SetMatrix4("view", view, true);

	glBindTexture(GL_TEXTURE_CUBE_MAP, GOSkybox[currentSkybox]->Skybox->textureID);
	GOSkybox[currentSkybox]->Draw(*SkyboxShader);
	

	glDepthFunc(GL_LESS);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		currentSkybox++;
		std::cout << depthIndex << std::endl;
		if (currentSkybox == GOSkybox.size())
			currentSkybox = 0;
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