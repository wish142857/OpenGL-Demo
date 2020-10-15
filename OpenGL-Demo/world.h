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


/********************
 * [����] ��������
 ********************/
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const std::string SHADER_VS_PATH = "shader.vs";
const std::string SHADER_FS_PATH = "shader.fs";


/********************
 * [��] ������
 * ����ģʽ
 * OpenGL 3D ������Ⱦ
 ********************/
class World {
public:
	// - ��ɫ�� -
	Shader* shader;		// ��ɫ������ָ��
	// - ����� -
	Camera *camera;		// ���������ָ��
	float lastX;
	float lastY;
	bool firstMouse;
	// - ��ʱ�� -
	float deltaTime;
	float lastFrame;
	// - �������� -
	GLuint VAO, VBO;

	// - �ӿں��� -
	static World* getInstance() { return &World::world; }
	int run();
private:
	static World world;
	World() : shader(nullptr), camera(nullptr), lastX(0), lastY(0), firstMouse(false), deltaTime(0), lastFrame(0), VAO(0), VBO(0) { }
	void processInput(GLFWwindow* window);	// ���봦����
};

extern void framebufferSizeCallback(GLFWwindow* window, int width, int height);	// ���ڳߴ�ص�����
extern void mouseCallback(GLFWwindow* window, double xpos, double ypos);		// ����ƶ��ص�����
extern void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);	// �����ֻص�����

#endif