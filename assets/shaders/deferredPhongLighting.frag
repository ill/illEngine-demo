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

uniform bool noLighting;

uniform sampler2D depthBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D diffuseBuffer;

#ifdef SPECULAR
uniform sampler2D specularBuffer;
#endif

uniform vec2 planes;

uniform float intensity;
uniform vec3 lightColor;

#ifdef VOLUME_LIGHT
#define MAX_PLANES 12
uniform vec4 attenuationPlanes[MAX_PLANES];
uniform float attenuationStarts[MAX_PLANES];	//these are the reciprocal so the shader can multiply instead of divide
#else
uniform float attenuationStart;
uniform float attenuationEnd;
#endif

#if(defined(POINT_LIGHT) || defined(SPOT_LIGHT))
uniform vec3 lightPosition;			//light position in eye space not world space
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
	//noLighting for the stencil volume prepass
	if(noLighting) {
		gl_FragData[0] = vec4(1.0);
	}
	else {
		//retreive position
		vec3 viewRay = vec3(viewPosition.xy / viewPosition.z, 1.0);   
		float depth = planes.y / (texelFetch2D(depthBuffer, ivec2(gl_FragCoord.xy), 0).x - planes.x);
		vec3 position = viewRay * depth;
	   
		//convert normal back from [0,1] color space  
		//vec3 normal = texelFetch2D(normalBuffer, ivec2(gl_FragCoord.xy), 0).xyz * 2.0 - 1.0;    
		vec3 normal = decodeNormal(texelFetch2D(normalBuffer, ivec2(gl_FragCoord.xy), 0).xy);
		  
		//light
		float lightNormDot;
		float attenuation;
		
#ifdef SPECULAR
		vec3 halfVec;
#endif		
   
#if(defined(POINT_LIGHT) || defined(SPOT_LIGHT))
		vec3 lightVector = lightPosition - position;
		float lightDistance = length(lightVector);   
		lightVector = normalize(lightVector);   
	   
		lightNormDot = dot(lightVector, normal);
		
#ifdef SPECULAR
		halfVec = normalize(lightVector + normalize(viewRay));
#endif
		
#elif(defined(DIRECTIONAL_LIGHT))
		lightNormDot = dot(-lightDirection, normal);
		
#ifdef SPECULAR
		halfVec = normalize(normalize(viewRay) - lightDirection);
#endif
		
#endif
   
		vec3 diffuseContribution = max(0.0, lightNormDot) * texelFetch2D(diffuseBuffer, ivec2(gl_FragCoord.xy), 0).rgb * lightColor;
		
#ifdef SPECULAR
		vec4 specular = texelFetch2D(specularBuffer, ivec2(gl_FragCoord.xy), 0);
		vec3 specularContribution = pow(max(0.0, dot(halfVec, normal)), specular.a * 1000.0) * specular.rgb * lightColor * clamp(lightNormDot * 4.0, 0.0, 1.0);
#endif

#ifdef VOLUME_LIGHT		
		//attenuation = clamp((dot(attenuationPlanes[0].xyz, position) + attenuationPlanes[0].w) * attenuationStarts[0], 0.0, 1.0);		
		/*attenuation = clamp((dot(attenuationPlanes[0].xyz, position) + attenuationPlanes[0].w) * attenuationStarts[0], 1.0, 1.0);
				
		if((dot(attenuationPlanes[0].xyz, position) + attenuationPlanes[0].w) < 0.0) {
			diffuseContribution = vec3(1.0, 0.0, 1.0);
		}
		
		if((dot(attenuationPlanes[1].xyz, position) + attenuationPlanes[1].w) < 0.0) {
			diffuseContribution = vec3(0.0, 1.0, 0.0);
		}
		
		if((dot(attenuationPlanes[2].xyz, position) + attenuationPlanes[2].w) < 0.0) {
			diffuseContribution = vec3(1.0, 0.0, 1.0);
		}
		
		if((dot(attenuationPlanes[3].xyz, position) + attenuationPlanes[3].w) < 0.0) {
			diffuseContribution = vec3(0.0, 1.0, 0.0);
		}
		
		if((dot(attenuationPlanes[4].xyz, position) + attenuationPlanes[4].w) < 0.0) {
			diffuseContribution = vec3(0.0, 1.0, 0.0);
		}
		
		if((dot(attenuationPlanes[5].xyz, position) + attenuationPlanes[5].w) < 0.0) {
			diffuseContribution = vec3(1.0, 0.0, 1.0);
		}*/
				
		//I'm really REALLY making sure the loop is unrolled...  Now let's hope MAX_PLANES never changes
		attenuation = min(min(min(min(min(min(min(min(min(min(min(
			clamp((dot(attenuationPlanes[0].xyz, position) + attenuationPlanes[0].w) * attenuationStarts[0], 0.0, 1.0),
			clamp((dot(attenuationPlanes[1].xyz, position) + attenuationPlanes[1].w) * attenuationStarts[1], 0.0, 1.0)),
			clamp((dot(attenuationPlanes[2].xyz, position) + attenuationPlanes[2].w) * attenuationStarts[2], 0.0, 1.0)),
			clamp((dot(attenuationPlanes[3].xyz, position) + attenuationPlanes[3].w) * attenuationStarts[3], 0.0, 1.0)),
			clamp((dot(attenuationPlanes[4].xyz, position) + attenuationPlanes[4].w) * attenuationStarts[4], 0.0, 1.0)),
			clamp((dot(attenuationPlanes[5].xyz, position) + attenuationPlanes[5].w) * attenuationStarts[5], 0.0, 1.0)),
			clamp((dot(attenuationPlanes[6].xyz, position) + attenuationPlanes[6].w) * attenuationStarts[6], 0.0, 1.0)),
			clamp((dot(attenuationPlanes[7].xyz, position) + attenuationPlanes[7].w) * attenuationStarts[7], 0.0, 1.0)),
			clamp((dot(attenuationPlanes[8].xyz, position) + attenuationPlanes[8].w) * attenuationStarts[8], 0.0, 1.0)),
			clamp((dot(attenuationPlanes[9].xyz, position) + attenuationPlanes[9].w) * attenuationStarts[9], 0.0, 1.0)),
			clamp((dot(attenuationPlanes[10].xyz, position) + attenuationPlanes[10].w) * attenuationStarts[10], 0.0, 1.0)),
			clamp((dot(attenuationPlanes[11].xyz, position) + attenuationPlanes[11].w) * attenuationStarts[11], 0.0, 1.0));
#else
		attenuation = clamp((attenuationEnd - lightDistance) / (attenuationEnd - attenuationStart), 0.0, 1.0);
#endif
   
#if(defined(SPOT_LIGHT))
		float coneThis = dot(-lightDirection, lightVector);   
		float spotAttenuation = clamp((coneThis - coneEnd) / (coneStart - coneEnd), 0.0, 1.0);
	   
		attenuation *= spotAttenuation;
#endif
            
		gl_FragData[0] = vec4(attenuation * intensity * diffuseContribution, 1.0);
		
#ifdef SPECULAR
		gl_FragData[1] = vec4(attenuation * intensity * specularContribution, 1.0);
#else
		gl_FragData[1] = vec4(0.0);
#endif
	   
		//debug draw slight haze from light volume
		//gl_FragData[0] = vec4(clamp(attenuation * intensity * diffuseContribution, 0.05, 1.0), 1.0);
	}
}
