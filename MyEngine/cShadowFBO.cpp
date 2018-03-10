#include "cShadowFBO.h"
#include "cShader.h"
#include <iostream>

cShadowFBO::cShadowFBO(GLuint width, GLuint height)
{
	this->sWidth = width;
	this->sHeight = height;

	float quadVertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	fboInit();
	return;
}

void cShadowFBO::BindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
}
void cShadowFBO::UnbindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void cShadowFBO::ClearBuffers(bool clearColor, bool clearDepth)
{
	glViewport(0, 0, this->sWidth, this->sHeight);
	GLfloat zero = 0.0f;
	GLfloat one = 1.0f;
	if (clearColor)
		glClearBufferfv(GL_COLOR, 0, &zero);
	if (clearDepth)
		glClearBufferfv(GL_DEPTH, 0, &one);
}

bool cShadowFBO::fboInit()
{
	bool ret = false;
	glGenFramebuffers(1, &this->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);


	// Depth Buffer
	glGenTextures(1, &this->texDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, this->texDepthBuffer);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, this->sWidth, this->sHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->texDepthBuffer, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Shadow FBO created successfully." << std::endl;
		ret = true;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return ret;
}
bool cShadowFBO::rboInit()
{
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->sWidth, this->sHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo);

	return true;
}
void cShadowFBO::Reset(GLuint width, GLuint height)
{
	glDeleteTextures(1, &(this->texDepthBuffer));

	glDeleteFramebuffers(1, &(this->fbo));

	this->sWidth = width;
	this->sHeight = height;
	this->fboInit();
}

void cShadowFBO::Draw(cShader shader)
{
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}