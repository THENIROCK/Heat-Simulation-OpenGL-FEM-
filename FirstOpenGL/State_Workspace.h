#pragma once

#include "AppState.h"
#include "Model.h"
#include "camera.h"

class Shader; // use forward declarations when implementation is not needed. https://stackoverflow.com/questions/1542623/syntax-error-missing-before
class ShaderWGeo;

class WorkspaceState : public AppState
{
	// Define Singleton Pattern
	//-------------------------
	static WorkspaceState s_WorkspaceState; //s for singleton, declaration (will be implemented in .cpp file)

public:
	// The Beginning and The End
	//---------------+AppEn-----------
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
	//-----------------------------------------------------------------------------
	Shader* vtuShader; // you can name your shader files however you like
	Shader* triangleShader;
	Shader* simpleShader;
	Shader* skyboxShader;
	AppEngine* app;
	Model ourModel;
	Camera* camera;
	int frame; // current simulation frame to render
	int prevFrame;
	bool perspectiveOn; // orthographic or perspective projection
	glm::mat4 projection;
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int cubemapTexture;
	int selectedEnvironment;
	int previousEnvironment;

	//My methods
	unsigned int loadCubemap(vector<std::string> faces);
	
protected:
	WorkspaceState() {} //Singleton Pattern
};