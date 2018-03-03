#version 430

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	vec3 result = texture(screenTexture, TexCoords).rgb;
	FragColor = vec4(result, 1.0f);
}