#version 430
out vec4 FragNormal;
out vec4 FragColor;
out vec3 FragVertex;
// From Vertex Shader
in vec3 Normal;
in vec3 ObjectPosition;
in vec3 TexCoords;
in vec3 LightPosition;

// Light Properties
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
// 1 Directional - the others are point and spot lights.
uniform sLight Lights[100];
// Camera Position
uniform vec3 eyePos;
// Texture Properties
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height;

uniform sampler2D screenTexture;

vec3 calcDirectionalLight(sLight light, vec3 normal, vec3 viewDir, vec4 textureCol);
vec3 calcPointLight(sLight light, vec3 normal, vec3 objPosition, vec3 viewDir, vec4 textureCol);
vec3 calcSpotLight(sLight light, vec3 normal, vec3 objPosition, vec3 viewDir, vec4 textureCol);

uniform sampler2D texFBOColor;
uniform sampler2D texFBONormal;
uniform sampler2D texFBOVertex;

uniform sampler2D FBOFinalImage;

uniform float screenWidth;
uniform float screenHeight;

uniform int numPass;

uniform samplerCube skybox;

uniform bool isSecondPass;
uniform bool isSkybox;
void main()
{   
	FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	FragNormal  = vec4(0.0f, 0.0f, 0.0f, 0.25f);
	FragVertex  = vec3(0.0f, 0.0f, 0.0f);
	
	vec4 textureColor = (texture(texture_diffuse1, TexCoords.xy));

	if(isSkybox)
	{
		FragColor = texture(skybox, TexCoords);
		FragVertex = ObjectPosition;
		FragNormal.a = 0.f;
		return;
	}

	if(isSecondPass)
	{
		vec2 textScreenCoords = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);

		vec4 colorAtThisPixel = texture(texFBOColor, textScreenCoords).rgba;
		vec4 normalAtThisPixel = texture(texFBONormal, textScreenCoords).rgba;
		vec4 vertexAtThisPixel = texture(texFBOVertex, textScreenCoords).rgba;


		vec3 norm = normalize(normalAtThisPixel.rgb);
		vec3 viewDir = normalize(eyePos - vertexAtThisPixel.rgb);

		vec3 result = vec3(0.0, 0.0, 0.0);

		for(int i = 0; i < NumLights; i++)
		{
			if(Lights[i].LightType == 0)
				result += calcDirectionalLight(Lights[i], norm.rgb, viewDir, colorAtThisPixel.rgba);			
			else if(Lights[i].LightType == 1)
				result += calcPointLight(Lights[i], norm.rgb, vertexAtThisPixel.rgb, viewDir, colorAtThisPixel.rgba);
			else if(Lights[i].LightType == 2)
				result += calcSpotLight(Lights[i], norm.rgb, vertexAtThisPixel.rgb, viewDir, colorAtThisPixel.rgba);
		}

		FragColor = vec4(result.rgb, 1.0);
		return;
	}

	FragColor = vec4(textureColor.rgb, 1.0);
	FragNormal = vec4(Normal, 1.0);
	FragVertex = ObjectPosition;
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
	
	
	
	
	
	

