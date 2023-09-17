#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "LightMode.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, int key, int scancode, int action, int mods);
unsigned int loadTexture(const char* resourcePath);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // the initial light cube position

// Shaders
Shader* lightingShader;
Shader* lightCubeShader;

// Wireframe toggle
bool wireframeToggle = false;

// Point light intensity configuration
const float constant = 1.0f;
const float linear = 0.7f;
const float quadratic = 1.8f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "LearnOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	// Setting callbacks
	// Window resize
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Cursor position
	glfwSetCursorPosCallback(window, mouse_callback);
	// Scroll movement
	glfwSetScrollCallback(window, scroll_callback);
	// Key press and release
	glfwSetKeyCallback(window, processInput);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	// ------------------------------------
	lightingShader = new Shader("Assets\\Shaders\\1.colors.vs", "Assets\\Shaders\\1.colors.fs");
	lightCubeShader = new Shader("Assets\\Shaders\\1.light_cube.vs", "Assets\\Shaders\\1.light_cube.fs");

	// Material settings
	Material material = {};

	// Cyan plastic
	material.diffuse = { 0.0f, 0.50980392f, 0.50980392f };
	material.specular = { 0.50196078f, 0.50196078f, 0.50196078f };
	material.shininess = 32.0f;

	// Light settings
	Light pointLight = {};
	Light spotLight = {};
	Light dirLight = {};

	pointLight.ambient = { 0.2f, 0.2f, 0.2f };
	pointLight.diffuse = { 0.5f, 0.5f, 0.5f };
	pointLight.specular = { 1.0f, 1.0f, 1.0f };

	spotLight.ambient = { 0.1f, 0.1f, 0.1f };
	spotLight.diffuse = { 0.8f, 0.8f, 0.8f };
	spotLight.specular = { 1.0f, 1.0f, 1.0f };

	dirLight.ambient = { 0.2f, 0.2f, 0.2f };
	dirLight.diffuse = { 0.5f, 0.5f, 0.5f };
	dirLight.specular = { 1.0f, 1.0f, 1.0f };

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// normals (surfaces for calculating the materials)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	// texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	// Load diffuse map image.
	unsigned int diffuseMap = loadTexture("Assets\\Images\\container2.png");

	// load the specular map image.
	unsigned int specularMap = loadTexture("Assets\\Images\\container2_specular.png");

	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader->use();
		lightingShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		lightingShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader->setVec3("viewPos", camera.Position);

		// Activate the first texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		// The second one, where the shininess is present
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		
		// Set the material
		lightingShader->setInt("material.diffuse", 0);
		lightingShader->setInt("material.specular", 1);
		lightingShader->setInt("material.emission", 2);
		lightingShader->setFloat("material.shininess", material.shininess);

		// directional light
		lightingShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		//lightingShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		//lightingShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);

		lightingShader->setVec3("dirLight.ambient", 0.01f, 0.01f, 0.2f);
		lightingShader->setVec3("dirLight.diffuse", 0.01f, 0.01f, 0.2f);
		lightingShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		lightingShader->setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader->setFloat("pointLights[0].constant", constant);
		lightingShader->setFloat("pointLights[0].linear", linear);
		lightingShader->setFloat("pointLights[0].quadratic", quadratic);
		// point light 2
		lightingShader->setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader->setFloat("pointLights[1].constant", constant);
		lightingShader->setFloat("pointLights[1].linear", linear);
		lightingShader->setFloat("pointLights[1].quadratic", quadratic);
		// point light 3
		lightingShader->setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader->setFloat("pointLights[2].constant", constant);
		lightingShader->setFloat("pointLights[2].linear", linear);
		lightingShader->setFloat("pointLights[2].quadratic", quadratic);
		// point light 4
		lightingShader->setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader->setFloat("pointLights[3].constant", constant);
		lightingShader->setFloat("pointLights[3].linear", linear);
		lightingShader->setFloat("pointLights[3].quadratic", quadratic);
		// spotLight
		lightingShader->setVec3("spotLight.position", camera.Position);
		lightingShader->setVec3("spotLight.direction", camera.Front);
		lightingShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader->setFloat("spotLight.constant", 1.0f);
		lightingShader->setFloat("spotLight.linear", 0.09f);
		lightingShader->setFloat("spotLight.quadratic", 0.032f);
		lightingShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(7.5f)));
		lightingShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(12.5f)));

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader->setMat4("projection", projection);
		lightingShader->setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader->setMat4("model", model);

		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader->setMat4("model", model);

			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// point light
		lightCubeShader->use();
		lightCubeShader->setMat4("projection", projection);
		lightCubeShader->setMat4("view", view);

		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightCubeShader->setMat4("model", model);

			glBindVertexArray(lightCubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);

	delete lightingShader;
	delete lightCubeShader;

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, true);

	if (key == GLFW_KEY_W)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (key == GLFW_KEY_S)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (key == GLFW_KEY_A)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (key == GLFW_KEY_D)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
		if (!wireframeToggle) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframeToggle = true;
		}
		else {
			wireframeToggle = false;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(const char* resourcePath) {
	unsigned int textureID = -1;
	int width, height, nrChannels;

	unsigned char* data = stbi_load(resourcePath, &width, &height, &nrChannels, 0);

	if (data) {
		GLuint format = 0;

		if (nrChannels == 1) {
			format = GL_RED;
		}
		else if (nrChannels == 3) {
			format = GL_RGB;
		}
		else if (nrChannels == 4) {
			format = GL_RGBA;
		}

		glGenTextures(1, &textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// free the loaded container image.
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Unable to load image data. Path: " << resourcePath << std::endl;
	}

	return textureID;
}
