#ifndef _FBO_HG_
#define _FBO_HG_

#include <glad\glad.h>
#include <GLFW\glfw3.h>

class cShader;

class cFBO
{
public:
	cFBO(GLuint width, GLuint height);
	void BindFBO();
	void UnbindFBO();
	void ClearBuffers(bool clearColor = true, bool clearDepth = true);
	void Draw(cShader shader);
	GLuint texColorBuffer;
	GLuint texNormalBuffer;
	GLuint texVertexBuffer;
	GLuint texDepthBuffer;
	GLint  numPass;
private:
	GLuint fbo;
	GLuint rbo;
	GLuint sWidth;
	GLuint sHeight;
	bool fboInit();
	bool rboInit();

	GLuint VAO;
	GLuint VBO;
};
#endif // !_FBO_HG
