#include "core/app.h"

#include "MyMathLibrary/Matrix3DHeader.h"
#include "MyMathLibrary/Vector3DHeader.h"
#include "MyMathLibrary/Vector2DHeader.h"

#include "stb_image.h"
#include <vector>

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
	GLuint UVBuffer;
	GLuint NormalBuffer = 0;
    GLuint ColorBuffer = 0;
	GLuint IndexBuffer;
	int NumberOfVertices;

	void Render();

	bool loadOBJ(const GLchar * path);

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
	GLuint CameraID;

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

	void Draw(Matrix3D MVP, Vector3D CameraPosition);
	void AddTransform(Vector3D Transform);
};

class LightSource
{
public:
    Vector3D Position = Vector3D(-2.0, 4.0, 0.0, 1.0);
	Vector3D Ambient = Vector3D(0.1, 0.1, 0.1, 1.0);
	Vector3D Diffuse = Vector3D(1.0, 1.0, 1.0, 1.0);
	Vector3D Specular = Vector3D(10.0, 10.0, 10.0, 1.0);
	GLfloat Intensity = 1.0;

	void Update(std::vector<GraphicsNode>* GraphicNodes);
	void AddPosition(GLfloat x, GLfloat y, GLfloat z);
};