#include "Resources.h"

#include <cstring>
#include <map>

MeshResource::~MeshResource()
{
	glDeleteBuffers(1, &this->VertexBuffer);
	glDeleteBuffers(1, &this->ColorBuffer);
	glDeleteBuffers(1, &this->IndexBuffer);
}

void MeshResource::Render()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	if (this->NormalBuffer != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, this->UVBuffer);
    	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindBuffer(GL_ARRAY_BUFFER, this->NormalBuffer);
    	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	} 

	else if (this->ColorBuffer == 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
	}

	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, this->ColorBuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	}	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
	glDrawElements(GL_TRIANGLES, this->NumberOfVertices, GL_UNSIGNED_INT, NULL);
	//glDrawArrays(GL_TRIANGLES, 0, this->NumberOfVertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

bool MeshResource::loadOBJ(const GLchar* path)
{
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices;
	std::vector<Vector3D> temp_vertices; 
	std::vector<Vector2D> temp_uvs;
	std::vector<Vector3D> temp_normals;
	std::map<GLint, GLint> IndexConverter;

	FILE * file = fopen(path, "r");
	if ( file == NULL )
	{
		printf("Impossible to open the file!\n");
		getchar();
		return false;
	}

	while (true)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if (strcmp(lineHeader, "v") == 0)
		{
			Vector3D vertex;
			fscanf(file, "%f %f %f\n", &vertex.vektor[0], &vertex.vektor[1], &vertex.vektor[2]);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			Vector2D uv;
			fscanf(file, "%f %f\n", &uv.vektor[0], &uv.vektor[1]);
			uv.vektor[1] = -uv.vektor[1]; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			Vector3D normal;
			fscanf(file, "%f %f %f\n", &normal.vektor[0], &normal.vektor[1], &normal.vektor[2] );
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
			&vertexIndex[0], &uvIndex[0], &normalIndex[0],
			&vertexIndex[1], &uvIndex[1], &normalIndex[1],
			&vertexIndex[2], &uvIndex[2], &normalIndex[2],
			&vertexIndex[3], &uvIndex[3], &normalIndex[3]);			
			
			if (matches == 9 || matches == 12)
			{
				for (int i = 0; i < matches/3; i++)
				{
					if (IndexConverter.count(vertexIndex[i] - 1) == 1 && IndexConverter[vertexIndex[i] - 1] != uvIndex[i] - 1)
					{
						std::vector<int> indexes;

						for (int j = 0; j < temp_vertices.size(); j++)
						{
							if (temp_vertices[j] == temp_vertices[vertexIndex[i] - 1])
							{
								indexes.push_back(j);
							}
						}

						bool IsKey = false;

						for (int j = 0; j < indexes.size(); j++)
						{
							if (IndexConverter[indexes[j]] == uvIndex[i] - 1)
							{
								IsKey = true;
								vertexIndex[i] = indexes[j] + 1;
								break;
							}
						}

						if (!IsKey)
						{
							temp_vertices.push_back(temp_vertices[vertexIndex[i] - 1]);
							temp_normals.push_back(temp_normals[normalIndex[i] - 1]);
							vertexIndex[i] = temp_vertices.size();
						}
					}
				}

				vertexIndices.push_back(vertexIndex[0] - 1);
				vertexIndices.push_back(vertexIndex[1] - 1);
				vertexIndices.push_back(vertexIndex[2] - 1);
				/*
				uvIndices.push_back(uvIndex[0] - 1);
				uvIndices.push_back(uvIndex[1] - 1);
				uvIndices.push_back(uvIndex[2] - 1);
				normalIndices.push_back(normalIndex[0] - 1);
				normalIndices.push_back(normalIndex[1] - 1);
				normalIndices.push_back(normalIndex[2] - 1);
				*/

				IndexConverter[vertexIndex[0] - 1] = uvIndex[0] - 1;
				IndexConverter[vertexIndex[1] - 1] = uvIndex[1] - 1;
				IndexConverter[vertexIndex[2] - 1] = uvIndex[2] - 1;

				if (matches == 12)
				{
					vertexIndices.push_back(vertexIndex[0] - 1);
					vertexIndices.push_back(vertexIndex[2] - 1);
					vertexIndices.push_back(vertexIndex[3] - 1);
					/*
					uvIndices.push_back(uvIndex[0] - 1);
					uvIndices.push_back(uvIndex[2] - 1);
					uvIndices.push_back(uvIndex[3] - 1);
					normalIndices.push_back(normalIndex[0] - 1);
					normalIndices.push_back(normalIndex[2] - 1);
					normalIndices.push_back(normalIndex[3] - 1);
					*/

					IndexConverter[vertexIndex[3] - 1] = uvIndex[3] - 1;
				}
			}
			else
			{
				matches = fscanf(file, "%d//%d %d//%d %d//%d\n",
				&vertexIndex[0], &normalIndex[0],
				&vertexIndex[1], &normalIndex[1],
				&vertexIndex[2], &normalIndex[2]);
				
				if (matches == 6)
				{
					vertexIndices.push_back(vertexIndex[0] - 1);
					vertexIndices.push_back(vertexIndex[1] - 1);
					vertexIndices.push_back(vertexIndex[2] - 1);
					//normalIndices.push_back(normalIndex[0] - 1);
					//normalIndices.push_back(normalIndex[1] - 1);
					//normalIndices.push_back(normalIndex[2] - 1);
				}
				else
				{
					printf("OBJ-file can't be read.\n");
					fclose(file);
					return false;
				}
			}
		}
		else
		{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}
	fclose(file);

	GLfloat *bufVertices = new GLfloat[temp_vertices.size()*4];
	for (int i = 0; i < temp_vertices.size(); i++)
	{
		int n = i*4;
		bufVertices[n] = temp_vertices[i].vektor[0];
		bufVertices[n + 1] = temp_vertices[i].vektor[1];
		bufVertices[n + 2] = temp_vertices[i].vektor[2];
		bufVertices[n + 3] = temp_vertices[i].vektor[3];
	}

	GLfloat *bufUVs = new GLfloat[temp_uvs.size()*2];
	for (int i = 0; i < temp_uvs.size(); i++)
	{		
		int n = i*2;

		bufUVs[n] = temp_uvs[IndexConverter[i]].vektor[0];
		bufUVs[n + 1] = temp_uvs[IndexConverter[i]].vektor[1];
	}

	GLfloat *bufNormals = new GLfloat[temp_normals.size()*4];
	for (int i = 0; i < temp_normals.size(); i++)
	{
		int n = i*4;
		bufNormals[n] = temp_normals[i].vektor[0];
		bufNormals[n + 1] = temp_normals[i].vektor[1];
		bufNormals[n + 2] = temp_normals[i].vektor[2];
		bufNormals[n + 3] = temp_normals[i].vektor[3];
	}

	unsigned int *bufIndices = new unsigned int[vertexIndices.size()];
	for (int i = 0; i < vertexIndices.size(); i++)
	{	
		bufIndices[i] = vertexIndices[i];
	}

	this->NumberOfVertices = vertexIndices.size();

	glGenBuffers(1, &this->VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * temp_vertices.size()*4, bufVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &this->UVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->UVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * temp_uvs.size()*2, bufUVs, GL_STATIC_DRAW);

	glGenBuffers(1, &this->NormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->NormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * temp_normals.size()*4, bufNormals, GL_STATIC_DRAW);

	glGenBuffers(1, &this->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * vertexIndices.size(), bufIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete [] bufVertices;
	delete [] bufUVs;
	delete [] bufNormals;
	delete [] bufIndices;

	return true;
}



