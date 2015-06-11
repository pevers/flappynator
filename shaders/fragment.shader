#version 130
in vec3 color;
in vec2 UV;
in vec3 normal;
in vec4 shadowCoord;
in vec3 cubeMapCoords;

out vec4 outColor;

struct SimpleDirectionalLight 
{ 
	vec3 color; 
	vec3 pos;
	vec3 direction; 
	float ambientIntensity; 
}; 

uniform SimpleDirectionalLight sunLight;
uniform sampler2DShadow shadowMap;
uniform samplerCube skybox;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

void main() {
	float diffuseIntensity = max(0.0, dot(normalize(normal), -sunLight.direction));

	// temp old code
	//float cosTheta = clamp(dot(normal, sunLight.direction), 0, 1);
	//float bias = 0.005*tan(acos(cosTheta));
	//bias = clamp(bias, 0,0.01);
	//float visibility = texture(shadowMap, vec3(shadowCoord.xy, (shadowCoord.z - bias)/shadowCoord.w));

	float visibility = 1.0;
	float bias = 0.005;
	for (int i=0; i<3; i++){
		int index = i;

		// being fully in the shadow will eat up 4*0.2 = 0.8
		// 0.2 potentially remain, which is quite dark.
		visibility -= 0.20*(1.0-texture(shadowMap, vec3(shadowCoord.xy + poissonDisk[index] / 900.0,  (shadowCoord.z - bias) / shadowCoord.w)));
	}

	outColor = visibility * vec4(color, 1.0) * vec4(sunLight.color * (diffuseIntensity + sunLight.ambientIntensity), 1.0); //vec4(sunLight.color*(sunLight.ambientIntensity+diffuseIntensity+specularIntensity), 1.0);
}


