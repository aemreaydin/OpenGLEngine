#include "cText.h"
#include <vector>
#include "cShader.h"


#include <SOIL2\SOIL2.h>

cText::cText(const char* texturePath)
{
	this->texturePath = texturePath;
	initText2D();
}

bool cText::initText2D()
{
	if (FT_Init_FreeType(&this->ftLib))
	{
		printf("Could not initialize FreeType Library.\n");
		return false;
	}
	if (FT_New_Face(ftLib, this->texturePath, 0, &ftFace))
	{
		printf("Failed to load the font.\n");
		return false;
	}
	FT_Set_Pixel_Sizes(ftFace, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c != 128; c++)
	{
		if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER))
		{
			printf("Failed to load glyph.\n");
			return false;
		}
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ftFace->glyph->bitmap.width,
			ftFace->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, ftFace->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		sCharacter character = { texture, glm::ivec2(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows),
							glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top), ftFace->glyph->advance.x };
		this->Characters.insert(std::pair<GLchar, sCharacter>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(ftFace);
	FT_Done_FreeType(ftLib);

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return true;
}
void cText::printText2D(cShader shader, std::string text, GLfloat  x, GLfloat  y, GLfloat scale, glm::vec3 color)
{
	shader.Use();
	shader.SetVector3f("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		sCharacter ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.Advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}