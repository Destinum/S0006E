#include "Resources.h"
#include "render/window.h"

Vector3D Lambda(float xUV, float yUV, Vector3D Normal, vector<vector<Vector3D>> &TextureBuffer)
{
	if (xUV < 0)
		xUV = 0;
	if (xUV > 1)
		xUV = 1;
	if (yUV < 0)
		yUV = 0;
	if (yUV > 1)
		yUV = 1;
	
	int x = xUV * (TextureBuffer[0].size() - 1);
	int y = yUV * (TextureBuffer.size() - 1);

	return TextureBuffer[y][x];
}


class Rasterizer
{
public:

	struct Vertex
	{
		Vector3D Coordinates;
		//float Normals[2];
		Vector2D UVs;
	};

	struct Triangle
	{
		Vertex Vertices[3];
	};

	~Rasterizer()
	{
		delete[] VertexBuffer;
		delete[] IndexBuffer;
		delete[] NormalBuffer;
	}

	void computeMatricesFromInputs(){

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


		this->Projection = this->Projection.ProjectionMatrix(initialFoV, 4.0f/3.0f, 0.1f, 100.0f);
		this->View = this->View.ViewMatrix(position, position + direction, up);
		this->MVP = this->Projection * this->View;

		// For the next frame, the "last time" will be "now"
		theLastTime = theCurrentTime;
	}	

	bool IsInRenderDistance(Vertex First, Vertex Second, Vertex Third)
	{
		for (int i = 0; i < 3; i++)
		{
			if (First.Coordinates.vektor[i] > 1.0 && Second.Coordinates.vektor[i] > 1.0 && Third.Coordinates.vektor[i] > 1.0)
				return false;
			
			else if (First.Coordinates.vektor[i] < -1.0 && Second.Coordinates.vektor[i] < -1.0 && Third.Coordinates.vektor[i] < -1.0)
				return false;
		}

		return true;
	}

	void DrawLine(Vector3D Start, Vector3D End)
	{
		int Width, Height;
		this->window->GetSize(Width, Height);
		float xIncrement = 2.0 / Width;
		float yIncrement = 2.0 / Height;
		float xResolutionMultiplier = float(Width)/Height;
		float Curve, x, y, yMax;

	////////		
		if (Start.vektor[1] < End.vektor[1])
		{
			y = Start.vektor[1];
			yMax = End.vektor[1];

			x = Start.vektor[0];

			if(x > End.vektor[0])
				xIncrement *= -1;
		}
		else
		{
			y = End.vektor[1];
			yMax = Start.vektor[1];

			x = End.vektor[0];

			if(x > Start.vektor[0])
				xIncrement *= -1;
		}
	////////	

		if ((End.vektor[0] - Start.vektor[0]) == 0)
			Curve = 1.0;
		else
			Curve = (yMax - y) / ((Start.vektor[0] - End.vektor[0]) * xResolutionMultiplier);

		float dx = abs(End.vektor[0] - Start.vektor[0]) * xResolutionMultiplier;
		float dy = abs(End.vektor[1] - Start.vektor[1]);

		if(abs(Curve) >= 1)
		{
			float p = 2*dx-dy;
			
			glBegin(GL_POINTS);
			while (y <= yMax)
			{
				glVertex2f(x, y);

				y += yIncrement;

				if(p >= 1)
				{
					x += xIncrement;
					p += 2*dx-2*dy;
				}
				else
					p += 2*dx;
				
			}

			glEnd();
		}
		else
		{
			float xMax, xMin;

			if (End.vektor[0] > Start.vektor[0])
			{
				xMax = End.vektor[0];
				xMin = Start.vektor[0];
			}
			else
			{
				xMax = Start.vektor[0];
				xMin = End.vektor[0];
			}

			float p = 2*dy-dx;
			
			glBegin(GL_POINTS);
			while (x <= xMax && x >= xMin)
			{
				glVertex2f(x, y);

				x += xIncrement;

				if(p >= 1)
				{
					y += yIncrement;
					p += 2*dy-2*dx;
				}
				else
					p += 2*dy;
				
			}

			glEnd();
		}
	}

