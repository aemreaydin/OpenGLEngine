#version 430
layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 FragNormal;
layout(location=2) out vec4 FragVertex;
layout(location=3) out vec4 FragDepth;
// From Vertex Shader
in vec3 Normal;
in vec3 ObjectPosition;
in vec2 TexCoords;
in vec3 LightPosition;
in vec4 LightPOV;

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
uniform sampler2D texFBODepth;

uniform sampler2D FBOFinalImage;

uniform float screenWidth;
uniform float screenHeight;
uniform float time;

uniform int numPass;

uniform samplerCube skybox;

uniform bool isSecondPass;
uniform bool isSkybox;

uniform bool isReflectRefract;
uniform float RefractCoeff;


uniform float blurOffsets[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
uniform float blurWeights[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216,
									   0.0540540541, 0.0162162162 );


uniform int FBOMode;
void main()
{   
	FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	FragNormal  = vec4(0.0f, 0.0f, 0.0f, 0.25f);
	FragVertex  = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	vec4 textureColor = (texture(texture_diffuse1, TexCoords.xy));


	if(isReflectRefract)
	{
		vec3 I = normalize(ObjectPosition - eyePos);
		vec3 ReflectR = reflect(I, normalize(Normal));
		vec3 RefractR = refract(I, normalize(Normal), RefractCoeff);

		vec4 ReflectColor = vec4(texture(skybox, ReflectR).rgb, 1.0);
		vec4 RefractColor = vec4(texture(skybox, RefractR).rgb, 1.0);

		vec3 viewVector = normalize(eyePos);
		float refractiveFactor = dot(viewVector, vec3(0.0, 1.0, 0.0));

		FragColor = mix(ReflectColor, RefractColor, refractiveFactor);
		FragNormal = vec4(normalize(Normal) * 0.5 + 0.5, 1.0);
		FragVertex = vec4(ObjectPosition, 1.0f);
		return;
	}

	if(isSecondPass)
	{
		vec2 textScreenCoords = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);

		vec4 colorAtThisPixel = texture(texFBOColor, textScreenCoords).rgba;
		vec4 normalAtThisPixel = texture(texFBONormal, textScreenCoords).rgba;
		vec4 vertexAtThisPixel = texture(texFBOVertex, textScreenCoords).rgba;
		vec4 depthAtThisPixel = texture(texFBODepth, textScreenCoords).rgba;


		vec3 norm = normalize(normalAtThisPixel.rgb);
		vec3 viewDir = normalize(eyePos - ObjectPosition.rgb);
		//viewDir = normalize(eyePos - ObjectPosition);

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
		if(FBOMode == 0) // Output with lighting
			FragColor = vec4(result, 1.0);
		else if(FBOMode == 1) // Output without lighting
			FragColor = vec4(colorAtThisPixel.rgb, 1.0);
		else if(FBOMode == 2) // Normals
			FragColor = vec4(normalAtThisPixel.rgb, 1.0);
		else if(FBOMode == 3) // Vertices
			FragColor = vec4(vertexAtThisPixel.rgb, 1.0);
		else if(FBOMode == 4) // Inverted Output	
			FragColor = vec4(1.0, 1.0, 1.0, 1.0) - vec4(colorAtThisPixel.rgb, 0.0);
		else if(FBOMode == 5) // Grayscale
		{
			float gray = (normalAtThisPixel.r * 0.299 + normalAtThisPixel.g * 0.587 + normalAtThisPixel.b * 0.114); 
			FragColor = vec4(gray, gray, gray, 1.0);
		}
		else if(FBOMode == 6) // Blur --- TODO: Gaussian Blur
		{
			// FragColor = textire(texFBOColor, textScreenCoords) * blurWeights[0];
			// for(int i = 1; i != 5; i++)
			// {
			// 	FragColor += texture(texFBOColor, vec2(textScreenCoords) + vec2(0.0, blurOffsets[i])) * blurWeights[i];
			// }
			// FragColor = vec4(sum.rgb, 1.0);
		}
		else if(FBOMode == 7) // Edge Detection
		{
			vec4 top = texture(texFBOColor, vec2(textScreenCoords.x, textScreenCoords.y + 1.0 / 400.0)).rgba;
			vec4 bottom = texture(texFBOColor, vec2(textScreenCoords.x, textScreenCoords.y - 1.0 / 400.0)).rgba;
			vec4 left = texture(texFBOColor, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y)).rgba;
			vec4 right = texture(texFBOColor, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y)).rgba;
			vec4 topLeft = texture(texFBOColor, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y + 1.0 / 400.0)).rgba;
			vec4 topRight = texture(texFBOColor, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y + 1.0 / 400.0)).rgba;
			vec4 bottomLeft = texture(texFBOColor, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y - 1.0 / 400.0)).rgba;
			vec4 bottomRight = texture(texFBOColor, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y - 1.0 / 400.0)).rgba;
			vec4 sx = -topLeft - 2 * left - bottomLeft + topRight + 2 * right + bottomRight;
			vec4 sy = -topLeft - 2 * top - topRight + bottomLeft + 2 * bottom + bottomRight;
			vec4 sobel = sqrt(sx * sx + sy * sy);
			FragColor = vec4(sobel.rgb, 1.0);
		}
		else if(FBOMode == 8) // Edge Detection Alternative
		{
			vec4 top = texture(texFBOColor, vec2(textScreenCoords.x, textScreenCoords.y + 1.0 / 400.0)).rgba;
			vec4 bottom = texture(texFBOColor, vec2(textScreenCoords.x, textScreenCoords.y - 1.0 / 400.0)).rgba;
			vec4 left = texture(texFBOColor, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y)).rgba;
			vec4 right = texture(texFBOColor, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y)).rgba;
			vec4 topLeft = texture(texFBOColor, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y + 1.0 / 400.0)).rgba;
			vec4 topRight = texture(texFBOColor, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y + 1.0 / 400.0)).rgba;
			vec4 bottomLeft = texture(texFBOColor, vec2(textScreenCoords.x - 1.0 / 600.0, textScreenCoords.y - 1.0 / 400.0)).rgba;
			vec4 bottomRight = texture(texFBOColor, vec2(textScreenCoords.x + 1.0 / 600.0, textScreenCoords.y - 1.0 / 400.0)).rgba;
			vec4 sx = -3 * topLeft - 10 * left - 3 * bottomLeft + 3 * topRight + 10 * right + 3 * bottomRight;
			vec4 sy = -3 * topLeft - 10 * top - 3 * topRight + 3 * bottomLeft + 10 * bottom + 3 * bottomRight;
			vec4 sobel = sqrt(sx * sx + sy * sy);
			FragColor = vec4(sobel.rgb, 1.0);
		}
		else if(FBOMode == 9) // Cos-Sine Time
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
		return;
	}
	//float Depth = texture(texFBODepth, TexCoords.xy).x;
	//Depth = 1.0 - (1.0 - Depth) * 25.0;
	FragColor = vec4(textureColor.rgb, 1.0);
	FragDepth = vec4(LightPOV.xyz, 1.0);
	FragNormal = vec4(normalize(Normal) * 0.5 + 0.5, 1.0);
	FragVertex = vec4(ObjectPosition, 1.0);
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
	
	
	
	
	
	

