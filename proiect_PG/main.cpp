#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "glm/gtx/string_cast.hpp"
#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include <iostream>

// window
gps::Window myWindow;
int currentWidth = 800, currentHeight = 600;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightDirColor;
glm::vec3 lightPosColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightDirColorLoc;
GLint lightPosColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 8.0f, 7.0f),
    glm::vec3(0.0f, 0.0f, -10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.5f;
int glWindowWidth = 800;
int glWindowHeight = 600;
float lastX = glWindowWidth / 2.0f;
float lastY = glWindowHeight / 2.0f;
bool firstMouse = true;
const float sensitivity = 0.1f;
float yaw = -90.0f;
float pitch = 4.0f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
GLfloat angle;

gps::Model3D scene;
gps::Model3D airPlane;
gps::Model3D containers;

// shaders
gps::Shader myBasicShader;


//ways to render
bool wireFrame = false;
bool smoothFrame = false;

float moveY = 0.0;
float moveZ = 0.0;


float redColor = 1.0f;
float greenColor = 1.0f;
float blueColor = 1.0f;
float poleColor = 1.0f;

GLint pointLightPosLoc;
GLint pointLightPosLoc1;
GLint pointLightPosLoc2;
GLint pointLightPosLoc3;

//shadows
GLuint shadowMapFBO;
GLuint depthMapTexture;
const unsigned int SHADOW_WIDTH = 8192; 
const unsigned int SHADOW_HEIGHT = 8192; 
gps::Shader depthMapShader;



GLenum glCheckError_(const char *file, int line)
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
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
    glViewport(0, 0, width, height);
    currentHeight = height;
    currentWidth = width;
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}


void mouseCallback(GLFWwindow* window, double xpos, double ypos) {


    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;


    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    myCamera.rotate(pitch, yaw);
    myBasicShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void processMovement() {
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_UP]) {
        moveZ += 0.4f;
    }
    if (pressedKeys[GLFW_KEY_DOWN]) {
        moveZ -= 0.4f;
    }
    if (pressedKeys[GLFW_KEY_KP_5]) {
        moveY += 0.4f;
        moveZ += 0.4f;
    }
    if (pressedKeys[GLFW_KEY_KP_2]) {
        moveY -= 0.4f;
        moveZ -= 0.4f;
    }
    if (pressedKeys[GLFW_KEY_N]) {
        redColor -= 0.05f;
        blueColor -= 0.05f;
        greenColor -= 0.05f;
        if (redColor < 0.05) {
            redColor = 0.0f;
            blueColor = 0.0f;
            greenColor = 0.0f;
        }
        printf("%f, %f, %f\n", redColor, greenColor, blueColor);
    } //decrease light
    if (pressedKeys[GLFW_KEY_M]) {
        redColor += 0.05f;
        blueColor += 0.05f;
        greenColor += 0.05f;
        printf("%f, %f, %f\n", redColor, greenColor, blueColor);
        if (redColor > 1) {
            redColor = 1.0f;
            blueColor = 1.0f;
            greenColor = 1.0f;
        }
    } //increase light
    
    
}

void processWaysToRender() {
    if (pressedKeys[GLFW_KEY_O]) {
        if (!wireFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            wireFrame = true;
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            wireFrame = false;
        }
    }
    if (pressedKeys[GLFW_KEY_P]) {
        if (!smoothFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            smoothFrame = true;
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            smoothFrame = false;
        }
    }

}

