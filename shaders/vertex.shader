#version 150 core
in vec3 position;
in vec2 texCoord;
uniform mat4 MVP;
out vec2 texcoord;
void main() {
	texcoord = texCoord;
	gl_Position = MVP * vec4(position, 1.0);
}

