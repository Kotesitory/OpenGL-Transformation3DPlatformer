#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Chunk.h"
#include "Camera.h"
#include "ChunkManager.h"
#include "Sphere.h"
#include "SphereWrapper.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const float SCALE = 15.0f;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// camera
static Camera camera(glm::vec3(100.0f));
static float lastX = SCR_WIDTH / 2.0f;
static float lastY = SCR_HEIGHT / 2.0f;
static bool firstMouse = true;


glm::mat4 view = glm::mat4(1.0f);
float z;
bool jump = false;

static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
		std::cout << "glewInit() Error!" << std::endl;

	glfwSwapInterval(1);

	std::cout << glGetString(GL_VERSION) << std::endl;
	GLCall(glEnable(GL_DEPTH_TEST));


	/*float vertices[] = {
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, //Front down left
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, //Front down right
			0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, //Front up right
		-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, //Front up left

		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, //Back down left
			0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, //Back down right
			0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, //Back up right
		-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f  //Back up left
	};

	unsigned int indices[] = {
		0, 1, 2,	//Front face
		2, 3, 0,

		4, 5, 6,	//Back face
		6, 7, 4,

		1, 5, 6,	//Right face
		6, 2, 1,

		0, 4, 7,	//Left face
		7, 3, 0,

		3, 2, 6,	//Top face
		6, 7, 3,

		0, 1, 5,	//Bottom face
		5, 4, 0
	};*/

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	ChunkManager::Init(camera.Position, SCALE);
	std::vector<float> points = ChunkManager::Update(camera, glm::vec3(1.0f), glm::vec3(1.0f)).first;

	Shader shader("res/shaders/vertex.shader", "res/shaders/fragment.shader");
	VertexArray va;
	VertexBuffer vb(nullptr, points.size() * sizeof(float));
	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(3);
	va.AddBuffer(vb, layout);

	va.Bind();
	shader.Bind();


	glm::mat4 view = glm::mat4(1.0f);
	// note that we're translating the scene in the reverse direction of where we want to move
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(65.0f), 800.0f / 800.0f, 1.0f, 100.0f);

	Sphere s(0.5f, 36, 18);
	SphereWrapper sphere(s);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		processInput(window);

		shader.Bind();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), SCR_WIDTH * 1.0f / SCR_HEIGHT, 4.0f, 1000.0f);
		shader.SetUniformMat4f("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		shader.SetUniformMat4f("view", view);

		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(SCALE));
		shader.SetUniformMat4f("model", model);

		shader.SetUniformVec3("viewPosition", camera.Position);
		shader.SetUniformVec3("lightPosition", camera.Position);

		float x = 400, y = 400;
		glm::vec3 near(x, y, 0.1f);
		glm::vec3 far(x, y, 1);

		near = glm::unProject(near, view, projection, glm::vec4(0, 0, 800, 800));
		far = glm::unProject(far, view, projection, glm::vec4(0, 0, 800, 800));
		near /= SCALE;
		far /= SCALE;
		glm::vec3 dir = far - near;
		std::pair<std::vector<float>, glm::vec3> update = ChunkManager::Update(camera, near, far);
		points = update.first;
		vb.SubData(&points[0], points.size() * sizeof(float));
		
		//Drawing terrain
		va.Bind();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, points.size() / 6));

		//Drawing pointing sphere
		points = sphere.getVertices();
		model = glm::translate(model, update.second);
		shader.SetUniformMat4f("model", model);

		vb.SubData(&points[0], points.size() * sizeof(float));
		GLCall(glDrawArrays(GL_TRIANGLES, 0, points.size() / 6));

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	ChunkManager::Stop();
	exit(0);
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width
	// and height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposd, double yposd) {
	float xpos = static_cast<float>(xposd);
	float ypos = static_cast<float>(yposd);
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset =
		lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
