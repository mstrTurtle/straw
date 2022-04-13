#include "encap.h"
#include <thread>
#include <mutex>

// 程序需要的部分状态，也许得把它改成原子的？
ImVec4 top_color = ImVec4(0.0f, 1.0f, 0.0f, 1.00f);
ImVec4 left_color = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
ImVec4 right_color = ImVec4(0.0f, 0.0f, 1.0f, 1.00f);
ImVec4 bottom_color = ImVec4(0.0f, 1.0f, 1.0f, 1.00f);
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
bool show_main_window = true;
bool the_same_color = false;
bool draw_trangle_without_render = false;
bool draw_trangle = false;
bool bonus_draw_line = false;
bool bonus_draw_another_trangle = false;

std::mutex m;

void th1() {
    /* 拉起一个窗口，并初始化ImGUI设定 */
    Graphic graphic("STRAW", 800, 600, true);
    //Graphic graphic1("TEST", 800, 600);


    // Render Loop
    while (!glfwWindowShouldClose(graphic.window))
    {

#pragma region noise

        // 创建ImGui
        glfwPollEvents();
        //Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Edit color", &show_main_window);
        if (ImGui::CollapsingHeader("Triangle"))
        {

            ImGui::Text("This is some useful text.");

            ImGui::ColorEdit3((const char*)u8"基础三角形 -- top color", (float*)&top_color);
            ImGui::ColorEdit3("Basic triangle -- left color", (float*)&left_color);
            ImGui::ColorEdit3("Basic triangle -- right color", (float*)&right_color);
            ImGui::ColorEdit3("Bonus triangle -- bottom color", (float*)&bottom_color);

            ImGui::Separator();

            ImGui::Checkbox("Draw triangle without rendering", &draw_trangle_without_render);
            ImGui::Checkbox("Basic draw triangle", &draw_trangle);
            ImGui::Checkbox("Bonus draw line", &bonus_draw_line);
            ImGui::Checkbox("Bonus draw another triangle", &bonus_draw_another_trangle);
        }

        ImGui::End();

#pragma endregion noise

        // 把窗口大小弄出来，以便调整视口大小
        int view_width, view_height;
        glfwGetFramebufferSize(graphic.window, &view_width, &view_height);
        glViewport(0, 0, view_width, view_height);
        // 然后把缓冲清除成指定颜色
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // 让ImGui先做渲染
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* 双缓冲，前缓冲是拿来投到屏上的东西，后缓冲用来实施渲染 */
        glfwSwapBuffers(graphic.window);
    }
}

void th2() {
    /* 拉起一个窗口 */
    Graphic graphic("TEST", 800, 600);

    static const char* VertexShaderCode = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 ourColor;
    void main()
    {
       gl_Position = vec4(aPos, 1.0);
       ourColor = aColor;
    }
    )";

    static const char* FragmentShaderCode = R"(
    #version 330 core
    out vec4 FragColor;
    in vec3 ourColor;
    void main()
    {
       FragColor = vec4(ourColor, 1.0f);
    }
    )";

    /* 编译链接ShaderProgram */
    int shaderProgram = buildShaderProgram(VertexShaderCode, FragmentShaderCode);

    unsigned int VBO, VAO, EBO;

    // Render Loop
    while (!glfwWindowShouldClose(graphic.window))
    {
        // 先内存里找个地方放所有顶点（两个三角形所用到的四个顶点）
        float vertices[] = {
            // Coordinate           // Color
             0.2f,  -0.2f,  0.0f,   right_color.x,  right_color.y,  right_color.z,      // bottom right
            -0.2f,  -0.2f,  0.0f,   left_color.x,   left_color.y,   left_color.z,       // bottom left
             0.0f,   0.2f,  0.0f,   top_color.x,    top_color.y,    top_color.z,        // top 
            -0.2f,  -0.4f,  0.0f,   bottom_color.x, bottom_color.y, bottom_color.z      // bottom
        };

        // 然后内存里用顶点索引表示你的三角形，方便它画（这东西最后被拷进显卡的Element Buffer Object，也叫Index Buffer Object里，索引数组）
        unsigned int indices[] = { // 注意索引从0开始
            0, 1, 2, // 第一个三角形
            0, 1, 3  // 第二个三角形
        };

        // 0. 申请 VAO、VBO、EBO 对象
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // 1. 绑定VAO
        glBindVertexArray(VAO);

        // 2. 把顶点数组复拷进VBO中
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // 3. 把索引数组拷进EBO中
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // 4. 为坐标设定VertexAttributePointer
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // 5. 为颜色设定VertexAttributePointer
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        //glEnableVertexAttribArray(1);

        // 6. 指定用前边编译的程序
        glUseProgram(shaderProgram);


        // 把窗口大小弄出来，以便调整视口大小
        int view_width, view_height;
        glfwGetFramebufferSize(graphic.window, &view_width, &view_height);
        glViewport(0, 0, view_width, view_height);
        // 然后把缓冲清除成指定颜色
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // 然后渲染我们自己的东西
        glfwPollEvents();

        // 1. 画黑三角形
        if (draw_trangle_without_render) {
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // 2. 画彩色三角形
        if (draw_trangle) {
            glEnableVertexAttribArray(1);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // 3. 画线
        if (bonus_draw_line) {
            glEnableVertexAttribArray(1);
            glBindVertexArray(VAO);
            glDrawArrays(GL_LINE_STRIP, 0, 2);
        }

        // 4. 再画个三角形
        if (bonus_draw_another_trangle) {
            glEnableVertexAttribArray(1);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        /* 双缓冲，前缓冲是拿来投到屏上的东西，后缓冲用来实施渲染 */
        glfwSwapBuffers(graphic.window);

        // 释放 VAO, VBO, EBO 对象
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

int main()
{
    std::thread thr1(th1);
    std::thread thr2(th2);
    thr1.join();
    thr2.join();

    return 0;
}
