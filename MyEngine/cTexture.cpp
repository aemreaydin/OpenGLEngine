#include "cTexture.h"

cTexture::cTexture(std::string path)
{
	glGenTextures(1, &texID);

	int width, height, nrComponents;
	unsigned char * data = SOIL_load_image(path.c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, texID);
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		SOIL_free_image_data(data);
	}
	else
		printf("texture could not be loaded.\n");
}

void cTexture::Bind(int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, this->texID);
}