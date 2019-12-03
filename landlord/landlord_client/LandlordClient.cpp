#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <cstring>
#include <vector>

#include "MyModule.h"
#include "MyMsg.h"
#include "LandlordHallC.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

class MyContext;
class MyLandlordClient : public MyModule
{
public:
    MyLandlordClient() :
        m_hall(nullptr),
        m_init(false),
        m_exit(false),
        show_demo_window(false),
        window(nullptr)
    {
        std::cout << "Create landlord client module" << std::endl;
    }
    virtual ~MyLandlordClient(){
        GLExit();
        std::cout << "Destory landlord client module" << std::endl;
    }

    virtual int Init(const char* param) override
    {
        SetRunInOneThread(true);
        Timeout(10, 0xff);
        m_hall.SetModule(this);
        std::cout << "landlord client start " << GetServiceName() << std::endl;
        return 0;
    }

    int GLInit()
    {
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return 1;

        const char* glsl_version = "#version 330";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

        // Create window with graphics context
        window = glfwCreateWindow(1280, 720, "Landlord demo client", NULL, NULL);
        if (window == NULL)
            return 1;
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        // Initialize OpenGL loader
        bool err = glewInit() != GLEW_OK;
        if (err)
        {
            fprintf(stderr, "Failed to initialize OpenGL loader!\n");
            return 1;
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        //io.Fonts->AddFontDefault();
        io.Fonts->AddFontFromFileTTF("landlord_client/resources/fonts/Cousine-Regular.ttf", 20.0f);
    }

    void GLUpdate(MyMsg* msg)
    {
        static ImVec4 clear_color{0.45f, 0.55f, 0.60f, 1.00f};

        glfwPollEvents();
 
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        m_hall.UIUpdate();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    void GLExit()
    {
        if(m_exit) return;
        m_exit = true;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    virtual int CB(MyMsg* msg) override
    {
        // 初始化opengl
        if(m_init == false){
            m_init = true;
            GLInit();
        }

        if(msg->GetMsgType() == MyMsg::MyMsgType::RESPONSE){
            MyRespMsg* rmsg = static_cast<MyRespMsg*>(msg);
            if(rmsg->GetRespMsgType() == MyRespMsg::MyRespMsgType::TIMER){
                if(!m_exit && !glfwWindowShouldClose(window)){
                    // 重新启动定时，用来刷新界面
                    Timeout(1, 0xff);
                    // 界面刷新
                    GLUpdate(msg);
                }else if(!m_exit){
                    GLExit();
                }
            }
        }

        if(msg->GetMsgType() == MyMsg::MyMsgType::TEXT){
            MyTextMsg* tmsg = static_cast<MyTextMsg*>(msg);
            m_hall.Parse(tmsg->session, tmsg->GetData());
        }
        return 0;
    }

    LandlordHallC m_hall;
    GLFWwindow* window;
    bool m_init;
    bool m_exit;
    bool show_demo_window;
};

extern "C" MyModule* my_mod_create()
{
    return new MyLandlordClient();
}

extern "C" void my_mod_destory(MyModule* m)
{
    delete m;
}
