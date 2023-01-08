// GLEW
#define GLEW_STATIC
#include <GL\glew.h>

// GLFW
#include <GLFW\glfw3.h>


// The math library
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Win32Window.h"

#include "Shader.h"

#include "Camera.h"

#include <iostream>

// -------------------->> NOTE <<----------------------
//
// Because m_ScreenWidth, m_ScreenHeight ,and m_Title are static members as declared in Win32Window.h
// they should be defined somewhere and without repeating the word static
// and also we initialized them btw

GLint Win32Window::m_ScreenWidth = 1;
GLint Win32Window::m_ScreenHeight = 1;
const char* Win32Window::m_Title = "";

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement(); 

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
// These two may cause a problem since I am using the value of width/height
// that was obtained from glfwGetFramebufferSize(...)
GLfloat lastX = Win32Window::GetScreenWidth() / 2.0f;
GLfloat lastY = Win32Window::GetScreenWidth() / 2.0f;
bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// The position of the light in our world
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

GLFWwindow *Win32Window::CreateWindow(GLint width, GLint height, const char* title)
{
	if (width <= 0 || height <= 0)
		return nullptr;

	// 2) Initialize GLFW
	if (!glfwInit())
	{
		std::cout << "glfwInit() failed\n";
		return nullptr;
	}

	// 3) Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 4) Creat a GLFWWindow object that we can use for GLFW's functions
	GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	// 5)
	/**
	* This is the actual or the accurate representation of what our window is
	* relative to our screen
	*/
	glfwGetFramebufferSize(window, &m_ScreenWidth, &m_ScreenHeight);
	m_Title = title;

	// 6) Test the window creation status
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}

	// 7) Make the context we created as the current context
	glfwMakeContextCurrent(window);

	/**
	* Now setting the callbacks
	*/
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return window;
}

void Win32Window::Run(GLFWwindow* window)
{

	// 8) Set this to true so GLEW knows to use a modern approach to retrieve function pointer and extentions
	glewExperimental = GL_TRUE;

	// 9) Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return;
	}

	// 10) Define the viewport  dimensions
	glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);

	// These is an addition to show the desired info below:
	std::cout << "OpenGL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "GLSL   version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl << std::endl;

	/** Enable the Blending and the Depth */
	// Enabling alpha support for images: png, .....
	glEnable(GL_BLEND | GL_DEPTH_TEST);
	// Enabling depth for 3d objects
	glEnable(GL_DEPTH_TEST);
	// Pixels can be drawn using a function that blends the incoming (source) RGBA 
	// values with the RGBA values that are already in the frame buffer (the destination values).
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/**
	* Define our vertex data that we are going to draw it
	*/
	// Create a Shader object
	Shader lightingShader("lighting.vs", "lighting.frag");
	Shader lampShader("lamp.vs", "lamp.frag");

	// Set up vertex

	// This vertices are to be used with Perspective projection
	
	GLfloat vertices[] = {
		// Position				// Normals
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,		0.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,		0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	-1.0f, 0.0f, 0.0f,

		0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,

		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f, 0.0f
	};

	/**
	* ---------- NOTE ---------------
	* We are going to have a vertex buffer object that is going to be reused
	* BUT we are going to have different vertex array object for each individual different shader
	*/

	// This refers to the box it self which is "lighting.vs, lighting.frag"
	// Create(Generate) the Vertext Array object and the Vertex Buffer Object
	GLuint boxVAO;
	glGenVertexArrays(1, &boxVAO);
	// Bind the VAO so the next vbo will be bound to it automatically
	glBindVertexArray(boxVAO);
	// Generate the Vertex buffer object ,then bind an area of storage for our VBO
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Define the data for our VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//
	// Pointer to Attribute
	// Tell OpenGL How to Link the VBO data with the a specific attribute in the Vertex Shader.
	//
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); // enabling the vertex attribute array (The attribute)
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	
	// unbinding
	glBindVertexArray(0); // 0 means break (unbinding) the existing vertex array object binding.

	// This refers to the lamp which is "lamp.vs, lamp.frag"
	// Create(Generate) the Vertext Array object and the Vertex Buffer Object
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	// Bind the VAO so the next vbo will be bound to it automatically
	glBindVertexArray(lightVAO);
	// Generate the Vertex buffer object ,then bind an area of storage for our VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Define the data for our VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//
	// Pointer to Attribute
	// Tell OpenGL How to Link the VBO data with the a specific attribute in the Vertex Shader.
	//
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); // enabling the vertex attribute array (The attribute)

	// unbinding
	glBindVertexArray(0); // 0 means break (unbinding) the existing vertex array object binding.


	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)(GetScreenWidth()) / (GLfloat)(GetScreenHeight()), 0.1f, 1000.0f);

	// 11) Now create our game loop
	while (!glfwWindowShouldClose(window))
	{
		/**
		* Now set the frame time
		*/
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;	// The time elapsed between frames
		lastFrame = currentFrame;

		// Check if any events have been activiated(key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Handle the movenent
		DoMovement();

		// Render
		// Clear the colorbuffer and the depth buffer
		//glClearColor(0.25f, 0.51f, 0.51f, 1.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.Use();
		GLint objectColorLoc = glGetUniformLocation(lightingShader.Program, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(lightingShader.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		//glUniform3f(lightColorLoc, 1.0f, 0.5f, 1.0f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		// Create a camera transformation
		glm::mat4 view;
		view = camera.GetViewMatrix();

		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(boxVAO);
		glm::mat4 model;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//2
		lampShader.Use();

		// Create a camera transformation
		view = camera.GetViewMatrix();

		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		

		// Swap the screen buffers
		glfwSwapBuffers(window);

	}

	// deallocating the resources that are out of use
	glDeleteVertexArrays(1, &boxVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// 12) Terminate GLFW, clearing any resources allocated by GLFW
	glfwTerminate();
}


GLint Win32Window::GetScreenWidth()
{
	return m_ScreenWidth;
}
GLint Win32Window::GetScreenHeight()
{
	return m_ScreenHeight;
}


// Handling our method and calling the appropriate methods in Camera class
void DoMovement()
{
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	}
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key <= 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;

		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;		// This is reversed because the y-coordinates start from the bottom left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}
