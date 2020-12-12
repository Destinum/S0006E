#include "core/app.h"

#include "MyMathLibrary/Matrix3DHeader.h"
#include "MyMathLibrary/Vector3DHeader.h"
#include "MyMathLibrary/Vector2DHeader.h"

#include "stb_image.h"

class TextureResource
{
public:
	GLuint TheTexture = 0;
	GLuint TextureID;

	void LoadTexture(const GLchar* filepath, GLuint program);
	void BindTexture();
};

class MeshResource
{
public:
	~MeshResource();

	GLuint VertexBuffer;
    GLuint ColorBuffer = 0;
	GLuint IndexBuffer;
	int NumberOfVertices;

	void Render();

	void Quad(float Scale);
	void Quad(float Scale, bool IsTextured);

	void Cube(float Scale);
	void Cube(float Scale, bool IsTextured);
};

class ShaderObject
{
public:
	GLuint program;

    GLuint VertexShader;
	GLuint FragmentShader;

	GLuint MatrixID;

	void Setup(const GLchar* PathToVertexShader, const GLchar* PathToFragmentShader);
};

class GraphicsNode
{
public:
    MeshResource Mesh;
	TextureResource Texture;
	ShaderObject Shaders;
	Matrix3D TransformationMatrix;
	GLuint TransformationID;

	void Draw(Matrix3D MVP);
	void AddTransform(Vector3D Transform);
};