#version 430 core

out vec4 FragColor;

in vec2 texCoords;
in vec3 normal;
in vec3 fragPos;

uniform vec3 ViewPos;

uniform struct MATERIAL
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
} Material;

uniform struct DIR_LIGHT
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} DirLight;

#define NUM_POINT_LIGHTS 4
uniform struct POINT_LIGHT
{
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} PointLight[NUM_POINT_LIGHTS];

uniform struct SPOT_LIGHT
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} SpotLight;

vec3 calcDirLight(DIR_LIGHT light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(POINT_LIGHT light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SPOT_LIGHT light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(ViewPos - fragPos);

    // Directional lighting
    vec3 result = calcDirLight(DirLight, norm, viewDir);

    // Point lights
    for (int i = 0; i < NUM_POINT_LIGHTS; ++i)
    {
        result += calcPointLight(PointLight[i], norm, fragPos, viewDir);
    }

    // Spot light
    result += calcSpotLight(SpotLight, norm, fragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DIR_LIGHT light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Material.shininess);

    // Combined
    vec3 ambient = light.ambient * vec3(texture(Material.diffuse, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(Material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(Material.specular, texCoords));

    return ambient + diffuse + specular;
}

vec3 calcPointLight(POINT_LIGHT light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combined
    vec3 ambient = light.ambient * vec3(texture(Material.diffuse, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(Material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(Material.specular, texCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 calcSpotLight(SPOT_LIGHT light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Material.shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Combined
    vec3 ambient = light.ambient * vec3(texture(Material.diffuse, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(Material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(Material.specular, texCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}
