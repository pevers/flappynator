#version 330
in vec3 inPosition;
in vec3 inNormal;
in vec2 inCoord;

uniform mat4 MVP;
uniform vec3 overrideColor;

out vec3 color;
out vec3 normal;
out vec2 texcoord;

void main() {
	color = overrideColor;
	if (inPosition.y <= 0.1)
		color = vec3(0.0, 0.4, 0.1);
	normal = inNormal;
	texcoord = inCoord;	
	gl_Position = MVP * vec4(inPosition, 1.0);
}

