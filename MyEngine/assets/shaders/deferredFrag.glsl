#version 430
layout(location = 0) out vec4 FragColor;

// in 3 Normal;
// in vec3 ObjectPosition;
in vec2 TexCoords;
//in vec3 LightPosition;
// in vec4 LightPOV;

uniform mat4 lightProjection;
uniform mat4 lightView;
uniform sampler2D shadowMap;
mat4 biasMatrix = mat4(0.5, 0.0, 0.0, 0.0,
						0.0, 0.5, 0.0, 0.0,
						0.0, 0.0, 0.5, 0.5,
						0.5, 0.5, 0.5, 1.0);

struct sLight
{
	vec4 AmbientColor;
	vec4 DiffuseColor;
	vec4 SpecularColor;
	vec3 Position;
	vec3 Attenuation;
	vec3 LightDirection;
	vec2 Cutoff;
	int LightType;
};
uniform int NumLights;
uniform sLight Lights[100];
uniform vec3 eyePos;

uniform sampler2D texFBOColor;
uniform sampler2D texFBONormal;
uniform sampler2D texFBOVertex;
uniform sampler2D texFBODepth;
uniform sampler2D stepTexture;

uniform float screenWidth;
uniform float screenHeight;

uniform float time;

uniform float blurOffsets[5] = float[](0.0, 1.0, 2.0, 3.0, 4.0);
uniform float blurWeights[5] = float[](0.2270270270, 0.1945945946, 0.1216216216,
	0.0540540541, 0.0162162162);

uniform int FBOMode;
uniform bool isShadowMapped;

vec3 calcDirectionalLight(sLight light, vec3 normal, vec3 viewDir, vec4 textureCol);
vec3 calcPointLight(sLight light, vec3 normal, vec3 objPosition, vec3 viewDir, vec4 textureCol);
vec3 calcSpotLight(sLight light, vec3 normal, vec3 objPosition, vec3 viewDir, vec4 textureCol);

float SobelNormalHorizontal(sampler2D texMap, vec2 texCoord, vec2 pixelSize);
float SobelNormalVertical(sampler2D texMap, vec2 texCoord, vec2 pixelSize);
float SobelDepthHorizontal(sampler2D texMap, vec2 texCoord, vec2 pixelSize);
float SobelDepthVertical(sampler2D texMap, vec2 texCoord, vec2 pixelSize);

