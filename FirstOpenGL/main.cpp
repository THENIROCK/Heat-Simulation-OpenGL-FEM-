//#include <imgui/imgui.h>
//#include <imgui/imgui_impl_opengl3.h>
//#include <imgui/imgui_impl_glfw.h>
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include "AppEngine.h"
//#include "State_Workspace.h"
//
//#include "shader_s.h"
//
//#include <iostream>
//
//#define PI 3.1415926535897932384626
//
//// settings
//const unsigned int SCR_WIDTH = 1600;
//const unsigned int SCR_HEIGHT = 1200;
//
//int main()
//{
//    AppEngine* app = AppEngine::Instance();
//
//    // Initialise the app 
//    app->Init("CryoSIM", SCR_WIDTH, SCR_HEIGHT);
//
//    // Load the Workspace
//    app->ChangeState(WorkspaceState::Instance());
//
//    // render loop
//    // -----------
//    while (!glfwWindowShouldClose(app->window))
//    {
//        // input
//        // -----
//        app->HandleEvents();
//
//        // Update
//        //-------
//        app->Update();
//        
//        // render
//        // ------
//        app->Draw();
//
//        int display_w, display_h;
//        glfwGetFramebufferSize(app->window, &display_w, &display_h);
//        glViewport(0, 0, display_w, display_h);
//
//        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//        // -------------------------------------------------------------------------------
//        glfwSwapBuffers(app->window);
//        glfwPollEvents();
//    }
//
//    app->Cleanup();
//}
