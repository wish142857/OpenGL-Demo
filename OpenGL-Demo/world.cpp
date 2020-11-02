#include "world.h"


/********************
 * [����] ���к���
 ********************/
int World::run() {
    // ----------------------
    // [glfw] ��ʼ���������봴������
    // ----------------------
    // --- ��ʼ������ ---
    glfwInit();
    // --- ���ô��� ---
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // --- �������� ---
    GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(pMonitor);
    sreenWidth = mode->width;
    sreenHeight = mode->height;
    GLFWwindow* window = glfwCreateWindow(sreenWidth, sreenHeight, "World", NULL, NULL);
    if (window == NULL) {
        std::cout << "ERROR::Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetWindowPos(window, 0, 32);
    glfwMakeContextCurrent(window);
    // - ���ûص� -
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    // - ��������ģʽ -
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // -------------------------------
    // [glad] ���� OpneGL ����ָ��
    // -------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR::Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // ---------------------------
    // [glad] ���� OpenGL ȫ��״̬
    // ---------------------------
    glEnable(GL_DEPTH_TEST);    // ������Ȳ���
    // ---------------------------
    // [glad] ������������ɫ������
    // ---------------------------
    objectShader = new Shader(OBJECT_SHADER_VS_PATH.c_str(), OBJECT_SHADER_FS_PATH.c_str());
    skyboxShader = new Shader(SKYBOX_SHADER_VS_PATH.c_str(), SKYBOX_SHADER_FS_PATH.c_str());
    lightShader = new Shader(LIGHT_SHADER_VS_PATH.c_str(), LIGHT_SHADER_FS_PATH.c_str());
    // ----------
    // �����ʼ��
    // ----------
    // - ��ʼ�� stb_image -
    stbi_set_flip_vertically_on_load(false);
    // - ��������ʼ����� -
    camera = new Camera(glm::vec3(0.0f, 0.2f, 1.0f));
    lastX = sreenWidth / 2.0f;
    lastY = sreenHeight / 2.0f;
    firstMouse = true;
    // - ��ʼ����ʱ�� -
    deltaTime = 0.0f;
    lastFrame = 0.0f;
    // - ��ʼ������ -
    materials.clear();
    // - ��ʼ����Դ -
    if (W_LIGHT_MODE) {
        // ƽ�й�
        dirLights.clear();
        dirLights.push_back(DirLight(
            glm::vec3(0, 0.2f, 1.0f),           // direction
            glm::vec3(0.05f, 0.05f, 0.05f),     // ambient
            glm::vec3(0.4f, 0.4f, 0.4f),        // diffuse
            glm::vec3(0.5f, 0.5f, 0.5f)         // specular
        ));
        // ���
        pointLights.clear();
        pointLights.push_back(PointLight(
            glm::vec3(0.0f, 0.5f, 0.0f),        // position
            1.0f,                               // constant
            0.09f,                              // linear
            0.032f,                             // quadratic
            glm::vec3(0.05f, 0.05f, 0.05f),     // ambient
            glm::vec3(0.8f, 0.8f, 0.8f),        // diffuse
            glm::vec3(1.0f, 1.0f, 1.0f)         // specular
        ));
        // �۹�
        spotLights.clear();
    }
    // ---------------
    // [glad] ����ģ��
    // ---------------
    // - ������պ� -
    if (W_SKYBOX_MODE) {
        float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,    -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f,    1.0f,  1.0f, -1.0f,    -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,    -1.0f, -1.0f, -1.0f,    -1.0f,  1.0f, -1.0f,    -1.0f,  1.0f, -1.0f,    -1.0f,  1.0f,  1.0f,    -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,    1.0f, -1.0f,  1.0f,    1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,    1.0f,  1.0f, -1.0f,    1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,    -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,    1.0f, -1.0f,  1.0f,    -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,    1.0f,  1.0f, -1.0f,    1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,    -1.0f,  1.0f,  1.0f,    -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,    -1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f,    -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f
        };
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        std::vector<std::string> faces{
            "resources/textures/skybox/right.jpg",
            "resources/textures/skybox/left.jpg",
            "resources/textures/skybox/top.jpg",
            "resources/textures/skybox/bottom.jpg",
            "resources/textures/skybox/front.jpg",
            "resources/textures/skybox/back.jpg",
        };
        cubemapTexture = loadCubemap(faces);
    }
    // - ���ع�Դ������ -
    if (W_LIGHT_MODE) {
        float lightCubeVertices[] = {
            -0.5f, -0.5f, -0.5f,    0.5f, -0.5f, -0.5f,    0.5f,  0.5f, -0.5f,    0.5f,  0.5f, -0.5f,    -0.5f,  0.5f, -0.5f,    -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,    0.5f, -0.5f,  0.5f,    0.5f,  0.5f,  0.5f,    0.5f,  0.5f,  0.5f,    -0.5f,  0.5f,  0.5f,    -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,    -0.5f,  0.5f, -0.5f,    -0.5f, -0.5f, -0.5f,    -0.5f, -0.5f, -0.5f,    -0.5f, -0.5f,  0.5f,    -0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,    0.5f,  0.5f, -0.5f,    0.5f, -0.5f, -0.5f,    0.5f, -0.5f, -0.5f,    0.5f, -0.5f,  0.5f,    0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,    0.5f, -0.5f, -0.5f,    0.5f, -0.5f,  0.5f,    0.5f, -0.5f,  0.5f,    -0.5f, -0.5f,  0.5f,    -0.5f, -0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,    0.5f,  0.5f, -0.5f,    0.5f,  0.5f,  0.5f,    0.5f,  0.5f,  0.5f,    -0.5f,  0.5f,  0.5f,    -0.5f,  0.5f, -0.5f,
        };
        glGenVertexArrays(1, &lightCubeVAO);
        glGenBuffers(1, &lightCubeVBO);
        glBindVertexArray(lightCubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lightCubeVertices), &lightCubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
    // - ��������ģ�� -
    objects.clear();
    {
        // - ������1 -
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 10.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        Object *object = new Object("С�� 1 ��", "resources/objects/std/dragon_vrip.ply", model, 
            glm::vec3(-0.4f, -0.2f, -0.6f), glm::vec3(0.4f, 0.2f, 0.6f));
        objects.push_back(object);
        // - �˶��������� -
        object->moveVector = glm::vec3(0.0f, 0.0f, 0.0f);
        object->accSpeedVetor.y = -0.2f;
    }
    /*
    {
        // - ������2 -
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        Object* object = new Object("С�� 2 ��", "resources/objects/bird/2/2.obj", model,
            glm::vec3(-0.6f, -0.2f, -0.9f), glm::vec3(0.5f, 0.2f, 0.9f));
        objects.push_back(object);
        // - �˶��������� -
        object->moveVector = glm::vec3(0.0f, -2.0f, 0.0f);
        object->accSpeedVetor.y = 0.2f;
    }
    {
        // - ���طɻ� -
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
        Object* object = new Object("F-35 ս����", "resources/objects/flight/F-35_Lightning_II/F-35_Lightning_II.obj", model, 
            glm::vec3(-6.4f, -0.8f, -4.4f), glm::vec3(8.0f, 0.6f, 4.4f));
        objects.push_back(object);
        // - �˶��������� -
    }
    */
    // ---------------
    // [glad] ���ع�Դ
    // ---------------
    objectShader->use();
    // - ����ƽ�й�Դ -
    // �������б�����
    if (!dirLights.empty()) {
        objectShader->setVec3("dirLight.direction", dirLights[0].direction);
        objectShader->setVec3("dirLight.ambient", dirLights[0].ambient);
        objectShader->setVec3("dirLight.diffuse", dirLights[0].diffuse);
        objectShader->setVec3("dirLight.specular", dirLights[0].specular);
    }
    // - ���ص��Դ -
    // �������������
    if (pointLights.size() > 0) {
        objectShader->setVec3("pointLights[0].position", pointLights[0].position);
        objectShader->setVec3("pointLights[0].ambient", pointLights[0].ambient);
        objectShader->setVec3("pointLights[0].diffuse", pointLights[0].diffuse);
        objectShader->setVec3("pointLights[0].specular", pointLights[0].specular);
        objectShader->setFloat("pointLights[0].constant", pointLights[0].constant);
        objectShader->setFloat("pointLights[0].linear", pointLights[0].linear);
        objectShader->setFloat("pointLights[0].quadratic", pointLights[0].quadratic);
    }
    if (pointLights.size() > 1) {
        objectShader->setVec3("pointLights[1].position", pointLights[1].position);
        objectShader->setVec3("pointLights[1].ambient", pointLights[1].ambient);
        objectShader->setVec3("pointLights[1].diffuse", pointLights[1].diffuse);
        objectShader->setVec3("pointLights[1].specular", pointLights[1].specular);
        objectShader->setFloat("pointLights[1].constant", pointLights[1].constant);
        objectShader->setFloat("pointLights[1].linear", pointLights[1].linear);
        objectShader->setFloat("pointLights[1].quadratic", pointLights[1].quadratic);
    }
    if (pointLights.size() > 2) {
        objectShader->setVec3("pointLights[2].position", pointLights[2].position);
        objectShader->setVec3("pointLights[2].ambient", pointLights[2].ambient);
        objectShader->setVec3("pointLights[2].diffuse", pointLights[2].diffuse);
        objectShader->setVec3("pointLights[2].specular", pointLights[2].specular);
        objectShader->setFloat("pointLights[2].constant", pointLights[2].constant);
        objectShader->setFloat("pointLights[2].linear", pointLights[2].linear);
        objectShader->setFloat("pointLights[2].quadratic", pointLights[2].quadratic);
    }
    // - ���ؾ۹�Դ -
    // ����������
    if (!spotLights.empty()) {
        // TODO
    }
    // --------------------
    // [glfw/glad] ��Ⱦѭ��
    // --------------------
    while (!glfwWindowShouldClose(window)) {
        // --- ��ʱ�� ---
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // --- �����˶� ---
        if (W_DYNAMIC_MODE) {
            for (Object* object : objects) {
                object->move(deltaTime);
                for (Object* target : objects)
                    if (object != target && object->checkCollision(*target)) {
                        object->unMove(deltaTime);
                        object->reverseSpeed();
                        std::cout << "World::Collision: " << object->name << "<->" << target->name << std::endl;
                        break;
                    }
            }
        }
        // - �������� -
        processInput(window);
        // - ��ջ��� -
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);            // ������������ɫ
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // �����ɫ���塢��Ȼ���
        // - ���ù۲�/ͶӰ���� -
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)sreenWidth / (float)sreenHeight, 0.1f, 100.0f);
        // --- ��ʼ��Ⱦ ---
        // - ��Ⱦ��Դ -
        if (W_LIGHT_MODE) {
            lightShader->use();
            lightShader->setMat4("view", view);
            lightShader->setMat4("projection", projection);
            glBindVertexArray(lightCubeVAO);
            for (const PointLight& pointLight : pointLights) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, pointLight.position);
                model = glm::scale(model, glm::vec3(0.01f));
                lightShader->setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        // - ��Ⱦģ�� -
        objectShader->use();
        objectShader->setMat4("view", view);
        objectShader->setMat4("projection", projection);
        objectShader->setVec3("viewPos", camera->position);
        objectShader->setBool("material.useMap", false);
        objectShader->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        objectShader->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        objectShader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        objectShader->setFloat("material.shininess", 32.0f);
        for (Object* object : objects)
            object->draw(*objectShader);
        // - ��Ⱦ��պ� -
        if (W_SKYBOX_MODE) {
            view = glm::mat4(glm::mat3(camera->getViewMatrix()));
            skyboxShader->use();
            skyboxShader->setMat4("view", view);
            skyboxShader->setMat4("projection", projection);
            glDepthFunc(GL_LEQUAL);
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS);
        }
        // --- ��Ⱦ��� ---
        // - ������ɫ���� -
        glfwSwapBuffers(window);
        // - ��鴥���¼� -
        glfwPollEvents();
    }
    // ---------------
    // [glfw] ���ٴ���
    // ---------------
    glfwTerminate();
    for (Object* object : objects)
        if (object)
            delete object;
    objects.clear();
    delete camera;
    delete objectShader;
    delete skyboxShader;
    delete lightShader;
    camera = nullptr;
    objectShader = skyboxShader = lightShader = nullptr;
    return 0;
}

