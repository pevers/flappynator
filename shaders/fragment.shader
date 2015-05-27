#version 150 core
out vec4 outColor;
in vec2 texcoord;
uniform sampler2D tex;
void main() {
	outColor = texture(tex, texcoord);
}

