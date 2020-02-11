#version 430 core

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D texture_diffuse1;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    vec3 test;
    FragColor = vec4(lightColor * objectColor, 1.0f) * texture(texture_diffuse1, texCoords);
}
