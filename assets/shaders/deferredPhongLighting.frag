/**
Accumulates a light's contribution to the scene after the G Buffer stage
*/
#extension GL_EXT_gpu_shader4 : enable    //For now...

#if(!defined(POINT_LIGHT) && !defined(SPOT_LIGHT) && !defined(DIRECTIONAL_LIGHT))
#error "One of the following: POINT_LIGHT, SPOT_LIGHT, or DIRECTIONAL_LIGHT must be defined in the lighting shader"
#endif

#if((defined(POINT_LIGHT) && defined(SPOT_LIGHT)) || (defined(POINT_LIGHT) && defined(DIRECTIONAL_LIGHT)) || (defined(SPOT_LIGHT) && defined(DIRECTIONAL_LIGHT))) 
#error "Only one of the following: POINT_LIGHT, SPOT_LIGHT, or DIRECTIONAL_LIGHT must be defined at one time in the lighting shader"
#endif

varying vec3 viewPosition;

uniform sampler2D depthBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D diffuseBuffer;
uniform sampler2D specularBuffer;

uniform vec2 planes;

uniform float intensity;
uniform vec3 lightColor;

#if(defined(POINT_LIGHT) || defined(SPOT_LIGHT))
uniform vec3 lightPosition;			//light position in eye space not world space
uniform float attenuationStart;
uniform float attenuationEnd;
#endif

#if(defined(SPOT_LIGHT))
uniform float coneStart;
uniform float coneEnd;
#endif

#if(defined(SPOT_LIGHT) || defined(DIRECTIONAL_LIGHT))
uniform vec3 lightDirection;
#endif

//decodes normals with that thing Cryengine 3 does
//http://aras-p.info/texts/CompactNormalStorage.html
vec3 decodeNormal(vec2 normal) {
   vec4 res = vec4(-1.0, -1.0, 1.0, -1.0);
   res.xy += normal * 2.0;
   
   float length = dot(res.xyz, -res.xyw);
   res.z = length;
   res.xy *= sqrt(length);
   
   return res.xyz * 2.0 + vec3(0.0, 0.0, -1.0);
}

void main(void) {
    //retreive position
    vec3 viewRay = vec3(viewPosition.xy / viewPosition.z, 1.0);   
    float depth = planes.y / (texelFetch2D(depthBuffer, ivec2(gl_FragCoord.xy), 0).x - planes.x);
    vec3 position = viewRay * depth;
   
    //convert normal back from [0,1] color space  
    //vec3 normal = texelFetch2D(normalBuffer, ivec2(gl_FragCoord.xy), 0).xyz * 2.0 - 1.0;    
    vec3 normal = decodeNormal(texelFetch2D(normalBuffer, ivec2(gl_FragCoord.xy), 0).xy);
      
    //light
    float lightNormDot;
    vec3 halfVec;   
   
#if(defined(POINT_LIGHT) || defined(SPOT_LIGHT))
    vec3 lightVector = lightPosition - position;
    float lightDistance = length(lightVector);   
    lightVector = normalize(lightVector);   
   
    lightNormDot = dot(lightVector, normal);
    halfVec = normalize(lightVector + normalize(viewRay));
#elif(defined(DIRECTIONAL_LIGHT))
    lightNormDot = dot(lightDirection, normal);
    halfVec = normalize(lightDirection + normalize(viewRay));
#endif
   
    vec3 diffuseContribution = max(0.0, lightNormDot) * texelFetch2D(diffuseBuffer, ivec2(gl_FragCoord.xy), 0).rgb * lightColor;
    vec4 specular = texelFetch2D(specularBuffer, ivec2(gl_FragCoord.xy), 0);
    vec3 specularContribution = pow(max(0.0, dot(halfVec, normal)), specular.a * 1000.0) * specular.rgb * lightColor * clamp(lightNormDot * 4.0, 0.0, 1.0);
   
#if(defined(POINT_LIGHT) || defined(SPOT_LIGHT))   
    float attenuation = clamp((attenuationEnd - lightDistance) / (attenuationEnd - attenuationStart), 0.0, 1.0);
   
#if(defined(SPOT_LIGHT))
    float coneThis = dot(-lightDirection, lightVector);   
    float spotAttenuation = clamp((coneThis - coneEnd) / (coneStart - coneEnd), 0.0, 1.0);
   
    attenuation *= spotAttenuation;
#endif
      
#elif(defined(DIRECTIONAL_LIGHT))
    finalColor = intensity * diffuseContribution;
#endif
            
    gl_FragData[0] = vec4(attenuation * intensity * diffuseContribution, 1.0);        
    gl_FragData[1] = vec4(attenuation * intensity * specularContribution, 1.0);
   
    //debug draw slight haze from light volume
    //gl_FragData[0] = vec4(clamp(attenuation * intensity * diffuseContribution, 0.05, 1.0), 1.0);
}