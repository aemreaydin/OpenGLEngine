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
#include "cText.h"
#include "cSkinnedGameObject.h"

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
cShader * Shader, * SkyboxShader, *TextShader;
cShader * LampShader, * StencilShader, * FBOShader;
cGameObject * Nanosuit, * SanFran;
cLightManager * LightManager;
cText * Text;
cSkinnedGameObject* ArmySoldier;

std::vector<cFBO*> FBOs;

std::vector< cGameObject * > GOVec;
std::vector< cGameObject * > GOSkybox;
int currentSkybox = 0;
cGameObject * FBOPlane;

float currentFrame;
double lastTime;
std::string fpsString;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void windowSizeCallback(GLFWwindow* window, int width, int height);
void RenderScene();
void RenderFboScene();
void RenderSkybox();
void RenderText();

int width = 1600;
int height = 900;
int nbFrames = 0;

Camera camera(glm::vec3(0.0f, 10.0f, 10.0f));
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

int currentLight = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GL_DEPTH_TYPE depthType;
int depthIndex = 0;

int FBOMode = 0;

TEST(TC_INIT, InitializeGLFW)
{
	GLCalls = new cGLCalls(width, height, "AssimpImport");
	ASSERT_TRUE(GLCalls->Initialize() == GL_TRUE);
}
TEST(TC_CREATE_WINDOW, CreateGLWindow)
{
	ASSERT_TRUE(GLCalls->CreateGLWindow() == GL_TRUE);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();	

	std::cout << glGetString(GL_RENDERER) << ", " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glfwSetFramebufferSizeCallback(GLCalls->GetWindow(), framebuffer_size_callback);
	glfwSetCursorPosCallback(GLCalls->GetWindow(), mouse_callback);
	glfwSetScrollCallback(GLCalls->GetWindow(), scroll_callback);
	glfwSetKeyCallback(GLCalls->GetWindow(), keyCallback);
	glfwSetWindowSizeCallback(GLCalls->GetWindow(), windowSizeCallback);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
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
	TextShader = new cShader("assets/shaders/textVertex.glsl", "assets/shaders/textFrag.glsl");

	FBOs.push_back(new cFBO(width, height));
	FBOs.push_back(new cFBO(width, height));

	Nanosuit = new cGameObject("Nanosuit", "assets/models/nanosuit/nanosuit.obj", glm::vec3(7.0f, 0.0f, 0.0f), glm::vec3(0.2), glm::vec3(0.0f));	
	SanFran = new cGameObject("Tree", "assets/models/sanfrancisco/houseSF.obj", glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.5f), glm::vec3(90.0f, 90.0f, 0.0f));
	FBOPlane = new cGameObject("FBOPlane", "assets/models/FBOPlane/FboPlane.obj", glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 90.0f));
	GOVec.push_back(Nanosuit);
	GOVec.push_back(SanFran);
	
	GOSkybox.push_back(new cGameObject("SkyboxSea", "assets/models/skybox/cube.obj", true, "assets/skybox/ocean"));
	GOSkybox.push_back(new cGameObject("SkyboxSpace", "assets/models/skybox/cube.obj", true, "assets/skybox/space"));

	LightManager = new cLightManager();
	LightManager->CreateLights();
	LightManager->LoadLampsIntoShader(*LampShader);

	Text = new cText("assets/fonts/04B_30__.TTF");

	ArmySoldier = new cSkinnedGameObject("DarkKnight", "assets/RPG Character Animation Pack/Animations/Unarmed/RPG-Character@Unarmed-Idle.FBX", glm::vec3(15.0f, 0.0f, 0.0f), glm::vec3(0.01f), glm::vec3(0.0f, 0.0f, 0.0f));


	lastTime = glfwGetTime();
	
	while (!glfwWindowShouldClose(GLCalls->GetWindow()))
	{
		currentFrame = glfwGetTime();
		Shader->SetFloat("time", currentFrame);
		nbFrames++;
		if (currentFrame - lastTime >= 1.0)
		{
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			float x = 1000.0 / double(nbFrames);
			fpsString = std::to_string(x) + " ms/frame";
			nbFrames = 0;
			lastTime += 1.0;
		}

		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		processInput(GLCalls->GetWindow());
		glfwGetFramebufferSize(GLCalls->GetWindow(), &width, &height);



		FBOs[0]->BindFBO();
		FBOs[0]->ClearBuffers();
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		Shader->SetInteger("isSecondPass", false);
		RenderSkybox();
		RenderScene();
		RenderText();


		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, ArmySoldier->Position);
		model = glm::rotate(model, glm::radians(ArmySoldier->OrientationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(ArmySoldier->OrientationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(ArmySoldier->OrientationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, ArmySoldier->Scale);
		Shader->SetMatrix4("model", model, true);
		ArmySoldier->Draw(*Shader);




		FBOs[0]->UnbindFBO();
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		Shader->Use();
		Shader->SetInteger("isSecondPass", true);

		glActiveTexture(GL_TEXTURE0 + 15);
		glBindTexture(GL_TEXTURE_2D, FBOs[0]->texColorBuffer);
		Shader->SetInteger("texFBOColor", 15, true);
		//FBOShader->SetInteger("texFBOColor", 15, true);

		glActiveTexture(GL_TEXTURE0 + 16);
		glBindTexture(GL_TEXTURE_2D, FBOs[0]->texNormalBuffer);
		Shader->SetInteger("texFBONormal", 16, true);
		//FBOShader->SetInteger("texFBONormal", 16, true);

		glActiveTexture(GL_TEXTURE0 + 17);
		glBindTexture(GL_TEXTURE_2D, FBOs[0]->texVertexBuffer);
		Shader->SetInteger("texFBOVertex", 17, true);
		//FBOShader->SetInteger("texFBOVertex", 17, true);

		//glActiveTexture(GL_TEXTURE0 + 18);
		//glBindTexture(GL_TEXTURE_2D, FBOs[0]->texDepthBuffer);
		//Shader->SetInteger("texFBODepth", 18);

		Shader->SetFloat("screenWidth", width, true);
		//FBOShader->SetFloat("screenWidth", width, true);
		Shader->SetFloat("screenHeight", height, true);
		Shader->SetInteger("FBOMode", FBOMode);
		FBOs[0]->Draw(*Shader);



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
	Shader->SetMatrix4("projection", projection);
	Shader->SetMatrix4("view", view);
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
	for (int i = 0; i < LightManager->NumLights; i++)
	{
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, LightManager->Lights[i].position);
		lightModel = glm::scale(lightModel, glm::vec3(0.2f));
		Shader->SetMatrix4("lightModel", lightModel, true);
	}
	LightManager->DrawLightsIntoScene(*LampShader);


	//Shader->Use();
	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, Nanosuit->Position);
	//model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.x), glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.y), glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(Nanosuit->OrientationEuler.z), glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::scale(model, Nanosuit->Scale);
	//Shader->SetMatrix4("model", model, true);
	//Nanosuit->Draw(*Shader);

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
	SkyboxShader->SetInteger("skybox", 20);
	//Shader->SetInteger("isSkybox", true);
	glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()), (float)width / (float)height, 0.1f, 100.0f);
	glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	SkyboxShader->SetMatrix4("projection", projection, true);
	SkyboxShader->SetMatrix4("view", view, true);

	GOSkybox[currentSkybox]->Draw(*SkyboxShader);

	//Shader->SetInteger("isSkybox", false);
	glDepthFunc(GL_LESS);
}
void RenderText()
{
	TextShader->Use();
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
	TextShader->SetMatrix4("projection", projection);
	TextShader->SetInteger("text", 0);
	Text->printText2D(*TextShader, fpsString, 25.f, 25.f, 1.f, glm::vec3(0.0f, 0.0f, 1.0f));
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

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
	{
		FBOMode = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		FBOMode = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		FBOMode = 2;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		FBOMode = 3;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		FBOMode = 4;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		FBOMode = 5;
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
		FBOMode = 6;
	}
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
	{
		FBOMode = 7;
	}
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
	{
		FBOMode = 8;
	}
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
	{
		FBOMode = 9;
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
		if (LightManager->Lights[currentLight].lightType == DIRECTIONAL_LIGHT)
			LightManager->Lights[currentLight].direction.x += 0.01f;
		else
			LightManager->Lights[currentLight].position.x += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		if (LightManager->Lights[currentLight].lightType == DIRECTIONAL_LIGHT)
			LightManager->Lights[currentLight].direction.x -= 0.01f;
		else
			LightManager->Lights[currentLight].position.x -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		if (LightManager->Lights[currentLight].lightType == DIRECTIONAL_LIGHT)
			LightManager->Lights[currentLight].direction.z += 0.01f;
		else
			LightManager->Lights[currentLight].position.z += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if (LightManager->Lights[currentLight].lightType == DIRECTIONAL_LIGHT)
			LightManager->Lights[currentLight].direction.z -= 0.01f;
		else
			LightManager->Lights[currentLight].position.z -= 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		if (LightManager->Lights[currentLight].lightType == DIRECTIONAL_LIGHT)
			LightManager->Lights[currentLight].direction.y += 0.01f;
		else
			LightManager->Lights[currentLight].position.y += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		if (LightManager->Lights[currentLight].lightType == DIRECTIONAL_LIGHT)
			LightManager->Lights[currentLight].direction.y -= 0.01f;
		else
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

void windowSizeCallback(GLFWwindow* window, int width, int height)
{
	for (int i = 0; i != FBOs.size(); i++)
	{
		if (FBOs[i]->sWidth != width || FBOs[i]->sHeight != height)
		{
			FBOs[i]->Reset(width, height);
		}
	}
}