#ifndef CUBE_H
#define CUBE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>
#include <glad/glad.h>

class Cube {
public:
    glm::vec3 position;  // Position of the cube
    glm::vec3 scale;
    unsigned int textureID;  // Texture ID of the cube
    
    glm::vec3 getPosition() const {
        return position;
    }

    glm::vec3 getScale() const {
        return scale;
    }

    // Constructor
    Cube(const glm::vec3 position, glm::vec3 scale, unsigned int textureID)
        : position(position), scale(scale), textureID(textureID) {}

    // Render function
    void render(Shader& shader, unsigned int VAO) const {
        // Bind the cube's texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
    
        // Set the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        shader.setMat4("model", model);
        shader.setFloat("maxDistance", 30.0f);

        // Render the cube
        glBindVertexArray(VAO);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
};

#endif
