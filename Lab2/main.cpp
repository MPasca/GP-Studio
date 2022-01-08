#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "D:/Faculta/An 3/Sem 1/PG/glm/glm.hpp"	                    //core glm functionality
#include "D:/Faculta/An 3/Sem 1/PG/glm/gtc/matrix_transform.hpp"    //glm extension for generating common transformation matrices
#include "D:/Faculta/An 3/Sem 1/PG/glm/gtc/matrix_inverse.hpp"      //glm extension for computing inverse matrices
#include "D:/Faculta/An 3/Sem 1/PG/glm/gtc/type_ptr.hpp"            //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include <iostream>
#include "SkyBox.hpp"

// window
gps::Window myWindow;
float ratio;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, -10.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
glm::vec3 cameraSize = glm::vec3(1.0f, 1.0f, 1.0f);

GLfloat cameraSpeed = 0.1f;
GLfloat ogSpeed = 0.1f;
GLfloat SPEEDSpeed = 0.5f;

GLboolean pressedKeys[1024];
float angleY = 0.0f;

double prevX, prevY;
double ogX, ogY;

// models
gps::Model3D door;
gps::Model3D book;
gps::Model3D cerealBox;
gps::Model3D scene;
gps::Model3D plant;
GLfloat angle;

// shaders
gps::Shader skyBoxShader;
gps::Shader sceneShader;
//gps::Shader lightShader;

// skybox
gps::SkyBox skyBox;
std::vector<const GLchar*> faces;

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM:
			error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error = "INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error = "INVALID_OPERATION";
			break;
		case GL_STACK_OVERFLOW:
			error = "STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			error = "STACK_UNDERFLOW";
			break;
		case GL_OUT_OF_MEMORY:
			error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	//fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
	//width = height * ratio;
	glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	float yaw, pitch;

	yaw = (prevY - ypos) * 0.01f;

	pitch = (xpos - prevX) * 0.01f;	// x axis - up, down

	myCamera.rotate(yaw, pitch);
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	prevY = ypos;
	prevX = xpos;
}

void processMovement() {
	/*
	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	}
	*/
	if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
		cameraSpeed = SPEEDSpeed;
	}
	else {
		cameraSpeed = ogSpeed;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	if (pressedKeys[GLFW_KEY_SPACE]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	if (pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	//std::cout << "light pos xyz:" << lightDir.x << " " << lightDir.y << " " << lightDir.z << "\n";
	if (pressedKeys[GLFW_KEY_U]) {
		lightDir.y -= cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightDir.y += cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
	}

	if (pressedKeys[GLFW_KEY_H]) {
		lightDir.x -= cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
	}

	if (pressedKeys[GLFW_KEY_K]) {
		lightDir.x += cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
	}

	if (pressedKeys[GLFW_KEY_O]) {
		lightDir.z -= cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
	}

	if (pressedKeys[GLFW_KEY_P]) {
		lightDir.z += cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
	}
}

void initOpenGLWindow() {
	myWindow.Create(1024, 768, "Final Project");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
	glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);

	glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
	ratio = myWindow.getWindowDimensions().width / myWindow.getWindowDimensions().height;

	ogX = myWindow.getWindowDimensions().width / 2;
	ogY = myWindow.getWindowDimensions().height / 2;
	prevX = ogX, prevY = ogY;

	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initScene() {
	scene.LoadModel("models/ground/bathroomGND.obj");
	scene.LoadModel("models/ground/kitchenGND.obj");
	scene.LoadModel("models/ground/livingRoomGND.obj");
	scene.LoadModel("models/brickWalls/brickwalls.obj");
	scene.LoadModel("models/table/table.obj");
	scene.LoadModel("models/couch/couch.obj");
	scene.LoadModel("models/bed/bed.obj");
	scene.LoadModel("models/bar_stools/bar_stools.obj");
	scene.LoadModel("models/fridge/fridge.obj");
	scene.LoadModel("models/kitchen/kitchen.obj");
	//scene.LoadModel("models/oven/oven.obj");
	scene.LoadModel("models/TV/TV_table.obj");
	scene.LoadModel("models/TV/TV1.obj");
	scene.LoadModel("models/TV/TV2.obj");
	scene.LoadModel("models/TV/TV4.obj");
	scene.LoadModel("models/TV/TV5.obj");
	scene.LoadModel("models/TV/TV6.obj");
	scene.LoadModel("models/walls/walls.obj");
	scene.LoadModel("models/brickWalls/bar.obj");

}

void initModels() {
	scene.LoadModel("models/bathroom_door/bathroom_door.obj");
	scene.LoadModel("models/plant/plant.obj");
	scene.LoadModel("models/cereal_box/cereal_box.obj");
	scene.LoadModel("models/book/book.obj");		// problema la cum imi citeste texturile
	scene.LoadModel("models/TV/TV3_cover.obj");

	initScene();
}

void initShaders() {
	sceneShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
	//sceneShader.loadShader("shaders/sceneShader.vert", "shaders/sceneShader.frag");
	sceneShader.useShaderProgram();
	//lightShader.loadShader("shaders/lightShader.vert", "shaders/lightShader.vert");
	//lightShader.useShaderProgram();
	skyBoxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag"); // init the shader
}

void initUniforms() {
	sceneShader.useShaderProgram();

	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0, -1.5, 0));
	modelLoc = glGetUniformLocation(sceneShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(sceneShader.shaderProgram, "view");
	// send view matrix to shader
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// compute normal matrix for teapot
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(sceneShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
		(float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
		0.1f, 20.0f);
	projectionLoc = glGetUniformLocation(sceneShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(sceneShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(sceneShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	skyBox.Draw(skyBoxShader, view, projection);

	sceneShader.useShaderProgram();

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	scene.Draw(sceneShader);


}

void cleanup() {
	myWindow.Delete();
	//cleanup code for your own data
}


void initSkybox() {
	faces.push_back("textures/skybox/right.tga");	//
	faces.push_back("textures/skybox/left.tga");	//	the cube
	faces.push_back("textures/skybox/top.tga");		//	textures
	faces.push_back("textures/skybox/bottom.tga");	//	and stuff
	faces.push_back("textures/skybox/back.tga");	//
	faces.push_back("textures/skybox/front.tga");	//
	skyBox.Load(faces);	// loads the faces
	skyBoxShader.useShaderProgram();
	view = myCamera.getViewMatrix();	// camera view
	glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));
}

int main(int argc, const char* argv[]) {

	try {
		initOpenGLWindow();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	initSkybox();
	initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
	setWindowCallbacks();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

	return EXIT_SUCCESS;
}
