#version 130
in vec3 position;
out vec3 TexCoords;

uniform mat4 MVP;

void main()
{
    vec4 pos = MVP * vec4(position, 1.0);  
    gl_Position = pos.xyww;
    TexCoords = position;
}  
