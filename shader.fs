#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    // Sample the texture color and output it
    FragColor = texture(texture1, TexCoords);
}
