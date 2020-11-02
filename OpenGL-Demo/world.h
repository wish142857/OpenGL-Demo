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
const bool W_DYNAMIC_MODE = false;
const bool W_LIGHT_MODE = true;
const bool W_SKYBOX_MODE = true;
const std::string OBJECT_SHADER_VS_PATH = "shader/object_shader.vs";
const std::string OBJECT_SHADER_FS_PATH = "shader/object_shader.fs";
const std::string SKYBOX_SHADER_VS_PATH = "shader/skybox_shader.vs";
const std::string SKYBOX_SHADER_FS_PATH = "shader/skybox_shader.fs";
const std::string LIGHT_SHADER_VS_PATH = "shader/light_shader.vs";
const std::string LIGHT_SHADER_FS_PATH = "shader/light_shader.fs";


/********************
 * [结构] 材质结构
 ********************/
struct Material {
	glm::vec3 ambient;	// 环境光
	glm::vec3 diffuse;	// 漫反射
	glm::vec3 specular;	// 镜面光
	float shininess;	// 反光度
};


/********************
 * [结构] 光源结构
 ********************/
// - 平行光结构 -
struct DirLight{
	glm::vec3 direction;     // 方向向量

	glm::vec3 ambient;       // 环境光强度
	glm::vec3 diffuse;       // 漫反射强度
	glm::vec3 specular;      // 镜面光强度

	DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
		direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) { }
};
// - 点光源结构 -
struct PointLight {
	glm::vec3 position;      // 世界坐标

	float constant;			 // 衰减系数常数项
	float linear;			 // 衰减系数一次项
	float quadratic;		 // 衰减系数二次项

	glm::vec3 ambient;       // 环境光强度
	glm::vec3 diffuse;       // 漫反射强度
	glm::vec3 specular;      // 镜面光强度
	PointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
		position(position), constant(constant), linear(linear), quadratic(quadratic), ambient(ambient), diffuse(diffuse), specular(specular) { }
};
// - 聚光源结构 -
struct SpotLight {
	glm::vec3 position;      // 世界坐标
	glm::vec3 direction;     // 方向向量
	float cutOff;			 // 切光角
	float outerCutOff;		 // 外切光角

	float constant;			 // 衰减系数常数项
	float linear;			 // 衰减系数一次项
	float quadratic;		 // 衰减系数二次项

	glm::vec3 ambient;       // 环境光强度
	glm::vec3 diffuse;       // 漫反射强度
	glm::vec3 specular;      // 镜面光强度
};


/********************
 * [类] 物体类
 ********************/
class Object {
public:
	// - 动态变量 -
	std::string name;			// 物体名称
	glm::vec3 moveVector;		// 位移向量	
	glm::vec3 speedVetor;		// 速度向量
	glm::vec3 accSpeedVetor;	// 加速度向量
	// - 构造函数 -
	Object(const std::string& name, const std::string &path, const glm::mat4 &modelMatrix, const glm::vec3 &minBoxPoint, const glm::vec3& maxBoxPoint)
		: name(name), model(path), modelMatrix(modelMatrix), minBoxPoint(minBoxPoint), maxBoxPoint(maxBoxPoint) {
		moveVector = speedVetor = accSpeedVetor = glm::vec3(0.0f);
		return;
	}
	// - 渲染函数 -
	void draw(Shader& shader) {
		shader.setMat4("model", glm::translate(modelMatrix, moveVector));
		model.drawModel(shader);
		return;
	}
	// - 运动 运动函数 -
	void move(float deltaTime) {
		moveVector += speedVetor * deltaTime;
		speedVetor += accSpeedVetor * deltaTime;
		return;
	}
	void unMove(float deltaTime) {
		speedVetor -= accSpeedVetor * deltaTime;
		moveVector -= speedVetor * deltaTime;
		return;
	}
	// - 运动 速度反转函数 -
	void reverseSpeed() {
		speedVetor = -speedVetor;
		return;
	}
	// - 运动 碰撞检测函数 -
	bool checkCollision(const glm::vec3& point) const {
		glm::vec3 p = point - moveVector;
		if (p.x < minBoxPoint.x) return false;
		if (p.y < minBoxPoint.y) return false;
		if (p.z < minBoxPoint.z) return false;
		if (p.x > maxBoxPoint.x) return false;
		if (p.y > maxBoxPoint.y) return false;
		if (p.z > maxBoxPoint.z) return false;
		return true;
	}
	bool checkCollision(const Object& object) const {
		glm::vec3 minS = minBoxPoint + moveVector;
		glm::vec3 maxS = maxBoxPoint + moveVector;
		glm::vec3 minT = object.minBoxPoint + object.moveVector;
		glm::vec3 maxT = object.maxBoxPoint + object.moveVector;
		return ((minS.x >= minT.x && minS.x <= maxT.x) || (minT.x >= minS.x && minT.x <= maxS.x)) && 
			((minS.y >= minT.y && minS.y <= maxT.y) || (minT.y >= minS.y && minT.y <= maxS.y)) &&
			((minS.z >= minT.z && minS.z <= maxT.z) || (minT.z >= minS.z && minT.z <= maxS.z));
	}

private:
	// - 静态变量 -
	Model model;				// 模型对象
	glm::mat4 modelMatrix;		// 模型矩阵（相对于原点）
	glm::vec3 minBoxPoint;		// 包围盒最小XYZ点坐标（位于原点）
	glm::vec3 maxBoxPoint;		// 包围盒最大XYZ点坐标（位于原点）
};


