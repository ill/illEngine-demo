//TODO: rename a bunch of stuff and use structs

#ifdef NORMAL_ATTRIBUTE
varying vec3 normalOut;     //called normalOut, because it comes out of the vertex shader and into the fragment shader
#endif

//if doing normal mapping, tangents will be needed
#ifdef TANGENT_ATTRIBUTE
varying vec3 tangentOut;
varying vec3 bitangentOut;
#endif

//if doing texture mapping, texture coordinates will be needed
#ifdef TEX_COORD_ATTRIBUTE
varying vec2 texCoordOut;
#endif

#ifdef DIFFUSE_MAP
uniform sampler2D diffuseMap;
#endif

#ifdef SPECULAR_MAP
uniform sampler2D specularMap;
#endif

#ifdef NORMAL_MAP
uniform sampler2D normalMap;
#endif

uniform vec3 diffuseColor;
uniform vec4 specularColor;
//TODO: normal multiplier

void main(void) {
   gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
