#include "Mesh.h"
#include "Texture.h"

class Object {
public:
    Mesh mesh;
    Texture texture;
    glm::mat4 modelMatrix;

    Object(const float* vertices, size_t vertexSize, const unsigned int* indices, size_t indexSize, const std::string& texturePath)
        : mesh(vertices, vertexSize, indices, indexSize), texture(texturePath) {
        modelMatrix = glm::mat4(1.0f);
    }

    void setModelMatrix(const glm::mat4& matrix) {
        modelMatrix = matrix;
    }

    glm::vec3 getPosition()
    {
        return glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
    }

    glm::mat4 getModelMatrix()
    {
        return modelMatrix;
    }
    
    void draw(ShaderProgram& shaderProgram) {
        int modelLoc = glGetUniformLocation(shaderProgram.ID, "ModelMatrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        texture.bind();
        mesh.bind();
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    }
};
