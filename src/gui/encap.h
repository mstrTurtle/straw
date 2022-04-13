#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <GL/gl3w.h>  //  使用gl3w，glad也行，注意要在项目工程中添加gl3w.c（或者glad.c/使用glad）
#include <GLFW/glfw3.h>
#include <iostream>
#include <mutex>
#include <vector>

//当用户改变窗口的大小的时候，视口也应该被调整。我们可以对窗口注册一个回调函数，它会在每次窗口大小被调整的时候被调用。
//其实我把教程的这个函数删掉对结果也没有影响，不过教程说对于Retina屏幕输入的width和height都会比原值大，所以保留这个函数
void window_size_callback(GLFWwindow* window, int width, int height);

//使用GLSL编写出来的顶点着色器和片段着色器的代码
//顶点着色器主要是把输入的数据转化为标准化设备坐标，把它们转换至OpenGL的可视区域内
//片段着色器所做的是计算像素最后的颜色输出，可以理解为我们所画的图元“上色”
//这两个着色器程序都是最简单的程序，没有对输入的数据做任何处理就输出了
int buildShaderProgram (const char* const& VertexShaderCode, const char* const& FragmentShaderCode) {
    // 1.顶点着色器
    int VShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VShader, 1, &VertexShaderCode, NULL);
    glCompileShader(VShader);

    // 检查顶点着色器是否编译成功
    int success_compiler;
    char wrong_information[512];
    glGetShaderiv(VShader, GL_COMPILE_STATUS, &success_compiler);
    if (!success_compiler)
    {
        glGetShaderInfoLog(VShader, 512, NULL, wrong_information);
        std::cout << "顶点着色器编译失败：\n" << wrong_information << std::endl;
    }

    // 2.片段着色器
    int FShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FShader, 1, &FragmentShaderCode, NULL);
    glCompileShader(FShader);

    // 检查片段着色器是否编译成功
    glGetShaderiv(FShader, GL_COMPILE_STATUS, &success_compiler);
    if (!success_compiler)
    {
        glGetShaderInfoLog(FShader, 512, NULL, wrong_information);
        std::cout << "片段着色器编译失败：\n" << wrong_information << std::endl;
    }

    // 3.链接着色器
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, VShader);
    glAttachShader(shaderProgram, FShader);
    glLinkProgram(shaderProgram);

    // 检查链接着色器是否成功
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success_compiler);
    if (!success_compiler) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, wrong_information);
        std::cout << "着色器程序链接失败：\n" << wrong_information << std::endl;
    }

    // 4.在把着色器对象链接到程序对象以后，删除不再需要的着色器对象
    glDeleteShader(VShader);
    glDeleteShader(FShader);
    return shaderProgram;
};

class Graphic;

class ImGuiManager {
public:
    ImGuiManager(Graphic& graphic);
    ~ImGuiManager() {
        // 释放ImGui资源
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    Graphic& m_graphic;
};

class Graphic {
public:
    Graphic(const char* Window_title, const unsigned int Window_width = 800, const unsigned int Window_height = 600,bool im = 0) {
        std::lock_guard<std::mutex> guard(sm_lock);
        // 实例化GLFW窗口
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // 创建一个窗口对象，即Context。其中存放了窗口相关的所有数据，被GLFW其他函数频繁用到。
        m_window = glfwCreateWindow(Window_width, Window_height, Window_title, NULL, NULL);
        if (m_window == NULL)
        {
            std::cerr << "Failed to create GLFW m_window" << std::endl;
            glfwTerminate();
            exit(-1);
        }

        // 指定这个Context，设置sizeCallback，设置双缓冲置换最小间隔
        glfwMakeContextCurrent(m_window);
        glfwSetFramebufferSizeCallback(m_window, window_size_callback);
        glfwSwapInterval(1);

        //初始化gl3w
        gl3wInit();

        if (im)
            m_immanager = new ImGuiManager(*this);
    }
    ~Graphic() {
        delete m_immanager;
        // 清除所有申请的glfw资源
        glfwTerminate();
    }
    GLFWwindow* const& window = m_window;
private:
    static std::mutex sm_lock;
    GLFWwindow* m_window = nullptr;
    ImGuiManager* m_immanager = nullptr;
};

std::mutex Graphic::sm_lock;


ImGuiManager::ImGuiManager(Graphic& graphic) : m_graphic(graphic) {
    //创建并绑定ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_graphic.window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    ImGui::StyleColorsClassic();
}


void window_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

class Drawable{
public:
    virtual ~Drawable(){}
    virtual void draw() = 0;
};

class MultiLine : Drawable{
public:
    void draw() {

    }
};

class Polygon : Drawable{
public:
    void draw() {

    }
};

/// 给它一些线、多边形，它能把它画到buffer上
void drawall(std::vector<Drawable> v) {
    for (auto& d : v) {
        d.draw();
    }
}