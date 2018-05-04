#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <glm/vec3.hpp>

class Sphere
{
public:
	void draw(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO);
	int getNumVectors() { return sizeof(indices) / sizeof(unsigned int); }

	/* Position and Roation */
	glm::mat4 position;

	/* Update Function */
private:
	float vertices[42] = {
		0.f, 0.5f, 0.f, //Top

		0.35f, 0.35f, 0.f, //Top Ring
		0.f, 0.35f, 0.35f,
		-0.35f, 0.35f, 0.f,
		0.f, 0.35f, -0.35f,

		0.5f, 0.0f, 0.0f, //Mid Ring
		0.f, 0.0f, 0.5f,
		-0.5f, 0.0f, 0.f,
		0.f, 0.0f, -0.5f,

		0.35f, -0.35f, 0.f, //Bottom Ring
		0.f, -0.35f, 0.35f,
		-0.35f, -0.35f, 0.f,
		0.f, -0.35f, -0.35f,

		0.f, -0.5f, 0.f //0-13
	};

	unsigned int indices[72]{
		0, 1, 2,     //Top - TopRing
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,

		1, 5, 6,	//TopRing - MiddleRing
		2, 6, 7,
		3, 7, 8,
		4, 8, 5,
		5, 1, 2,	//MiddleRing - TopRing
		6, 2, 3,
		7, 3, 4,
		8, 4, 1,

		5, 9, 10,	//MiddleRing - BottomRing
		6, 10, 11,
		7, 11, 12,
		8, 12, 9,
		9, 5, 6,	//BottomRing - TopRing
		10, 6, 7,
		11, 7, 8,
		12, 8, 5,

		13, 9, 10,     //Bottom - BottomRing
		13, 10, 11,
		13, 11, 12,
		13, 12, 9
	};

};

#endif