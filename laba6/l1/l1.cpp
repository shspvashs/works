#define GLEW_DLL
#define GLFW_DLL

#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "ShaderLib.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Model.h"



glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 5.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

float lastX = SCR_WIDTH / 2,
lastY = SCR_HEIGHT / 2;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;
float sensitivity = 0.1f;
float fov = 45.0f;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 90.0f) fov = 90.0f;
}

void settingMat4(int ID, const char *name, glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}

void settingMat3(int ID, const char* name, glm::mat3& mat) {
    glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
}

void settingVec3(int ID, const char* name, glm::vec3& value) {
    glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
}




GLfloat points[] = {
     0.0, 0.5, 0.0,
    -0.5, -0.5, 0.0,
    0.5, -0.5, 0.0 };

GLuint index[] = { 0,1,2 };

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 0.01;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Var 19", NULL, NULL);
    glfwMakeContextCurrent(window);


    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);



    glewExperimental = GL_TRUE;
    glewInit();

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader ourShader("vertex_shader.glsl", "fragment_shader.glsl");
    Model Arm("..\\bl\\nika (1).obj");
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        float timeValue = glfwGetTime();
        float Red = abs(sin(timeValue));
        float Blue = abs(cos(timeValue));

        ourShader.Use();
        ourShader.SetUniform("ourColour", Red, 0.0, Blue, 0.5);
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        glm::mat4 modelMatrix = glm::mat4(1.0);

        
        //матрицы
        
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        settingMat3(ourShader.Program, "normalMatrix", normalMatrix);

        //свет

        glm::vec3 lightcolor = glm::vec3(1.0, 1.0, 1.0);
        glm::vec3 ambient = lightcolor * glm::vec3(0.1);
        glm::vec3 diffuse = lightcolor * glm::vec3(0.7);
        glm::vec3 specular = lightcolor;
        glm::vec3 position = glm::vec3(20.0, 5.0, 0.0);

        settingVec3(ourShader.Program, "light_1.ambient", ambient);
        settingVec3(ourShader.Program, "light_1.diffuse", diffuse);
        settingVec3(ourShader.Program, "light_1.specular", specular);
        settingVec3(ourShader.Program, "light_1.position", position);

        //материал

        glm::vec3 matambient = glm::vec3(0.2, 0.0, 0.2);
        glm::vec3 matdiffuse = glm::vec3(0.1, 0.0, 0.4);
        glm::vec3 matspecular = glm::vec3(0.4, 0.0, 1.0);
        glm::vec3 matshinies = glm::vec3(32.0);

        settingVec3(ourShader.Program, "mat_1.ambient", matambient);
        settingVec3(ourShader.Program, "mat_1.diffuse", matdiffuse);
        settingVec3(ourShader.Program, "mat_1.specular", matspecular);
        settingVec3(ourShader.Program, "mat_1.shinies", matshinies);

        //позиция для расчета
        settingVec3(ourShader.Program, "viewPos", cameraPos);



        settingMat4(ourShader.Program, "projection", projection);
        settingMat4(ourShader.Program, "view", view);
        settingMat4(ourShader.Program, "model", model);
        
        
        
        normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        settingMat3(ourShader.Program, "normalMatrix", normalMatrix);

        Arm.Draw(ourShader);

        glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
        processInput(window);
    }
    glfwTerminate();
}



