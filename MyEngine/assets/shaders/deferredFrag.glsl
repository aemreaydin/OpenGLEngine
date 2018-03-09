#version 430
layout(location = 0) out vec4 FragColor;

// in 3 Normal;
// in vec3 ObjectPosition;
in vec2 TexCoords;
//in vec3 LightPosition;
// in vec4 LightPOV;

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

uniform float screenWidth;
uniform float screenHeight;

uniform float time;

uniform float blurOffsets[5] = float[](0.0, 1.0, 2.0, 3.0, 4.0);
uniform float blurWeights[5] = float[](0.2270270270, 0.1945945946, 0.1216216216,
	0.0540540541, 0.0162162162);

uniform int FBOMode;

vec3 calcDirectionalLight(sLight light, vec3 normal, vec3 viewDir, vec4 textureCol);
vec3 calcPointLight(sLight light, vec3 normal, vec3 objPosition, vec3 viewDir, vec4 textureCol);
vec3 calcSpotLight(sLight light, vec3 normal, vec3 objPosition, vec3 viewDir, vec4 textureCol);


void main()
{
	vec2 textScreenCoords = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);

	vec4 colorAtThisPixel = texture(texFBOColor, textScreenCoords).rgba;
	vec4 normalAtThisPixel = texture(texFBONormal, textScreenCoords).rgba;
	vec4 vertexAtThisPixel = texture(texFBOVertex, textScreenCoords).rgba;


	vec3 norm = normalize(normalAtThisPixel.rgb);
	vec3 viewDir = normalize(eyePos - vertexAtThisPixel.rgb);

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
	FragColor = vec4(result, 1.0);
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







