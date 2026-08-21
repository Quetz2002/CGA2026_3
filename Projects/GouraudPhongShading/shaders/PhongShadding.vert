#version 460 core

out vec4 fragColor;
in vec4 GouraudColor;

void main()
{
    fragColor = GouraudColor;
}
