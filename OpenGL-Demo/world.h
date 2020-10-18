#pragma once
#ifndef WORLD_H
#define WORLD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <stb_image.h>

#include <iostream>
#include "camera.hpp"
#include "shader.hpp"
#include "model.hpp"


/********************
 * [����] ��������
 ********************/
const std::string SHADER_VS_PATH = "shader/shader.vs";
const std::string SHADER_FS_PATH = "shader/shader.fs";
const std::string SKYBOX_SHADER_VS_PATH = "shader/skybox.vs";
const std::string SKYBOX_SHADER_FS_PATH = "shader/skybox.fs";

/********************
 * [�ṹ] ����3D��
 ********************/
struct WorldPoint {
	float x;
	float y;
	float z;
	WorldPoint(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(y) { }
};

/********************
 * [�ṹ] ����3D��Χ��
 ********************/
struct WorldBox {


};

/********************
 * [��] ������
 * ����ģʽ
 * OpenGL 3D ������Ⱦ
 ********************/
class World {
public:
	// - ����� -
	Camera* camera;		// ���������ָ��
	float lastX;
	float lastY;
	bool firstMouse;
	// - ��ɫ�� -
	Shader* objectShader;	// ������ɫ������ָ��
	Shader* skyboxShader;	// ��պ���ɫ������ָ��
	// - ��ʱ�� -
	float deltaTime;
	float lastFrame;
	// - �������� -
	GLuint VAO, VBO;
	// - �ӿں��� -
	static World* getInstance() { return &World::world; }
	int run();
private:
	// - ���캯�� -
	World() : camera(nullptr), lastX(0), lastY(0), firstMouse(false), 
		objectShader(nullptr), skyboxShader(nullptr) , deltaTime(0), lastFrame(0), 
		VAO(0), VBO(0), sreenWidth(1280), sreenHeight(960) { }
	// - ˽�к��� -
	static World world;
	unsigned int sreenWidth;
	unsigned int sreenHeight;
	// - ˽�к��� -
	void processInput(GLFWwindow* window);				// ���봦����
	GLuint loadTexture(const char* path);				// ������غ���
	GLuint loadCubemap(std::vector<std::string> faces);	// ��������ͼ���غ���

};

extern void framebufferSizeCallback(GLFWwindow* window, int width, int height);	// ���ڳߴ�ص�����
extern void mouseCallback(GLFWwindow* window, double xpos, double ypos);		// ����ƶ��ص�����
extern void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);	// �����ֻص�����

#endif