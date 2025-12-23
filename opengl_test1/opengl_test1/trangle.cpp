#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// 顶点着色器源码（添加旋转矩阵）
const char* vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec3 aPos;

uniform float rotationAngle;

void main()
{
    // 创建旋转矩阵（绕Z轴旋转）
    float cosTheta = cos(rotationAngle);
    float sinTheta = sin(rotationAngle);
    
    mat4 rotationMatrix = mat4(
        cosTheta, -sinTheta, 0.0, 0.0,
        sinTheta,  cosTheta, 0.0, 0.0,
        0.0,        0.0,     1.0, 0.0,
        0.0,        0.0,     0.0, 1.0
    );
    
    gl_Position = rotationMatrix * vec4(aPos, 1.0);
})";

//片段着色器源码
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = ourColor;\n"
"}\0";


float mouseX = 0.0f;
float mouseY = 0.0f;
float rotationAngle = 0.0f;  // 旋转角度
float rotationSpeed = 1.0f;  // 旋转速度

// 窗口大小变化回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// 键盘输入处理
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// 键盘控制旋转（可选）
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		rotationAngle = 0.0f;  // 按R键重置旋转
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// 更新全局鼠标位置
	mouseX = static_cast<float>(xpos);
	mouseY = static_cast<float>(ypos);
	// 可选：打印鼠标位置
	std::cout << "Mouse: (" << mouseX << ", " << mouseY << ")\n";
}


// 鼠标按钮回调函数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			std::cout << "左键点击 - 顺时针旋转\n";
			rotationSpeed = 1.0f;  // 顺时针
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			std::cout << "右键点击 - 逆时针旋转\n";
			rotationSpeed = -1.0f; // 逆时针
		}
	}
	else if (action == GLFW_RELEASE) {
		// 松开时停止旋转
		rotationSpeed = 0.0f;
	}
}


int main()
{
	//初始化GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口并设置 OpenGL 上下文，window地址为null表示窗口创建失败，销毁glfw
	GLFWwindow* window = glfwCreateWindow(600, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// 使用 GLAD 加载 OpenGL 函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// 开始使用OpenGl函数
	glViewport(0, 0, 600, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);        // 注册鼠标移动回调
	glfwSetMouseButtonCallback(window, mouse_button_callback); // 注册鼠标按钮回调

	// 1. 顶点输入
	// 定义顶点数据
	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};
	// 创建顶点缓冲对象VBO,顶点数组对象VAO
	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	// 绑定顶点数组对象VAO
	glBindVertexArray(VAO);
	// 顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER,VBO绑定到GL_ARRAY_BUFFER目标上
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData是一个专门用来把用户定义的数据复制到当前绑定缓冲的函数
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//2.顶点着色器
	//定义顶点着色器
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//着色器源码附加到着色器对象上，然后编译它
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//检测编译时错误,进行错误处理
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//3. 片段着色器
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//4. 着色器程序对象
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	//5. 设置顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	int colorLocation = glGetUniformLocation(shaderProgram, "ourColor");
	//获取旋转角度uniform的位置
	int rotationLocation = glGetUniformLocation(shaderProgram, "rotationAngle");

	// 设置帧率控制
	float lastFrame = 0.0f;
	float deltaTime = 0.0f;


	while (!glfwWindowShouldClose(window))
	{
		// 计算帧时间
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 输入处理,ESC键退出
		processInput(window);

		//glClearColor这是一个使用状态的函数，因为它使用当前状态来检索清除颜色。
		glClearColor(0.0f, 0.1f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 使用着色器程序
		glUseProgram(shaderProgram);

		// 更新旋转角度
		rotationAngle += rotationSpeed * deltaTime * 2.0f;
		glUniform1f(rotationLocation, rotationAngle);

		// 根据鼠标位置计算颜色
		// 将鼠标坐标从[0,800]和[0,600]映射到[0,1]范围
		float redValue = mouseX / 600.0f;
		float greenValue = mouseY / 600.0f;
		float blueValue = (redValue + greenValue) * 0.5f; // 混合色
		float alphaValue = 1.0f;

		// 设置uniform颜色值
		glUniform4f(colorLocation, redValue, greenValue, blueValue, alphaValue);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);


		// 双缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 删除分配的资源
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	// 释放GLFW分配的资源
	glfwTerminate();
	return 0;

}

//1. 初始化GLFW窗口
//2. 初始化GLAD加载器
//3. 准备顶点数据
//4. 创建并配置VAO / VBO
//5. 编译着色器
//6. 创建着色器程序
//7. 获取Uniform位置
//8. 渲染循环（清屏→使用程序→传递Uniform→绘制）
//9. 交换缓冲区
//10. 清理资源