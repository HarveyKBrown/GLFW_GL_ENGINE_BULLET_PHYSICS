#ifndef ENGINE_H
#define ENGINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Engine {
	public:
		bool init(const char* title, int height, int width);
		bool initShaders();
		bool initBullet();
		void clean();
		void update();
		void render();
		void calculateDeltaTime();

		bool Running() { return isRunning && !glfwWindowShouldClose(window); };
		float DeltaTime() { return deltaTime; };

	private:
		static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
		bool isRunning = true;

		double deltaTime;
		double NOW;
		double LAST;

		GLFWwindow *window;
};

#endif
