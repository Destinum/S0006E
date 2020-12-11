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
"layout(location=2) in vec2 InTexture;\n"

"uniform mat4 MVP;\n"
"uniform vec4 ObjectPosition;\n"
"uniform mat4 ObjectRotation;\n"

//"out vec4 Color;\n"
"out vec2 TextureCoordinates;\n"

"void main()\n"
"{\n"
"	gl_Position = MVP * (ObjectRotation * pos + vec4(ObjectPosition.xyz, 0));\n"
//"	Color = color;\n"
"	TextureCoordinates = InTexture;\n"
"}\n";

const GLchar* ps =
"#version 430\n"
"out vec4 color;\n"

//"in vec4 Color;\n"
"in vec2 TextureCoordinates;\n"

"uniform sampler2D Texture;\n"

"void main()\n"
"{\n"
//"	color = Color;\n"
"	color = texture(Texture, TextureCoordinates);\n"
"}\n";

using namespace Display;
namespace Example
{

void ExampleApp::computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double theLastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double theCurrentTime = glfwGetTime();
	float theDeltaTime = float(theCurrentTime - theLastTime);

	//cout << theDeltaTime << endl;

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(this->window->window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(this->window->window, 1024/2, 768/2);

	// Compute new orientation
	if (glfwGetMouseButton(this->window->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		horizontalAngle += mouseSpeed * float(1024/2 - xpos );
		verticalAngle   += mouseSpeed * float( 768/2 - ypos );
	}

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	Vector3D direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle),
		0
	);
	
	// Right vector
	Vector3D right(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f),
		0
	);
	
	// Up vector
	Vector3D up = right.CrossProduct(direction);

	// Move forward
	if (glfwGetKey( window->window, GLFW_KEY_UP ) == GLFW_PRESS || glfwGetKey( window->window, GLFW_KEY_W ) == GLFW_PRESS){
		position = position + direction * theDeltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window->window, GLFW_KEY_DOWN ) == GLFW_PRESS || glfwGetKey( window->window, GLFW_KEY_S ) == GLFW_PRESS){
		position = position - direction * theDeltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window->window, GLFW_KEY_RIGHT ) == GLFW_PRESS || glfwGetKey( window->window, GLFW_KEY_D ) == GLFW_PRESS){
		position = position + right * theDeltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window->window, GLFW_KEY_LEFT ) == GLFW_PRESS || glfwGetKey( window->window, GLFW_KEY_A ) == GLFW_PRESS){
		position = position - right * theDeltaTime * speed;
	}


	this->Projection = Projection.ProjectionMatrix(initialFoV, 4.0f/3.0f, 0.1f, 100.0f);
	this->View = View.ViewMatrix(position, position + direction, up);
	this->MVP = Projection * View;

	glUniformMatrix4fv(this->MatrixID, 1, GL_FALSE, &(this->MVP).matris[0][0]);

	// For the next frame, the "last time" will be "now"
	theLastTime = theCurrentTime;
}


bool ExampleApp::Open()
{
	App::Open();
	this->window = new Display::Window;

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


		glUniform1i(glGetUniformLocation(this->program, "Texture"), 0);

		// setup camera
		this->MatrixID = glGetUniformLocation(this->program, "MVP");

		// setup renderable objects
		TextureResource aTexture;
		aTexture.LoadTexture("../4Texture.png");
		this->Object.Quad(1.0, aTexture);
		//this->Object.Cube(1.0, aTexture);
		//this->Object.Cube(1.0);
		//this->Object2.Cube(2.0);

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
	glUseProgram(this->program);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	float Movement = 0.001;

	//this->Object.Position = this->Object.Position + Vector3D(3, 0, 0, 1);

	while (this->window->IsOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->window->Update();

		// do stuff

		computeMatricesFromInputs();

		/*
		if (this->Object.Position.vektor[0] <= -0.5 || this->Object.Position.vektor[0] >= 0.5)
			Movement *= -1;

		this->Object.Position = this->Object.Position + Vector3D(Movement, 0, 0, 1);
		this->Object.Rotation = this->Object.Rotation.vectorRotation(0.1, Vector3D(1, 1, 1, 0));
		*/

		this->Object.RenderQuad(this->program);
		//this->Object2.RenderCube(this->program);
		//this->Object2.RenderQuad(this->program);

		this->window->SwapBuffers();
	}
}

} // namespace Example