	void DrawOutline(Vector3D Start, Vector3D End, bool IsRightEdge)
	{
		float xIncrementTemp = xIncrement;
		float Curve, x, y, yMax;

	////////
		if (Start.vektor[1] == End.vektor[1])
			return;

		else if (Start.vektor[1] < End.vektor[1])
		{
			y = Start.vektor[1];
			yMax = End.vektor[1];

			x = Start.vektor[0];
		}
		else
		{
			y = End.vektor[1];
			yMax = Start.vektor[1];

			x = End.vektor[0];
		}
		if (yMax > 1.0)
			yMax = 1.0;

	////////	

		if ((End.vektor[0] - Start.vektor[0]) == 0)
			Curve = 1.0;
		else
			Curve = (Start.vektor[1] - End.vektor[1]) / ((Start.vektor[0] - End.vektor[0]) * xResolutionMultiplier);

		if (abs(Curve) < 1)
			xIncrementTemp /= Curve;
		else if (Curve < 0)
			xIncrementTemp *= -1;
		

		float dx = abs(End.vektor[0] - Start.vektor[0]) * xResolutionMultiplier;
		float dy = abs(End.vektor[1] - Start.vektor[1]);
		float p = 2*dx-dy;
			
		while (y <= yMax)
		{
			if (y >= -1.0)
			{
				if (IsRightEdge)
					this->RightEdge.push_back(Vector2D(x, y));
				else
					this->LeftEdge.push_back(Vector2D(x, y));
			}

			y += yIncrement;

			if(p >= 1)
			{
				x += xIncrementTemp;
				p += 2*dx-2*dy;
			}
			else
				p += 2*dx;
			
		}
	}

	void RasterTriangle(Vertex First, Vertex Second, Vertex Third, Vector3D Normal)
	{
		bool HasTwoRightEdges = true;
		Vertex Vertices[3];

		First.Coordinates = (MVP * First.Coordinates).ProjectionNormalize();
		Second.Coordinates = (MVP * Second.Coordinates).ProjectionNormalize();
		Third.Coordinates = (MVP * Third.Coordinates).ProjectionNormalize();
		
		if (!IsInRenderDistance(First, Second, Third))
			return;

		if (First.Coordinates.vektor[1] > Second.Coordinates.vektor[1])
		{
			Vertices[0] = First;
			Vertices[1] = Second;
		}
		else
		{
			Vertices[0] = Second;
			Vertices[1] = First;
		}

		if (Third.Coordinates.vektor[1] > Vertices[1].Coordinates.vektor[1])
		{
			Vertices[2] = Vertices[1];
			Vertices[1] = Third;

			if (Third.Coordinates.vektor[1] > Vertices[0].Coordinates.vektor[1])
			{
				Vertices[1] = Vertices[0];
				Vertices[0] = Third;
			}
		}
		else
			Vertices[2] = Third;
		

		Vector2D A(Vertices[2].Coordinates.vektor[0] - Vertices[1].Coordinates.vektor[0], Vertices[2].Coordinates.vektor[1] - Vertices[1].Coordinates.vektor[1]);
		Vector2D B(Vertices[0].Coordinates.vektor[0] - Vertices[1].Coordinates.vektor[0], Vertices[0].Coordinates.vektor[1] - Vertices[1].Coordinates.vektor[1]);
		float Cross = A.vektor[0] * B.vektor[1] - B.vektor[0] * A.vektor[1];

		if (Cross > 0)
			HasTwoRightEdges = false;
		
		DrawOutline(Vertices[2].Coordinates, Vertices[1].Coordinates, HasTwoRightEdges);
		DrawOutline(Vertices[2].Coordinates, Vertices[0].Coordinates, !HasTwoRightEdges);
		DrawOutline(Vertices[1].Coordinates, Vertices[0].Coordinates, HasTwoRightEdges);
		

////////////////////////////////
		glBegin(GL_POINTS);
		for (int Index = 0; Index < this->LeftEdge.size(); Index++)
		{
			if (Index >= this->RightEdge.size())
				break;

			float i = LeftEdge[Index].vektor[0];
			if (i < -1.0)
				i = -1.0;

			while (i <= RightEdge[Index].vektor[0] && i <= 1.0)
			{				
				float u, v, UVx, UVy;
				
				u =
				(
					Vertices[0].Coordinates.vektor[0] * (Vertices[2].Coordinates.vektor[1] - Vertices[0].Coordinates.vektor[1]) + 
					(RightEdge[Index].vektor[1] - Vertices[0].Coordinates.vektor[1]) * (Vertices[2].Coordinates.vektor[0] - Vertices[0].Coordinates.vektor[0]) -
					i * (Vertices[2].Coordinates.vektor[1] - Vertices[0].Coordinates.vektor[1])
				)
				/
				(	
					(Vertices[1].Coordinates.vektor[1] - Vertices[0].Coordinates.vektor[1]) * (Vertices[2].Coordinates.vektor[0] - Vertices[0].Coordinates.vektor[0]) - 
					(Vertices[1].Coordinates.vektor[0] - Vertices[0].Coordinates.vektor[0]) * (Vertices[2].Coordinates.vektor[1] - Vertices[0].Coordinates.vektor[1])
				);

				v = 
				(
					RightEdge[Index].vektor[1] -
					Vertices[0].Coordinates.vektor[1] -
					u * (Vertices[1].Coordinates.vektor[1] - Vertices[0].Coordinates.vektor[1])
				) 
				/ 
				(Vertices[2].Coordinates.vektor[1] - Vertices[0].Coordinates.vektor[1]);	

				UVx = Vertices[0].UVs.vektor[0] + u * (Vertices[1].UVs.vektor[0] - Vertices[0].UVs.vektor[0]) + v * (Vertices[2].UVs.vektor[0] - Vertices[0].UVs.vektor[0]);
				UVy = Vertices[0].UVs.vektor[1] + u * (Vertices[1].UVs.vektor[1] - Vertices[0].UVs.vektor[1]) + v * (Vertices[2].UVs.vektor[1] - Vertices[0].UVs.vektor[1]);

				Vector3D Color = Lambda(UVx, UVy, Normal, this->TextureBuffer);
				glColor3f(Color.vektor[0], Color.vektor[1], Color.vektor[2]);
				glVertex2f(i, RightEdge[Index].vektor[1]);
				i += this->xIncrement;
			}
		}

		glEnd();
////////////////////////////////

		this->LeftEdge.clear();
		this->RightEdge.clear();
	}