void main()
{
	vec2 textScreenCoords = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);
	vec2 pixelSize = vec2(1.0 / screenWidth, 1.0 / screenHeight);

	vec4 colorAtThisPixel = texture(texFBOColor, textScreenCoords).rgba;
	vec4 normalAtThisPixel = texture(texFBONormal, textScreenCoords).rgba;
	vec4 vertexAtThisPixel = texture(texFBOVertex, textScreenCoords).rgba;
	vec4 ShadowCoord = biasMatrix * lightProjection * lightView * vec4(vertexAtThisPixel);
	//float shadowFactor = textureProj(shadowMap, ShadowCoord);

	vec3 norm = normalize(normalAtThisPixel.rgb);
	vec3 viewDir = normalize(eyePos - vertexAtThisPixel.rgb);

	float NdotL = dot(normalAtThisPixel.rgb, Lights[0].LightDirection);
	float stepFloat = texture(stepTexture, vec2(NdotL, 0.5)).r;

	vec3 result = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < NumLights; i++)
	{
		if (Lights[i].LightType == 0)
			result += calcDirectionalLight(Lights[i], norm.rgb, viewDir, colorAtThisPixel.rgba);
		else if (Lights[i].LightType == 1)
			result += calcPointLight(Lights[i], norm.rgb, vertexAtThisPixel.rgb, viewDir, colorAtThisPixel.rgba);
		else if (Lights[i].LightType == 2)
			result += calcSpotLight(Lights[i], norm.rgb, vertexAtThisPixel.rgb, viewDir, colorAtThisPixel.rgba);
	}

	if(FBOMode == 0) // Final Output
		FragColor = vec4(result.rgb, 1.0);
	else if (FBOMode == 1) // Inverted Output
	{
		FragColor.rgb = vec3(1.0, 1.0, 1.0) - vec3(result.rgb);
		FragColor.a = 1.0;
	}	
	else if (FBOMode == 2) // Grayscale
	{
		float gray = (result.r * 0.299 + result.g * 0.587 + result.b * 0.114);
		FragColor = vec4(gray, gray, gray, 1.0);
	}
	else if (FBOMode == 3) // Blur --- TODO: Gaussian Blur
	{
		const float blurSizeH = 1.0 / pixelSize.x;
		const float blurSizeW = 1.0 / pixelSize.y;

		vec4 sum = vec4(0.0);
		for(int i = -4; i <= 4; i++)
		{
			for(int j = -4; j <= 4; j++)
			{
				sum += texture(texFBOColor, vec2(textScreenCoords.x + i * blurSizeH, textScreenCoords.y + j * blurSizeW)) / 81.0f;
			}
		}
		FragColor = vec4(sum.rgb, 1.0);
	}
	else if (FBOMode == 4) // Edge Detection with Colors for some reason
	{
		vec4 top = texture(texFBOColor, vec2(textScreenCoords.x, textScreenCoords.y + 1.0 / screenHeight)).rgba;
		vec4 bottom = texture(texFBOColor, vec2(textScreenCoords.x, textScreenCoords.y - 1.0 / screenHeight)).rgba;
		vec4 left = texture(texFBOColor, vec2(textScreenCoords.x - 1.0 / screenWidth, textScreenCoords.y)).rgba;
		vec4 right = texture(texFBOColor, vec2(textScreenCoords.x + 1.0 / screenWidth, textScreenCoords.y)).rgba;
		vec4 topLeft = texture(texFBOColor, vec2(textScreenCoords.x - 1.0 / screenWidth, textScreenCoords.y + 1.0 / screenHeight)).rgba;
		vec4 topRight = texture(texFBOColor, vec2(textScreenCoords.x + 1.0 / screenWidth, textScreenCoords.y + 1.0 / screenHeight)).rgba;
		vec4 bottomLeft = texture(texFBOColor, vec2(textScreenCoords.x - 1.0 / screenWidth, textScreenCoords.y - 1.0 / screenHeight)).rgba;
		vec4 bottomRight = texture(texFBOColor, vec2(textScreenCoords.x + 1.0 / screenWidth, textScreenCoords.y - 1.0 / screenHeight)).rgba;
		vec4 sx = -topLeft - 2 * left - bottomLeft + topRight + 2 * right + bottomRight;
		vec4 sy = -topLeft - 2 * top - topRight + bottomLeft + 2 * bottom + bottomRight;
		vec4 sobel = sqrt(sx * sx + sy * sy);
		FragColor = vec4(sobel.rgb, 1.0);
	}
	else if (FBOMode == 5) // Toon Shading
	{
		float SobelDepth = SobelDepthHorizontal(texFBODepth, textScreenCoords, pixelSize) * SobelDepthVertical(texFBODepth, textScreenCoords, pixelSize);
		float SobelNormal = SobelNormalHorizontal(texFBONormal, textScreenCoords, pixelSize) * SobelNormalVertical(texFBONormal, textScreenCoords, pixelSize);

		FragColor = vec4(result.rgb * SobelDepth * SobelNormal * stepFloat, 1.0);
	}
	else if (FBOMode == 6) // Night Vision TODO: Improve
	{
		vec2 timeUV;
		timeUV.x = 0.35 * sin(time * 50.0);
		timeUV.y = 0.35 * cos(time * 50.0);

		vec4 color = vec4(texture(texFBOColor, textScreenCoords).rgb, 1.0);
		vec3 greenDom = vec3(0.3, 0.59, 0.11);
		float intensity = dot(greenDom, color.rgb);

		float greenOut = clamp(0.5 * (intensity - 0.5) + 0.5, 0.0, 1.0);
		vec3 outColor = vec3(0, greenOut, 0);

		FragColor = vec4(outColor, 1.0);
	}
	else if(FBOMode == 7)
	{
		int samples = 128;
		float intensity = 0.1;
		float decay = 0.96875;
		vec2 texCoords = textScreenCoords;
		vec2 direction = vec2(0.5) - texCoords;
		direction /= samples;
		vec3 outColor = texture(texFBOColor, texCoords).rgb;


		for(int i = 0; i != samples; i++)
		{
			outColor += texture(texFBOColor, texCoords).rgb * intensity;
			intensity *= decay;
			texCoords += direction;
		}
		FragColor = vec4(outColor, 1.0);
	}

	if(isShadowMapped)
	{

	if(texture(shadowMap, ShadowCoord.xy).r < ShadowCoord.z - 0.0005 )
	{
		FragColor.rgb *= 0.3;
	}
	}
	
}


