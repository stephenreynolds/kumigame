#version 430 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D text;
uniform vec4 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texCoords).r);
    fragColor = textColor * sampled;
}
