#ifndef _GL_CALLS_
#define _GL_CALLS_

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <iostream>

class cGLCalls
{
public:
	cGLCalls(GLint width, GLint height, std::string title);
	bool Initialize();
	bool CreateGLWindow();
	GLFWwindow* GetWindow();
private:
	GLFWwindow * window;
	GLint height, width;
	std::string title;
};


#endif