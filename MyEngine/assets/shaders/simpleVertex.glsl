#version 430
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;
layout (location = 5) in vec4 aBoneIDs;
layout (location = 6) in vec4 aBoneWeights;

out vec3 Normal;
out vec3 ObjectPosition;
out vec2 TexCoords;
out vec3 Tangent;
out vec3 BiTangent;

in vec3 skyboxPosition;
in vec3 lightPosition;
uniform mat4 lightModel;

#define MAX_NUMBER_OF_BONES 100
uniform mat4 Bones[MAX_NUMBER_OF_BONES];
uniform int numBonesUsed;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool isSecondPass;
uniform bool isSkinnedMesh;

mat4 applyBoneTransform();
void main()
{
	if(isSecondPass)
	{
    	TexCoords.xy = aTexCoords;
    	gl_Position = vec4(aPos.xy, 0.0f, 1.0f);
		return;
	}

	if(isSkinnedMesh)
	{
		vec4 vPosition = vec4(aPos, 1.0);    
	
		mat4 boneTransform = Bones[int(aBoneIDs[0])] * aBoneWeights[0];
    	boneTransform += Bones[int(aBoneIDs[1])] * aBoneWeights[1];
   		boneTransform += Bones[int(aBoneIDs[2])] * aBoneWeights[2];
    	boneTransform += Bones[int(aBoneIDs[3])] * aBoneWeights[3];
		mat4 boneNormal = inverse(transpose(boneTransform * model));

		vPosition = boneTransform * vPosition;

		TexCoords = aTexCoords.xy;
		ObjectPosition = (model * vPosition).xyz;
		Normal = mat3(boneNormal) * normalize(aNormal.xyz);
		Tangent = mat3(boneNormal) * aTangent.xyz;
		BiTangent = mat3(boneNormal) * aBiTangent.xyz;
		gl_Position = projection * view * model * vPosition;
		return;
	}
	else
	{
		TexCoords.xy = aTexCoords;    
		ObjectPosition = vec3(model * vec4(aPos, 1.0));
		Normal = mat3(transpose(inverse(model))) * aNormal;	
		Tangent = aTangent;
		BiTangent = aBiTangent;	
		gl_Position = projection * view * model * vec4(aPos, 1.0);
		return;
	}
}