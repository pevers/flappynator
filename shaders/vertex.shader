#version 330
in vec3 inPosition;
in vec2 inCoord;
in vec3 inNormal;

uniform mat4 MVP;
uniform vec3 overrideColor;

out vec2 texCoord;
out vec3 color;
out vec3 normal;

void main() {
	color = overrideColor;
	texCoord = inCoord;
	normal = inNormal;
	gl_Position = MVP * vec4(inPosition, 1.0);
}

