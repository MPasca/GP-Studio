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

#include "MediaPlayer.h"

extern "C" {
	_declspec(dllexport) int NvOptimusEnablement = 0x00000001;
}

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
glm::mat4 lightRotation;
GLfloat lightAngle;

gps::Model3D lightCube;

// shadow
GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap;

glm::vec3 bookPos = glm::vec3(-3.0f, -1.0f, 0.02f);
bool moveBook;

// tv light parameters
bool isTVOn;
glm::vec3 tvPos;
glm::vec3 tvLightDir;

GLint tvPosLoc;
GLint tvLightDirLoc;
GLint isTVonLoc;

// door opening
bool openDoor;
glm::vec3 doorPos = glm::vec3(3.51f, 1.16f, 3.15f);

// spilling cereal
bool cerealSpilled;
glm::vec3 cerealPos = glm::vec3(0.039f, 0.033f, -4.158f);

// smoke/fog
bool isSmokey;
GLint isSmokeyLoc;

// day cycles
bool isDay;

// wall lights
glm::vec3 wallLightsPositions[] = {
	glm::vec3(4.3627f,  5.8728f,  3.39823f),
	glm::vec3(3.78756f, 5.68063f, 2.98536f),
	glm::vec3(4.75649f,  5.71798f, 2.96634f),
	glm::vec3(4.4341f,  5.67128f, 2.72644f),
	glm::vec3(3.75614, 5.58234, 2.30304),
	glm::vec3(4.10425, 5.58855, 2.26019),
	glm::vec3(4.53368, 5.61986, 1.93849),
	glm::vec3(3.75809, 5.53615, 1.60692),
	glm::vec3(4.24105, 5.45638, 1.63722),
	glm::vec3(4.78422, 5.49807, 1.39514),
	glm::vec3(4.54942, 5.33139, 1.01812),
	glm::vec3(3.73847, 5.29568, 0.904004),
	glm::vec3(4.22911, 5.37028, 0.42218)
};

// sound
audio::MediaPlayer mediaPlayer = audio::MediaPlayer(irrklang::vec3df(-9.99f, -51.555f, 6.6016f));
bool playSound;

audio::MediaPlayer tvPlayer = audio::MediaPlayer(irrklang::vec3df(-3.088f, 1.14f, -2.725f));

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;


// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, -10.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
glm::vec3 cameraSize = glm::vec3(1.0f, 1.0f, 1.0f);

GLfloat cameraSpeed = 0.3f;
GLfloat ogSpeed = 0.1f;
GLfloat SPEEDSpeed = 0.5f;

// virtual tour
// move/rotate	- direction	- value
enum action { move, rotate };
enum direction { forward, left, right };

bool onTour;
int tourStep = 0;

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
gps::Model3D tv;
gps::Model3D brickWall;
gps::Model3D halfWall;

gps::Model3D screenQuad;
gps::Model3D playerSphere;

gps::Model3D speakers;
gps::Model3D wallLights;

gps::Model3D ground;
gps::Model3D walls;
gps::Model3D windows;
GLfloat angle;

// shaders
gps::Shader skyBoxShader;
gps::Shader sceneShader;
gps::Shader depthMapShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;

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

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;


	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		if (length(myCamera.getCameraPosition() - doorPos) <= 1.5f) {
			openDoor = !openDoor;
		}
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		if (length(myCamera.getCameraPosition() - tvPos) <= 10.0f) {
			isTVOn = !isTVOn;
		}

		if (isTVOn) {
			tvPlayer.playSoundEffect("tv_static");
		}
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
		playSound = !playSound;

		if (playSound) {
			mediaPlayer.playSong();
		}
		else {
			mediaPlayer.pauseSong();
		}
	}

	if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		isDay = !isDay;
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		isSmokey = !isSmokey;
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		mediaPlayer.volUp();
	}

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		mediaPlayer.volDown();
	}

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		mediaPlayer.prevSong();
	}

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		mediaPlayer.nextSong();
	}

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		moveBook = !moveBook;
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

	if (onTour) return;

	float yaw, pitch;

	yaw = (xpos - prevX) * 0.01f;

	pitch = (prevY - ypos) * 0.01f;

	// so that it doesn't lock
	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	else if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	myCamera.rotate(yaw, pitch);
	view = myCamera.getViewMatrix();

	//for (int i = 0; i < 4; i++) {
	//	for (int j = 0; j < 4; j++) {
	//		std::cout << view[i][j] << " ";
	//	}
	//	std::cout << "\n";
	//}

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glCheckError();

	prevY = ypos;
	prevX = xpos;


	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		moveBook = !moveBook;

		if (length(myCamera.getCameraPosition() - cerealPos) <= 1.5f) {
			cerealSpilled = !cerealSpilled;
		}
	}
}

