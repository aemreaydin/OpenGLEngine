#version 430
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 lightModel;

//out vec4 LightPOV;
// uniform mat4 lightSpaceMatrix;

void main()
{
	TexCoords = aTexCoords;
	//LightPosition = vec3(lightModel * vec4(lightPosition, 1.0));
	gl_Position = vec4(aPos, 0.0f, 1.0f);
}