	void GenerateTexture(const char* Texture)
	{
		// load and generate the texture
		int nrChannels;
		unsigned char *BaseData = stbi_load(Texture, &TextureWidth, &TextureHeight, &nrChannels, 0);
		unsigned char *data = BaseData;

		if (data != nullptr && TextureWidth > 0 && TextureHeight > 0 && nrChannels >= 3)
		{
			for(int i = 0; i < TextureHeight; i++)
			{
				vector<Vector3D> temp;

				for(int j = 0; j < TextureWidth; j++)
				{
					temp.push_back(Vector3D());
					temp[j].vektor[0] = static_cast<float>(data[0]) / 255.0;
					temp[j].vektor[1] = static_cast<float>(data[1]) / 255.0;
					temp[j].vektor[2] = static_cast<float>(data[2]) / 255.0;

					data += nrChannels;
				}

				this->TextureBuffer.push_back(temp);
			}
		}
		stbi_image_free(BaseData);
	}

	bool OnUserCreate()
	{
		this->window = new Display::Window;
		//this->window->SetSize(1000, 1000);
		
		if (this->window->Open())
		{
			// Set clear color to gray
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			int Width, Height;
			this->window->GetSize(Width, Height);
			this->xIncrement = 2.0 / Width;
			this->yIncrement = 2.0 / Height;
			this->xResolutionMultiplier = float(Width)/Height;

			
			this->VertexBuffer = new Vertex[4]
			{
				{Vector3D(0.5, 0.5, 0.0), Vector2D(1.0, 0.0)},
				{Vector3D(-0.5, 0.5, 0.0), Vector2D(0.0, 0.0)},
				{Vector3D(0.5, -0.5, 0.0), Vector2D(1.0, 1.0)},
				{Vector3D(-0.5, -0.5, 0.0), Vector2D(0.0, 1.0)}
			};

			this->NumberOfVertices = 6;
			//this->NumberOfVertices = 3;

			this->IndexBuffer = new int[this->NumberOfVertices]
			{
				0, 1, 2,	//Triangle 1
				3, 1, 2		//Triangle 2
			};

			this->NormalBuffer = new Vector3D[2]
			{
				Vector3D(0.0, 0.0, 1.0, 0.0),	//Triangle 1
				Vector3D(0.0, 0.0, 1.0, 0.0)	//Triangle 2
			};

			const char* Texture = "../projects/Graphics/Textures/4Texture.png";
			GenerateTexture(Texture);
			//computeMatricesFromInputs();
		}
		return true;
	}

