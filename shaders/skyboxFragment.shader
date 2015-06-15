#version 130
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;
uniform float darken;

void main()
{
    vec4 s1 = texture(skybox, TexCoords - 1.0 / 1024.0 - 1.0 / 1024.0);
    vec4 s2 = texture(skybox, TexCoords + 1.0 / 1024.0 - 1.0 / 1024.0);
    vec4 s3 = texture(skybox, TexCoords - 1.0 / 1024.0 + 1.0 / 1024.0);
    vec4 s4 = texture(skybox, TexCoords + 1.0 / 1024.0 + 1.0 / 1024.0);
    vec4 sx = 4.0 * ((s4 + s3) - (s2 + s1));
    vec4 sy = 4.0 * ((s2 + s4) - (s1 + s3));
    vec4 sobel = sqrt(sx * sx + sy * sy);
    
    //color = texture(skybox, TexCoords);
    color = mix(texture(skybox, TexCoords), sobel, darken);
}
