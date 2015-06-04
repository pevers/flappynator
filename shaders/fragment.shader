#version 330
in vec3 color;
in vec3 normal;

out vec4 outColor;

struct SimpleDirectionalLight 
{ 
   vec3 color; 
   vec3 direction; 
   float ambientIntensity; 
}; 

uniform SimpleDirectionalLight sunLight;

void main() {
	float diffuseIntensity = max(0.0, dot(normalize(normal), -sunLight.direction));
   	outColor = vec4(color, 1.0) * vec4(sunLight.color*(sunLight.ambientIntensity+diffuseIntensity), 1.0);
}

