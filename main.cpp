#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <iostream>
#include <vector>

#include "cube.h"


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera settings
glm::vec3 cameraPos(0.0f, 1.0f, 5.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// mouse control
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

// functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void updateBallPhysics(float deltaTime, const std::vector<Cube>& cubes);
std::vector<float> generateSphereVertices(float radius, unsigned int segments, unsigned int rings);
std::vector<unsigned int> generateSphereIndices(unsigned int segments, unsigned int rings);

//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// texture loading
unsigned int loadTexture(const char* path);

glm::vec3 ballPos(0.0f, 1.0f, 0.0f); // Ball position
glm::vec3 ballVelocity(0.0f, 0.0f, 0.0f); // Ball velocity
float ballRadius = 0.5f; // Ball radius
float ballMass = 1.0f; // Ball mass
float ballFriction = 0.95f; // Friction coefficient
float ballBounce = 0.5f; // Bounce coefficient
glm::vec3 gravityVec(0.0f, -9.8f, 0.0f); // Gravity


int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Baseplate", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // load shaders
    Shader cubeShader("shader.vs", "shader.fs");
    Shader lightShader("light_shader.vs", "light_shader.fs");

    // set up cube data
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // Generate VAO, VBO
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load texture
    unsigned int yellow = loadTexture("C:/Users/Drystan/Documents/project/yellow.jpg");
    unsigned int planks = loadTexture("C:/Users/Drystan/Documents/project/planks.jpg");
    unsigned int white = loadTexture("C:/Users/Drystan/Documents/project/white.jpg");

    // Light source VAO
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::vector<float> sphereVertices = generateSphereVertices(ballRadius, 64, 64);
    std::vector<unsigned int> sphereIndices = generateSphereIndices(64, 64);

    unsigned int sphereVAO, sphereVBO, sphereEBO;
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), &sphereIndices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture Coordinates
    glEnableVertexAttribArray(2);


    std::vector<Cube> cubes = {
        Cube(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(10.0f, 0.5f, 10.0f), planks),
    };


// Render loop
    while (!glfwWindowShouldClose(window)) {
        // Time management
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input handling
        processInput(window);

        // Rendering
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeShader.use();
        cubeShader.setVec3("lightPos", glm::vec3(sin(currentFrame) * 2.0f, 2.0f, 0.0f)); // Hard light position
        cubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // Light emit colour
        cubeShader.setVec3("viewPos", cameraPos); // Camera position

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        cubeShader.setMat4("projection", projection);
        cubeShader.setMat4("view", view);

        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, planks);

        for (const Cube& cube : cubes) {
            cube.render(cubeShader, cubeVAO);
        }
        updateBallPhysics(deltaTime, cubes);
        
        cubeShader.use();
        glm::mat4 ballModel = glm::mat4(1.0f);
        ballModel = glm::translate(ballModel, ballPos);
        cubeShader.setMat4("model", ballModel);

        glBindVertexArray(sphereVAO);
        glBindTexture(GL_TEXTURE_2D, white);
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

        // Light source rendering
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        // Oscillating light position using sin function
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(sin(currentFrame) * 2.0f, 2.0f, 0.0f)); // Moves left-right along X-axis
        model = glm::scale(model, glm::vec3(0.5f));
        lightShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        ballVelocity.z -= 25.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        ballVelocity.z += 25.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        ballVelocity.x -= 25.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        ballVelocity.x += 25.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

GLuint loadTexture(const char* filename)
{
    int width = 0, height = 0, n = 0;
    unsigned char* data = stbi_load
    (
        filename,
        &width,
        &height,
        &n,
        3
    );

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glActiveTexture(GL_TEXTURE0);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return texture;
}

void updateBallPhysics(float deltaTime, const std::vector<Cube>& cubes) {
    // Apply gravity
    ballVelocity += gravityVec * deltaTime;

    // Update position
    ballPos += ballVelocity * deltaTime;

    // Check for collisions with baseplate
    for (const Cube& cube : cubes) {
        glm::vec3 cubeMin = cube.position - cube.scale * 0.5f;
        glm::vec3 cubeMax = cube.position + cube.scale * 0.5f;

        if (ballPos.x + ballRadius > cubeMin.x && ballPos.x - ballRadius < cubeMax.x &&
            ballPos.y - ballRadius < cubeMax.y && ballPos.y + ballRadius > cubeMin.y &&
            ballPos.z + ballRadius > cubeMin.z && ballPos.z - ballRadius < cubeMax.z) {
            ballVelocity.y *= -ballBounce;
            ballPos.y = cubeMax.y + ballRadius; // Reset position to surface
        }
    }

    // Apply friction
    ballVelocity.x *= (1.0f - ballFriction * deltaTime);
    ballVelocity.z *= (1.0f - ballFriction * deltaTime);


    // Stop very small movements
    if (glm::length(ballVelocity) < 0.01f) {
        ballVelocity = glm::vec3(0.0f);
    }
}

std::vector<float> generateSphereVertices(float radius, unsigned int segments, unsigned int rings) {
    std::vector<float> vertices;
    for (unsigned int y = 0; y <= rings; ++y) {
        for (unsigned int x = 0; x <= segments; ++x) {
            float xSegment = (float)x / (float)segments;
            float ySegment = (float)y / (float)rings;
            float xPos = radius * cos(xSegment * 2.0f * glm::pi<float>()) * sin(ySegment * glm::pi<float>());
            float yPos = radius * cos(ySegment * glm::pi<float>());
            float zPos = radius * sin(xSegment * 2.0f * glm::pi<float>()) * sin(ySegment * glm::pi<float>());

            // Add position
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);

            // Add normal
            vertices.push_back(xPos / radius);
            vertices.push_back(yPos / radius);
            vertices.push_back(zPos / radius);

            // Add texture coordinates
            vertices.push_back(xSegment);
            vertices.push_back(ySegment);
        }
    }
    return vertices;
}
std::vector<unsigned int> generateSphereIndices(unsigned int segments, unsigned int rings) {
    std::vector<unsigned int> indices;
    for (unsigned int y = 0; y < rings; ++y) {
        for (unsigned int x = 0; x < segments; ++x) {
            indices.push_back((y + 1) * (segments + 1) + x);
            indices.push_back(y * (segments + 1) + x + 1);
            indices.push_back(y * (segments + 1) + x);

            indices.push_back((y + 1) * (segments + 1) + x);
            indices.push_back((y + 1) * (segments + 1) + x + 1);
            indices.push_back(y * (segments + 1) + x + 1);
        }
    }
    return indices;
}






