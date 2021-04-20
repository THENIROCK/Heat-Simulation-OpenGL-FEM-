#pragma once
#include "AppState.h"

class Shader; // use forward declarations when implementation is not needed. https://stackoverflow.com/questions/1542623/syntax-error-missing-before

class HomeScreenState : public AppState
{
	// Define Singleton Pattern
	//-------------------------
	static HomeScreenState s_HomeScreenState; //s for singleton, declaration (will be implemented in .cpp file)

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
	static HomeScreenState* Instance() { return &s_HomeScreenState; }
	// Make it impossible to call the constructor in any way possible
	HomeScreenState(const HomeScreenState&) = delete;
	// Make it impossible to: create a duplicate through 
	//assigning an instance to a variable
	HomeScreenState operator=(const HomeScreenState&) = delete;

	// Variables
	//----------
	Shader* ourShader; // you can name your shader files however you like
	Shader* triangle_shader;
	AppEngine* app;

protected:
	HomeScreenState() {} //Singleton Pattern
};

