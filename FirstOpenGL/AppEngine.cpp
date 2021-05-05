#include "AppEngine.h"
#include "AppState.h"
#include "XMLParser.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <filesystem>

using namespace std;

AppEngine AppEngine::s_AppEngine;

// definitions to avoid unresolved external symbol error https://stackoverflow.com/questions/195207/unresolved-external-symbol-on-static-class-members
float AppEngine::lastX;
float AppEngine::lastY;
bool AppEngine::firstMouse;
Camera AppEngine::camera;
float AppEngine::deltaTime;
float AppEngine::lastFrame;

//The Beginning and The End
//-------------------------
int AppEngine::Init(const char* title, int width, int height, int bpp, bool fullscreen)
{

    // variable assignments
    scr_width = width;
    scr_height = height;
    camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    lastX = scr_width / 2.0f;
    lastY = scr_height / 2.0f;
    firstMouse = true;
    // timing
    deltaTime = 0.0f;
    lastFrame = 0.0f;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    // use GL 3.2
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glsl_version = "#version 150";
#else
    // use GL 3.0
    glsl_version = "#version 130";
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // set GLFW input modes
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui context (Needs to be after OpenGL/GLAD because it accesses the OpenGL version)
    //-------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(AppEngine::window, true);
    ImGui_ImplOpenGL3_Init(AppEngine::glsl_version);

    return 1;
}
int AppEngine::Cleanup()
{
    // call cleanup in all application states stored
    while (!states.empty())
    {
        states.back()->Cleanup();
        states.pop_back();
    }

    // cleanup imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // de-allocate all resources once they've outlived their purpose:
    // --------------------------------------------------------------
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// Application State Manager
//--------------------------
void AppEngine::ChangeState(AppState* state)
{
    // cleanup the current state
    if (!states.empty()) {
        states.back()->Cleanup();
        states.pop_back();
    }

    // store and init the new state
    states.push_back(state);
    states.back()->Init();
}
void AppEngine::PushState(AppState* state)
{
    // store and init the new state
    states.push_back(state);
    states.back()->Init();
}
void AppEngine::PopState()
{
    // cleanup the current state
    if (!states.empty()) {
        states.back()->Cleanup();
        states.pop_back();
    }
}

// On Update Events
//-----------------
void AppEngine::HandleEvents()
{
    processInput(window);

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // make the active state handle its events as well
    //------------------------------------------------
    states.back()->HandleEvents();
}
void AppEngine::Update()
{
    // per-frame time logic
        // --------------------
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Get active state and call its update function
    states.back()->Update();
}
void AppEngine::Draw()
{
    states.back()->Draw();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void AppEngine::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        std::cout << "W pressed \n" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void AppEngine::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void AppEngine::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void AppEngine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void AppEngine::create_triangle(unsigned int& vbo, unsigned int& vao, unsigned int& ebo)
{
    // create the triangle
    float triangle_vertices[] = {
        0.0f, 0.25f, 0.0f,	// position vertex 1
        1.0f, 0.0f, 0.0f,	 // color vertex 1
        0.25f, -0.25f, 0.0f,  // position vertex 1
        0.0f, 1.0f, 0.0f,	 // color vertex 1
        -0.25f, -0.25f, 0.0f, // position vertex 1
        0.0f, 0.0f, 1.0f,	 // color vertex 1
    };
    unsigned int triangle_indices[] = {
        0, 1, 2 };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);
}

void AppEngine::parse_points_file(string frame)
{
    ifstream readFile("points.txt");
    string line; //coordinate

    // read the file line by line
    while (getline(readFile, line)) {
        // Create a string stream to parse the line https://www.youtube.com/watch?v=cR-N5DIrAGM
        istringstream iss(line);
        float coord;
        while (iss >> coord)
        {
            vertices.push_back(coord);
        };
    }

    //print the vector
  /*  for (int i = 0; i < vertices.size(); i++) {
        cout << "vertex" << vertices[i] << endl;
    }*/


    // Close the file
    readFile.close();
}

void AppEngine::parse_connectivity_file(string frame)
{
    // Next text file
    //-----------------------------------------------------------------------------------------
    ifstream readCoordFile("connectivity.txt");
    string coordLine; //coordinate

    // read the file line by line
    while (getline(readCoordFile, coordLine)) {
        if (coordLine.empty())
        {
            cout << "empty line" << endl;
        }
        else
        {
            // These connectivity indices are specified in quads so they must be converted to triangles.
            // i.e. 1 2 3 4 --> 1 2 3, 2 3 4
            // Create a string stream to parse the line https://www.youtube.com/watch?v=cR-N5DIrAGM
            istringstream iss(coordLine);

            // we will store 3 vertices at a time and push them onto the vector 3 at a time.
            vector<int> quad;
            //temp to store current coordinate
            int coord;

            // skip blank first line

            // while the string stream has not reached the end
            while (iss >> coord)
            {
                quad.push_back(coord); // push the coordinate to the vector
            }

            // first triangle
            indices.push_back(quad[0]);
            indices.push_back(quad[1]);
            indices.push_back(quad[2]);

            //second triangle
            indices.push_back(quad[2]);
            indices.push_back(quad[3]);
            indices.push_back(quad[0]);
        }
    }

    //print the vector
    //for (int i = 0; i < indices.size(); i++) {
    //    cout << "index" << indices[i] << endl;
    //}

    // Close the file
    readCoordFile.close();
}

void AppEngine::parse_temperatures_file(string frame)
{
    ifstream readFile("UV.txt");
    string temperatures;
    getline(readFile, temperatures);
    getline(readFile, temperatures);
    istringstream iss(temperatures);
    float temp;
    while (iss >> temp)
    {
        v_temps.push_back(temp);
    }
    readFile.close();

    //print the vector
    /*for (int i = 0; i < v_temps.size(); i++) {
        cout << "temp" << v_temps[i] << endl;
    }*/
}

void AppEngine::create_object(int frame, unsigned int& vbo, unsigned int& vao, unsigned int& ebo)
{
    string threeDigfileNumber; // declare a variable to hold the 3 digit version of 'frame'
    ostringstream oss; // open a string stream to feed a string into 'threeDigfileNumber'

    // we set the spaces to be filled with 0's like so: 1 --> 000001
    // then we set the width to a length of 3 like so: 000001 --> 001
    // finally, convert that to a string and feed it into the variable.
    oss << setfill('0') << setw(3) << to_string(frame);
    threeDigfileNumber = oss.str();

    parse_points_file(threeDigfileNumber);
    parse_connectivity_file(threeDigfileNumber);
    parse_temperatures_file(threeDigfileNumber);
    
    //convert array to vector for testing purposes
    /*vector<float> triangle_vertices(a_triangle_vertices, a_triangle_vertices +
        sizeof(a_triangle_vertices) / sizeof(float));*/

    // allocate memory to 1 vertex array, buffer, element buffer
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &colorBuffer);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, v_temps.size() * sizeof(float), v_temps.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // vertex position
    glEnableVertexAttribArray(1); // color

    // Vertices are in stride 12 bytes
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Colors are in stride 4 bytes
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0 );
    
    // Colours are in stride 24 bytes
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);
}

