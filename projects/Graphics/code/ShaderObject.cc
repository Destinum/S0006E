#include "Resources.h"
#include <cstring>

#include <fstream>
#include <sstream> 

void ShaderObject::Setup(const GLchar* PathToVertexShader, const GLchar* PathToFragmentShader)
{
    //Setup Vertex Shader

    std::string ShaderCode;
    std::ifstream VertexShaderStream(PathToVertexShader, std::ios::in);
	if(VertexShaderStream.is_open())
    {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		ShaderCode = sstr.str();
		VertexShaderStream.close();
	}
    else
    {
		printf("Impossible to open %s.\n", PathToVertexShader);
		getchar();
		return;
	}

    printf("Compiling shader : %s\n", PathToVertexShader);

    this->VertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar * shader = ShaderCode.c_str();
    GLint length = static_cast<GLint>(std::strlen(shader));
    glShaderSource(this->VertexShader, 1, &shader, &length);
    glCompileShader(this->VertexShader);

    // get error log
    GLint shaderLogSize;
    glGetShaderiv(this->VertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
    if (shaderLogSize > 0)
    {
        GLchar* buf = new GLchar[shaderLogSize];
        glGetShaderInfoLog(this->VertexShader, shaderLogSize, NULL, buf);
        printf("[SHADER COMPILE ERROR]: %s", buf);
        delete[] buf;
    }

    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    //Setup Fragment Shader

    //ShaderCode;
    std::ifstream FragmentShaderStream(PathToFragmentShader, std::ios::in);
	if(FragmentShaderStream.is_open())
    {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		ShaderCode = sstr.str();
		FragmentShaderStream.close();
	}
    else
    {
		printf("Impossible to open %s.\n", PathToFragmentShader);
		getchar();
		return;
	}

    printf("Compiling shader : %s\n", PathToFragmentShader);

    this->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	shader = ShaderCode.c_str();
    length = static_cast<GLint>(std::strlen(shader));
    glShaderSource(this->FragmentShader, 1, &shader, &length);
    glCompileShader(this->FragmentShader);

    // get error log
    //shaderLogSize;
    glGetShaderiv(this->FragmentShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
    if (shaderLogSize > 0)
    {
        GLchar* buf = new GLchar[shaderLogSize];
        glGetShaderInfoLog(this->FragmentShader, shaderLogSize, NULL, buf);
        printf("[SHADER COMPILE ERROR]: %s", buf);
        delete[] buf;
    }

    //------------------------------------------------------------------------------
    //------------------------------------------------------------------------------
    // create a program object

    this->program = glCreateProgram();
    glAttachShader(this->program, this->VertexShader);
    glAttachShader(this->program, this->FragmentShader);
    glLinkProgram(this->program);
    glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &shaderLogSize);
    if (shaderLogSize > 0)
    {
        GLchar* buf = new GLchar[shaderLogSize];
        glGetProgramInfoLog(this->program, shaderLogSize, NULL, buf);
        printf("[PROGRAM LINK ERROR]: %s", buf);
        delete[] buf;
    }

    this->MatrixID = glGetUniformLocation(this->program, "MVP");
    this->CameraID = glGetUniformLocation(this->program, "CameraPosition");
}