#version 430 core

out vec4 FragColor;

in vec2 texCoords;
in vec3 normal;
in vec3 fragPos;
in vec3 lightPos;

uniform sampler2D Texture_diffuse1;

uniform vec3 LightColor;
uniform vec3 ViewPos;

void main()
{
    // Object color
    vec4 objectColor = texture(Texture_diffuse1, texCoords);

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * LightColor;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * LightColor;

    // Result
    vec4 result = vec4(ambient + diffuse + specular, 1.0f) * objectColor;

    FragColor = result;
}
