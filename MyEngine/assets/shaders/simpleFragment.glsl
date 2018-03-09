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

	if(FBOMode == 0) // Final Output
		FragColor = vec4(colorAtThisPixel.rgb, 1.0);
	else if (FBOMode == 1) // Inverted Output	
		FragColor = vec4(1.0, 1.0, 1.0, 1.0) - vec4(colorAtThisPixel.rgb, 0.0);
	else if (FBOMode == 2) // Grayscale
	{
		float gray = (colorAtThisPixel.r * 0.299 + colorAtThisPixel.g * 0.587 + colorAtThisPixel.b * 0.114);
		FragColor = vec4(gray, gray, gray, 1.0);
	}
	else if (FBOMode == 3) // Blur --- TODO: Gaussian Blur
	{
		// FragColor = textire(texFBOColor, textScreenCoords) * blurWeights[0];
		// for(int i = 1; i != 5; i++)
		// {
		// 	FragColor += texture(texFBOColor, vec2(textScreenCoords) + vec2(0.0, blurOffsets[i])) * blurWeights[i];
		// }
		// FragColor = vec4(sum.rgb, 1.0);
	}
	else if (FBOMode == 4) // Edge Detection
	{
		vec4 top = texture(finalTexture, vec2(textScreenCoords.x, textScreenCoords.y + 1.0 / 400.0)).rgba;
		vec4 bottom = texture(finalTexture, vec2(textScreenCoords.x, textScreenCoords.y - 1.0 / 400.0)).rgba;
		vec4 left = texture(finalTexture, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y)).rgba;
		vec4 right = texture(finalTexture, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y)).rgba;
		vec4 topLeft = texture(finalTexture, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y + 1.0 / 400.0)).rgba;
		vec4 topRight = texture(finalTexture, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y + 1.0 / 400.0)).rgba;
		vec4 bottomLeft = texture(finalTexture, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y - 1.0 / 400.0)).rgba;
		vec4 bottomRight = texture(finalTexture, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y - 1.0 / 400.0)).rgba;
		vec4 sx = -topLeft - 2 * left - bottomLeft + topRight + 2 * right + bottomRight;
		vec4 sy = -topLeft - 2 * top - topRight + bottomLeft + 2 * bottom + bottomRight;
		vec4 sobel = sqrt(sx * sx + sy * sy);
		FragColor = vec4(sobel.rgb, 1.0);
	}
	else if (FBOMode == 5) // Edge Detection Alternative
	{
		vec4 top = texture(finalTexture, vec2(textScreenCoords.x, textScreenCoords.y + 1.0 / 400.0)).rgba;
		vec4 bottom = texture(finalTexture, vec2(textScreenCoords.x, textScreenCoords.y - 1.0 / 400.0)).rgba;
		vec4 left = texture(finalTexture, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y)).rgba;
		vec4 right = texture(finalTexture, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y)).rgba;
		vec4 topLeft = texture(finalTexture, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y + 1.0 / 400.0)).rgba;
		vec4 topRight = texture(finalTexture, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y + 1.0 / 400.0)).rgba;
		vec4 bottomLeft = texture(finalTexture, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y - 1.0 / 400.0)).rgba;
		vec4 bottomRight = texture(finalTexture, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y - 1.0 / 400.0)).rgba;
		vec4 sx = -3 * topLeft - 10 * left - 3 * bottomLeft + 3 * topRight + 10 * right + 3 * bottomRight;
		vec4 sy = -3 * topLeft - 10 * top - 3 * topRight + 3 * bottomLeft + 10 * bottom + 3 * bottomRight;
		vec4 sobel = sqrt(sx * sx + sy * sy);
		FragColor = vec4(sobel.rgb, 1.0);
	}
	else if (FBOMode == 6) // Night Vision TODO: Improve
	{
		vec2 timeUV;
		timeUV.x = 0.35 * sin(time * 50.0);
		timeUV.y = 0.35 * cos(time * 50.0);

		vec4 color = vec4(texture(finalTexture, textScreenCoords).rgb, 1.0);
		vec3 greenDom = vec3(0.3, 0.59, 0.11);
		float intensity = dot(greenDom, color.rgb);

		float greenOut = clamp(0.5 * (intensity - 0.5) + 0.5, 0.0, 1.0);
		vec3 outColor = vec3(0, greenOut, 0);

		FragColor = vec4(outColor, 1.0);
	}
}