float movementPoints = 0;
void processMovement() {
	if (onTour) {
		switch (tourStep) {
		case 0:
			myCamera.resetCamera();
			movementPoints = 30;
			tourStep++;
			break;
		case 1:
			if (movementPoints <= 0) {
				tourStep++;
				movementPoints = 40;
				//z = false;
				//tourStep = 0;
				break;
			}
			myCamera.rotate(0.01f, 0.0f);
			movementPoints--;
			break;

		case 2:
			if (movementPoints <= 0) {
				tourStep++;
				movementPoints = 500;
				//onTour = false;
				//tourStep = 0;
				break;
			}
			myCamera.move(gps::MOVE_FORWARD, 0.2f);
			movementPoints--;
			break;

		case 3:
			if (movementPoints <= 0) {
				tourStep++;
				movementPoints = 30;
				//onTour = false;
				//tourStep = 0;
				break;
			}
			myCamera.rotate(0.01f, 0.0f);
			movementPoints--;
			break;

		case 4:
			if (movementPoints <= 0) {
				//tourStep++;
				onTour = false;
				tourStep = 0;
				break;
			}
			myCamera.move(gps::MOVE_FORWARD, 0.1f);
			movementPoints--;
			break;
		}
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	// --------- on tour boiii

	if (pressedKeys[GLFW_KEY_Z]) {
		onTour = true;
	}

	if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
		cameraSpeed = SPEEDSpeed;
	}
	else {
		cameraSpeed = ogSpeed;
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glCheckError();
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

	if (pressedKeys[GLFW_KEY_B]) {
		std::cout << "light pos xyz:" << lightDir.x << " " << lightDir.y << " " << lightDir.z << "\n";
		std::cout << "camera pos xyz: " << myCamera.getCameraPosition().x << " " << myCamera.getCameraPosition().y << " " << myCamera.getCameraPosition().z << "\n";
		std::cout << "light angle: " << lightAngle;
	}

	if (pressedKeys[GLFW_KEY_KP_8]) {
		lightDir.y -= cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
	}

	if (pressedKeys[GLFW_KEY_KP_5]) {
		lightDir.y += cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
	}

	if (pressedKeys[GLFW_KEY_KP_4]) {
		lightDir.x -= cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
	}

	if (pressedKeys[GLFW_KEY_KP_6]) {
		lightDir.x += cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
	}

	if (pressedKeys[GLFW_KEY_KP_1]) {
		lightDir.z -= cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
	}

	if (pressedKeys[GLFW_KEY_KP_3]) {
		lightDir.z += cameraSpeed;
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
	}

	// viewing modes

	if (pressedKeys[GLFW_KEY_L]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_P]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (pressedKeys[GLFW_KEY_N]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_O]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_SMOOTH);
	}

	if (pressedKeys[GLFW_KEY_R]) {
		onTour = false;
	}

	// sit on couch
	// -7.12275 1.55587 -3.20282  
	if (pressedKeys[GLFW_KEY_C]) {
		myCamera.resetAngles();

		myCamera.setCameraPosition(glm::vec3(-7.123f, 0.55f, -3.20f));
		myCamera.setCameraTarget(glm::vec3(-3.361f, 0.11f, -2.97f));
		myCamera.setCameraDirection(glm::vec3(0, -1, 0));
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	}

	mediaPlayer.setListenerPosition(myCamera.getCameraPosition(), myCamera.getCameraDirection(), myCamera.getCameraUp());
	tvPlayer.setListenerPosition(myCamera.getCameraPosition(), myCamera.getCameraDirection(), myCamera.getCameraUp());
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
	glClearColor(0.541f, 0.525f, 0.556f, 1.0f);

	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
	ratio = myWindow.getWindowDimensions().width / myWindow.getWindowDimensions().height;

	ogX = myWindow.getWindowDimensions().width / 2;
	ogY = myWindow.getWindowDimensions().height / 2;
	prevX = ogX, prevY = ogY;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initScene() {
	ground.LoadModel("models/ground/bathroomGND.obj");
	ground.LoadModel("models/ground/kitchenGND.obj");
	ground.LoadModel("models/ground/livingRoomGND.obj");

	scene.LoadModel("models/table/table.obj");
	scene.LoadModel("models/couch/couch.obj");
	scene.LoadModel("models/bed/bed.obj");
	scene.LoadModel("models/bar_stools/bar_stools.obj");
	scene.LoadModel("models/fridge/fridge.obj");
	scene.LoadModel("models/kitchen/kitchen.obj");
	scene.LoadModel("models/oven/oven.obj");

	scene.LoadModel("models/TV/TV_table.obj");
	scene.LoadModel("models/TV/TV1.obj");
	scene.LoadModel("models/TV/TV2.obj");
	scene.LoadModel("models/TV/TV4.obj");
	scene.LoadModel("models/TV/TV5.obj");
	scene.LoadModel("models/TV/TV6.obj");

	scene.LoadModel("models/bathroom_door/entrance_door.obj");

	walls.LoadModel("models/walls/walls.obj");
}

