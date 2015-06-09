#version 330
in vec3 color;
in vec2 UV;
in vec3 normal;
in vec4 shadowCoord;

out vec4 outColor;

struct SimpleDirectionalLight 
{ 
   vec3 color; 
   vec3 direction; 
   float ambientIntensity; 
}; 

uniform SimpleDirectionalLight sunLight;
uniform sampler2DShadow shadowMap;

void main() {
	float diffuseIntensity = max(0.0, dot(normalize(normal), -sunLight.direction));
	float bias = 0.005;
	float visibility = texture(shadowMap, vec3(shadowCoord.xy, (shadowCoord.z - bias)/shadowCoord.w));
	
	outColor = visibility * vec4(color, 1.0) * vec4(sunLight.color*(sunLight.ambientIntensity+diffuseIntensity), 1.0);
}

