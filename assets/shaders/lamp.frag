#version 430 core

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D Texture_diffuse1;

void main()
{
    FragColor = texture(Texture_diffuse1, texCoords);
}