void initModels() {
	//windows.LoadModel("models/windows/windowGlass.obj");

	door.LoadModel("models/bathroom_door/bathroom_door.obj");
	cerealBox.LoadModel("models/cereal_box/cereal_box.obj");
	book.LoadModel("models/book/book.obj");		// de refacut texturile
	speakers.LoadModel("models/speakers/speaker.obj");

	wallLights.LoadModel("models/wallLight/wallLights.obj");

	brickWall.LoadModel("models/brickWalls/brickwalls.obj");
	halfWall.LoadModel("models/brickWalls/bar.obj");
	halfWall.LoadModel("models/brickWalls/table.obj");

	tv.LoadModel("models/TV/TV3_cover.obj");

	windows.LoadModel("models/windows/windowPane.obj");

	plant.LoadModel("models/plant/plant.obj");
	plant.LoadModel("models/plant/hanging_plant.obj");

	playerSphere.LoadModel("models/player.obj");
	initScene();

	lightCube.LoadModel("models/cube/cube.obj");
	screenQuad.LoadModel("models/quad/quad.obj");

}

void initShaders() {
	sceneShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
	sceneShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	skyBoxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag"); // init the shader

	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();

	depthMapShader.loadShader("shaders/depthShader.vert", "shaders/depthShader.frag");
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
	glCheckError();

	// compute normal matrix for teapot
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(sceneShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
		(float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
		0.1f, 100.0f);
	projectionLoc = glGetUniformLocation(sceneShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	// -8.49999f, 0.235001f, 0.137999f
	// -17.6f, 0.835001f, -0.062001f
	// -8.19999f, -0.064999f, -0.062001f
	lightDir = glm::vec3(-17.6f, 0.835001f, -0.062001f);

	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(sceneShader.shaderProgram, "lightDir");
	glCheckError();
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
	glCheckError();
	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // white light
	lightColorLoc = glGetUniformLocation(sceneShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	// tv light uniforms
	tvPos = glm::vec3(-3.0f, 1.1f, 3.0f);

	isTVonLoc = glGetUniformLocation(sceneShader.shaderProgram, "isTVon");
	glUniform1i(isTVonLoc, isTVOn);

	isSmokeyLoc = glGetUniformLocation(sceneShader.shaderProgram, "isSmokey");
	glUniform1i(isSmokeyLoc, isSmokey);

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
}

void initFBO() {
	//generate FBO ID 
	glGenFramebuffers(1, &shadowMapFBO);	// FBO ID

	glGenTextures(1, &depthMapTexture);	// depth texture for FBO
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	// 1st arg: texture target \
	   2nd arg: mipmap level \
	   3rd arg: the format (depth map) \
	   4th, 5th arg: width, height \
	   6th arg: always 0 \
	   7th arg: format (depth map) \
	   8th arg: data type (float) \
	   9th arg: actual image data

	// texture filtering when minimizing/magnifying\
		GL_NEAREST - returns the value of the closest point
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// depth map's border colour
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//		this is for unwanted visual artifacts when generating the shadows (coordinates that pass the border take its colour)\
		glTexParameteri(texture_target, axis, texture_wrapping)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// attach texture to FBO 
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	// these are needed, but depth mapping doesn't require colour or stencil
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// once the buffer is completed, it should be unbound til it's actually used
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void wallLightsUniforms() {
	view = myCamera.getViewMatrix();

	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0, -1.5, 0));

	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[0]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[0]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[1]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[1]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[2]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[2]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[3]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[3]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[4]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[4]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[5]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[5]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[6]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[6]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[7]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[7]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[8]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[8]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[9]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[9]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[10]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[10]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[11]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[11]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[12]"), 1, glm::value_ptr(glm::mat3(view * model) * wallLightsPositions[12]));

	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[0]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[0]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[1]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[1]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[2]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[2]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[3]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[3]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[4]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[4]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[5]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[5]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[6]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[6]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[7]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[7]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[8]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[8]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[9]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[9]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[10]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[10]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[11]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[11]));
	//glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[12]"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * wallLightsPositions[12]));


	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[0]"), 1, glm::value_ptr(wallLightsPositions[0]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[1]"), 1, glm::value_ptr(wallLightsPositions[1]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[2]"), 1, glm::value_ptr(wallLightsPositions[2]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[3]"), 1, glm::value_ptr(wallLightsPositions[3]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[4]"), 1, glm::value_ptr(wallLightsPositions[4]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[5]"), 1, glm::value_ptr(wallLightsPositions[5]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[6]"), 1, glm::value_ptr(wallLightsPositions[6]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[7]"), 1, glm::value_ptr(wallLightsPositions[7]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[8]"), 1, glm::value_ptr(wallLightsPositions[8]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[9]"), 1, glm::value_ptr(wallLightsPositions[9]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[10]"), 1, glm::value_ptr(wallLightsPositions[10]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[11]"), 1, glm::value_ptr(wallLightsPositions[11]));
	glUniform3fv(glGetUniformLocation(sceneShader.shaderProgram, "pointLights[12]"), 1, glm::value_ptr(wallLightsPositions[12]));
}

