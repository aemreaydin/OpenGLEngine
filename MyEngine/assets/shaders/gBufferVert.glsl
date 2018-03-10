#version 430

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

//in vec3 lightPosition;

out vec3 Normal;
out vec3 ObjectPosition;
out vec2 TexCoords;
//out vec3 LightPosition;
//out vec4 ShadowCoord;

uniform mat4 lightProjection;
uniform mat4 lightView;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{


	TexCoords.xy = aTexCoords;
	Normal = mat3(transpose(inverse(model))) * aNormal;
	ObjectPosition = vec3(model * vec4(aPos, 1.0));


	//ShadowCoord = biasMatrix * lightProjection * lightView * vec4(ObjectPosition, 1.0);

	gl_Position = projection * view * model * vec4(aPos, 1.0);
	return;
}