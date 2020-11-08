#include "world.h"

/********************
 * [函数] 运行函数
 ********************/
int World::run(const std::string& mode) {
    if (mode == "STD")
        return this->runStdMode();
    if (mode == "RAY")
        return this->runRayMode();
    return this->runStdMode();
}


/********************
 * [函数] 运行标准模式函数
 ********************/
int World::runStdMode() {
    // ----------------------
    // [glfw] 初始化、设置与创建窗口
    // ----------------------
    // --- 初始化窗口 ---
    glfwInit();
    // --- 设置窗口 ---
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // --- 创建窗口 ---
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
    // - 设置回调 -
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    // - 设置输入模式 -
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // -------------------------------
    // [glad] 加载 OpneGL 函数指针
    // -------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR::Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // ---------------------------
    // [glad] 设置 OpenGL 全局状态
    // ---------------------------
    glEnable(GL_DEPTH_TEST);    // 开启深度测试
    // ---------------------------
    // [glad] 创建并编译着色器程序
    // ---------------------------
    objectShader = new Shader(OBJECT_SHADER_VS_PATH.c_str(), OBJECT_SHADER_FS_PATH.c_str());
    if (W_SKYBOX_MODE)
        skyboxShader = new Shader(SKYBOX_SHADER_VS_PATH.c_str(), SKYBOX_SHADER_FS_PATH.c_str());
    if (W_LIGHT_MODE)
        lightShader = new Shader(LIGHT_SHADER_VS_PATH.c_str(), LIGHT_SHADER_FS_PATH.c_str());
    // ----------
    // 程序初始化
    // ----------
    // - 初始化 stb_image -
    stbi_set_flip_vertically_on_load(false);
    // - 创建并初始化相机 -
    camera = new Camera(glm::vec3(0.0f, 0.2f, 1.0f));
    lastX = sreenWidth / 2.0f;
    lastY = sreenHeight / 2.0f;
    firstMouse = true;
    // - 初始化计时器 -
    deltaTime = 0.0f;
    lastFrame = 0.0f;
    // - 初始化材质 -
    materials.clear();
    // - 初始化光源 -
    if (W_LIGHT_MODE) {
        // 平行光
        dirLights.clear();
        dirLights.push_back(DirLight(
            glm::vec3(0.2f, 0.5f, 1.0f),        // direction
            glm::vec3(0.1f, 0.1f, 0.1f),        // ambient
            glm::vec3(0.8f, 0.8f, 0.8f),        // diffuse
            glm::vec3(1.0f, 1.0f, 1.0f)         // specular
        ));
        // 点光
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
        // 聚光
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
    // [glad] 加载模型
    // ---------------
    // - 加载天空盒 -
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
    // - 加载光源立方体 -
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
    // - 加载物体模型 -
    objects.clear();
    {
        // - 加载模型 1.bun_zipper -
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
        // - 加载模型 2.dragon_vrip -
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
        // - 加载模型 3.happy_vrip -
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
    // [glad] 加载光源
    // ---------------
    if (W_LIGHT_MODE) {
        objectShader->use();
        // - 加载平行光源 -
        // 仅加载列表首项
        if (!dirLights.empty()) {
            objectShader->setVec3("dirLight.direction", dirLights[0].direction);
            objectShader->setVec3("dirLight.ambient", dirLights[0].ambient);
            objectShader->setVec3("dirLight.diffuse", dirLights[0].diffuse);
            objectShader->setVec3("dirLight.specular", dirLights[0].specular);
        }
        // - 加载点光源 -
        // 仅加载最多三项
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
        // - 加载聚光源 -
        // 仅加载首项
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
    // [glfw/glad] 渲染循环
    // --------------------
    while (!glfwWindowShouldClose(window)) {
        // --- 计时器 ---
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // --- 处理运动 ---
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
        // - 处理输入 -
        processInput(window);
        // - 清空缓冲 -
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);            // 设置清空填充颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清空颜色缓冲、深度缓冲
        // - 设置观察/投影矩阵 -
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)sreenWidth / (float)sreenHeight, 0.1f, 100.0f);
        // --- 开始渲染 ---
        // - 渲染光源 -
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
        // - 渲染模型 -
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
        // - 渲染天空盒 -
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
        // --- 渲染完毕 ---
        // - 交换颜色缓冲 -
        glfwSwapBuffers(window);
        // - 检查触发事件 -
        glfwPollEvents();
    }
    // ---------------
    // [glfw] 销毁窗口
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
 * [函数] 运行光线追踪模式函数
 ********************/
int World::runRayMode() {
    // ----------------------
    // [glfw] 初始化、设置与创建窗口
    // ----------------------
    // --- 初始化窗口 ---
    glfwInit();
    // --- 设置窗口 ---
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // --- 创建窗口 ---
    sreenWidth = 640;
    sreenHeight = 480;
    GLFWwindow* window = glfwCreateWindow(sreenWidth, sreenHeight, "World", NULL, NULL);
    if (window == NULL) {
        std::cout << "ERROR::Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // - 设置回调 -
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    // - 设置输入模式 -
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // -------------------------------
    // [glad] 加载 OpneGL 函数指针
    // -------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR::Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // ---------------------------
    // [glad] 设置 OpenGL 全局状态
    // ---------------------------
    // glEnable(GL_DEPTH_TEST);    // 开启深度测试
    // ---------------------------
    // [glad] 创建并编译着色器程序
    // ---------------------------
    Shader* rayShader = new Shader(RAY_SHADER_VS_PATH.c_str(), RAY_SHADER_FS_PATH.c_str());
    // ----------
    // 程序初始化
    // ----------
    // - 初始化 stb_image -
    stbi_set_flip_vertically_on_load(false);
    // - 创建并初始化相机 -
    camera = new Camera(glm::vec3(0.0f, 2.0f, 3.0f));
    lastX = sreenWidth / 2.0f;
    lastY = sreenHeight / 2.0f;
    firstMouse = true;
    // - 初始化计时器 -
    deltaTime = 0.0f;
    lastFrame = 0.0f;
    // ---------------
    // [glad] 加载模型
    // ---------------
    // - 加载渲染点 -
    float point[] = { 0, 0, 0 };
    GLuint pointVAO, pointVBO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // - 构建场景 -
    RayTracing::Scene scene;
    // - 加载物体模型 -
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
        // - 加载模型 1.bun_zipper -
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
        // - 加载模型 2.dragon_vrip -
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
        // - 加载模型 3.happy_vrip -
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
    // [glfw/glad] 渲染循环
    // --------------------
    glm::vec3 viewPos = glm::vec3(0.0f, 2.0f, 3.0f);
    glm::vec3 viewFront = glm::vec3(0, 0, -1);
    glm::vec3 viewUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 viewRight = glm::normalize(glm::cross(viewFront, viewUp));
    bool isPainted = false;
    // --- 开始循环 ---
    while (!glfwWindowShouldClose(window)) {
        // --- 计时器 ---
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // - 处理输入 -
        processInput(window);
        // - 判断是否已绘制 -
        if (isPainted) {
            glfwPollEvents();
            continue;
        }
        // - 清空缓冲 -
        glClearColor(0.3, 0.3, 0.3, 1.0f);       // 设置清空填充颜色
        glClear(GL_COLOR_BUFFER_BIT);            // 清空颜色缓冲
        // - 设置观察/投影矩阵 -
        glm::mat4 view = glm::lookAt(viewPos, viewPos + viewFront, viewUp);
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)sreenWidth / sreenHeight, 0.1f, 100.0f);
        // glm::mat4 view = camera->getViewMatrix();
        // glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)sreenWidth / (float)sreenHeight, 0.1f, 100.0f);
        // --- 开始渲染 ---
        rayShader->use();
        glBindVertexArray(pointVAO);
        if (!isPainted) {
            // std::cout << "WORLD::REPAINT" << std::endl;
            for (unsigned int i = 0; i < sreenWidth; i++)
                for (unsigned int j = 0; j < sreenHeight; j++) {
                    // 将像素坐标分量映射到[0, 1]
                    glm::vec3 pos(float(i) * 2 / sreenWidth - 1.0f, float(j) * 2 / sreenHeight - 1.0f, 0.0f);
                    rayShader->setVec2("screenPos", pos.x, pos.y);

                    // 计算像素在世界坐标中的位置
                    glm::vec3 globalPos = viewPos + viewFront + pos.x * viewRight * (float(sreenWidth) / sreenHeight) + pos.y * viewUp;
                    // glm::vec3 globalPos = camera->position + camera->front + pos.x * camera->right * (float(sreenWidth) / sreenHeight) + pos.y * camera->up;
                    // 计算出光线并进行光线追踪
                    RayTracing::Ray ray(viewPos, globalPos);
                    // RayTracing::Ray ray(camera->position, globalPos);

                    // 绘制该处的像素
                    rayShader->setVec3("vertexColor", scene.traceRay(ray));
                    glDrawArrays(GL_POINTS, 0, 1);
                }
            isPainted = true;
        }
        // --- 渲染完毕 ---
        // - 交换颜色缓冲 -
        glfwSwapBuffers(window);
        // - 检查触发事件 -
        glfwPollEvents();
    }
    // ---------------
    // [glfw] 销毁窗口
    // ---------------
    glfwTerminate();
    // - 销毁实体 -


    // - 销毁相机 -
    if (camera)
        delete camera;
    // - 销毁着色器 -
    if (rayShader)
        delete rayShader;
    camera = nullptr;
    rayShader = nullptr;
    return 0;
}


