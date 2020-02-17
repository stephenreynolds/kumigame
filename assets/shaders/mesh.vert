#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBittangent;

out vec2 texCoords;
out vec3 normal;
out vec3 fragPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat3 Normal;

void main()
{
    gl_Position = Projection * View * Model * vec4(aPos, 1.0);
    normal = Normal * aNormal;
    fragPos = vec3(View * Model * vec4(aPos, 1.0));
    texCoords = aTexCoords;
}
