#version 430

uniform mat4 lightModel;
uniform mat4 view;
uniform mat4 projection;

in vec3 lightPosition;

void main()
{
	mat4 MVP = projection * view * lightModel;
	gl_Position = MVP * vec4(lightPosition, 1.0);
}