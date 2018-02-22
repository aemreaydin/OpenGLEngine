#include "cGLCalls.h"

cGLCalls::cGLCalls(GLint width, GLint height, std::string title)
{
	this->width = width;
	this->height = height;
	this->title = title;
}

bool cGLCalls::Initialize()
{
	if (!glfwInit())
	{
		return false;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	return true;
}
bool cGLCalls::CreateGLWindow()
{
	this->window = glfwCreateWindow(width, height, this->title.c_str(), NULL, NULL);
	if (!window)
	{
		return false;
	}
	glfwGetFramebufferSize(this->window, &this->width, &this->height);
	glfwMakeContextCurrent(this->window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glViewport(0, 0, this->width, this->height);

	return true;
}
GLFWwindow* cGLCalls::GetWindow()
{
	return this->window;
}