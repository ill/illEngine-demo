#version 120

attribute vec4 position;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec3 bitangent;
attribute vec4 boneIndices;
attribute vec4 weights;
attribute vec2 texCoords;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

varying vec2 texCoordsOut;
varying vec3 lightVec;

#define MAX_BONES 128
uniform mat4 bones[MAX_BONES];

void main()
{
	texCoordsOut = texCoords;

	//skinning
	mat4 transformedMat = bones[int(boneIndices[0])] * weights[0];
	transformedMat += bones[int(boneIndices[1])] * weights[1];
	transformedMat += bones[int(boneIndices[2])] * weights[2];
	transformedMat += bones[int(boneIndices[3])] * weights[3];
	
	vec4 skinnedPos = modelViewMatrix * transformedMat * position;
	
	//lighting/normal mapping
	mat3 transformedNormMat = mat3(transformedMat);
	
    vec3 lightNorm = normalize(normalMatrix * normal);
	vec3 lightTan = normalize(normalMatrix * tangent);
	vec3 lightBitan = normalize(normalMatrix * bitangent);
	
	vec3 lightDir = normalize(vec3(0.0, 100.0, 100.0) - skinnedPos.xyz);		//TODO: pass in light pos
	
	lightVec = normalize(vec3(
		dot(lightDir, lightTan),
		dot(lightDir, lightBitan),
		dot(lightDir, lightNorm)));
	
	//position
    gl_Position = modelViewProjectionMatrix * transformedMat * position;;
}