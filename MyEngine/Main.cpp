#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <vector>

GLint sWidth = 1600;
GLint sHeight = 900;

int main(void)
{
	GLFWwindow* window;
	if (!glfwInit())
	{
		printf("GLFW can't be initialized.\n");
		return 0;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(1920, 1080, "Windiana James", NULL, NULL);
	if (!window)
	{
		printf("Window can't be created.\n");
		glfwTerminate();
		return 0;
	}
	glfwGetFramebufferSize(window, &sWidth, &sHeight);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glViewport(0, 0, sWidth, sWidth);
	glEnable(GL_DEPTH);
	while (!glfwWindowShouldClose(window))
	{


		glfwSwapBuffers(window);
	}
	return 0;
}
