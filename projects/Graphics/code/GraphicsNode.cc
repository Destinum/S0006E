#include "Resources.h"

void GraphicsNode::Draw(Matrix3D MVP, Vector3D CameraPosition)
{
    glUseProgram(this->Shaders.program);
    glUniformMatrix4fv(this->Shaders.MatrixID, 1, GL_FALSE, &(MVP).matris[0][0]);
    glUniform4f(this->Shaders.CameraID, CameraPosition.vektor[0], CameraPosition.vektor[1], CameraPosition.vektor[2], 1.0);
    glUniformMatrix4fv(this->TransformationID, 1, GL_FALSE, &(this->TransformationMatrix).matris[0][0]);

    if (this->Texture.TheTexture != 0)
        this->Texture.BindTexture();

    this->Mesh.Render();
}

void GraphicsNode::AddTransform(Vector3D Transform)
{
    this->TransformationMatrix.matris[3][0] += Transform.vektor[0];
    this->TransformationMatrix.matris[3][1] += Transform.vektor[1];
    this->TransformationMatrix.matris[3][2] += Transform.vektor[2];
}