/********************
 * [����] ���봦����
 ********************/
void World::processInput(GLFWwindow* window) {
    static bool bCameraPrint = true;

    // --- ESC ������ ---
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true); // ���ô��ڹرձ��
    }
    // --- SPACE ������ ---
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        // ��ӡ����ͷ����
        Camera *camera = World::getInstance()->camera;
        if (camera && bCameraPrint) {
            std::cout << "World::Current camera postion: (" << camera->position.x << "," << camera->position.y << "," << camera->position.z << ")" << std::endl;
            bCameraPrint = false;
        }
    }
    else {
        bCameraPrint = true;
    }
    // --- UP/W ������ ---
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
    // --- DOWN/S ������ ---
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
    // --- LEFT/A ������ ---
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::LEFT, deltaTime);
    // --- RIGHT/D ������ ---
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::RIGHT, deltaTime);
    return;
}

/********************
 * [����] ������غ���
 ********************/
GLuint World::loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "ERROR::Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

/********************
 * [����] ��������ͼ���غ���
 ********************/
// ˳��+X (right)  -X (left)  +Y (top)  -Y (bottom)  +Z (front)  -Z (back)
GLuint World::loadCubemap(std::vector<std::string> faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "ERROR::Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


/********************
 * [����] ���ڳߴ�ص�����
 ********************/
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);    // �����ӿ�
    return;
}

/********************
 * [����] ����ƶ��ص�����
 ********************/
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    World* world = World::getInstance();
    if (world->firstMouse) {
        world->lastX = float(xpos);
        world->lastY = float(ypos);
        world->firstMouse = false;
    }
    float xoffset = float(xpos) - world->lastX;
    float yoffset = world->lastY - float(ypos);
    world->lastX = float(xpos);
    world->lastY = float(ypos);
    world->camera->processMouseMovement(xoffset, yoffset);
    return;
}

/********************
 * [����] �����ֻص�����
 ********************/
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    World* world = World::getInstance();
    world->camera->processMouseScroll(float(yoffset));
    return;
}
