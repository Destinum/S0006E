#include "core/app.h"

#include "MyMathLibrary/Matrix3DHeader.h"
#include "MyMathLibrary/Vector3DHeader.h"
#include "MyMathLibrary/Vector2DHeader.h"

#include "stb_image.h"

class TextureResource
{
public:

	unsigned int TheTexture;

	unsigned int LoadTexture(char const* filepath);
	void BindTexture();
};

class MeshResource
{
public:
	//MeshResource();
	~MeshResource();

    Vector3D Position;
    Matrix3D Rotation;

	GLuint VertexBuffer;
    GLuint ColorBuffer = 0;
	GLuint IndexBuffer;

	TextureResource Texture;

	void Quad(float Scale);
	void Quad(float Scale, TextureResource Texture);
	void RenderQuad(GLuint Program);

	void Cube(float Scale);
	void Cube(float Scale, TextureResource Texture);
	void RenderCube(GLuint Program);
};