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

void ExampleApp::computeMatricesFromInputs()
{
	// glfwGetTime is called only once, the first time this function is called
	static double theLastTime = glfwGetTime();
	static double xposLast, yposLast;
	static bool ControllingCamera = false;

	// Compute time difference between current and last frame
	double theCurrentTime = glfwGetTime();
	float theDeltaTime = float(theCurrentTime - theLastTime);

	// Check for mouse input 
	if (ControllingCamera == false && glfwGetMouseButton(this->window->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		ControllingCamera = true;
		glfwGetCursorPos(this->window->window, &xposLast, &yposLast);
		glfwSetInputMode(this->window->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		int Width, Height;
		this->window->GetSize(Width, Height);
		glfwSetCursorPos(this->window->window, Width/2, Height/2);
	}
	else if (ControllingCamera == true && glfwGetMouseButton(this->window->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		ControllingCamera = false;
		glfwSetCursorPos(this->window->window, xposLast, yposLast);
		glfwSetInputMode(this->window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (ControllingCamera)
	{
		// Get mouse position
		double xpos, ypos;
		glfwGetCursorPos(this->window->window, &xpos, &ypos);
		int Width, Height;
		this->window->GetSize(Width, Height);

		// Reset mouse position for next frame
		glfwSetCursorPos(this->window->window, Width/2, Height/2);

		// Compute new orientation
		horizontalAngle += mouseSpeed * float(Width/2 - xpos);
		verticalAngle   += mouseSpeed * float(Height/2 - ypos);
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
		const GLchar* VertexShader = "../projects/Graphics/Shaders/LightVertexShader.vert";
		const GLchar* FragmentShader = "../projects/Graphics/Shaders/LightFragmentShader.frag";
		const GLchar* Texture = "../projects/Graphics/Textures/GolfBallTexture.jpg";

		//Setup GraphicNode with Shaders, Texture and Mesh
		this->GraphicNodes.push_back(GraphicsNode());
		this->GraphicNodes.push_back(GraphicsNode());

		int index = 0;

		//First GrauphicsNode

		this->GraphicNodes[index].Shaders.Setup(VertexShader, FragmentShader);
		this->GraphicNodes[index].Texture.LoadTexture(Texture, this->GraphicNodes[index].Shaders.program);
		this->GraphicNodes[index].Mesh.loadOBJ("../projects/Graphics/Meshes/GolfBall.obj");
		this->GraphicNodes[index].TransformationID = glGetUniformLocation(this->GraphicNodes[index].Shaders.program, "TransformationMatrix");
		this->GraphicNodes[index].AddTransform(Vector3D(-3, 0, 0, 1));

		//Second GrauphicsNode
		index++;

		this->GraphicNodes[index].Shaders.Setup(VertexShader, FragmentShader);
		this->GraphicNodes[index].Texture.LoadTexture(Texture, this->GraphicNodes[index].Shaders.program);
		this->GraphicNodes[index].Mesh.loadOBJ("../projects/Graphics/Meshes/GolfBall.obj");
		this->GraphicNodes[index].TransformationID = glGetUniformLocation(this->GraphicNodes[index].Shaders.program, "TransformationMatrix");
		this->GraphicNodes[index].AddTransform(Vector3D(3, 0, 0, 1));

		this->TheLightSource.Update(&this->GraphicNodes);

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

	float Movement = 0.01;

	while (this->window->IsOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->window->Update();

		// Update camera based on keyboard and mouse imputs
		computeMatricesFromInputs();

		if (this->TheLightSource.Position.vektor[0] <= -5.0 || this->TheLightSource.Position.vektor[0] >= 5.0)
			Movement *= -1;

		this->TheLightSource.AddPosition(Movement, 0, 0);
		this->TheLightSource.Update(&this->GraphicNodes);	
		
		for (int i = 0; i < this->GraphicNodes.size(); i++)
		{
			this->GraphicNodes[i].Draw(this->MVP, this->position);
		}

		this->window->SwapBuffers();
	}
}

} // namespace Example