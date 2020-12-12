//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>

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

	// For the next frame, the "last time" will be "now"
	theLastTime = theCurrentTime;
}


bool ExampleApp::Open()
{
	App::Open();
	this->window = new Display::Window;

	if (this->window->Open())
	{
		// Set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// Set filepath of shaders and texture
		const GLchar* VertexShader = "../projects/Graphics/Shaders/VertexShader.vert";
		const GLchar* FragmentShader = "../projects/Graphics/Shaders/FragmentShader.frag";
		const GLchar* Texture = "../projects/Graphics/Textures/DiceTexture.jpg";

		//Setup GraphicNode with Shaders, Texture and Mesh
		this->GraphicNodes.push_back(GraphicsNode());
		this->GraphicNodes.push_back(GraphicsNode());

		this->GraphicNodes[0].Shaders.Setup(VertexShader, FragmentShader);
		this->GraphicNodes[0].Texture.LoadTexture(Texture, this->GraphicNodes[0].Shaders.program);
		this->GraphicNodes[0].Mesh.Cube(1.0, true);
		this->GraphicNodes[0].TransformationID = glGetUniformLocation(this->GraphicNodes[0].Shaders.program, "TransformationMatrix");

		VertexShader = "../projects/Graphics/Shaders/ColorVertexShader.vert";
		FragmentShader = "../projects/Graphics/Shaders/ColorFragmentShader.frag";
		//Texture = "../projects/Graphics/Textures/4Texture.png";

		this->GraphicNodes[1].Shaders.Setup(VertexShader, FragmentShader);
		//this->GraphicNodes[1].Texture.LoadTexture(Texture, this->GraphicNodes[1].Shaders.program);
		this->GraphicNodes[1].Mesh.Quad(1.0, true);
		this->GraphicNodes[1].Mesh.Quad(1.0);
		this->GraphicNodes[1].TransformationID = glGetUniformLocation(this->GraphicNodes[1].Shaders.program, "TransformationMatrix");
		this->GraphicNodes[1].AddTransform(Vector3D(3, 0, 0, 1));
			

		// setup texture
		//glUniform1i(glGetUniformLocation(this->program, "Texture"), 0);

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
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	float Movement = 0.001;

	while (this->window->IsOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->window->Update();

		// Update camera based on keyboard and mouse imputs
		computeMatricesFromInputs();

		/*
		if (this->GraphicNodes[0].TransformationMatrix.matris[3][0] <= -0.5 || this->GraphicNodes[0].TransformationMatrix.matris[3][0] >= 0.5)
			Movement *= -1;

		this->GraphicNodes[0].AddTransform(Vector3D(Movement, 0, 0, 1));
		this->GraphicNodes[0].TransformationMatrix.vectorRotation(0.1, Vector3D(1, 1, 1, 0));
		*/
		
		for (int i = 0; i < this->GraphicNodes.size(); i++)
		{
			this->GraphicNodes[i].Draw(this->MVP);
		}

		this->window->SwapBuffers();
	}
}

} // namespace Example