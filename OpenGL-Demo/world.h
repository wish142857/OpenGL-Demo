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
	// - �������� -
	std::vector<Object *> objects;
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