#version 330
in vec3 inPosition;
in vec3 inNormal;

uniform mat4 MVP;
uniform vec3 overrideColor;

out vec3 color;
out vec3 normal;

void main() {
	color = overrideColor;
	normal = inNormal;
	gl_Position = MVP * vec4(inPosition, 1.0);
}

