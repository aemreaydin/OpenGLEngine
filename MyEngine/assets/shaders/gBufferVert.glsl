#version 430

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

//in vec3 lightPosition;

out vec3 Normal;
out vec3 ObjectPosition;
out vec2 TexCoords;
//out vec3 LightPosition;
//out vec4 LightPOV;

// uniform mat4 lightModel;
// uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	TexCoords.xy = aTexCoords;
	Normal = mat3(transpose(inverse(model))) * aNormal;
	//LightPosition = vec3(lightModel * vec4(lightPosition, 1.0));
	ObjectPosition = vec3(model * vec4(aPos, 1.0));
	//LightPOV = lightSpaceMatrix * vec4(ObjectPosition, 1.0);
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	return;
}