/********************
 * [函数] 输入处理函数
 ********************/
void World::processInput(GLFWwindow* window) {
    static bool bCameraPrint = true;
    // --- ESC 键按下 ---
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true); // 设置窗口关闭标记
    }
    // --- SPACE 键按下 ---
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        // 打印摄像头坐标
        Camera *camera = World::getInstance()->camera;
        if (camera && bCameraPrint) {
            std::cout << "World::Current camera postion: (" << camera->position.x << "," << camera->position.y << "," << camera->position.z << ")" << std::endl;
            bCameraPrint = false;
        }
    }
    else {
        bCameraPrint = true;
    }
    // --- UP/W 键按下 ---
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
    // --- DOWN/S 键按下 ---
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
    // --- LEFT/A 键按下 ---
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::LEFT, deltaTime);
    // --- RIGHT/D 键按下 ---
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::RIGHT, deltaTime);
    return;
}


/********************
 * [函数] 纹理加载函数
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
 * [函数] 立方体贴图加载函数
 ********************/
// 顺序：+X (right)  -X (left)  +Y (top)  -Y (bottom)  +Z (front)  -Z (back)
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
 * [函数] 窗口尺寸回调函数
 ********************/
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);    // 调整视口
    return;
}


/********************
 * [函数] 鼠标移动回调函数
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
 * [函数] 鼠标滚轮回调函数
 ********************/
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    World* world = World::getInstance();
    world->camera->processMouseScroll(float(yoffset));
    return;
}
