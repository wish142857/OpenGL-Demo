#pragma once
#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "shader.hpp"

using namespace std;


/********************
 * [�ṹ] ����ṹ��
 ********************/
struct Vertex {
    // - λ���������� -
    glm::vec3 position;
    // - ������ -
    glm::vec3 normal;
    // - ������������ -
    glm::vec2 texCoords;
    // - �������� -
    glm::vec3 tangent;
    // - ˫�������� -
    glm::vec3 bitangent;
};


/********************
 * [�ṹ] ����ṹ��
 ********************/
struct Texture {
    // - ID -
    GLuint id;
    // - ���� -
    string type;
    // - ·�� -
    string path;
};


/********************
 * [��] ������
 ********************/
class Mesh {
public:
    //-------
    // �����
    //-------
    vector<Vertex> vertices;    // �����б�
    vector<GLuint> indices;     // �����б�
    vector<Texture> textures;   // �����б�
    GLuint VAO; // �����������

    //---------
    // ���캯��
    //---------
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setupMesh();
        return;
    }

    //-------------
    // ������Ⱦ����
    //-------------
    void drawMesh(Shader& shader) {
        // �󶨺��ʵ�����
        unsigned int diffuseNr = 1;     // �������������
        unsigned int specularNr = 1;    // ������������
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);
            else if (name == "texture_height")
                number = std::to_string(heightNr++);
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        // ��������
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        return;
    }

private:
    //-------
    // �����
    //-------
    GLuint VBO; // ���㻺�����
    GLuint EBO; // �����������
    
    //-------------
    // �������ú���
    //-------------
    void setupMesh() {
        // - ���� VAO/VBO/EBO -
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // - ���ö�������ָ�� -
        // λ����������
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // ������
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        // ������������
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        // ��������
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        // ˫��������
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
        
        // ��� VAO
        glBindVertexArray(0);
        return;
    }
};

#endif