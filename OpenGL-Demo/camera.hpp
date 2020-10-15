#pragma once
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


/********************
 * [常量] 相机默认参数
 ********************/
const float DEFAULT_YAW = -90.0f;
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_SPEED = 2.5f;
const float DEFAULT_SENSITIVITY = 0.1f;
const float DEFAULT_ZOOM = 45.0f;


/********************
 * [枚举] 相机移动枚举
 ********************/
enum class CameraMovement {
    STAY = 0,
    FORWARD = 1,
    BACKWARD = 2,
    LEFT = 3,
    RIGHT = 4,
};


/********************
 * [类] 摄像机类
 * 处理鼠标键盘输入
 * 计算欧拉角、向量及矩阵
 ********************/
class Camera {
public:
    //---------
    // 成员属性
    //---------
    // - 相机属性 -
    glm::vec3 position;     // 位置向量
    glm::vec3 front;        // 前向量
    glm::vec3 up;           // 上向量
    glm::vec3 right;        // 右向量
    glm::vec3 worldUp;      // 世界上向量
    // - 欧拉角 -
    float yaw;              // 偏航角 - Y
    float pitch;            // 俯仰角 - X
    // - 相机参数 -
    float speed;            // 相机移动速度
    float sensitivity;      // 鼠标灵敏度
    float zoom;             // 镜头缩放度

    //---------
    // 构造函数
    //---------
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH)
        : front(glm::vec3(0.0f, 0.0f, -1.0f)), speed(DEFAULT_SPEED), sensitivity(DEFAULT_SENSITIVITY), zoom(DEFAULT_ZOOM) {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
        return;
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : front(glm::vec3(0.0f, 0.0f, -1.0f)), speed(DEFAULT_SPEED), sensitivity(DEFAULT_SENSITIVITY), zoom(DEFAULT_ZOOM) {
        this->position = glm::vec3(posX, posY, posZ);
        this->worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
        return;
    }

    //-----------------
    // 计算视图矩阵函数
    //-----------------
    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    //-----------------
    // 处理键盘输入函数
    //-----------------
    void processKeyboard(CameraMovement direction, float deltaTime) {
        float velocity = speed * deltaTime;
        switch (direction) {
            case CameraMovement::FORWARD:
                position += front * velocity;
                break;
            case CameraMovement::BACKWARD:
                position -= front * velocity;
                break;
            case CameraMovement::LEFT:
                position -= right * velocity;
                break;
            case CameraMovement::RIGHT:
                position += right * velocity;
                break;
            default:
                break;
        }
        return;
    }

    //-----------------
    // 处理鼠标移动函数
    //-----------------
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            else if (pitch < -89.0f)
                pitch = -89.0f;
        }

        updateCameraVectors();
        return;
    }

    //-----------------
    // 处理鼠标滚轮函数
    //-----------------
    void processMouseScroll(float yoffset) {
        zoom -= yoffset;
        if (zoom < 1.0f)
            zoom = 1.0f;
        else if (zoom > 45.0f)
            zoom = 45.0f;
        return;
    }

private:
    
    //-----------------
    // 更新相机向量函数
    //-----------------
    void updateCameraVectors() {
        // - 更新前向量 -
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->front = glm::normalize(front);
        // - 更新右向量 -
        this->right = glm::normalize(glm::cross(this->front, this->worldUp));
        // - 更新上向量 -
        this->up = glm::normalize(glm::cross(this->right, this->front));
        return;
    }
};

#endif
