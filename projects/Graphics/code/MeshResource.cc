#include "Resources.h"

MeshResource::~MeshResource()
{
	glDeleteBuffers(1, &this->VertexBuffer);
	glDeleteBuffers(1, &this->ColorBuffer);
	glDeleteBuffers(1, &this->IndexBuffer);
}

void MeshResource::Quad(float Scale)
{

    GLfloat buf[] =
	{
		-1.0,	1.0,	1.0,	1,	// Top Left
		1.0,	1.0,	1.0,	1,	// Top Right
		-1.0,	-1.0,	1.0, 	1,	// Bottom Left
		1.0,	-1.0,	1.0, 	1	// Bottom Right
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

	unsigned int bufIndices[] = 
	{
		0, 1, 2,		//Triangle 1
		3, 1, 2,		//Triangle 2
	};


    glGenBuffers(1, &this->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);

    glGenBuffers(1, &this->ColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->ColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bufColor), bufColor, GL_STATIC_DRAW);

    glGenBuffers(1, &this->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bufIndices), bufIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshResource::Quad(float Scale, TextureResource Texture)
{

    GLfloat buf[] =
	{
		-1.0,	1.0,	1.0,	1,	// Top Left
		0.0,	0.0,				//Top Left UV
		1.0,	1.0,	1.0,	1,	// Top Right
		1.0,	0.0,				//Top Right UV
		-1.0,	-1.0,	1.0, 	1,	// Bottom Left
		0.0,	1.0,				//Bottom Left UV
		1.0,	-1.0,	1.0, 	1,	// Bottom Right
		1.0,	1.0					//Bottom Right UV
	};

	for (int i = 0; i < sizeof(buf) / sizeof(buf[0]); i++)
	{
		buf[i++] *= Scale;
		buf[i++] *= Scale;
		buf[i++] *= Scale;
	}

	unsigned int bufIndices[] = 
	{
		0, 1, 2,		//Triangle 1
		3, 1, 2,		//Triangle 2
	};


    glGenBuffers(1, &this->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);

    glGenBuffers(1, &this->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bufIndices), bufIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	this->Texture = Texture;
}

void MeshResource::RenderQuad(GLuint Program)
{
    glUniform4f(glGetUniformLocation(Program, "ObjectPosition"), this->Position.vektor[0], this->Position.vektor[1], this->Position.vektor[2], this->Position.vektor[3]);
    glUniformMatrix4fv(glGetUniformLocation(Program, "ObjectRotation"), 1, GL_FALSE, &(this->Rotation).matris[0][0]);

	if (this->ColorBuffer != 0)
	{
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, this->ColorBuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
		glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, NULL);
		//glDrawArrays(GL_TRIANGLES, 0, 2*3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	else
	{
		this->Texture.BindTexture();
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
		glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, NULL);
		//glDrawArrays(GL_TRIANGLES, 0, 2*3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(2);
	}  
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

	unsigned int bufIndices[] = 
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


    glGenBuffers(1, &this->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);

    glGenBuffers(1, &this->ColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->ColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bufColor), bufColor, GL_STATIC_DRAW);

    glGenBuffers(1, &this->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bufIndices), bufIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshResource::Cube(float Scale, TextureResource Texture)
{

    GLfloat buf[] =		//Specifically made for "DiceTexture.jpg", although not mapped properly due to the use of indexing.
	{
		-1.0,	1.0,	1.0,	1,	// Top Left, Front
		0.5,	0.5,
		1.0,	1.0,	1.0,	1,	// Top Right, Front
		0.75,	0.5,
		-1.0,	-1.0,	1.0, 	1,	// Bottom Left, Front
		0.5,	0.75,
		1.0,	-1.0,	1.0, 	1,	// Bottom Right, Front
		0.75,	0.75,

		-1.0,	1.0,	-1.0,	1,	// Top Left, Back
		0.5,	0.25,
		1.0,	1.0,	-1.0,	1,	// Top Right, Back
		0.75,	0.25,
		-1.0,	-1.0,	-1.0, 	1,	// Bottom Left, Back
		0.5,	1.0,
		1.0,	-1.0,	-1.0, 	1,	// Bottom Right, Back
		0.75,	1.0
	};

	for (int i = 0; i < sizeof(buf) / sizeof(buf[0]); i++)
	{
		buf[i++] *= Scale;
		buf[i++] *= Scale;
		buf[i++] *= Scale;
	}

	unsigned int bufIndices[] = 
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


    glGenBuffers(1, &this->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);

    glGenBuffers(1, &this->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bufIndices), bufIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	this->Texture = Texture;
}

void MeshResource::RenderCube(GLuint Program)
{
    glUniform4f(glGetUniformLocation(Program, "ObjectPosition"), this->Position.vektor[0], this->Position.vektor[1], this->Position.vektor[2], this->Position.vektor[3]);
    glUniformMatrix4fv(glGetUniformLocation(Program, "ObjectRotation"), 1, GL_FALSE, &(this->Rotation).matris[0][0]);

	if (this->ColorBuffer != 0)
	{
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, this->ColorBuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
		glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, NULL);
		//glDrawArrays(GL_TRIANGLES, 0, 2*3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	else
	{
		this->Texture.BindTexture();
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, this->VertexBuffer);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));		

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndexBuffer);
		glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, NULL);
		//glDrawArrays(GL_TRIANGLES, 0, 2*3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(2);
	}
	
}