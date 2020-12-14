#include "Resources.h"

void LightSource::Update(std::vector<GraphicsNode>* GraphicNodes)
{
    for (int i = 0; i < GraphicNodes->size(); i++)
    {
        glUseProgram(GraphicNodes->at(i).Shaders.program);
        glUniform4f(glGetUniformLocation(GraphicNodes->at(i).Shaders.program, "LightSource.Position"), this->Position.vektor[0], this->Position.vektor[1], this->Position.vektor[2], 1.0);
        glUniform4f(glGetUniformLocation(GraphicNodes->at(i).Shaders.program, "LightSource.Ambient"), this->Ambient.vektor[0], this->Ambient.vektor[1], this->Ambient.vektor[2], 1.0);
        glUniform4f(glGetUniformLocation(GraphicNodes->at(i).Shaders.program, "LightSource.Diffuse"), this->Diffuse.vektor[0], this->Diffuse.vektor[1], this->Diffuse.vektor[2], 1.0);
        glUniform4f(glGetUniformLocation(GraphicNodes->at(i).Shaders.program, "LightSource.Specular"), this->Specular.vektor[0], this->Specular.vektor[1], this->Specular.vektor[2], 1.0);
        glUniform1f(glGetUniformLocation(GraphicNodes->at(i).Shaders.program, "LightSource.Intensity"), this->Intensity);
    }
}

void LightSource::AddPosition(GLfloat x, GLfloat y, GLfloat z)
{
    this->Position.vektor[0] += x;
    this->Position.vektor[1] += y;
    this->Position.vektor[2] += z;
}