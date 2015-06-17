#version 330
in vec3 inPosition;
in vec2 vertexUV;
in vec3 inNormal;

uniform mat4 model, view, proj;
uniform vec3 overrideColor;
uniform mat4 depthBiasMVP;
uniform bool isTerrain;

out vec3 color;
out vec3 normal;
out vec2 UV;
out vec3 v;
out vec3 N;
out vec3 cubeMapCoords;
out vec4 shadowCoord;

void main() {
	color = overrideColor;
	if (isTerrain) {
		if (inPosition.y <= 1.0)
			//color = vec3(0.255, 0.412, 0.882);	// sea blue
			color = vec3(0.333, 0.42, 0.184);
		else
			color = vec3(0.8, 0.8, 0.8);
	}

	normal = inNormal;

	mat4 normalMatrix = transpose(inverse(view * model));
	N = vec3(normalize(normalMatrix * vec4(inNormal, 0.0)));
	v = vec3(view * model * vec4(inPosition, 1.0));

	shadowCoord = depthBiasMVP * vec4(inPosition, 1.0);
	UV = vertexUV;
	gl_Position = proj * view * model * vec4(inPosition, 1.0);
}

