#ifndef _SKYBOX_HG_
#define _SKYBOX_HG_

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <string>
#include <vector>

class cShader;

class cSkybox
{
public:
	cSkybox(std::string folder, cShader shader);
	bool LoadCubeMap();
	void Draw(cShader shader);

	GLuint textureID;
	std::vector<std::string> Faces;

private:
	void init();
	std::string folderName;
	GLuint VAO, VBO;
	cShader* shader;
};
#endif // !_SKYBOX_HG_
