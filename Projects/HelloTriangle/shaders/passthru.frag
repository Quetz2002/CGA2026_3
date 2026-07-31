#version 460 core

out vec4 fragColor;

in vec4 color;


void main()
{
    //fragColor = vec4(0.0, 0.6f, 0.3f, 1); 
    fragColor = color;
}