void initOpenGLWindow() {
    myWindow.Create(glWindowWidth, glWindowHeight , "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    
    //scene.LoadModel("models/hangarAircraft/hangarAircraft.obj");
    airPlane.LoadModel("models/airPlane/airPlane.obj");
    scene.LoadModel("models/hangarAircraft/hangarAircraft.obj");
    containers.LoadModel("models/containers/containers.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    depthMapShader.loadShader("shaders/shadows.vert", "shaders/shadows.frag");
    depthMapShader.useShaderProgram();
}

void initUniforms() {
	myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightDirColor = glm::vec3(redColor, greenColor, blueColor); //white light
	lightDirColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDirColor");
	// send light color to shader
	glUniform3fv(lightDirColorLoc, 1, glm::value_ptr(lightDirColor));

    //set light color
    lightPosColor = glm::vec3(poleColor, poleColor, poleColor); //white light
    //vec3(138.282013, 31.581530, -208.992477)
    lightPosColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPosColor");
    // send light color to shader
    glUniform3fv(lightPosColorLoc, 1, glm::value_ptr(lightPosColor));

    pointLightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "PLPosition");
    pointLightPosLoc1 = glGetUniformLocation(myBasicShader.shaderProgram, "PLPosition1");
    pointLightPosLoc2 = glGetUniformLocation(myBasicShader.shaderProgram, "PLPosition2");
    pointLightPosLoc3 = glGetUniformLocation(myBasicShader.shaderProgram, "PLPosition3");
     
    glm::vec3 pointLightPos = glm::vec3(-16.0f, 32.0f, 3.3f);
    glUniform3fv(pointLightPosLoc, 1, glm::value_ptr(pointLightPos));

    glm::vec3 pointLightPos1 = glm::vec3(138.282013f, 31.581530f, -208.992477f);
    glUniform3fv(pointLightPosLoc1, 1, glm::value_ptr(pointLightPos1));

    glm::vec3 pointLightPos2 = glm::vec3(-13.218852, 31.715172, -209.195816);
    glUniform3fv(pointLightPosLoc2, 1, glm::value_ptr(pointLightPos2));

    glm::vec3 pointLightPos3 = glm::vec3(137.584427, 30.759180, 5.210773);
    glUniform3fv(pointLightPosLoc3, 1, glm::value_ptr(pointLightPos3));
    
}

void renderHangar(gps::Shader shader, bool depth) {
    // select active shader program
    shader.useShaderProgram();

    //model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 modelHangar(1.0f);
    glm::mat4 normalMatrix(1.0f);
    modelHangar = glm::mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelHangar));

    if (!depth) {

        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelHangar));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }




    scene.Draw(shader);
}

void renderAirPlane(gps::Shader shader, bool depth) {
    // select active shader program
    shader.useShaderProgram();

    //model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::translate(model, glm::vec3(0, moveY, -moveZ));

    glm::mat4 modelAirplane(1.0f);
    glm::mat4 normalMatrix(1.0f);
    modelAirplane = glm::mat4(1.0f);
    modelAirplane *= glm::translate(glm::vec3(0, moveY, -moveZ));

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelAirplane));

    if (!depth) {

        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelAirplane));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }



    // draw teapot
    airPlane.Draw(shader);

}

void renderContainers(gps::Shader shader, bool depth) {
    
    shader.useShaderProgram();

    //model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 modelContainers(1.0f);
    glm::mat4 normalMatrixContainers(1.0f);
    modelContainers = glm::mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelContainers));

    if (!depth) {

        normalMatrix = glm::mat3(glm::inverseTranspose(view * modelContainers));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrixContainers));
    }




    containers.Draw(shader);
}

void initShadows() {
    glGenFramebuffers(1, &shadowMapFBO);
    //create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    //attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
        0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 calculateLightThings() {
    /*glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 100.0f;
    glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;*/

    glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 300.0f;
    glm::mat4 lightProjection = glm::ortho(-90.0f, 90.0f, -90.0f, 90.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix; 
}

void renderScene(gps::Shader usedShader, bool depth) {





    usedShader.useShaderProgram();
    renderHangar(usedShader, depth);
    renderAirPlane(usedShader, depth);
    renderContainers(usedShader, depth);

    //std::cout << glm::to_string(myCamera.getPos()) << std::endl;

    

}

void renderWithBasicShader() {
    glViewport(0, 0, currentWidth, currentHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    myBasicShader.useShaderProgram();

    
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
    //bind the shadow map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);
    
    glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(calculateLightThings()));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    renderScene(myBasicShader, false);
}

void renderShadowScene() {

    depthMapShader.useShaderProgram();
    
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(calculateLightThings()));
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderScene(depthMapShader, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void renderFull() {
    renderShadowScene();
    renderWithBasicShader();
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    initShadows();
    setWindowCallbacks();
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //calculateLightThings();
    

	glCheckError();

	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        processWaysToRender();
	    //renderScene(myBasicShader);
        renderFull();
        //renderWithBasicShader();
        //renderScene(myBasicShader, false);

        lightDirColor = glm::vec3(redColor, greenColor, blueColor); //white light
        lightDirColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDirColor");
        // send light color to shader
        glUniform3fv(lightDirColorLoc, 1, glm::value_ptr(lightDirColor));

        lightPosColor = glm::vec3(poleColor, poleColor, poleColor); //white light
        lightPosColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPosColor");
        // send light color to shader
        glUniform3fv(lightPosColorLoc, 1, glm::value_ptr(lightPosColor));

        if (redColor > 0.5f) {
            poleColor = 0.0f;
        }
        else {
            poleColor = 1.0f;
        }

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
