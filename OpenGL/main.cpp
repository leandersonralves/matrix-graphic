#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, IBO, shader, uniformModel, uniformProjection;

// Vertex Shader
static const char* vShader = "													\n\
#version 330																	\n\
																				\n\
layout (location = 0) in vec3 pos;												\n\
																				\n\
out vec4 vecColor;																\n\
																				\n\
uniform mat4 model;																\n\
uniform mat4 projection;														\n\
																				\n\
void main ()																	\n\
{																				\n\
	gl_Position = projection * model * vec4(pos.x, pos.y, pos.z, 1.0);			\n\
	vecColor = vec4(clamp(pos, 0, 1), 1.0);										\n\
}";


// Fragment Shader
static const char* fShader = "											\n\
#version 330															\n\
																		\n\
in vec4 vecColor;														\n\
out vec4 colour;														\n\
																		\n\
void main ()															\n\
{																		\n\
	colour = vecColor;													\n\
}";

void CreateTriangle()
{
	unsigned int indices[] = {
			0, 1, 4,
			0, 2, 1,
			0, 3, 2,
			0, 4, 3,
			1, 3, 2,
			1, 4, 3
	};

	//Vertices coordinates.
	GLfloat vertices[] = {
		0.0f, 1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,
		-0.5f, 0.0f, 0.5f,
		-0.5f, 0.0f, -0.5f,
		0.5f, 0.0f, -0.5f,
	};

#pragma region IOB
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
#pragma endregion

#pragma region  VAO
	//ONLY Generate a VAO name on graphics card, array object.
	glGenVertexArrays(1, &VAO);
	//Bind the specified VAO name.
	glBindVertexArray(VAO);
#pragma endregion

#pragma region VBO
	//ONLY Generate a VBO name on graphics card, buffer object.
	glGenBuffers(1, &VBO);
	//Bind the specified buffer object.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
#pragma endregion

#pragma region CREATING BUFFER AND ARRAY VERTEX
	//Create and Init a buffer data, with a specified TYPE BUFFER, SIZE, DATA and TYPE OPERATION
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Specify the location and data format of the array of generic vertex attribute at a INDEX to used when rendering
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//Enable a specified vertext attributes, If enabled, the generic vertex attribute array is used when glDrawArrays, glMultiDrawArrays, glDrawElements, glMultiDrawElements, or glDrawRangeElements is called.
	glEnableVertexAttribArray(0);

	//Bind a Buffer, in this case BIND the buffer to vertex attributes above.
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Bind a vertex array, in this case bind the vertex above.
	glBindVertexArray(0);
#pragma endregion

}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	//Create a shader with specified type.
	GLuint theShader = glCreateShader(shaderType);

	//Converting char to GLchar and getting your size.
	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	//Define the source code (string) to a specified shader created above.
	glShaderSource(theShader, 1, theCode, codeLength);
	//Compile shader.
	glCompileShader(theShader);

	//Memory to store the compiling shader log.
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	//Return a specified data from shader, in this case status compilation.
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	//Attach a shader (VERTEX, FRAGMENT, GEOMETRY, TESS and away) to a program shader (COMPLETE SHADER).
	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	//Create a GL Program, linking a shader to a specified program and checking compatibility.
	shader = glCreateProgram();
	if (!shader)
	{
		printf("Error creating shader program!\n");
		return;
	}

	//Function to add shader to a GL program.
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	//Link a shader to a program shader (COMPLETE SHADER), and check some error.
	glLinkProgram(shader);
	//Getting a data from program shader, in this case a link status.
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	//Validade the program shader, considering the current state of OpenGL.
	glValidateProgram(shader);
	//Getting a data from program shader, in this case a validate status.
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");
	uniformProjection = glGetUniformLocation(shader, "projection");
}

int main()
{
	//Initializing GLFW, open-source multiplataform library to OpenGL, OpenGL ES and Vulkan development on the desktop.
	if (!glfwInit())
	{
		printf("GLFW Init failed!");
		glfwTerminate();
		return 1;
	}

	//Defining Major version to run this context.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

	//Defining Minor version requirement to run this context.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//Defining which Profile from OpenGL will use, If requesting an OpenGL version below 3.2, GLFW_OPENGL_ANY_PROFILE must be used.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//If GLFW_TRUE all functionality DEPRECATED are REMOVED, this must only be used if the requested OpenGL version is 3.0 or above.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

	//Creating Window in specified resolution (WIDTH and HEIGHT)
	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "First OPENGL", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	int bufferWidth, bufferHeight;
	//Querying framebuffer Size from window, because can be different.
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//Put the Window on the Current Context, in multi-thread only once windows must be in Current Context.
	glfwMakeContextCurrent(mainWindow);

	//https://community.khronos.org/t/glewexperimental/72738
	//Allow extension entry points to be loaded even if the extension isn’t present in the driver’s extensions string
	glewExperimental = GL_TRUE;

	//OpenGL Extension Wrangler Library, to manage in runtime extensions from OpenGL compatible with target plataform.
	if (glewInit() != GLEW_OK)
	{
		printf("GLEW init failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//Enable depth test, wich are disable by Default.
	glEnable(GL_DEPTH_TEST);

	//Specify the viewport on current window
	glViewport(0, 0, bufferWidth, bufferHeight);

	//Create a triangle mesh (VAO, VBO), to be rendered.
	CreateTriangle();

	//Compile the Shaders and send to GPU.
	CompileShaders();

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f);

	float toRadians = 3.141516f / 180.0f;
	float currentDegrees = 0.0f;
	float angleIncrement = 3.75f;

	float currentScale = 0.1f;
	float scaleIncrement = 0.01f;
	bool isIncreasing = true;
	float maxScale = 1.0f;
	float minScale = 0.1f;

	bool direction = true;
	float triOffset = 0.0f;
	float triMaxOffset = 0.95f;
	float triIncrement = 0.025f;

	//loop while window is open.
	while (!glfwWindowShouldClose(mainWindow))
	{
		//Getting all events in window.
		glfwPollEvents();

		if (direction)
		{
			triOffset += triIncrement;
		}
		else
		{
			triOffset -= triIncrement;
		}

		if (abs(triOffset) > triMaxOffset)
		{
			direction = !direction;
		}

		if (currentDegrees > 360.0f)
		{
			currentDegrees -= 360.0f;
		}
		currentDegrees += angleIncrement;

		if ((currentScale > maxScale && isIncreasing) || (currentScale < minScale && !isIncreasing))
		{
			isIncreasing = !isIncreasing;
		}

		if (isIncreasing)
		{
			currentScale += scaleIncrement;
		}
		else
		{
			currentScale -= scaleIncrement;
		}

		//Specified a clear color, to be draw when glClear in called.
		glClearColor(115.0f / 255.0f, 95.0f / 255.0f, 186.0f / 255.0f, 1.0f);
		//Clear a specified BUFFER (Color, Depth or Stencil)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Define a shader program to be executed by GPU processors.
		glUseProgram(shader);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(triOffset, 0.0f, -2.5f));
		model = glm::rotate(model, currentDegrees * toRadians, glm::vec3(1.0f, 0.3f, 0.75f));
		model = glm::scale(model, glm::vec3(0.5));// currentScale, currentScale, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		//Bind (Callbatch) a vertex array
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		//Clear the current program used.
		glUseProgram(0);

		//Swap between the front and back screen buffer.
		glfwSwapBuffers(mainWindow);
	}

	return 0;
}