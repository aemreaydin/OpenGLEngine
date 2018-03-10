#ifndef _TEXTURE_HG_
#define _TEXTURE_HG_

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <SOIL2\SOIL2.h>

#include <string>

class cTexture
{
public:
	cTexture(std::string path);

	void Bind(int);

	GLuint texID;
};
#endif // !_TEXTURE_HG_
