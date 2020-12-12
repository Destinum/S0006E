#version 430

out vec4 color;

in vec4 Color;

uniform sampler2D Texture;

void main()
{
	color = Color;
};
