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
#include "ShaderWithGeo.h"

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
    //vtuShader = new ShaderWGeo("vtu-shader.vert", "vtu-shader.frag", "vtu-shader.geom");
    vtuShader = new Shader("vtu-shader.vert", "vtu-shader.frag");
    triangleShader = new Shader("simplge-shader.vert", "simple-shader.frag");
    simpleShader = new Shader("3.3.shader.vert", "3.3.shader.frag");

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);

    // load models
    // -----------       
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //FL_LINE or GL_FILL
    ourModel = Model("backpack/backpack.obj");
    app->parse_all_obj_to_txt();
    app->create_object(frame, app->vbo, app->vao, app->ebo);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    //SKYBOX -------------------------------------------------------------------------------------------
    skyboxShader = new Shader("6.1.skybox.vert", "6.1.skybox.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
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
    //-----------
    
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    // load textures
    //--------------
    vector<std::string> faces
    {
        "skybox/right.jpg",
        "skybox/left.jpg",
        "skybox/top.jpg",
        "skybox/bottom.jpg",
        "skybox/front.jpg",
        "skybox/back.jpg"
    };
    cubemapTexture = loadCubemap(faces);
    previousEnvironment = 0; // for performance. You don't want to be reloading an entire skybox every frame from file.

    // shader configuration
    // --------------------
    skyboxShader->use();
    skyboxShader->setInt("skybox", 0);
    //--------------------------------------------------------------------------------------------------
}

void WorkspaceState::Cleanup()
{
    delete vtuShader;
    delete triangleShader;
    delete simpleShader;
    delete skyboxShader;
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVAO);
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
    ImGui::End();

    static const char* environments[]{ "Skybox", "Dark", "Bright", "Grid" }; 
    static int selectedEnvItem = 0;

    ImGui::Begin("Data/Settings");
    
        if (ImGui::Button("Ortho/Perspective"))
        {
            perspectiveOn = !perspectiveOn;
        }
        ImGui::ListBox("Environment", &selectedEnvItem, environments, IM_ARRAYSIZE(environments));
        selectedEnvironment = selectedEnvItem;
        if (selectedEnvironment != previousEnvironment)
        {
            if (selectedEnvironment == 3)
            {
                // load textures
                //--------------
                vector<std::string> faces
                {
                    "skybox/myGrid.png",
                    "skybox/myGrid.png",
                    "skybox/myGrid.png",
                    "skybox/myGrid.png",
                    "skybox/myGrid.png",
                    "skybox/myGrid.png"
                };
                cubemapTexture = loadCubemap(faces);
                previousEnvironment = 3; // for performance. You don't want to be reloading an entire skybox every frame from file.
            }
            else if (selectedEnvironment == 0) 
            {
                // load textures
                //--------------
                vector<std::string> faces
                {
                    "skybox/right.jpg",
                    "skybox/left.jpg",
                    "skybox/top.jpg",
                    "skybox/bottom.jpg",
                    "skybox/front.jpg",
                    "skybox/back.jpg"
                };
                cubemapTexture = loadCubemap(faces);
                previousEnvironment = 0; // for performance. You don't want to be reloading an entire skybox every frame from file.
            }
        }
        
        

        // shader configuration
        // --------------------
        skyboxShader->use();
        skyboxShader->setInt("skybox", 0);
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
    glClearColor(0, 0, 0, 0);
    
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

    if (selectedEnvironment == 0 || selectedEnvironment == 3)
    {
        //SKYBOX
        //----------------------------------------------------
        // draw skybox first
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader->use();
        glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
        skyboxShader->setMat4("view", view);
        skyboxShader->setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        //----------------------------------------------------
    }
    

    // Render dear imgui onto screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int WorkspaceState::loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
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