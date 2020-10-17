#include "world.h"


/********************
 * [函数] 运行函数
 ********************/
int World::run() {
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
    objectShader = new Shader(SHADER_VS_PATH.c_str(), SHADER_FS_PATH.c_str());
    skyboxShader = new Shader(SKYBOX_SHADER_VS_PATH.c_str(), SKYBOX_SHADER_FS_PATH.c_str());
    // ----------
    // 程序初始化
    // ----------
    // - 创建并初始化相机 -
    camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
    lastX = sreenWidth / 2.0f;
    lastY = sreenHeight / 2.0f;
    firstMouse = true;
    // - 初始化计时器 -
    deltaTime = 0.0f;
    lastFrame = 0.0f;
    // - 初始化 stb_image -
    stbi_set_flip_vertically_on_load(false);
    // ---------------
    // [glad] 加载模型
    // ---------------
    Model ourModel("resources/objects/test/1.obj");
    Model ourModel2("resources/objects/bag/treasure_chest.obj");
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    std::vector<std::string> faces {
        "resources/textures/skybox/right.jpg",
        "resources/textures/skybox/left.jpg",
        "resources/textures/skybox/top.jpg",
        "resources/textures/skybox/bottom.jpg",
        "resources/textures/skybox/front.jpg",
        "resources/textures/skybox/back.jpg",
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    // --------------------
    // [glfw/glad] 渲染循环
    // --------------------
    while (!glfwWindowShouldClose(window)) {
        // --- 计时器 ---
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // - 处理输入 -
        processInput(window);
        // - 清空缓冲 -
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);            // 设置清空填充颜色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清空颜色缓冲、深度缓冲
        // - 激活着色器 -
        objectShader->use();
        // --- 开始渲染 ---
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)sreenWidth / (float)sreenHeight, 0.1f, 100.0f);
        glm::mat4 view = camera->getViewMatrix();
        objectShader->setMat4("projection", projection);
        objectShader->setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        objectShader->setMat4("model", model);
        ourModel.drawModel(*objectShader);
        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        objectShader->setMat4("model", model);
        ourModel2.drawModel(*objectShader);

        // - 渲染天空盒 -
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader->use();
        view = glm::mat4(glm::mat3(camera->getViewMatrix())); // remove translation from the view matrix
        skyboxShader->setMat4("view", view);
        skyboxShader->setMat4("projection", projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
        // --- 渲染完毕 ---

        // - 交换颜色缓冲 -
        glfwSwapBuffers(window);
        // - 检查触发事件 -
        glfwPollEvents();
    }
    // ---------------
    // [glad] 释放资源
    // ---------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO); 
    // ---------------
    // [glfw] 销毁窗口
    // ---------------
    glfwTerminate();

    delete camera;
    delete objectShader;
    delete skyboxShader;
    return 0;
}

/********************
 * [函数] 输入处理函数
 ********************/
void World::processInput(GLFWwindow* window) {
    // --- ESC 键按下 ---
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); // 设置窗口关闭标记
    // --- W 键按下 ---
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
    // --- S 键按下 ---
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
    // --- A 键按下 ---
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::LEFT, deltaTime);
    // --- D 键按下 ---
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
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
