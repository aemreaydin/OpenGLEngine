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
	GLuint sWidth;
	void Reset(GLuint width, GLuint height);
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
