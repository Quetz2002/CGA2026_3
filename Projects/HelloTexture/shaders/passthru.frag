#version 460 core

out vec4 fragColor;
out vec2 vTexCoords;
uniform vec4 color;
uniform sampler2D texture0;

void main()
{
    //fragColor = color;
    fragColor = texture(texture0, vTexCoords);
}