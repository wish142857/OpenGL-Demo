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
 * [常量] 常量定义
 ********************/
const std::string SHADER_VS_PATH = "shader/shader.vs";
const std::string SHADER_FS_PATH = "shader/shader.fs";
const std::string SKYBOX_SHADER_VS_PATH = "shader/skybox.vs";
const std::string SKYBOX_SHADER_FS_PATH = "shader/skybox.fs";

/********************
 * [结构] 世界3D点
 ********************/
struct WorldPoint {
	float x;
	float y;
	float z;
	WorldPoint(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(y) { }
};

/********************
 * [结构] 世界3D包围盒
 ********************/
struct WorldBox {


};

/********************
 * [类] 世界类
 * 单例模式
 * OpenGL 3D 场景渲染
 ********************/
class World {
public:
	// - 摄像机 -
	Camera* camera;		// 摄像机对象指针
	float lastX;
	float lastY;
	bool firstMouse;
	// - 着色器 -
	Shader* objectShader;	// 物体着色器对象指针
	Shader* skyboxShader;	// 天空盒着色器对象指针
	// - 计时器 -
	float deltaTime;
	float lastFrame;
	// - 顶点数据 -
	GLuint VAO, VBO;
	// - 接口函数 -
	static World* getInstance() { return &World::world; }
	int run();
private:
	// - 构造函数 -
	World() : camera(nullptr), lastX(0), lastY(0), firstMouse(false), 
		objectShader(nullptr), skyboxShader(nullptr) , deltaTime(0), lastFrame(0), 
		VAO(0), VBO(0), sreenWidth(1280), sreenHeight(960) { }
	// - 私有函数 -
	static World world;
	unsigned int sreenWidth;
	unsigned int sreenHeight;
	// - 私有函数 -
	void processInput(GLFWwindow* window);				// 输入处理函数
	GLuint loadTexture(const char* path);				// 纹理加载函数
	GLuint loadCubemap(std::vector<std::string> faces);	// 立方体贴图加载函数

};

extern void framebufferSizeCallback(GLFWwindow* window, int width, int height);	// 窗口尺寸回调函数
extern void mouseCallback(GLFWwindow* window, double xpos, double ypos);		// 鼠标移动回调函数
extern void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);	// 鼠标滚轮回调函数

#endif