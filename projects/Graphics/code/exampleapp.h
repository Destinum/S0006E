#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "Resources.h"
#include "render/window.h"

namespace Example
{
class ExampleApp : public Core::App
{
public:
	/// open app
	bool Open();
	/// run app
	void Run();

	void computeMatricesFromInputs();

private:
	Display::Window* window;

	Matrix3D MVP;

	std::vector<GraphicsNode> GraphicNodes;
	LightSource TheLightSource;

	//Camera controlls
	
	// Initial position : on +Z
	Vector3D position = Vector3D(0, 0, 10, 1); 
	// Initial horizontal angle : toward -Z
	float horizontalAngle = 3.14f;
	// Initial vertical angle : none
	float verticalAngle = 0.0f;
	// Initial Field of View
	float initialFoV = 45.0f;

	float speed = 3.0f; // 3 units / second
	float mouseSpeed = 0.001f;
};
} // namespace Example