vec3 calcDirectionalLight(sLight light, vec3 normal, vec3 viewDir, vec4 textureCol)
{
	vec3 lightDir = normalize(-light.LightDirection);

	float diffuseRatio = max(dot(normal, lightDir), 0.0);
	float specularRatio = pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), 64);

	vec3 ambient = 0.2 * light.AmbientColor.xyz * textureCol.xyz;
	vec3 diffuse = light.DiffuseColor.xyz * diffuseRatio * textureCol.xyz;
	vec3 specular = light.SpecularColor.xyz * specularRatio * textureCol.xyz;

	return (ambient + diffuse + specular);
}
vec3 calcPointLight(sLight light, vec3 normal, vec3 objPosition, vec3 viewDir, vec4 textureCol)
{
	vec3 lightDir = normalize(light.Position - objPosition);

	float diffuseRatio = max(dot(normal, lightDir), 0.0);
	float specularRatio = pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), 64);

	float dist = length(light.Position - objPosition);
	float attenuation = 1.0 / (light.Attenuation.x + light.Attenuation.y * dist + light.Attenuation.z * (dist * dist));

	vec3 ambient = 0.05 * light.AmbientColor.xyz * textureCol.xyz;
	vec3 diffuse = light.DiffuseColor.xyz * diffuseRatio * textureCol.xyz;
	vec3 specular = light.SpecularColor.xyz * specularRatio * textureCol.xyz;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}
vec3 calcSpotLight(sLight light, vec3 normal, vec3 objPosition, vec3 viewDir, vec4 textureCol)
{
	vec3 lightDir = normalize(light.Position - objPosition);

	float diffuseRatio = max(dot(normal, lightDir), 0.0);
	float specularRatio = pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), 64);

	float dist = length(light.Position - objPosition);
	float attenuation = 1.0 / (light.Attenuation.x + light.Attenuation.y * dist + light.Attenuation.z * (dist * dist));

	float theta = dot(lightDir, normalize(-light.LightDirection));
	float epsilon = light.Cutoff.y - light.Cutoff.x;
	float intensity = clamp((theta - light.Cutoff.y) / epsilon, 0.0, 1.0);

	vec3 ambient = 0.05 * light.AmbientColor.xyz * textureCol.xyz;
	vec3 diffuse = light.DiffuseColor.xyz * diffuseRatio * textureCol.xyz;
	vec3 specular = light.SpecularColor.xyz * specularRatio * textureCol.xyz;

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}

