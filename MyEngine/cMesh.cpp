#include "cMesh.h"

#include <glm/glm.hpp>

cMesh::cMesh(std::vector<sVertex> vertices, std::vector<GLuint> indices, std::vector<sTexture> textures)
{
	this->VecVertices = vertices;
	this->VecIndices = indices;
	this->VecTextures = textures;

	this->setupMesh();
	return;
}

cMesh::cMesh(std::vector<sSkinnedMeshVertex> vertices, std::vector<GLuint> indices, std::vector<sTexture> textures)
{
	this->VecSkinnedMeshVertices = vertices;
	this->VecIndices = indices;
	this->VecTextures = textures;

	this->setupSkinnedMesh();
	return;
}


cMesh::~cMesh()
{
	return;
}

void cMesh::Draw(cShader shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	for (unsigned int i = 0; i < this->VecTextures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = this->VecTextures[i].Type;

		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		else if (name == "texture_height")
			number = std::to_string(heightNr++);

		std::string toSend = name + number;

		shader.SetInteger((toSend).c_str(), i, true);
		glBindTexture(GL_TEXTURE_2D, this->VecTextures[i].ID);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, this->VecIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void cMesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, this->VecVertices.size() * sizeof(sVertex), &VecVertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->VecIndices.size() * sizeof(GLuint), &VecIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)offsetof(sVertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)offsetof(sVertex, TexCoords));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)offsetof(sVertex, Tangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)offsetof(sVertex, BiTangent));

	glBindVertexArray(0);
}

void cMesh::setupSkinnedMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, this->VecSkinnedMeshVertices.size() * sizeof(sSkinnedMeshVertex), &VecSkinnedMeshVertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->VecIndices.size() * sizeof(GLuint), &VecIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sSkinnedMeshVertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sSkinnedMeshVertex), (void*)offsetof(sSkinnedMeshVertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sSkinnedMeshVertex), (void*)offsetof(sSkinnedMeshVertex, TexCoords));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(sSkinnedMeshVertex), (void*)offsetof(sSkinnedMeshVertex, Tangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(sSkinnedMeshVertex), (void*)offsetof(sSkinnedMeshVertex, BiTangent));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(sSkinnedMeshVertex), (void*)offsetof(sSkinnedMeshVertex, BoneID));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(sSkinnedMeshVertex), (void*)offsetof(sSkinnedMeshVertex, BoneWeights));

	glBindVertexArray(0);
}
