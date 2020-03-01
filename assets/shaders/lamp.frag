#version 430 core

out vec4 FragColor;

in vec2 texCoords;

uniform struct MATERIAL
{
    sampler2D diffuse;
} Material;

void main()
{
    FragColor = texture(Material.diffuse, texCoords);
}
