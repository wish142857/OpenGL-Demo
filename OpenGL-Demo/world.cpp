#include "world.h"

/********************
 * [����] ���к���
 ********************/
int World::run(const std::string& mode) {
    if (mode == "STD")
        return this->runStdMode();
    if (mode == "RAY")
        return this->runRayMode();
    return this->runStdMode();
}


/********************
 * [����] ���б�׼ģʽ����
 ********************/
int World::runStdMode() {
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
    if (W_SKYBOX_MODE)
        skyboxShader = new Shader(SKYBOX_SHADER_VS_PATH.c_str(), SKYBOX_SHADER_FS_PATH.c_str());
    if (W_LIGHT_MODE)
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
            glm::vec3(0.2f, 0.5f, 1.0f),        // direction
            glm::vec3(0.1f, 0.1f, 0.1f),        // ambient
            glm::vec3(0.8f, 0.8f, 0.8f),        // diffuse
            glm::vec3(1.0f, 1.0f, 1.0f)         // specular
        ));
        // ���
        pointLights.clear();
        pointLights.push_back(PointLight(
            glm::vec3(0.0f, 0.5f, 0.5f),        // position
            1.0f,                               // constant
            0.09f,                              // linear
            0.032f,                             // quadratic
            glm::vec3(0.05f, 0.05f, 0.05f),     // ambient
            glm::vec3(0.8f, 0.8f, 0.8f),        // diffuse
            glm::vec3(1.0f, 1.0f, 1.0f)         // specular
        ));
        // �۹�
        spotLights.clear();
        spotLights.push_back(SpotLight(
            glm::vec3(0.0f, 0.0f, 0.0f),        // position
            glm::vec3(0.0f, 0.0f, 0.0f),        // direction
            glm::cos(glm::radians(7.5f)),       // cutOff
            glm::cos(glm::radians(10.0f)),      // outerCutOff
            1.0f,                               // constant
            0.09f,                              // linear
            0.032f,                             // quadratic
            glm::vec3(0.0f, 0.0f, 0.0f),        // ambient
            glm::vec3(1.0f, 1.0f, 1.0f),        // diffuse
            glm::vec3(1.0f, 1.0f, 1.0f)         // specular
        ));
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
        // - ����ģ�� 1.bun_zipper -
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        Object* object = new Object("<bun_zipper>", "resources/objects/std/bun_zipper.ply", model, 
            glm::vec3(-0.4f, -0.2f, -0.6f), glm::vec3(0.4f, 0.2f, 0.6f));
        object->useMap = false;
        object->material.ambient = glm::vec3(0.19225f, 0.19225f, 0.19225f);
        object->material.diffuse = glm::vec3(0.50754f, 0.50754f, 0.50754f);
        object->material.specular = glm::vec3(0.508273f, 0.508273f, 0.508273f);
        object->material.shininess = 51.2f;
        objects.push_back(object);
    }
    {
        // - ����ģ�� 2.dragon_vrip -
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.3f, 0.0f, 0.0f));
        // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        Object* object = new Object("<dragon_vrip>", "resources/objects/std/dragon_vrip.ply", model,
            glm::vec3(-0.4f, -0.2f, -0.6f), glm::vec3(0.4f, 0.2f, 0.6f));
        object->useMap = false;
        object->material.ambient = glm::vec3(0.24725f, 0.1995f, 0.0745f);
        object->material.diffuse = glm::vec3(0.75164f, 0.60648f, 0.22648f);
        object->material.specular = glm::vec3(0.628281f, 0.555802f, 0.366065f);
        object->material.shininess = 51.2f;
        objects.push_back(object);
    }
    {
        // - ����ģ�� 3.happy_vrip -
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.2f, 0.0f, 0.0f));
        // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        Object* object = new Object("<happy_vrip>", "resources/objects/std/happy_vrip.ply", model,
            glm::vec3(-0.4f, -0.2f, -0.6f), glm::vec3(0.4f, 0.2f, 0.6f));
        object->useMap = false;
        object->material.ambient = glm::vec3(0.19125f, 0.0735f, 0.0225f);
        object->material.diffuse = glm::vec3(0.7038f, 0.27048f, 0.0828f);
        object->material.specular = glm::vec3(0.256777f, 0.137622f, 0.086014f);
        object->material.shininess = 12.8f;
        objects.push_back(object);
    }
    // ---------------
    // [glad] ���ع�Դ
    // ---------------
    if (W_LIGHT_MODE) {
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
        objectShader->setInt("pointLightNumber", pointLights.size() <= 3 ? pointLights.size() : 3);
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
            objectShader->setVec3("spotLight.position", camera->position);
            objectShader->setVec3("spotLight.direction", camera->front);
            objectShader->setVec3("spotLight.ambient", spotLights[0].ambient);
            objectShader->setVec3("spotLight.diffuse", spotLights[0].diffuse);
            objectShader->setVec3("spotLight.specular", spotLights[0].specular);
            objectShader->setFloat("spotLight.constant", spotLights[0].constant);
            objectShader->setFloat("spotLight.linear", spotLights[0].linear);
            objectShader->setFloat("spotLight.quadratic", spotLights[0].quadratic);
            objectShader->setFloat("spotLight.cutOff", spotLights[0].cutOff);
            objectShader->setFloat("spotLight.outerCutOff", spotLights[0].outerCutOff);
        }
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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);            // ������������ɫ
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
        if (W_LIGHT_MODE && !spotLights.empty()) {
            objectShader->setVec3("spotLight.position", camera->position);
            objectShader->setVec3("spotLight.direction", camera->front);
        }
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
    if (camera)
        delete camera;
    if (objectShader)
        delete objectShader;
    if (skyboxShader)
        delete skyboxShader;
    if (lightShader)
        delete lightShader;
    camera = nullptr;
    objectShader = skyboxShader = lightShader = nullptr;
    return 0;
}


