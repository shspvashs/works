#define GLEW_DLL
#define GLFW_DLL

#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main()
{
    glfwInit();
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* y;
    y = glfwCreateWindow(512, 512, "Triangle var  №19", NULL, NULL);

    glfwMakeContextCurrent(y);
    glewExperimental = GL_TRUE;
    glewInit();

    while (!glfwWindowShouldClose(y)) {
        glClearColor(1.0, 1.0, 1.0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(0.3, 1.0, 1.0);
        glBegin(GL_TRIANGLES);

        glVertex2f(0.0, 0.5);
        glVertex2f(-0.5, -0.5);
        glVertex2f(0.5, -0.5);

        glEnd();

        glfwSwapBuffers(y);
        glfwPollEvents();
    }

    glfwTerminate();
}
