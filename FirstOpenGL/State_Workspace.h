#pragma once

#include "AppState.h"
#include "Model.h"
#include "camera.h"

class Shader; // use forward declarations when implementation is not needed. https://stackoverflow.com/questions/1542623/syntax-error-missing-before

class WorkspaceState : public AppState
{
	// Define Singleton Pattern
	//-------------------------
	static WorkspaceState s_WorkspaceState; //s for singleton, declaration (will be implemented in .cpp file)

public:
	// The Beginning and The End
	//--------------------------
	void Init();
	void Cleanup();

	// On Update
	//----------
	void HandleEvents();
	void Update();
	void Draw();

	// Define Singleton Pattern Functions
	// https://www.youtube.com/watch?v=_USRzpb7dLg
	//-----------------------------------
	// Get the "single" static instance
	static WorkspaceState* Instance() { return &s_WorkspaceState; } 
	// Make it impossible to call the constructor in any way possible
	WorkspaceState(const WorkspaceState&) = delete; 
	// Make it impossible to: create a duplicate through 
	//assigning an instance to a variable
	WorkspaceState operator=(const WorkspaceState&) = delete;

	// Variables
	//----------
	Shader* ourShader; // you can name your shader files however you like
	Shader* triangle_shader;
	AppEngine* app;
	Model ourModel;
	Camera* camera;
	
protected:
	WorkspaceState() {} //Singleton Pattern
};