void AppEngine::draw_object()
{
    glBindVertexArray(vao);
    glDisable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// turn all vtk files into txt files.
void AppEngine::parse_all_obj_to_txt()
{
    bool endReached = false; // initially we have not reached the end

    int frame = 0; // begin with file 000

    cout << "beginning while loop" << endl;

    while (!endReached) // self-explanatory. While the end has not been reached...
    {

        string threeDigfileNumber; // declare a variable to hold the 3 digit version of 'frame'
        ostringstream oss; // open a string stream to feed a string into 'threeDigfileNumber'

        // we set the spaces to be filled with 0's like so: 1 --> 000001
        // then we set the width to a length of 3 like so: 000001 --> 001
        // finally, convert that to a string and feed it into the variable.
        oss << setfill('0') << setw(3) << to_string(frame);
        threeDigfileNumber = oss.str();

        // need to check whether a next file exists to determine whether to terminate the loop
        bool fileIsVTK = filesystem::exists("solution-" + threeDigfileNumber + ".vtu");

        // in other words: if there is a next file
        if (fileIsVTK)
        {
            // parse the file
            XMLParser::parseXMLFile("solution-"+ threeDigfileNumber+".vtu", threeDigfileNumber);
            frame++; // don't forget to increment!
        }
        else 
        {
            endReached = true; // if a next file doens't exist, exit the while loop.
        }
    }
}