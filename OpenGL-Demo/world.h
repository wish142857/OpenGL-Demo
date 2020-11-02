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
 * [�ṹ] ���ʽṹ
 ********************/
struct Material {
	glm::vec3 ambient;	// ������
	glm::vec3 diffuse;	// ������
	glm::vec3 specular;	// �����
	float shininess;	// �����
};


/********************
 * [�ṹ] ��Դ�ṹ
 ********************/
// - ƽ�й�ṹ -
struct DirLight{
	glm::vec3 direction;     // ��������

	glm::vec3 ambient;       // ������ǿ��
	glm::vec3 diffuse;       // ������ǿ��
	glm::vec3 specular;      // �����ǿ��

	DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
		direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) { }
};
// - ���Դ�ṹ -
struct PointLight {
	glm::vec3 position;      // ��������

	float constant;			 // ˥��ϵ��������
	float linear;			 // ˥��ϵ��һ����
	float quadratic;		 // ˥��ϵ��������

	glm::vec3 ambient;       // ������ǿ��
	glm::vec3 diffuse;       // ������ǿ��
	glm::vec3 specular;      // �����ǿ��
	PointLight(glm::vec3 position, float constant, float linear, float quadratic, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
		position(position), constant(constant), linear(linear), quadratic(quadratic), ambient(ambient), diffuse(diffuse), specular(specular) { }
};
// - �۹�Դ�ṹ -
struct SpotLight {
	glm::vec3 position;      // ��������
	glm::vec3 direction;     // ��������
	float cutOff;			 // �й��
	float outerCutOff;		 // ���й��

	float constant;			 // ˥��ϵ��������
	float linear;			 // ˥��ϵ��һ����
	float quadratic;		 // ˥��ϵ��������

	glm::vec3 ambient;       // ������ǿ��
	glm::vec3 diffuse;       // ������ǿ��
	glm::vec3 specular;      // �����ǿ��
};


/********************
 * [��] ������
 ********************/
class Object {
public:
	// - ��̬���� -
	std::string name;			// ��������
	glm::vec3 moveVector;		// λ������	
	glm::vec3 speedVetor;		// �ٶ�����
	glm::vec3 accSpeedVetor;	// ���ٶ�����
	// - ���캯�� -
	Object(const std::string& name, const std::string &path, const glm::mat4 &modelMatrix, const glm::vec3 &minBoxPoint, const glm::vec3& maxBoxPoint)
		: name(name), model(path), modelMatrix(modelMatrix), minBoxPoint(minBoxPoint), maxBoxPoint(maxBoxPoint) {
		moveVector = speedVetor = accSpeedVetor = glm::vec3(0.0f);
		return;
	}
	// - ��Ⱦ���� -
	void draw(Shader& shader) {
		shader.setMat4("model", glm::translate(modelMatrix, moveVector));
		model.drawModel(shader);
		return;
	}
	// - �˶� �˶����� -
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
	// - �˶� �ٶȷ�ת���� -
	void reverseSpeed() {
		speedVetor = -speedVetor;
		return;
	}
	// - �˶� ��ײ��⺯�� -
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
	// - ��̬���� -
	Model model;				// ģ�Ͷ���
	glm::mat4 modelMatrix;		// ģ�;��������ԭ�㣩
	glm::vec3 minBoxPoint;		// ��Χ����СXYZ�����꣨λ��ԭ�㣩
	glm::vec3 maxBoxPoint;		// ��Χ�����XYZ�����꣨λ��ԭ�㣩
};


/********************
 * [��] ������
 * ����ģʽ
 * OpenGL 3D ������Ⱦ
 ********************/
class World {
public:
	// - �ӿں��� -
	static World* getInstance() { return &World::world; }
	int run();

private:
	// - ����� -
	Camera* camera;		// ���������ָ��
	float lastX;
	float lastY;
	bool firstMouse;
	// - ��ɫ�� -
	Shader* objectShader;	// ������ɫ������ָ��
	Shader* skyboxShader;	// ��պ���ɫ������ָ��
	Shader* lightShader;	// ��Դ��ɫ������ָ��
	// - ��ʱ�� -
	float deltaTime;
	float lastFrame;
	// - ��պ� -
	GLuint skyboxVAO;
	GLuint skyboxVBO;
	GLuint cubemapTexture;
	// - ���� -
	std::vector<Material> materials;		// �����б�
	// - ��Դ -
	GLuint lightCubeVAO;
	GLuint lightCubeVBO;
	std::vector<DirLight> dirLights;		// ƽ�й��б�
	std::vector<PointLight> pointLights;	// ����б�
	std::vector<SpotLight> spotLights;		// �۹��б�
	// - �������� -
	std::vector<Object*> objects;
	// - ���캯�� -
	World() : camera(nullptr), lastX(0), lastY(0), firstMouse(false), 
		objectShader(nullptr), skyboxShader(nullptr) , lightShader(nullptr), deltaTime(0), lastFrame(0),
		skyboxVAO(0), skyboxVBO(0), cubemapTexture(0), lightCubeVAO(0), lightCubeVBO(0),
		sreenWidth(1280), sreenHeight(960) { }
	// - ˽�к��� -
	static World world;
	unsigned int sreenWidth;
	unsigned int sreenHeight;
	// - ˽�к��� -
	void processInput(GLFWwindow* window);				// ���봦����
	GLuint loadTexture(const char* path);				// ������غ���
	GLuint loadCubemap(std::vector<std::string> faces);	// ��������ͼ���غ���

	friend void framebufferSizeCallback(GLFWwindow* window, int width, int height); // ���ڳߴ�ص�����
	friend void mouseCallback(GLFWwindow* window, double xpos, double ypos);		// ����ƶ��ص�����
	friend void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);	// �����ֻص�����
};

extern void framebufferSizeCallback(GLFWwindow* window, int width, int height);	// ���ڳߴ�ص�����
extern void mouseCallback(GLFWwindow* window, double xpos, double ypos);		// ����ƶ��ص�����
extern void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);	// �����ֻص�����

#endif