#version 430

out vec4 FragColor;
out vec4 FragNormal;
out vec4 FragVertex;

in vec2 TexCoords;

uniform sampler2D texFBOColor;
uniform sampler2D texFBONormal;
uniform sampler2D texFBOVertex;

uniform sampler2D screenTexture;

uniform float screenWidth;
uniform float screenHeight;

void main()
{
	FragColor = vec4(0.f, 0.f, 0.f, 0.f);
	FragNormal = vec4(0.f, 0.f, 0.f, 0.f);
	FragVertex = vec4(0.f, 0.f, 0.f, 0.f);

	vec2 scrCoords = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);
	vec4 colorAtThisPixel = texture(texFBOColor, scrCoords).rgba;
	vec4 normalAtThisPixel = texture(texFBONormal, scrCoords).rgba;
	vec4 vertexAtThisPixel = texture(texFBOVertex, scrCoords).rgba;


	//vec3 result = texture(colorAtThisPixel, TexCoords).rgb;
	FragColor = colorAtThisPixel;
	FragNormal = normalAtThisPixel;
	FragVertex = vertexAtThisPixel;
	//FragColor = vec4(FragVertex.rgb, 1.f);
}