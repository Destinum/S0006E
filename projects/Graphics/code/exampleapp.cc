//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>

const GLchar* vs =
"#version 430\n"
"layout(location=0) in vec4 pos;\n"
"layout(location=1) in vec4 color;\n"


"uniform vec4 ObjectPosition;\n"
"uniform mat4 ObjectRotation;\n"

"out vec4 Color;\n"

"void main()\n"
"{\n"
"	gl_Position = ObjectRotation * pos + vec4(ObjectPosition.xyz, 0);\n"
"	Color = color;\n"
"}\n";

const GLchar* ps =
"#version 430\n"
"out vec4 color;\n"

"in vec4 Color;\n"

"void main()\n"
"{\n"
"	color = Color;\n"
"}\n";

using namespace Display;
namespace Example
{

//------------------------------------------------------------------------------
/**
*/
ExampleApp::ExampleApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ExampleApp::~ExampleApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ExampleApp::Open()
{
	App::Open();
	this->window = new Display::Window;
	window->SetKeyPressFunction([this](int32, int32, int32, int32)
	{
		this->window->Close();
	});


	GLfloat buf[] =
	{
		-0.5,	0.5,	-1, 	1,	// Top Left
		0.5,	0.5,	-1, 	1,	// Top Right
		-0.5,	-0.5,	-1, 	1,	// Bottom Left
		0.5,	-0.5,	-1, 	1	// Bottom Right
	};

	GLfloat bufColor[] = 
	{
		0,		1,		0,		1,	// Top Left
		0,		0,		1,		1,	// Top Right
		0,		0,		1,		1,	// Bottom Left
		0,		1,		0,		1	// Bottom Right
	};

	unsigned int bufIndices[] = 
	{
		0, 1, 2,		//Triangle 1
		3, 1, 2			//Triangle 2
	};

	if (this->window->Open())
	{
		// set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// setup vertex shader
		this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLint length = static_cast<GLint>(std::strlen(vs));
		glShaderSource(this->vertexShader, 1, &vs, &length);
		glCompileShader(this->vertexShader);

		// get error log
		GLint shaderLogSize;
		glGetShaderiv(this->vertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(this->vertexShader, shaderLogSize, NULL, buf);
			printf("[SHADER COMPILE ERROR]: %s", buf);
			delete[] buf;
		}

		// setup pixel shader
		this->pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		length = static_cast<GLint>(std::strlen(ps));
		glShaderSource(this->pixelShader, 1, &ps, &length);
		glCompileShader(this->pixelShader);

		// get error log
		shaderLogSize;
		glGetShaderiv(this->pixelShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(this->pixelShader, shaderLogSize, NULL, buf);
			printf("[SHADER COMPILE ERROR]: %s", buf);
			delete[] buf;
		}

		// create a program object
		this->program = glCreateProgram();
		glAttachShader(this->program, this->vertexShader);
		glAttachShader(this->program, this->pixelShader);
		glLinkProgram(this->program);
		glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetProgramInfoLog(this->program, shaderLogSize, NULL, buf);
			printf("[PROGRAM LINK ERROR]: %s", buf);
			delete[] buf;
		}

		// setup vbo
		glGenBuffers(1, &this->Quad.VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, this->Quad.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);

		glGenBuffers(1, &this->Quad.ColorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, this->Quad.ColorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bufColor), bufColor, GL_STATIC_DRAW);

		glGenBuffers(1, &this->Quad.IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Quad.IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bufIndices), bufIndices, GL_STATIC_DRAW);


		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::Run()
{
	float Movement = 0.001;

	while (this->window->IsOpen())
	{
		glUseProgram(this->program);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->window->Update();

		// do stuff

		if (this->Quad.Position.vektor[0] <= -0.5 || this->Quad.Position.vektor[0] >= 0.5)
			Movement *= -1;

		this->Quad.Position = this->Quad.Position + Vector3D(Movement, 0, 0, 1);
		this->Quad.Rotation = this->Quad.Rotation.vectorRotation(0.1, Vector3D(0, 0, 1, 0));

		glUniform4f(glGetUniformLocation(this->program, "ObjectPosition"), this->Quad.Position.vektor[0], this->Quad.Position.vektor[1], this->Quad.Position.vektor[2], this->Quad.Position.vektor[3]);
		glUniformMatrix4fv(glGetUniformLocation(this->program, "ObjectRotation"), 1, GL_FALSE, &(this->Quad.Rotation).matris[0][0]);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, this->Quad.VertexBuffer);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, this->Quad.ColorBuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->Quad.IndexBuffer);
		glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, NULL);
		//glDrawArrays(GL_TRIANGLES, 0, 2*3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		this->window->SwapBuffers();
	}
}

} // namespace Example