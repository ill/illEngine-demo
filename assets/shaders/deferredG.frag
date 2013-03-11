//TODO: rename a bunch of stuff and use structs
//#pragma optimize(off)	//TODO: for development only, take this out

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

/**
G buffer layout
render target 0:
r10, g10, b10, a2
norm.x, norm.y, unused, materialType

Normals are stured using CryEngine 3 style spherical harmonics encoding.  This seems to make them more precise
and makes specular highlights with high glossiness factors look muuuuch better.

render target 1:
r8, g8, b8, a8
diffuse.r, diffuse.g, diffuse.b, unused

render target 2:
r8, g8, b8, a8
specular.r, specular.g, specular.b, specular power		

TODO: eventually I might eliminate specular color and leave it black and white, this should reduce it to 2 render targets.
Then use the z slot for specular whiteness, and the unused alpha component of diffuse for specular power.
*/

//encodes normals with that thing Cryengine 3 does
//http://aras-p.info/texts/CompactNormalStorage.html
vec2 encodeNormal(vec3 normal) {
   return normalize(normal.xy) * sqrt(-normal.z * 0.5 + 0.5) * 0.5 + 0.5;
}

void main(void) {
   //////////////////////////////////
   //normals
   vec3 finalNormal =
   
#ifdef NORMAL_MAP
      texture2D(normalMap, texCoordOut).xyz * 2.0 - 1.0;
   
   //Fix normal map pixels that point away, to create blackness
   //TODO: is this efficient?
   /*if(finalNormal.z <= 0.0) {
      finalNormal = vec3(0.0);
   }*/
   
   //convert normals to view space
   finalNormal = normalize(finalNormal.x * tangentOut
                         + finalNormal.y * bitangentOut
                         + finalNormal.z * normalOut);
#else
      normalize(normalOut);
#endif
      
   /*gl_FragData[0].xyz = finalNormal * 0.5 + 0.5;
   gl_FragData[0].a = 1.0;*/
   gl_FragData[0] = vec4(encodeNormal(finalNormal), 0.0, 1.0);
   
   //TODO: the alpha component will store material type
   //TODO: make use of the unused z slot
   
   //////////////////////////////////
   //diffuse
   gl_FragData[1].rgb
#ifdef DIFFUSE_MAP
      = (vec4(diffuseColor, 1.0) * texture2D(diffuseMap, texCoordOut)).rgb;
#else
      = diffuseColor;
#endif
   
   ///////////////////////////////////
   //specular
   gl_FragData[2]
#ifdef SPECULAR_MAP
      = specularColor * texture2D(specularMap, texCoordOut);
#else
      = specularColor;
#endif
}
