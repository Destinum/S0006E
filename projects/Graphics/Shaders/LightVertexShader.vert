#version 430

layout(location=0) in vec4 pos;
layout(location=1) in vec2 InTexture;
layout(location=2) in vec4 Normals;

uniform mat4 MVP;
uniform mat4 TransformationMatrix;

out vec4 VertexPosition;
out vec2 TextureCoordinates;
out vec4 TheNormals;

void main()
{
    gl_Position = MVP * TransformationMatrix * pos;
    VertexPosition = TransformationMatrix * pos;
	TextureCoordinates = InTexture;
    TheNormals = Normals;
};
