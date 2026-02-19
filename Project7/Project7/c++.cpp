#include <iostream>
#include <cstdlib> 

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform float uOffsetY;\n" 
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y + uOffsetY, aPos.z, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";


const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"uniform float uAlpha;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, uAlpha);\n"
"}\n\0";


float offsetY = 0.0f;
float alphaValue = 1.0f;


void processInput(GLFWwindow* window)
{
    static bool colorChanged = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        offsetY += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        offsetY -= 0.01f;

    
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        alphaValue += 0.01f;
        if (alphaValue > 1.0f) alphaValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        alphaValue -= 0.01f;
        if (alphaValue < 0.0f) alphaValue = 0.0f;
    }

   
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !colorChanged)
    {
        colorChanged = true;
        
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
        colorChanged = false;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG Project with Interaction", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
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

   
    float vertices[] = {
        
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         
         -0.2f, -0.2f, 0.5f,  0.0f, 0.0f, 1.0f,
          0.8f, -0.2f, 0.5f,  0.0f, 0.0f, 1.0f,
          0.3f,  0.8f, 0.5f,  0.0f, 0.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); 

    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        
        int alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
        glUniform1f(alphaLoc, alphaValue);

        int offsetLoc = glGetUniformLocation(shaderProgram, "uOffsetY");
        glUniform1f(offsetLoc, offsetY);

        static bool colorChanged = false;
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !colorChanged)
        {
            colorChanged = true;
            for (int i = 0; i < 6; ++i)
            {
                vertices[i * 6 + 3] = static_cast<float>(rand()) / RAND_MAX; 
                vertices[i * 6 + 4] = static_cast<float>(rand()) / RAND_MAX; 
                vertices[i * 6 + 5] = static_cast<float>(rand()) / RAND_MAX; 
            }
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
            colorChanged = false;

      
        glDrawArrays(GL_TRIANGLES, 3, 3);
     
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}