#include "cFBO.h"
#include "cShader.h"
#include <iostream>

cFBO::cFBO(GLuint width, GLuint height)
{
	this->sWidth = width;
	this->sHeight = height;

	//float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	//						 // positions   // texCoords
	//	-1.0f,  1.0f,  0.0f, 1.0f,
	//	-1.0f, -1.0f,  0.0f, 0.0f,
	//	1.0f, -1.0f,  1.0f, 0.0f,

	//	-1.0f,  1.0f,  0.0f, 1.0f,
	//	1.0f, -1.0f,  1.0f, 0.0f,
	//	1.0f,  1.0f,  1.0f, 1.0f
	//};

	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);

	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	//glBindVertexArray(0);

	fboInit();
	return;
}

void cFBO::BindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
}
void cFBO::UnbindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void cFBO::ClearBuffers(bool clearColor, bool clearDepth)
{
	glViewport(0, 0, this->sWidth, this->sHeight);
	GLfloat zero = 0.0f;
	GLfloat one = 1.0f;
	if (clearColor)
		glClearBufferfv(GL_COLOR, 0, &zero);
	if (clearDepth)
		glClearBufferfv(GL_DEPTH, 0, &one);
}

bool cFBO::fboInit()
{
	bool ret = false;
	glGenFramebuffers(1, &this->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Color Buffer
	glGenTextures(1, &this->texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, this->texColorBuffer);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, this->sWidth, this->sHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Normal Buffer
	glGenTextures(1, &this->texNormalBuffer);
	glBindTexture(GL_TEXTURE_2D, this->texNormalBuffer);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, this->sWidth, this->sHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Vertex Buffer
	glGenTextures(1, &this->texVertexBuffer);
	glBindTexture(GL_TEXTURE_2D, this->texVertexBuffer);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, this->sWidth, this->sHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Depth Buffer
	glGenTextures(1, &this->texDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, this->texDepthBuffer);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, this->sWidth, this->sHeight);

	// Create the framebuffer textures
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->texColorBuffer, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, this->texNormalBuffer, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, this->texVertexBuffer, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->texDepthBuffer, 0);

	static const GLenum drawBuffers[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};
	glDrawBuffers(3, drawBuffers);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "FBO created successfully." << std::endl;
		ret = true;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return ret;
}
bool cFBO::rboInit()
{
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->sWidth, this->sHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo);

	return true;
}

void cFBO::Draw(cShader shader)
{
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, this->texColorBuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}