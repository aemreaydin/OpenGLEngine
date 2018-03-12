#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;
layout (location = 5) in vec4 aBoneIDs;
layout (location = 6) in vec4 aBoneWeights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

#define MAX_NUMBER_OF_BONES 100
uniform mat4 Bones[MAX_NUMBER_OF_BONES];
uniform int numBonesUsed;

out vec3 fObjectPosition;
out vec3 fNormal;
out vec2 fTexCoords;
out vec3 fTangent;
out vec3 fBiTangent;
//out vec3 fLightPosition;
 //fLightPosition = vec3(lightModel * vec4*LightPosition)
mat4 applyBoneTransform();

void main()
{
    vec4 vPosition = vec4(aPos, 1.0);    
   
    mat4 boneTransform = applyBoneTransform();
    mat4 boneNormal = inverse(transpose(boneTransform * model));

    vPosition *= boneTransform;



    fTexCoords = aTexCoords.xy;
    fObjectPosition = (model * vPosition).xyz;
    fNormal = mat3(boneNormal) * normalize(aNormal.xyz);
    fTangent = mat3(boneNormal) * aTangent.xyz;
    fBiTangent = mat3(boneNormal) * aBiTangent.xyz;
    gl_Position = projection * view * model * vPosition;
}

mat4 applyBoneTransform()
{
    mat4 boneTransform = Bones[int(aBoneIDs[0])] * aBoneWeights[0];
    boneTransform += Bones[int(aBoneIDs[1])] * aBoneWeights[1];
    boneTransform += Bones[int(aBoneIDs[2])] * aBoneWeights[2];
    boneTransform += Bones[int(aBoneIDs[3])] * aBoneWeights[3];

    return boneTransform;
}