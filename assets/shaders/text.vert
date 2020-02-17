#version 430 core

layout (location = 0) in vec4 vertex;

out vec2 texCoords;

uniform mat4 Projection;

void main()
{
    gl_Position = Projection * vec4(vertex.xy, 0.0, 1.0);
    texCoords = vertex.zw;
}
