#ifndef CUBE_H
#define CUBE_H

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

class Cube
{
public:
	void draw(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO);
	int getNumVectors() { return sizeof(indices) / sizeof(unsigned int); }

	/* Position and Roation */
	glm::mat4 position;

	/* Update Function */
private:
	float vertices[24] = {
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,

		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f
	};

	unsigned int indices[36]{
		5, 6, 7,     //front
		4, 5, 6,

		0, 1, 2,    //back
		1, 2, 3,

		1, 3, 5,    //right
		3, 5, 7,

		0, 2, 6,    //left
		0, 4, 6,

		0, 1, 4,    //top
		1, 4, 5,

		2, 3, 6,    //bottom
		3, 6, 7,
	};

};

#endif