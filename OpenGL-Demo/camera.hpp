#pragma once
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


/********************
 * [����] ���Ĭ�ϲ���
 ********************/
const float DEFAULT_YAW = -90.0f;
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_SPEED = 2.5f;
const float DEFAULT_SENSITIVITY = 0.1f;
const float DEFAULT_ZOOM = 45.0f;


/********************
 * [ö��] ����ƶ�ö��
 ********************/
enum class CameraMovement {
    STAY = 0,
    FORWARD = 1,
    BACKWARD = 2,
    LEFT = 3,
    RIGHT = 4,
};


/********************
 * [��] �������
 * ��������������
 * ����ŷ���ǡ�����������
 ********************/
class Camera {
public:
    //---------
    // ��Ա����
    //---------
    // - ������� -
    glm::vec3 position;     // λ������
    glm::vec3 front;        // ǰ����
    glm::vec3 up;           // ������
    glm::vec3 right;        // ������
    glm::vec3 worldUp;      // ����������
    // - ŷ���� -
    float yaw;              // ƫ���� - Y
    float pitch;            // ������ - X
    // - ������� -
    float speed;            // ����ƶ��ٶ�
    float sensitivity;      // ���������
    float zoom;             // ��ͷ���Ŷ�

    //---------
    // ���캯��
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
    // ������ͼ������
    //-----------------
    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    //-----------------
    // ����������뺯��
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
    // ��������ƶ�����
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
    // ���������ֺ���
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
    // ���������������
    //-----------------
    void updateCameraVectors() {
        // - ����ǰ���� -
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->front = glm::normalize(front);
        // - ���������� -
        this->right = glm::normalize(glm::cross(this->front, this->worldUp));
        // - ���������� -
        this->up = glm::normalize(glm::cross(this->right, this->front));
        return;
    }
};

#endif
