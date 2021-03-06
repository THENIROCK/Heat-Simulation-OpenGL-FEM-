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

    // camera
    camera = &(app->camera);
    
    app->create_triangle(app->vbo, app->vao, app->ebo);

    // build and compile shaders
    // -------------------------
    ourShader = new Shader("1.model_loading.vert", "1.model_loading.frag");
    triangle_shader = new Shader("simple-shader.vert", "simple-shader.frag");

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // load models
    // -----------       
    ourModel = Model("backpack/backpack.obj");

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
}

void WorkspaceState::Cleanup()
{
    delete ourShader;
    delete triangle_shader;
}

void WorkspaceState::HandleEvents()
{

}

void WorkspaceState::Update()
{
    ImGui::Begin("Demo window");
    if (ImGui::Button("Hello!"))
    {
        app->ChangeState(HomeScreenState::Instance());
    }
    ImGui::End();

    ImGui::Begin("Triangle Position/Color");
    static float rotation = 0.0;
    ImGui::SliderFloat("rotation", &rotation, 0, 2 * PI);
    static float translation[] = { 0.0, 0.0 };
    ImGui::SliderFloat2("position", translation, -1.0, 1.0);
    static float color[4] = { 1.0f,1.0f,1.0f,1.0f };
    // pass the parameters to the shader
    triangle_shader->setUniform("rotation", rotation);
    triangle_shader->setUniform("translation", translation[0], 
        translation[1]);
    // color picker
    ImGui::ColorEdit3("color", color);
    // multiply triangle's color with this color
    triangle_shader->setUniform("color", color[0], color[1], 
        color[2]);
    ImGui::End();

    std::cout << camera->Position.x << std::endl;
}

void WorkspaceState::Draw()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //white
    glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

    // render moveable triangle
    /*triangle_shader->use();
    glBindVertexArray(app->vao);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);*/

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)app->scr_width / (float)app->scr_height, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);

    // render the loaded model
    ourShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// scale it down
    ourShader->setMat4("model", model);
    ourModel.Draw(*ourShader);

    // Render dear imgui onto screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}