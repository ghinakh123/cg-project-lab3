#include <iostream>
#include <vector>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int activeLight = 0;
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform int lightIndex;\n"
"uniform int objectID;\n"
"void main()\n"
"{\n"
"   if(objectID == 0)\n" 
"       FragColor = vec4(0.2, 0.2, 0.2, 1.0);\n"
"   else if(objectID == 1)\n" 
"       FragColor = (lightIndex == 0) ? vec4(1,0,0,1) : vec4(0.3,0,0,1);\n"
"   else if(objectID == 2)\n" 
"       FragColor = (lightIndex == 1) ? vec4(1,1,0,1) : vec4(0.3,0.3,0,1);\n"
"   else if(objectID == 3)\n" 
"       FragColor = (lightIndex == 2) ? vec4(0,1,0,1) : vec4(0,0.3,0,1);\n"
"}\n\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        activeLight = 0;

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        activeLight = 1;

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        activeLight = 2;
}


void createCircle(float cx, float cy, float r, int segments,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices)
{
    unsigned int startIndex = vertices.size() / 3;

    vertices.push_back(cx);
    vertices.push_back(cy);
    vertices.push_back(0.0f);

    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * 3.1415926f * i / segments;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }

    for (int i = 1; i <= segments; i++)
    {
        indices.push_back(startIndex);
        indices.push_back(startIndex + i);
        indices.push_back(startIndex + i + 1);
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Triangle Traffic Light", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    
    vertices = {
        -0.8f, -0.7f, 0.0f,
         0.8f, -0.7f, 0.0f,
         0.0f,  0.8f, 0.0f
    };

    indices = { 0,1,2 };

  
    createCircle(0.0f, 0.3f, 0.12f, 40, vertices, indices);   
    createCircle(0.0f, 0.0f, 0.12f, 40, vertices, indices);   
    createCircle(0.0f, -0.3f, 0.12f, 40, vertices, indices);  

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "lightIndex"), activeLight);

        glBindVertexArray(VAO);

        int offset = 0;

        
        glUniform1i(glGetUniformLocation(shaderProgram, "objectID"), 0);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)));
        offset += 3;

        
        for (int i = 0; i < 3; i++)
        {
            glUniform1i(glGetUniformLocation(shaderProgram, "objectID"), 1 + i);
            glDrawElements(GL_TRIANGLES, 40 * 3, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)));
            offset += 40 * 3;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}