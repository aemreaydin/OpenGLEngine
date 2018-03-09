#version 430

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragNormal;
layout(location = 2) out vec4 FragVertex;

in vec3 Normal;
in vec3 ObjectPosition;
in vec2 TexCoords;
in vec3 LightPosition;
in vec4 LightPOV;

uniform sampler2D texture_diffuse1;
uniform vec3 eyePos;

void main
{
	if (isReflectRefract)
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

	FragColor = vec4(texture(texture_diffuse1, TexCoords).rgb, 1.0);
	FragNormal = vec4(normalize(Normal) * 0.5 + 0.5, 1.0);
	FragVertex = vec4(ObjectPOsition, 1.0);
}