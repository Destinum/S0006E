#version 430

out vec4 color;

in vec2 TextureCoordinates;

uniform sampler2D Texture;

void main()
{
	color = texture(Texture, TextureCoordinates);
};
