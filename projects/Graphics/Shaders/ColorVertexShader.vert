#version 430

layout(location=0) in vec4 pos;
layout(location=1) in vec4 color;
layout(location=2) in vec2 InTexture;

uniform mat4 MVP;
uniform mat4 TransformationMatrix;

out vec4 Color;

void main()
{
    gl_Position = MVP * TransformationMatrix * pos;
	Color = color;
};
