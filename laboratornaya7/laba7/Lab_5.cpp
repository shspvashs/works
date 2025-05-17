#include "Shader.h"
#include "Model.h"
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <matrix_transform.hpp>
#include <type_ptr.hpp>
#include <iostream>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

glm::vec3 cameraPos = glm::vec3(7.0f, 7.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct ObjectTransform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 pivotPoint = glm::vec3(0.0f);


    struct {
        float min = -3.5f;
        float max = 2.0f;
    } zLimit;

    struct {
        float min = -1.7f;
        float max = 0.0f;
    } yLimit;


    struct {
        float min = -90.0f;
        float max = 90.0f;
    } rotationLimit;
};

std::vector<ObjectTransform> objectTransforms;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

glm::mat4 calculateModelMatrix(int index) {
    glm::mat4 model = glm::mat4(1.0f);

    switch (index) {
    case 0: // Base 
        return model;

    case 2: // X01 
        model = glm::translate(model, objectTransforms[1].position);
        return model;

    case 3: // X02 
        model = glm::translate(model, objectTransforms[1].position);
        model = glm::translate(model, objectTransforms[2].position);
        return model;

    case 4: // X03 
        model = glm::translate(model, objectTransforms[1].position);
        model = glm::translate(model, objectTransforms[2].position);
        model = glm::translate(model, objectTransforms[3].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[3].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[3].pivotPoint);
        return model;

    case 1: // X04 
        model = glm::translate(model, objectTransforms[1].position);
        model = glm::translate(model, objectTransforms[2].position);
        model = glm::translate(model, objectTransforms[4].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[4].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[4].pivotPoint);
        return model;

    default:
        return model;
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Model Transformations", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader shader("vertex_sheder.glsl", "fragment_shader.glsl");
    Model ourModel("nika.obj");

    objectTransforms.resize(5);

    // Установка точек вращения
    objectTransforms[3].pivotPoint = glm::vec3(4.0f, 0.3f, -7.09525f);
    objectTransforms[4].pivotPoint = glm::vec3(4.0f, 0.3f, -5.89525f);

    // Установка ограничений
    objectTransforms[1].yLimit = { 0.0f, 0.0f };
    objectTransforms[2].zLimit = { 0.0f, 0.0f };
    objectTransforms[3].rotationLimit = { -30.0f, 90.0f };
    objectTransforms[4].rotationLimit = { -90.0f, 30.0f };

    shader.use();
    shader.setVec3("light.position", glm::vec3(5.2f, 7.0f, -5.0f));
    shader.setVec3("light.ambient", glm::vec3(0.4f, 0.4f, 0.4f));
    shader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.7f));
    shader.setVec3("light.specular", glm::vec3(1.0f));
    shader.setVec3("material.ambient", glm::vec3(0.2f, 0.0f, 0.5f));
    shader.setVec3("material.diffuse", glm::vec3(0.2f, 0.0f, 0.7f));
    shader.setVec3("material.specular", glm::vec3(0.4f, 0.0f, 1.0f));
    shader.setFloat("material.shininess", 32.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setVec3("viewPos", cameraPos);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        for (size_t i = 0; i < ourModel.meshTransforms.size(); ++i) {
            ourModel.meshTransforms[i] = calculateModelMatrix(i);
        }

        ourModel.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 20.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    float moveSpeed = 1.5f * deltaTime;
    float rotateSpeed = 50.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        objectTransforms[1].position.z += moveSpeed;
        if (objectTransforms[1].position.z > objectTransforms[1].zLimit.max)
            objectTransforms[1].position.z = objectTransforms[1].zLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        objectTransforms[1].position.z -= moveSpeed;
        if (objectTransforms[1].position.z < objectTransforms[1].zLimit.min)
            objectTransforms[1].position.z = objectTransforms[1].zLimit.min;
    }

    // Управление X02 (движение поршней по Y) - клавиши Y/H
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        objectTransforms[2].position.y += moveSpeed;
        if (objectTransforms[2].position.y > objectTransforms[2].yLimit.max)
            objectTransforms[2].position.y = objectTransforms[2].yLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        objectTransforms[2].position.y -= moveSpeed;
        if (objectTransforms[2].position.y < objectTransforms[2].yLimit.min)
            objectTransforms[2].position.y = objectTransforms[2].yLimit.min;
    }

    // Управление X03 (вращение катушки) - клавиши U/J
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        objectTransforms[3].rotation.y += rotateSpeed;
        if (objectTransforms[3].rotation.y > objectTransforms[3].rotationLimit.max)
            objectTransforms[3].rotation.y = objectTransforms[3].rotationLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        objectTransforms[3].rotation.y -= rotateSpeed;
        if (objectTransforms[3].rotation.y < objectTransforms[3].rotationLimit.min)
            objectTransforms[3].rotation.y = objectTransforms[3].rotationLimit.min;
    }

    // Управление X04 (вращение катушки) - клавиши I/K
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        objectTransforms[4].rotation.y += rotateSpeed;
        if (objectTransforms[4].rotation.y > objectTransforms[4].rotationLimit.max)
            objectTransforms[4].rotation.y = objectTransforms[4].rotationLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        objectTransforms[4].rotation.y -= rotateSpeed;
        if (objectTransforms[4].rotation.y < objectTransforms[4].rotationLimit.min)
            objectTransforms[4].rotation.y = objectTransforms[4].rotationLimit.min;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
}