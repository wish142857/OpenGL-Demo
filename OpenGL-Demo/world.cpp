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
    objectShader = new Shader(SHADER_VS_PATH.c_str(), SHADER_FS_PATH.c_str());
    skyboxShader = new Shader(SKYBOX_SHADER_VS_PATH.c_str(), SKYBOX_SHADER_FS_PATH.c_str());
    // ----------
    // �����ʼ��
    // ----------
    // - ��������ʼ����� -
    camera = new Camera(glm::vec3(0.0f, 0.0f, 12.0f));
    lastX = sreenWidth / 2.0f;
    lastY = sreenHeight / 2.0f;
    firstMouse = true;
    // - ��ʼ����ʱ�� -
    deltaTime = 0.0f;
    lastFrame = 0.0f;
    // - ��ʼ�� stb_image -
    stbi_set_flip_vertically_on_load(false);
    // ---------------
    // [glad] ����ģ��
    // ---------------
    Model modelBird1("resources/objects/bird/1/1.obj");
    Model modelBird2("resources/objects/bird/2/2.obj");
    Model modelFlight("resources/objects/flight/F-35_Lightning_II/F-35_Lightning_II.obj");
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
    // [glfw/glad] ��Ⱦѭ��
    // --------------------
    while (!glfwWindowShouldClose(window)) {
        // --- ��ʱ�� ---
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // - �������� -
        processInput(window);
        // - ��ջ��� -
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);            // ������������ɫ
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // �����ɫ���塢��Ȼ���
        // - ������ɫ�� -
        objectShader->use();
        // --- ��ʼ��Ⱦ ---
        // - ���ù۲�/ͶӰ���� -
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)sreenWidth / (float)sreenHeight, 0.1f, 100.0f);
        objectShader->setMat4("view", view);
        objectShader->setMat4("projection", projection);
        // - ��Ⱦģ�� -
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.0f + sin((float)glfwGetTime()) / 4, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        objectShader->setMat4("model", model);
        modelBird1.drawModel(*objectShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f + cos((float)glfwGetTime()) / 4, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        objectShader->setMat4("model", model);
        modelBird2.drawModel(*objectShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
        objectShader->setMat4("model", model);
        modelFlight.drawModel(*objectShader);
        // - ��Ⱦ��պ� -
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
        // --- ��Ⱦ��� ---

        // - ������ɫ���� -
        glfwSwapBuffers(window);
        // - ��鴥���¼� -
        glfwPollEvents();
    }
    // ---------------
    // [glad] �ͷ���Դ
    // ---------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO); 
    // ---------------
    // [glfw] ���ٴ���
    // ---------------
    glfwTerminate();

    delete camera;
    delete objectShader;
    delete skyboxShader;
    return 0;
}

/********************
 * [����] ���봦����
 ********************/
void World::processInput(GLFWwindow* window) {
    // --- ESC ������ ---
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); // ���ô��ڹرձ��
    // --- W ������ ---
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
    // --- S ������ ---
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
    // --- A ������ ---
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->processKeyboard(CameraMovement::LEFT, deltaTime);
    // --- D ������ ---
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
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