void MeshResource::Quad(float Scale)
{

    GLfloat buf[] =
	{
		-1.0,	1.0,	0.0,	1,	// Top Left
		1.0,	1.0,	0.0,	1,	// Top Right
		-1.0,	-1.0,	0.0, 	1,	// Bottom Left
		1.0,	-1.0,	0.0, 	1	// Bottom Right
	};

	for (int i = 0; i < sizeof(buf) / sizeof(buf[0]); i++)
	{
		buf[i++] *= Scale;
		buf[i++] *= Scale;
		buf[i++] *= Scale;
	}

	GLfloat bufColor[] = 
	{
		0,		1,		0,		1,	// Top Left
		0,		0,		1,		1,	// Top Right
		0,		0,		1,		1,	// Bottom Left
		0,		1,		0,		1	// Bottom Right
	};

	unsigned int bufVertexIndices[] = 
	{
		0, 1, 2,		//Triangle 1
		3, 1, 2,		//Triangle 2
	};

	this->NumberOfVertices = sizeof(bufVertexIndices) / sizeof(bufVertexIndices[0]);

    glGenBuffers(1, &this->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);

    glGenBuffers(1, &this->ColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->ColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bufColor), bufColor, GL_STATIC_DRAW);

    glGenBuffers(1, &this->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bufVertexIndices), bufVertexIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshResource::Quad(float Scale, bool IsTextured)
{

    GLfloat buf[] =
	{
		-1.0,	1.0,	0.0,	1,	// Top Left
		0.0,	0.0,				//Top Left UV
		1.0,	1.0,	0.0,	1,	// Top Right
		1.0,	0.0,				//Top Right UV
		-1.0,	-1.0,	0.0, 	1,	// Bottom Left
		0.0,	1.0,				//Bottom Left UV
		1.0,	-1.0,	0.0, 	1,	// Bottom Right
		1.0,	1.0					//Bottom Right UV
	};

	for (int i = 0; i < sizeof(buf) / sizeof(buf[0]); i++)
	{
		buf[i++] *= Scale;
		buf[i++] *= Scale;
		buf[i++] *= Scale;
		i++;
		i++;
	}

	unsigned int bufVertexIndices[] = 
	{
		0, 1, 2,		//Triangle 1
		3, 1, 2,		//Triangle 2
	};

	this->NumberOfVertices = sizeof(bufVertexIndices) / sizeof(bufVertexIndices[0]);

    glGenBuffers(1, &this->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);

    glGenBuffers(1, &this->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bufVertexIndices), bufVertexIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshResource::Cube(float Scale)
{

    GLfloat buf[] =
	{
		-1.0,	1.0,	1.0,	1,	// Top Left, Front
		1.0,	1.0,	1.0,	1,	// Top Right, Front
		-1.0,	-1.0,	1.0, 	1,	// Bottom Left, Front
		1.0,	-1.0,	1.0, 	1,	// Bottom Right, Front

		-1.0,	1.0,	-1.0,	1,	// Top Left, Back
		1.0,	1.0,	-1.0,	1,	// Top Right, Back
		-1.0,	-1.0,	-1.0, 	1,	// Bottom Left, Back
		1.0,	-1.0,	-1.0, 	1	// Bottom Right, Back
	};

	for (int i = 0; i < sizeof(buf) / sizeof(buf[0]); i++)
	{
		buf[i++] *= Scale;
		buf[i++] *= Scale;
		buf[i++] *= Scale;
	}

	GLfloat bufColor[] = 
	{
		0,		1,		0,		1,	// Top Left, Front
		0,		0,		1,		1,	// Top Right, Front
		0,		0,		1,		1,	// Bottom Left, Front
		0,		1,		0,		1,	// Bottom Right, Front

		0,		0,		1,		1,	// Top Left, Back
		0,		1,		0,		1,	// Top Right, Back
		0,		1,		0,		1,	// Bottom Left, Back
		0,		0,		1,		1	// Bottom Right, Back
	};

	unsigned int bufVertexIndices[] = 
	{
		0, 1, 2,		//Front Triangle 1
		3, 1, 2,		//Front Triangle 2

		0, 4, 2,		//Left Triangle 1
		6, 4, 2,		//Left Triangle 2

		1, 5, 3,		//Right Triangle 1
		7, 5, 3,		//Right Triangle 2

		0, 1, 4,		//Top Triangle 1
		5, 1, 4,		//Top Triangle 2

		2, 3, 6,		//Bottom Triangle 1
		7, 3, 6,		//Bottom Triangle 2

		4, 5, 6,		//Back Triangle 1
		7, 5, 6			//Back Triangle 2
	};

	this->NumberOfVertices = sizeof(bufVertexIndices) / sizeof(bufVertexIndices[0]);

    glGenBuffers(1, &this->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);

    glGenBuffers(1, &this->ColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->ColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bufColor), bufColor, GL_STATIC_DRAW);

    glGenBuffers(1, &this->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bufVertexIndices), bufVertexIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshResource::Cube(float Scale, bool IsTextured)
{

    GLfloat buf[] =		//Specifically made for "DiceTexture.jpg"
	{
		-1.0,	1.0,	1.0,	1,	// Top Left, Front, 		#0
		0.5,	0.5,
		1.0,	1.0,	1.0,	1,	// Top Right, Front, 		#1
		0.75,	0.5,
		-1.0,	-1.0,	1.0, 	1,	// Bottom Left, Front, 		#2
		0.5,	0.75,
		1.0,	-1.0,	1.0, 	1,	// Bottom Right, Front, 	#3
		0.75,	0.75,

		-1.0,	1.0,	-1.0,	1,	// Top Left, Back 1, 		#4
		0.5,	0.25,
		1.0,	1.0,	-1.0,	1,	// Top Right, Back 1, 		#5
		0.75,	0.25,
		-1.0,	-1.0,	-1.0, 	1,	// Bottom Left, Back 1, 	#6
		0.5,	1.0,
		1.0,	-1.0,	-1.0, 	1,	// Bottom Right, Back 1, 	#7
		0.75,	1.0,

		-1.0,	1.0,	-1.0,	1,	// Top Left, Back 2, 		#8
		0.25,	0.5,
		1.0,	1.0,	-1.0,	1,	// Top Right, Back 2, 		#9
		1.0,	0.5,
		-1.0,	-1.0,	-1.0, 	1,	// Bottom Left, Back 2, 	#10
		0.25,	0.75,
		1.0,	-1.0,	-1.0, 	1,	// Bottom Right, Back 2, 	#11
		1.0,	0.75,

		1.0,	1.0,	-1.0,	1,	// Top Right, Back 3, 		#12
		0.0,	0.5,
		1.0,	-1.0,	-1.0, 	1,	// Bottom Right, Back 3, 	#13
		0.0,	0.75
	};

	for (int i = 0; i < sizeof(buf) / sizeof(buf[0]); i++)
	{
		buf[i++] *= Scale;
		buf[i++] *= Scale;
		buf[i++] *= Scale;
		i++;
		i++;
	}

	unsigned int bufVertexIndices[] = 
	{
		0, 1, 2,		//Front Triangle 1
		3, 1, 2,		//Front Triangle 2

		0, 1, 4,		//Top Triangle 1
		5, 1, 4,		//Top Triangle 2

		2, 3, 6,		//Bottom Triangle 1
		7, 3, 6,		//Bottom Triangle 2

		0, 8, 2,		//Left Triangle 1
		10, 8, 2,		//Left Triangle 2

		1, 9, 3,		//Right Triangle 1
		11, 9, 3,		//Right Triangle 2

		8, 12, 10,		//Back Triangle 1
		13, 12, 10		//Back Triangle 2
	};

	this->NumberOfVertices = sizeof(bufVertexIndices) / sizeof(bufVertexIndices[0]);

    glGenBuffers(1, &this->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);

    glGenBuffers(1, &this->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bufVertexIndices), bufVertexIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}