#version 430 core

out vec4 FragColor;

in vec2 texCoords;

const int OFF = 0;
const int BLUR = 1;
const int EDGE = 2;
const int GREYSCALE = 3;
const int INVERT = 4;
const int SHARPEN = 5;

uniform sampler2D ScreenTexture;
uniform struct sPostProcess {
    int operation;
    float kernel[9];
    vec2 offsets[9];
} PostProcess;

void main()
{
    vec3 color = vec3(0.0);

    if (PostProcess.operation == INVERT)
    {
        color = vec3(1.0 - texture(ScreenTexture, texCoords));
    }
    else if (PostProcess.operation != OFF)
    {
        for (int i = 0; i < 9; ++i)
        {
            color += vec3(texture(ScreenTexture, texCoords.st + PostProcess.offsets[i])) * PostProcess.kernel[i];
        }
    }
    else
    {
        color = vec3(texture(ScreenTexture, texCoords));
    }

    FragColor = vec4(color, 1.0);
}
