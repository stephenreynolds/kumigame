#version 430 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D Text;
uniform vec4 TextColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(Text, texCoords).r);
    fragColor = TextColor * sampled;
}
