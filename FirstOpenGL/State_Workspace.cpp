#include "State_Workspace.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// linked states:
#include "State_HomeScreen.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AppEngine.h"
#include "Model.h"
#include "shader_s.h"

#include <iostream>
#include <filesystem>

#define PI 3.1415926535897932384626

WorkspaceState WorkspaceState::s_WorkspaceState; // implement/create a Singleton instance on the stack

void WorkspaceState::Init()
{
    app = AppEngine::Instance();

    app->set_up_buffers_and_arrays();

    // set default to perspective projection lolol
    perspectiveOn = true;

    // camera
    camera = &(app->camera);
    
    frame = 0;

    // build and compile shaders
    // -------------------------
    vtuShader = new Shader("vtu-shader.vert", "vtu-shader.frag");
    triangleShader = new Shader("simplge-shader.vert", "simple-shader.frag");
    simpleShader = new Shader("3.3.shader.vert", "3.3.shader.frag");

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // load models
    // -----------       
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //FL_LINE or GL_FILL
    ourModel = Model("backpack/backpack.obj");
    app->parse_all_obj_to_txt();
    app->create_object(frame, app->vbo, app->vao, app->ebo);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}

void WorkspaceState::Cleanup()
{
    delete vtuShader;
    delete triangleShader;
    delete simpleShader;
}

void WorkspaceState::HandleEvents()
{

}

void WorkspaceState::Update()
{
    ImGui::Begin("Navigation");
    if (ImGui::Button("Home"))
    {
        app->ChangeState(HomeScreenState::Instance());
    }
    if (ImGui::Button("Ortho/Perspective"))
    {
        perspectiveOn = !perspectiveOn;
    }
    ImGui::End();

    prevFrame = frame; // set before you change it

    ImGui::Begin("Time evolution slider");
    static int sliderFrame = 0;
    ImGui::SliderInt("frame", &sliderFrame, 0, 200); // TODO: change range from 0 to max frame.
    //frame = sliderFrame;
    if (glfwGetKey(app->window, GLFW_KEY_P) == GLFW_PRESS) // P key pressed
    {
        frame++;
        cout << "new frame" << frame << endl;
        // may seem redundant but do not remove this conditional
        // rendering simulation frames is expensive so this makes it much more efficient.
        app->create_object(frame, app->vbo, app->vao, app->ebo); // load object into memory
    }else if (glfwGetKey(app->window, GLFW_KEY_O) == GLFW_PRESS) // P key pressed
    {
        frame--;
        cout << "new frame" << frame << endl;
        // may seem redundant but do not remove this conditional
        // rendering simulation frames is expensive so this makes it much more efficient.
        app->create_object(frame, app->vbo, app->vao, app->ebo); // load object into memory
    }

    // change frames according to slider
    if (sliderFrame != prevFrame)
    {
        frame = sliderFrame;
        app->create_object(frame, app->vbo, app->vao, app->ebo);
    }

    ImGui::End();

    ImGui::Begin("Triangle Position/Color");
    static float rotation = 0.0;
    ImGui::SliderFloat("rotation", &rotation, 0, 2 * PI);
    static float translation[] = { 0.0, 0.0 };
    ImGui::SliderFloat2("position", translation, -1.0, 1.0);
    static float color[4] = { 1.0f,1.0f,1.0f,1.0f };
    // pass the parameters to the shader
    triangleShader->setUniform("rotation", rotation);
    triangleShader->setUniform("translation", translation[0], 
        translation[1]);
    // color picker
    ImGui::ColorEdit3("color", color);
    // multiply triangle's color with this color
    triangleShader->setUniform("color", color[0], color[1], 
        color[2]);
    ImGui::End();

    //std::cout << camera->Position.x << std::endl;
}

void WorkspaceState::Draw()
{
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);

    glClearColor(0, 0, 0, 0); // black
    //glClearColor(1.0f,1.0f,1.0f,1.0f); //white

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // view/projection transformations
    
    if (perspectiveOn)
    {
        projection = glm::perspective(glm::radians(camera->Zoom), (float)app->scr_width / (float)app->scr_height, 0.1f, 100.0f);

    }
    else
    {
        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f, 100.0f);
    }
    glm::mat4 view = camera->GetViewMatrix();
    vtuShader->setMat4("projection", projection);
    vtuShader->setMat4("view", view);


    // ISSUE: Not refreshing newly drawn object
    //-------------------------------------------------------------
    // https://stackoverflow.com/questions/33607194/opengl-updating-vertices-array-buffer
    // https://stackoverflow.com/questions/41784790/how-to-update-vertex-buffer-data-frequently-every-frame-opengl/41784937
    //-------------------------------------------------------------

    // render VTK (.vtu) object
    // ----------------------------------------------------------------------------------------------------------------
    //if (prevFrame != frame) // if the user has changed the frame
    //{
    //    cout << "new frame" << frame << endl;
    //    glClearColor(0, 0, 0, 0); // black
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //    // may seem redundant but do not remove this conditional
    //    // rendering simulation frames is expensive so this makes it much more efficient.
    //    app->create_object(frame, app->vbo, app->vao, app->ebo); // load object into memory
    //}

    //app->create_object(frame, app->vbo, app->vao, app->ebo);

    //cout << "frame: " << frame << endl;

    vtuShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// scale it down
    vtuShader->setMat4("model", model);

    app->draw_object();

    // render the loaded model
    //ourShader->use();
    //glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    //model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// scale it down
    //ourShader->setMat4("model", model);
    //ourModel.Draw(*ourShader);

    // Render dear imgui onto screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}