float SobelNormalHorizontal(sampler2D texMap, vec2 texCoord, vec2 pixelSize)
{
	vec3 sum = vec3(0.0);

	sum -= texture(texMap, vec2(texCoord.x - pixelSize.x, texCoord.y + pixelSize.y)).rgb * 3.0;
	sum -= texture(texMap, vec2(texCoord.x - pixelSize.x, texCoord.y)).rgb * 10.0;
	sum -= texture(texMap, vec2(texCoord.x - pixelSize.x, texCoord.y - pixelSize.y)).rgb * 3.0;

	sum += texture(texMap, vec2(texCoord.x + pixelSize.x, texCoord.y + pixelSize.y)).rgb * 3.0;
	sum += texture(texMap, vec2(texCoord.x + pixelSize.x, texCoord.y)).rgb * 10.0;
	sum += texture(texMap, vec2(texCoord.x + pixelSize.x, texCoord.y - pixelSize.y)).rgb * 3.0;

	float edgeFactor = dot(sum, sum);

	if(edgeFactor < 0.5)
		return 1.0;
	else
		return 0.0;
}
float SobelNormalVertical(sampler2D texMap, vec2 texCoord, vec2 pixelSize)
{
	vec3 sum = vec3(0.0);

	sum += texture(texMap, vec2(texCoord.x - pixelSize.x, texCoord.y + pixelSize.y)).rgb * 3.0;
	sum += texture(texMap, vec2(texCoord.x, texCoord.y + pixelSize.y)).rgb * 10.0;
	sum += texture(texMap, vec2(texCoord.x + pixelSize.x, texCoord.y + pixelSize.y)).rgb * 3.0;

	sum -= texture(texMap, vec2(texCoord.x - pixelSize.x, texCoord.y - pixelSize.y)).rgb * 3.0;
	sum -= texture(texMap, vec2(texCoord.x, texCoord.y - pixelSize.y)).rgb * 10.0;
	sum -= texture(texMap, vec2(texCoord.x + pixelSize.x, texCoord.y - pixelSize.y)).rgb * 3.0;

	float edgeFactor = dot(sum, sum);

	if(edgeFactor < 0.5)
		return 1.0;
	else
		return 0.0;
}
float SobelDepthHorizontal(sampler2D texMap, vec2 texCoord, vec2 pixelSize)
{
	float sum = 0.0;

	sum -= texture(texMap, vec2(texCoord.x - pixelSize.x, texCoord.y + pixelSize.y)).r * 3.0;
	sum -= texture(texMap, vec2(texCoord.x - pixelSize.x, texCoord.y)).r * 10.0;
	sum -= texture(texMap, vec2(texCoord.x - pixelSize.x, texCoord.y - pixelSize.y)).r * 3.0;

	sum += texture(texMap, vec2(texCoord.x + pixelSize.x, texCoord.y + pixelSize.y)).r * 3.0;
	sum += texture(texMap, vec2(texCoord.x + pixelSize.x, texCoord.y)).r * 10.0;
	sum += texture(texMap, vec2(texCoord.x + pixelSize.x, texCoord.y - pixelSize.y)).r * 3.0;

	if(sum < 0.05)
		return 1.0;
	else
		return 0.0;
}
float SobelDepthVertical(sampler2D texMap, vec2 texCoord, vec2 pixelSize)
{
	float sum = 0.0;

	sum += texture(texMap, vec2(texCoord.x - pixelSize.x, texCoord.y + pixelSize.y)).r * 3.0;
	sum += texture(texMap, vec2(texCoord.x, texCoord.y + pixelSize.y)).r * 10.0;
	sum += texture(texMap, vec2(texCoord.x + pixelSize.x, texCoord.y + pixelSize.y)).r * 3.0;

	sum -= texture(texMap, vec2(texCoord.x - pixelSize.x, texCoord.y - pixelSize.y)).r * 3.0;
	sum -= texture(texMap, vec2(texCoord.x, texCoord.y - pixelSize.y)).r * 10.0;
	sum -= texture(texMap, vec2(texCoord.x + pixelSize.x, texCoord.y - pixelSize.y)).r * 3.0;

	if(sum < 0.05)
		return 1.0;
	else
		return 0.0;
}




