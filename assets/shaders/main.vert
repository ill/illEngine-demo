#version 120
#pragma optimize(off)	//TODO: for development only, take this out

//TODO: rename a bunch of stuff and use structs
//TODO: this pretty much only works for deferred shading at the moment

//this should pretty much always be here
#ifdef POSITION_TRANSFORM
uniform mat4 modelViewProjection;

//TODO: if forward shading, also include the modelViewMatrix

attribute vec3 positionIn;
#endif

#if defined(NORMAL_ATTRIBUTE) || defined(TANGENT_ATTRIBUTE)
uniform mat3 normalMat;
#endif

#ifdef NORMAL_ATTRIBUTE
attribute vec3 normalIn;

//TODO: this is only for deferred shading
varying vec3 normalOut;
#endif

//if doing normal mapping, tangents will be needed
#ifdef TANGENT_ATTRIBUTE
attribute vec3 tangentIn;
attribute vec3 bitangentIn;

//TODO: this is only for deferred shading
varying vec3 tangentOut;
varying vec3 bitangentOut;
#endif

//if doing texture mapping, texture coordinates will be needed
#ifdef TEX_COORD_ATTRIBUTE
attribute vec2 texCoordIn;
varying vec2 texCoordOut;
#endif

//if doing skeletal animation, also send that stuff
#ifdef SKELETAL_ANIMATION

	#ifndef MAX_BONES
	/*
	usually just allocate 128 bones

	keep in mind, custom allocating different bone sizes will require 
	compiling of multiple shaders instead of having 1 common shader, requiring more state changes!!!!

	So avoid doing that and just use 128 bones in most cases.  Shader model 1.1 is limited to 24 though.
	*/
	//TODO: check if shader model 1.1?  and make the default be 24 instead of 128
	#define MAX_BONES 128
	#endif

uniform mat4 boneMatrix[MAX_BONES];		//TODO: use mat3x4

attribute uvec4 blendIndices;
attribute vec4 blendWeights;

#endif

//TODO: vertex color and specular

void main(void) {

#ifdef SKELETAL_ANIMATION
	mat4 skinningMat = boneMatrix[int(blendIndices[0])] * blendWeights[0];
	skinningMat += boneMatrix[int(blendIndices[1])] * blendWeights[1];
	skinningMat += boneMatrix[int(blendIndices[2])] * blendWeights[2];
	skinningMat += boneMatrix[int(blendIndices[3])] * blendWeights[3];

	#if defined(NORMAL_ATTRIBUTE) || defined(TANGENT_ATTRIBUTE)
	mat3 skinningNormalMat(skinningMat);
	#endif
#endif

    //position    
#ifdef POSITION_TRANSFORM
    gl_Position = modelViewProjection
	#ifdef SKELETAL_ANIMATION
		* skinningMat
	#endif
		* vec4(positionIn, 1.0);
#endif
    
    //normals    
#ifdef NORMAL_ATTRIBUTE
    normalOut = normalMat
	#ifdef SKELETAL_ANIMATION
		* skinningNormalMat
	#endif
		* normalIn;
#endif
    
    //tangents and bitangents   
#ifdef TANGENT_ATTRIBUTE
    tangentOut = normalMat
	#ifdef SKELETAL_ANIMATION
		* skinningNormalMat
	#endif
		* tangentIn;
	
    bitangentOut = normalMat
	#ifdef SKELETAL_ANIMATION
		* skinningNormalMat
	#endif
		* bitangentIn;
#endif
   
//texture coord   
#ifdef TEX_COORD_ATTRIBUTE
	texCoordOut = texCoordIn;
#endif

	//TODO: vertex color
}