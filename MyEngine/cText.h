#ifndef _TEXT_HG_
#define _TEXT_HG_

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include "ft2build.h"
#include FT_FREETYPE_H
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <map>
#include <string>
class cShader;

struct sCharacter
{
	GLuint TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	GLuint Advance;
};

class cText
{
public:
	cText(const char* texturePath);
	void printText2D(cShader shader, std::string text, GLfloat  x, GLfloat  y, GLfloat size, glm::vec3 color);
	GLuint VAO;
	GLuint VBO;
	std::map<GLchar, sCharacter> Characters;
private:
	bool initText2D();
	const char* texturePath;
	FT_Library ftLib;
	FT_Face ftFace;
};
#endif // !_TEXT_HG_
