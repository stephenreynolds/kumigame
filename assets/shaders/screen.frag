#version 430 core

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D ScreenTexture;

void main()
{
    FragColor = texture(ScreenTexture, texCoords);
}
