#ifndef DUCK_H
#define DUCK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

class Duck {
public:
    glm::vec3 position;   // Position of the duck in the world
    unsigned int texture; // Texture for the duck

    // Constructor
    Duck(const glm::vec3& position, unsigned int texture)
        : position(position), texture(texture) {}

    // Render the duck
    void render(Shader& shader, unsigned int VAO) const {
        glBindTexture(GL_TEXTURE_2D, texture);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);

        // Draw torso
        glm::mat4 torso = model;
        torso = glm::scale(torso, glm::vec3(1.0f, 1.5f, 1.0f)); // Scale for torso
        shader.setMat4("model", torso);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw head
        glm::mat4 head = model;
        head = glm::translate(head, glm::vec3(0.0f, 1.25f, 0.0f)); // Above the torso
        shader.setMat4("model", head);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw beak
        glm::mat4 beak = head;
        beak = glm::translate(beak, glm::vec3(0.0f, -0.25f, -0.6f)); // Front of the head
        beak = glm::scale(beak, glm::vec3(0.5f, 0.25f, 0.25f));
        shader.setMat4("model", beak);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw arms
        glm::mat4 leftArm = model;
        leftArm = glm::translate(leftArm, glm::vec3(-0.75f, 0.5f, 0.0f)); // Side of the torso
        leftArm = glm::scale(leftArm, glm::vec3(0.25f, 1.0f, 0.25f));
        shader.setMat4("model", leftArm);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 rightArm = model;
        rightArm = glm::translate(rightArm, glm::vec3(0.75f, 0.5f, 0.0f));
        rightArm = glm::scale(rightArm, glm::vec3(0.25f, 1.0f, 0.25f));
        shader.setMat4("model", rightArm);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw legs
        glm::mat4 leftLeg = model;
        leftLeg = glm::translate(leftLeg, glm::vec3(-0.25f, -1.0f, 0.0f)); // Below the torso
        leftLeg = glm::scale(leftLeg, glm::vec3(0.25f, 1.0f, 0.25f));
        shader.setMat4("model", leftLeg);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 rightLeg = model;
        rightLeg = glm::translate(rightLeg, glm::vec3(0.25f, -1.0f, 0.0f));
        rightLeg = glm::scale(rightLeg, glm::vec3(0.25f, 1.0f, 0.25f));
        shader.setMat4("model", rightLeg);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
};

#endif // DUCK_H
