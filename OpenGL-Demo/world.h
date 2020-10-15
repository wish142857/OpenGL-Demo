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
 * [常量] 常量定义
 ********************/
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const std::string SHADER_VS_PATH = "shader.vs";
const std::string SHADER_FS_PATH = "shader.fs";


/********************
 * [类] 世界类
 * 单例模式
 * OpenGL 3D 场景渲染
 ********************/
class World {
public:
	// - 着色器 -
	Shader* shader;		// 着色器对象指针
	// - 摄像机 -
	Camera *camera;		// 摄像机对象指针
	float lastX;
	float lastY;
	bool firstMouse;
	// - 计时器 -
	float deltaTime;
	float lastFrame;
	// - 顶点数据 -
	GLuint VAO, VBO;

	// - 接口函数 -
	static World* getInstance() { return &World::world; }
	int run();
private:
	static World world;
	World() : shader(nullptr), camera(nullptr), lastX(0), lastY(0), firstMouse(false), deltaTime(0), lastFrame(0), VAO(0), VBO(0) { }
	void processInput(GLFWwindow* window);	// 输入处理函数
};

extern void framebufferSizeCallback(GLFWwindow* window, int width, int height);	// 窗口尺寸回调函数
extern void mouseCallback(GLFWwindow* window, double xpos, double ypos);		// 鼠标移动回调函数
extern void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);	// 鼠标滚轮回调函数

#endif