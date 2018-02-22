#ifndef _cMesh_HG_
#define _cMesh_HG_

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <string>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "cShader.h"

struct sVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 BiTangent;
};
struct sTexture
{
	GLuint ID;
	std::string Type;
	std::string Path;
};

class cMesh
{
public:
	cMesh(std::vector<sVertex> vertices, std::vector<GLuint> indices, std::vector<sTexture> textures);
	~cMesh();

	void Draw(cShader shader);

	std::vector<sVertex> VecVertices;
	std::vector<GLuint> VecIndices;
	std::vector<sTexture> VecTextures;
private:
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};


#endif
