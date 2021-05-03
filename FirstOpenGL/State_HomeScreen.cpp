#include "State_HomeScreen.h"

// linked states:
#include "State_Workspace.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include "AppEngine.h"

#include "shader_s.h"

#define PI 3.1415926535897932384626

HomeScreenState HomeScreenState::s_HomeScreenState; // implement/create a Singleton instance on the stack

void HomeScreenState::Init()
{
    app = AppEngine::Instance();

    app->create_object(app->vbo, app->vao, app->ebo);

    // build and compile our shader program
    // ------------------------------------
    ourShader = new Shader("3.3.shader.vert", "3.3.shader.frag");
    triangle_shader = new Shader("simple-shader.vert", "simple-shader.frag");

    // Setup Dear ImGui style
    ImGui::StyleColorsLight();
}

void HomeScreenState::Cleanup()
{
    delete ourShader;
    delete triangle_shader;
}

void HomeScreenState::HandleEvents()
{

}

void HomeScreenState::Update()
{
    ImGui::Begin("Demo window");
    if (ImGui::Button("Hello!"))
    {
        app->ChangeState(WorkspaceState::Instance());
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
}

void HomeScreenState::Draw()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //white
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render moveable triangle
    triangle_shader->use();
    glBindVertexArray(app->vao);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Render dear imgui onto screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}