/********************
 * [类] 世界类
 * 单例模式
 * OpenGL 3D 场景渲染
 ********************/
class World {
public:
	// - 接口函数 -
	static World* getInstance() { return &World::world; }
	int run();

private:
	// - 摄像机 -
	Camera* camera;		// 摄像机对象指针
	float lastX;
	float lastY;
	bool firstMouse;
	// - 着色器 -
	Shader* objectShader;	// 物体着色器对象指针
	Shader* skyboxShader;	// 天空盒着色器对象指针
	Shader* lightShader;	// 光源着色器对象指针
	// - 计时器 -
	float deltaTime;
	float lastFrame;
	// - 天空盒 -
	GLuint skyboxVAO;
	GLuint skyboxVBO;
	GLuint cubemapTexture;
	// - 材质 -
	std::vector<Material> materials;		// 材质列表
	// - 光源 -
	GLuint lightCubeVAO;
	GLuint lightCubeVBO;
	std::vector<DirLight> dirLights;		// 平行光列表
	std::vector<PointLight> pointLights;	// 点光列表
	std::vector<SpotLight> spotLights;		// 聚光列表
	// - 物体数据 -
	std::vector<Object*> objects;
	// - 构造函数 -
	World() : camera(nullptr), lastX(0), lastY(0), firstMouse(false), 
		objectShader(nullptr), skyboxShader(nullptr) , lightShader(nullptr), deltaTime(0), lastFrame(0),
		skyboxVAO(0), skyboxVBO(0), cubemapTexture(0), lightCubeVAO(0), lightCubeVBO(0),
		sreenWidth(1280), sreenHeight(960) { }
	// - 私有函数 -
	static World world;
	unsigned int sreenWidth;
	unsigned int sreenHeight;
	// - 私有函数 -
	void processInput(GLFWwindow* window);				// 输入处理函数
	GLuint loadTexture(const char* path);				// 纹理加载函数
	GLuint loadCubemap(std::vector<std::string> faces);	// 立方体贴图加载函数

	friend void framebufferSizeCallback(GLFWwindow* window, int width, int height); // 窗口尺寸回调函数
	friend void mouseCallback(GLFWwindow* window, double xpos, double ypos);		// 鼠标移动回调函数
	friend void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);	// 鼠标滚轮回调函数
};

extern void framebufferSizeCallback(GLFWwindow* window, int width, int height);	// 窗口尺寸回调函数
extern void mouseCallback(GLFWwindow* window, double xpos, double ypos);		// 鼠标移动回调函数
extern void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);	// 鼠标滚轮回调函数

#endif