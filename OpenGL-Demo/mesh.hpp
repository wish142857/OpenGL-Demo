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
 * [结构] 顶点结构体
 ********************/
struct Vertex {
    // - 位置坐标向量 -
    glm::vec3 position;
    // - 法向量 -
    glm::vec3 normal;
    // - 纹理坐标向量 -
    glm::vec2 texCoords;
    // - 切线向量 -
    glm::vec3 tangent;
    // - 双切线向量 -
    glm::vec3 bitangent;
};


/********************
 * [结构] 纹理结构体
 ********************/
struct Texture {
    // - ID -
    GLuint id;
    // - 类型 -
    string type;
    // - 路径 -
    string path;
};


/********************
 * [类] 网格类
 ********************/
class Mesh {
public:
    //-------
    // 类变量
    //-------
    vector<Vertex> vertices;    // 顶点列表
    vector<GLuint> indices;     // 索引列表
    vector<Texture> textures;   // 纹理列表
    GLuint VAO; // 顶点数组对象

    //---------
    // 构造函数
    //---------
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setupMesh();
        return;
    }

    //-------------
    // 网格渲染函数
    //-------------
    void drawMesh(Shader& shader) {
        // 绑定合适的纹理
        unsigned int diffuseNr = 1;     // 漫反射纹理序号
        unsigned int specularNr = 1;    // 镜面光纹理序号
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
        // 绘制网格
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        return;
    }

private:
    //-------
    // 类变量
    //-------
    GLuint VBO; // 顶点缓冲对象
    GLuint EBO; // 索引缓冲对象
    
    //-------------
    // 网格设置函数
    //-------------
    void setupMesh() {
        // - 设置 VAO/VBO/EBO -
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // - 设置顶点属性指针 -
        // 位置坐标向量
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // 法向量
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        // 纹理坐标向量
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        // 切线向量
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
        // 双切线向量
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
        
        // 解绑 VAO
        glBindVertexArray(0);
        return;
    }
};

#endif