/********************
 * [����] ���й���׷��ģʽ����
 ********************/
int World::runRayMode() {
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
    sreenWidth = 640;
    sreenHeight = 480;
    GLFWwindow* window = glfwCreateWindow(sreenWidth, sreenHeight, "World", NULL, NULL);
    if (window == NULL) {
        std::cout << "ERROR::Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
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
    // glEnable(GL_DEPTH_TEST);    // ������Ȳ���
    // ---------------------------
    // [glad] ������������ɫ������
    // ---------------------------
    Shader* rayShader = new Shader(RAY_SHADER_VS_PATH.c_str(), RAY_SHADER_FS_PATH.c_str());
    // ----------
    // �����ʼ��
    // ----------
    // - ��ʼ�� stb_image -
    stbi_set_flip_vertically_on_load(false);
    // - ��������ʼ����� -
    camera = new Camera(glm::vec3(0.0f, 2.0f, 3.0f));
    lastX = sreenWidth / 2.0f;
    lastY = sreenHeight / 2.0f;
    firstMouse = true;
    // - ��ʼ����ʱ�� -
    deltaTime = 0.0f;
    lastFrame = 0.0f;
    // ---------------
    // [glad] ����ģ��
    // ---------------
    // - ������Ⱦ�� -
    float point[] = { 0, 0, 0 };
    GLuint pointVAO, pointVBO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // - �������� -
    RayTracing::Scene scene;
    // - ��������ģ�� -
    scene.addLight(new RayTracing::DirLight(
        glm::vec3(0.2f, 0.2f, 0.2f),
        glm::vec3(0.6f, 0.6f, 0.6f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(-0.5f, -1.0f, -1.0f)
    ));
    RayTracing::Material planeMaterial;
    planeMaterial.kShade = 0.7f;
    planeMaterial.kReflect = 0.3f;
    planeMaterial.kRefract = 0.0f;
    auto isBlack = [](const glm::vec3& pos) {
        return fmod(floor(pos.x) + floor(pos.z), 2) == 0;
    };
    planeMaterial.ambient = [=](const glm::vec3& pos)->glm::vec3 {
        glm::vec3 color(1.0f, 1.0f, 1.0f);
        return isBlack(pos) ? color : glm::vec3(1.0f, 1.0f, 1.0f) - color;
    };
    planeMaterial.diffuse = [=](const glm::vec3& pos)->glm::vec3 {
        glm::vec3 color(1.0f, 1.0f, 1.0f);
        return isBlack(pos) ? color : glm::vec3(1.0f, 1.0f, 1.0f) - color;
    };
    planeMaterial.specular = [=](const glm::vec3& pos)->glm::vec3 {
        glm::vec3 color(1.0f, 1.0f, 1.0f);
        return isBlack(pos) ? color : glm::vec3(1.0f, 1.0f, 1.0f) - color;
    };
    planeMaterial.shininess = [](const glm::vec3& pos)->float {
        return 32.0f;
    };
    RayTracing::Plane* plane = new RayTracing::Plane(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    plane->material = planeMaterial;
    scene.addEntity(plane);


    plane = new RayTracing::Plane(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    plane->material = planeMaterial;
    scene.addEntity(plane);

    RayTracing::Material ballMaterial;
    ballMaterial.kShade = 0.6f;
    ballMaterial.kReflect = 0.2f;
    ballMaterial.kRefract = 0.2f;
    ballMaterial.refractiveIndex = 1.5f;
    ballMaterial.ambient = [](const glm::vec3& pos)->glm::vec3 {
        return { 1.0f, 1.0f, 1.0f };
    };
    ballMaterial.diffuse = [](const glm::vec3& pos)->glm::vec3 {
        return { 1.0f, 1.0f, 1.0f };
    };
    ballMaterial.specular = [](const glm::vec3& pos)->glm::vec3 {
        return  { 0.6f, 0.6f, 0.6f };
    };
    ballMaterial.shininess = [](const glm::vec3& pos)->float {
        return 32.0f;
    };
    RayTracing::Sphere* ball = new RayTracing::Sphere(glm::vec3(0.0f, 2.0f, 0.0f), 1.0f);
    ball->material = ballMaterial;
    scene.addEntity(ball);
    /*
    objects.clear();
    {
        // - ����ģ�� 1.bun_zipper -
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        Object* object = new Object("<bun_zipper>", "resources/objects/std/bun_zipper.ply", model,
            glm::vec3(-0.4f, -0.2f, -0.6f), glm::vec3(0.4f, 0.2f, 0.6f));
        object->useMap = false;
        object->material.ambient = glm::vec3(0.19225f, 0.19225f, 0.19225f);
        object->material.diffuse = glm::vec3(0.50754f, 0.50754f, 0.50754f);
        object->material.specular = glm::vec3(0.508273f, 0.508273f, 0.508273f);
        object->material.shininess = 51.2f;
        objects.push_back(object);
    }
    {
        // - ����ģ�� 2.dragon_vrip -
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.3f, 0.0f, 0.0f));
        // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        Object* object = new Object("<dragon_vrip>", "resources/objects/std/dragon_vrip.ply", model,
            glm::vec3(-0.4f, -0.2f, -0.6f), glm::vec3(0.4f, 0.2f, 0.6f));
        object->useMap = false;
        object->material.ambient = glm::vec3(0.24725f, 0.1995f, 0.0745f);
        object->material.diffuse = glm::vec3(0.75164f, 0.60648f, 0.22648f);
        object->material.specular = glm::vec3(0.628281f, 0.555802f, 0.366065f);
        object->material.shininess = 51.2f;
        objects.push_back(object);
    }
    {
        // - ����ģ�� 3.happy_vrip -
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.2f, 0.0f, 0.0f));
        // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        Object* object = new Object("<happy_vrip>", "resources/objects/std/happy_vrip.ply", model,
            glm::vec3(-0.4f, -0.2f, -0.6f), glm::vec3(0.4f, 0.2f, 0.6f));
        object->useMap = false;
        object->material.ambient = glm::vec3(0.19125f, 0.0735f, 0.0225f);
        object->material.diffuse = glm::vec3(0.7038f, 0.27048f, 0.0828f);
        object->material.specular = glm::vec3(0.256777f, 0.137622f, 0.086014f);
        object->material.shininess = 12.8f;
        objects.push_back(object);
    }
    */

    // --------------------
    // [glfw/glad] ��Ⱦѭ��
    // --------------------
    glm::vec3 viewPos = glm::vec3(0.0f, 2.0f, 3.0f);
    glm::vec3 viewFront = glm::vec3(0, 0, -1);
    glm::vec3 viewUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 viewRight = glm::normalize(glm::cross(viewFront, viewUp));
    bool isPainted = false;
    // --- ��ʼѭ�� ---
    while (!glfwWindowShouldClose(window)) {
        // --- ��ʱ�� ---
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // - �������� -
        processInput(window);
        // - �ж��Ƿ��ѻ��� -
        if (isPainted) {
            glfwPollEvents();
            continue;
        }
        // - ��ջ��� -
        glClearColor(0.3, 0.3, 0.3, 1.0f);       // ������������ɫ
        glClear(GL_COLOR_BUFFER_BIT);            // �����ɫ����
        // - ���ù۲�/ͶӰ���� -
        glm::mat4 view = glm::lookAt(viewPos, viewPos + viewFront, viewUp);
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)sreenWidth / sreenHeight, 0.1f, 100.0f);
        // glm::mat4 view = camera->getViewMatrix();
        // glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)sreenWidth / (float)sreenHeight, 0.1f, 100.0f);
        // --- ��ʼ��Ⱦ ---
        rayShader->use();
        glBindVertexArray(pointVAO);
        if (!isPainted) {
            // std::cout << "WORLD::REPAINT" << std::endl;
            for (unsigned int i = 0; i < sreenWidth; i++)
                for (unsigned int j = 0; j < sreenHeight; j++) {
                    // �������������ӳ�䵽[0, 1]
                    glm::vec3 pos(float(i) * 2 / sreenWidth - 1.0f, float(j) * 2 / sreenHeight - 1.0f, 0.0f);
                    rayShader->setVec2("screenPos", pos.x, pos.y);

                    // �������������������е�λ��
                    glm::vec3 globalPos = viewPos + viewFront + pos.x * viewRight * (float(sreenWidth) / sreenHeight) + pos.y * viewUp;
                    // glm::vec3 globalPos = camera->position + camera->front + pos.x * camera->right * (float(sreenWidth) / sreenHeight) + pos.y * camera->up;
                    // ��������߲����й���׷��
                    RayTracing::Ray ray(viewPos, globalPos);
                    // RayTracing::Ray ray(camera->position, globalPos);

                    // ���Ƹô�������
                    rayShader->setVec3("vertexColor", scene.traceRay(ray));
                    glDrawArrays(GL_POINTS, 0, 1);
                }
            isPainted = true;
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
    // - ����ʵ�� -


    // - ������� -
    if (camera)
        delete camera;
    // - ������ɫ�� -
    if (rayShader)
        delete rayShader;
    camera = nullptr;
    rayShader = nullptr;
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