glm::mat4 computeLightSpaceTrMatrix() {
	lightAngle += glfwGetTime() * 0.0001f;

	glm::mat4 locLightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glCheckError();
	glm::mat4 lightView = glm::lookAt(glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glCheckError();
	const GLfloat near_plane = 0.1f, far_plane = 40.0f;
	glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);

	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	glCheckError();

	return lightSpaceTrMatrix;
}

void drawObj(gps::Shader shader, bool depthPass) {
	shader.useShaderProgram();
	//model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0, -1.5, 0));
	glCheckError();

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glCheckError();
		glUniform1i(isSmokeyLoc, isSmokey);
		glUniform1i(isTVonLoc, isTVOn);
		glUniform1i(glGetUniformLocation(sceneShader.shaderProgram, "isDay"), isDay);
	}

	if (openDoor) {
		glm::mat4 doorModel = glm::mat4(1.0f);
		doorModel = glm::translate(							// T-1
			glm::rotate(									// R
				glm::translate(model, doorPos),				// T
				glm::radians(60.0f), glm::vec3(0, 1.0f, 0)), -doorPos);
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(doorModel));
		glCheckError();
		door.Draw(shader);
	}

	if (cerealSpilled) {
		glm::mat4 cerealModel = glm::mat4(1.0f);
		cerealModel = glm::translate(							// T-1
			glm::rotate(										// R
				glm::translate(model, cerealPos),				// T
				glm::radians(90.0f), glm::vec3(1, 0, 0)), -cerealPos);
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(cerealModel));
		glCheckError();
		cerealBox.Draw(shader);
	}

	glm::mat4 playerModel = glm::mat4(1.0f);
	playerModel = glm::translate(playerModel, myCamera.getCameraPosition());
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(playerModel));
	playerSphere.Draw(shader);

	if (moveBook) {
		glm::mat4 bookModel = glm::mat4(1.0f);
		bookModel = glm::translate(							// T-1
			glm::rotate(										// R
				glm::translate(model, bookPos),				// T
				glm::radians(90.0f), glm::vec3(0, 0, -1)), -bookPos);
		bookModel = glm::translate(bookModel, glm::vec3(1.0f, 1.0f, 1.0f));
		//bookModel = glm::translate(bookModel, myCamera.getCameraPosition());


		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(bookModel));
		book.Draw(shader);
	}
	else {
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		book.Draw(shader);
	}

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glCheckError();
	scene.Draw(shader);
	halfWall.Draw(shader);
	brickWall.Draw(shader);

	speakers.Draw(shader);
	wallLights.Draw(shader);

	walls.Draw(shader);

	windows.Draw(shader);

	ground.Draw(shader);

	plant.Draw(shader);

	if (!cerealSpilled) {
		cerealBox.Draw(shader);
	}
	if (!openDoor) {
		door.Draw(shader);
	}
}

