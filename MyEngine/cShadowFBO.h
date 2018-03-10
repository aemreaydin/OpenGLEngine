#ifndef _SHADOW_FBO_HG_
#define _SHADOW_FBO_HG_

#include <glad\glad.h>
#include <GLFW\glfw3.h>

class cShader;

class cShadowFBO
{
public:
	cShadowFBO(GLuint width, GLuint height);
	void BindFBO();
	void UnbindFBO();
	void ClearBuffers(bool clearColor = true, bool clearDepth = true);
	void Draw(cShader shader);
	void Reset(GLuint width, GLuint height);
	GLuint texDepthBuffer;
	GLuint sWidth;
	GLuint sHeight;
private:
	GLuint fbo;
	GLuint rbo;
	bool fboInit();
	bool rboInit();

	GLuint VAO;
	GLuint VBO;
};
#endif // !_FBO_HG
