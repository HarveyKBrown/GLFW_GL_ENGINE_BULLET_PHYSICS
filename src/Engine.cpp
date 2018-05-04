#include "Engine.h"
#include "EventManager.h"
#include "ShaderConstructor.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Cube.h"
#include "Sphere.h"
#include "world.h"

/* Bullet Physics*/
#include <btBulletDynamicsCommon.h>


/* Shapes */
Sphere sphereA;
Sphere sphereB;
Cube cube;

/* Bullet Vars */
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;
/* Bullet Shapes */
btRigidBody* sphereARigidBody;
btRigidBody* sphereBRigidBody;
btRigidBody* cubeRigidBody;
btRigidBody* bottomRigidBody;
btRigidBody* topRigidBody;
btRigidBody* leftRigidBody;
btRigidBody* rightRigidBody;

/* Renderer Vars */
int shaderProgram;
int numberOfVertices;
unsigned int VBO, VAO, EBO;
int numberOfVert;

/* Shader vars */
glm::mat4 rotMatrix;
glm::mat4 cameraPosMatrix;
glm::mat4 projMatrix = glm::perspective(glm::radians(45.0f), (float)500 / (float)500, 0.1f, 100.0f);

bool Engine::init(const char* title, int width, int height)
{
	/* Initialise GLFW */
	if (!glfwInit())
	{
		std::cout << "Failed to initialise GLFW" << std::endl;
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	/* Create  Window */
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create window " << title << std::endl;
		return false;
	}
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwMakeContextCurrent(window);

	/* Initialize InputHandler */
	glfwSetKeyCallback(window, EventManager::handleEvents);
	EventManager::registerEvent(GLFW_KEY_ESCAPE, [&] () { isRunning = false; });
	EventManager::registerEvent(GLFW_KEY_Q, [&] () { isRunning = false; });

	/* Initialise GLEW */
	glewInit();
	glEnable(GL_DEPTH_TEST);

	cameraPosMatrix = glm::translate(cameraPosMatrix, glm::vec3(0.0f, 0.0f, -20.0f));

	isRunning = true;
	return true;
}

bool Engine::initShaders()
{
	int vertShad = ShaderConstructor::LoadShader("shaders/simple.vert", GL_VERTEX_SHADER);
	int fragShad = ShaderConstructor::LoadShader("shaders/simple.frag", GL_FRAGMENT_SHADER);

	shaderProgram = ShaderConstructor::CreateShaderProgram(vertShad, fragShad);

	/* Temporary Overwrite */
	//sphereShaderProgram = cubeShaderProgram;

	glDeleteShader(vertShad);
	glDeleteShader(fragShad);

	return true;
}

