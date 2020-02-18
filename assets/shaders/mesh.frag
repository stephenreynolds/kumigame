#version 430 core

out vec4 FragColor;

in vec2 texCoords;
in vec3 normal;
in vec3 fragPos;

uniform struct MATERIAL
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emissive;
    float shininess;
} Material;

uniform struct LIGHT
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} Light;

void main()
{
    // Ambient
    vec3 ambient = Light.ambient * texture(Material.diffuse, texCoords).rgb;

    // Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(Light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = Light.diffuse * diff * texture(Material.diffuse, texCoords).rgb;

    // Specular
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Material.shininess);
    vec3 specular = Light.specular * spec * texture(Material.specular, texCoords).rgb;

    // Result
    vec4 result = vec4(ambient + diffuse + specular, 1.0f);

    FragColor = result;
}