void renderScene() {
	//glCullFace(GL_FRONT);

	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glCheckError();


	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawObj(depthMapShader, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glCullFace(GL_BACK);

	// render depth map on screen - toggled with the M key
	// to solve: invalid operation when moving the light in showDepthMap mode

	if (showDepthMap) {
		glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sceneShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(sceneShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(sceneShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObj(sceneShader, false);

		if (!isTVOn) {
			tv.Draw(sceneShader);
			tvPlayer.pauseSong();
		}

		//draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);

		skyBox.Draw(skyBoxShader, view, projection);

		sceneShader.useShaderProgram();

	}
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);

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
	skyBox.Load(faces);				 // loads the faces	
	skyBoxShader.useShaderProgram();
	view = myCamera.getViewMatrix();	// camera view
	glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));
}

void initMediaPlayer() {
	mediaPlayer.addAudioFile("Beck - Loser");
	mediaPlayer.addAudioFile("Joy Division - Transmission");
	mediaPlayer.addAudioFile("New Order - Blue Monday");
	mediaPlayer.addAudioFile("The Cure - Boys Don't Cry");
	mediaPlayer.addAudioFile("Smashing Pumpkins - Today");
	mediaPlayer.addAudioFile("Sonic Youth - Teen Age Riot");
}

int main(int argc, const char* argv[]) {

	try {
		initOpenGLWindow();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	isTVOn = false;
	openDoor = false;
	cerealSpilled = false;
	playSound = false;
	isSmokey = false;

	moveBook = false;

	isDay = true;

	initOpenGLState();
	initModels();
	initShaders();
	initUniforms();

	initMediaPlayer();

	initSkybox();
	initFBO();
	setWindowCallbacks();

	glCheckError();

	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
		processMovement();

		//if (lightAngle - 1 > 0) {
		//	isDay = !isDay;
		//	
		//}

		renderScene();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

	return EXIT_SUCCESS;
}
