#version 430
layout(location=0) out vec4 FragColor;

uniform sampler2D finalTexture;
uniform float screenWidth;
uniform float screenHeight;

uniform int FBOMode;
uniform float time;

void main()
{   
	vec2 textScreenCoords = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);
	vec4 colorAtThisPixel = texture(finalTexture, textScreenCoords).rgba;

	FragColor = vec4(colorAtThisPixel.rgb, 1.0);
}