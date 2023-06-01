#version 330 core

layout(location = 0) out vec4 color;

in float pixColor;

void main()
{
	color = vec4(pixColor, pixColor, pixColor, pixColor);
};