bool Engine::initBullet()
{
	// Build the broadphase
	broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and dispatcher
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	solver = new btSequentialImpulseConstraintSolver;

	// The world.
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, GRAVITY, 0));

	/* Making rigbods and adding them to the dynamics world */
	btCollisionShape* bottomShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0); //Flat Ground
	btCollisionShape* topShape = new btStaticPlaneShape(btVector3(0, -1, 0), 0); //Flat Ground
	btCollisionShape* leftShape = new btStaticPlaneShape(btVector3(1, 0, 0), 0); //Flat Ground
	btCollisionShape* rightShape = new btStaticPlaneShape(btVector3(-1, 0, 0), 0); //Flat Ground
	btDefaultMotionState* topMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -8, 0)));
	btDefaultMotionState* bottomMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 8, 0)));
	btDefaultMotionState* leftMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-8, 0, 0)));
	btDefaultMotionState* rightMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(8, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo bottomRigidBodyCI(0, topMotionState, bottomShape, btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo topRigidBodyCI(0, bottomMotionState, topShape, btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo leftRigidBodyCI(0, leftMotionState, leftShape, btVector3(0, 0, 0));
	btRigidBody::btRigidBodyConstructionInfo rightRigidBodyCI(0, rightMotionState, rightShape, btVector3(0, 0, 0));
	bottomRigidBody = new btRigidBody(bottomRigidBodyCI);
	topRigidBody = new btRigidBody(topRigidBodyCI);
	leftRigidBody = new btRigidBody(leftRigidBodyCI);
	rightRigidBody = new btRigidBody(rightRigidBodyCI);
	bottomRigidBody->setRestitution(COE); //Set the bounce coefficient
	topRigidBody->setRestitution(COE); //Set the bounce coefficient
	leftRigidBody->setRestitution(COE); //Set the bounce coefficient
	rightRigidBody->setRestitution(COE); //Set the bounce coefficient
	dynamicsWorld->addRigidBody(bottomRigidBody);
	dynamicsWorld->addRigidBody(topRigidBody);
	dynamicsWorld->addRigidBody(leftRigidBody);
	dynamicsWorld->addRigidBody(rightRigidBody);

	btCollisionShape* shapeSphere = new btSphereShape(0.5); //Falling Sphere
	btCollisionShape* shapeCube = new btBoxShape(btVector3(0.5, 0.5, 0.5)); //Falling Cube
	btDefaultMotionState* sphereAMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 1, 0, 1), btVector3(0, 2.5, 0))); // Starting pos and rot
	btDefaultMotionState* sphereBMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 1, 1), btVector3(0, 4.5, 0))); // Starting pos and rot
	btDefaultMotionState* cubeMotionState = new btDefaultMotionState(btTransform(btQuaternion(1, 0, 0, 1), btVector3(0, 6.5, 0))); // Starting pos and rot
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	shapeSphere->calculateLocalInertia(mass, fallInertia);
	shapeCube->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo sphereARigidBodyCI(mass, sphereAMotionState, shapeSphere, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo sphereBRigidBodyCI(mass, sphereBMotionState, shapeSphere, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo cubeRigidBodyCI(mass, cubeMotionState, shapeCube, fallInertia);
	sphereARigidBody = new btRigidBody(sphereARigidBodyCI);
	sphereBRigidBody = new btRigidBody(sphereBRigidBodyCI);
	cubeRigidBody = new btRigidBody(cubeRigidBodyCI);
	sphereARigidBody->setRestitution(COE);
	sphereBRigidBody->setRestitution(COE);
	cubeRigidBody->setRestitution(COE);
	sphereARigidBody->setLinearVelocity(btVector3(-5, 20, 0)); // Initial velocity
	sphereBRigidBody->setLinearVelocity(btVector3(-5, 20, 0));
	cubeRigidBody->setLinearVelocity(btVector3(-5, 20, 0));
	sphereARigidBody->setLinearFactor(btVector3(1, 1, 0));
	sphereBRigidBody->setLinearFactor(btVector3(1, 1, 0));
	cubeRigidBody->setLinearFactor(btVector3(1, 1, 0));
	dynamicsWorld->addRigidBody(sphereARigidBody);
	dynamicsWorld->addRigidBody(sphereBRigidBody);
	dynamicsWorld->addRigidBody(cubeRigidBody);
}

void Engine::calculateDeltaTime()
{
	LAST = NOW;
	NOW = glfwGetTime();
	deltaTime = NOW - LAST;
}

void Engine::update()
{
	dynamicsWorld->stepSimulation(deltaTime /10, 10); // Run 1 frame of bullet simulation : Not sure if this will work in my system??? Limits FR to 60fps OR does it assume 60fps

	//sphereA.position = glm::rotate(sphereA.position, (float) deltaTime * glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//sphereB.position = glm::rotate(sphereB.position, (float) deltaTime * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	/* Set sphereA position to fallRigidBody position */
	glm::mat4 tempPos;
	btTransform trans;
	/* Shape 1 */
	sphereARigidBody->getMotionState()->getWorldTransform(trans);
	tempPos = glm::translate(glm::mat4(1.0f), glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
	tempPos = glm::rotate(tempPos, trans.getRotation().getAngle(), glm::vec3(trans.getRotation().getAxis().getX(), trans.getRotation().getAxis().getY(), trans.getRotation().getAxis().getZ()));
	sphereA.position = tempPos;
	/* Shape 2 */
	sphereBRigidBody->getMotionState()->getWorldTransform(trans);
	tempPos = glm::translate(glm::mat4(1.0f), glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
	tempPos = glm::rotate(tempPos, trans.getRotation().getAngle(), glm::vec3(trans.getRotation().getAxis().getX(), trans.getRotation().getAxis().getY(), trans.getRotation().getAxis().getZ()));
	sphereB.position = tempPos;
	/* Shape 3 */
	cubeRigidBody->getMotionState()->getWorldTransform(trans);
	tempPos = glm::translate(glm::mat4(1.0f), glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()));
	tempPos = glm::rotate(tempPos, trans.getRotation().getAngle(), glm::vec3(trans.getRotation().getAxis().getX(), trans.getRotation().getAxis().getY(), trans.getRotation().getAxis().getZ()));
	cube.position = tempPos;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Engine::render()
{
	glClearColor(0.5f, 0.2f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/* START VAO LOOP */
	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			/* Load Shape 1 */
			sphereA.draw(VAO, VBO, EBO);
			numberOfVertices = sphereA.getNumVectors();
			rotMatrix = sphereA.position;
			break;
		case 1:
			/* Load Shape 2 */
			sphereB.draw(VAO, VBO, EBO);
			numberOfVertices = sphereB.getNumVectors();
			rotMatrix = sphereB.position;
			break;
		case 2:
			/* Load Shape 3 */
			cube.draw(VAO, VBO, EBO);
			numberOfVertices = cube.getNumVectors();
			rotMatrix = cube.position;
			break;
		}

		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "uModel");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(rotMatrix));

		unsigned int cameraPosition = glGetUniformLocation(shaderProgram, "uView");
		glUniformMatrix4fv(cameraPosition, 1, GL_FALSE, value_ptr(cameraPosMatrix));

		unsigned int perspectivePointer = glGetUniformLocation(shaderProgram, "uProjection");
		glUniformMatrix4fv(perspectivePointer, 1, GL_FALSE, value_ptr(projMatrix));

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, numberOfVertices, GL_UNSIGNED_INT, 0);

		glDeleteVertexArrays(sizeof(VAO), &VAO);
		glDeleteBuffers(sizeof(VBO), &VBO);
		glDeleteBuffers(sizeof(EBO), &EBO);
		/* END VAO LOOP */
	}

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Engine::clean()
{
	glfwDestroyWindow(window);
	window = NULL;
	glfwTerminate();
}

void Engine::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
