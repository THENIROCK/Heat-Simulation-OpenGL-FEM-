#pragma once

//Put glad before GLFW so that GLFW doesn't duplicate include OpenGL
//https://stackoverflow.com/questions/44345811/glad-h-giving-error-that-opengl-header-is-included/47430152
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "camera.h"

#include <vector>
using namespace std;

class AppState;

class AppEngine
{
	// Define Singleton Pattern
	//-------------------------
	static AppEngine s_AppEngine; // s for singleton, creates new object on the stack

	// Private Variables
	//------------------
	// the stack of states
	vector<AppState*> states;
	bool m_running;
	bool m_fullscreen;

public:
	//The Beginning and The End
	//-------------------------
	int Init(const char* title, int width = 640, int height = 480, int bpp = 0, bool fullscreen = false);
	int Cleanup();

	// Application State Manager
	//--------------------------
	void ChangeState(AppState* state);
	void PushState(AppState* state);
	void PopState();

	// On Update Events
	//-----------------
	void HandleEvents();
	void Update();
	void Draw();

	// Utilities
	//----------
	bool IsRunning() { return m_running; }
	void Quit() { m_running = false; }
	void create_triangle(unsigned int& vbo, unsigned int& vao, unsigned int& ebo);
	void parse_points_file(int frame);
	void parse_connectivity_file(int frame);
	void parse_temperatures_file(int frame);
	void create_object(int frame, unsigned int& vbo, unsigned int& vao, unsigned int& ebo);
	void draw_object();

	// glfw: process inputs
	//---------------------
	void processInput(GLFWwindow* window);

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height); //callbacks need to be static
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	// Define Singleton Pattern Functions
	// https://www.youtube.com/watch?v=_USRzpb7dLg
	//-----------------------------------
	// Get the "single" static instance
	static AppEngine* Instance() { return &s_AppEngine; }
	// Make it impossible to call the constructor in any way possible
	AppEngine(const AppEngine&) = delete;
	// Make it impossible to: create a duplicate through 
	//assigning an instance to a variable
	AppEngine operator=(const AppEngine&) = delete;

	//TODO add in window variables n stuff
	//------------------------------------
	GLFWwindow* window;
	unsigned int vbo, vao, ebo, colorBuffer; // vertex buffer object, vertex array object, element buffer object 
	const char* glsl_version;
	unsigned int scr_width;
	unsigned int scr_height;
	static float lastX;
	static float lastY;
	static bool firstMouse;
	static Camera camera;
	static float deltaTime;
	static float lastFrame;
	vector<float> vertices;
	vector<int> indices;
	vector<float> v_temps;

protected:
	AppEngine() {} // Singleton Design Pattern 
};

