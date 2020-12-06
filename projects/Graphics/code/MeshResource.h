#include "core/app.h"

#include "MyMathLibrary/Matrix3DHeader.h"
#include "MyMathLibrary/Vector3DHeader.h"
#include "MyMathLibrary/Vector2DHeader.h"

class MeshResource
{
public:
	//MeshResource();
	//~MeshResource();

    Vector3D Position;
    Matrix3D Rotation;

	GLuint VertexBuffer;
    GLuint ColorBuffer;
	GLuint IndexBuffer;
};