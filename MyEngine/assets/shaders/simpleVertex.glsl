#version 430
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

in vec3 skyboxPosition;

in vec3 lightPosition;

out vec3 Normal;
out vec3 ObjectPosition;
out vec3 TexCoords;
out vec3 LightPosition;

uniform mat4 lightModel;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool isSecondPass;

uniform bool isSkybox;

void main()
{
	if(isSkybox)
	{
		TexCoords = skyboxPosition;    
		vec4 pos = projection * view * vec4(skyboxPosition, 1.0f);
    	gl_Position = pos.xyww;
		return;
	}

	if(isSecondPass)
	{
    	TexCoords.xy = aTexCoords;
    	gl_Position = vec4(aPos.xy, 0.0f, 1.0f);
		return;
	}
	else
	{
		TexCoords.xy = aTexCoords;    
		Normal = mat3(transpose(inverse(model))) * aNormal;
		LightPosition = vec3(lightModel * vec4(lightPosition, 1.0));
		ObjectPosition = vec3(model * vec4(aPos, 1.0));
		gl_Position = projection * view * model * vec4(aPos, 1.0);
		return;
	}
}