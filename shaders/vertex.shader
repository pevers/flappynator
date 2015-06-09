#version 330
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 inNormal;

uniform mat4 MVP;
uniform vec3 overrideColor;
uniform mat4 depthBiasMVP;

out vec3 color;
out vec3 normal;
out vec2 UV;
out vec4 shadowCoord;

void main() {
	color = overrideColor;
	if (inPosition.y <= 1.0)
		//color = vec3(0.255, 0.412, 0.882);	// see blue
		color = vec3(0.333, 0.42, 0.184);
	else
		color = vec3(0.6, 0.6, 0.6);
	normal = inNormal;	
	gl_Position = MVP * vec4(inPosition, 1.0);

	shadowCoord = depthBiasMVP * vec4(inPosition, 1.0);
	UV = vertexUV;
}