	bool OnUserUpdate()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->window->Update();
		
		computeMatricesFromInputs();

		for (int i = 0; i < this->NumberOfVertices; i += 3)
		{
			RasterTriangle(VertexBuffer[IndexBuffer[i]], VertexBuffer[IndexBuffer[i + 1]], VertexBuffer[IndexBuffer[i + 2]], NormalBuffer[(i + 1) / 3]);
		}

		this->window->SwapBuffers();
	}

private:
	Display::Window* window;
	Matrix3D Projection;
	Matrix3D View;
	Matrix3D MVP;
	Vertex* VertexBuffer;
	int* IndexBuffer;
	Vector3D* NormalBuffer;
	int NumberOfVertices;
	//Vector3D* TextureBuffer;
	vector<vector<Vector3D>> TextureBuffer;
	int TextureWidth, TextureHeight;
	vector<Vector2D> LeftEdge;
	vector<Vector2D> RightEdge;
	float xIncrement, yIncrement, xResolutionMultiplier;

	//float TestColor[3];

	//Camera controlls
	
	// Initial position : on +Z
	Vector3D position = Vector3D(0, 0, 5, 1); 
	// Initial horizontal angle : toward -Z
	float horizontalAngle = 3.14f;
	// Initial vertical angle : none
	float verticalAngle = 0.0f;
	// Initial Field of View
	float initialFoV = 45.0f;

	float speed = 3.0f; // 3 units / second
	float mouseSpeed = 0.001f;
};









































		/*
		if(abs(Curve) >= 1)
		{
			float p = 2*dx-dy;
			
			//glBegin(GL_POINTS);
			while (y <= yMax)
			{
				if (IsRightEdge)
					this->RightEdge.push_back(Vector2D(x, y));
				else
					this->LeftEdge.push_back(Vector2D(x, y));

				//glVertex2f(x, y);

				y += yIncrement;

				if(p >= 1)
				{
					x += xIncrementTemp;
					p += 2*dx-2*dy;
				}
				else
					p += 2*dx;
				
			}

			//glEnd();
		}
		else
		{
			float xMax, xMin;

			if (End.vektor[0] > Start.vektor[0])
			{
				xMax = End.vektor[0];
				xMin = Start.vektor[0];
			}
			else
			{
				xMax = Start.vektor[0];
				xMin = End.vektor[0];
			}

			/*
			if (xMin < -1.0)			//????????
				xMin = -1.0;
			if (xMax > 1.0)
				xMax = 1.0;
			if (y > 1.0)
				y = 1.0;				//???????? ends
			

			float p = 2*dy-dx;
			
			//glBegin(GL_POINTS);
			//glVertex2f(x, y);
			
			if((IsRightEdge && xIncrementTemp < 0) || !IsRightEdge && xIncrementTemp > 0)
			{
				if (IsRightEdge)
					this->RightEdge.push_back(Vector2D(x, y));
				else
					this->LeftEdge.push_back(Vector2D(x, y));
			}

			while (x <= xMax && x >= xMin)
			{
				x += xIncrementTemp;

				if(p >= 1)
				{
					y += yIncrement;
					p += 2*dy-2*dx;

					if(IsRightEdge && xIncrementTemp < 0)
						this->RightEdge.push_back(Vector2D(x, y));
					else if (!IsRightEdge && xIncrementTemp > 0)
						this->LeftEdge.push_back(Vector2D(x, y));
					else
					{
						//glVertex2f(x - xIncrement, y - yIncrement);
						if (IsRightEdge)
							this->RightEdge.push_back(Vector2D(x - xIncrementTemp, y - yIncrement));
						else
							this->LeftEdge.push_back(Vector2D(x - xIncrementTemp, y - yIncrement));
					}
				}
				else
					p += 2*dy;
				
			}

			//glEnd();
		}
		*/