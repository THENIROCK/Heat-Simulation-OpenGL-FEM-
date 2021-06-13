#pragma once

#include "AppEngine.h"

class AppState // Abstract - but needs Singleton Pattern implemented in child classes
{
public:
	// The Beginning and The End
	//--------------------------
	virtual void Init() = 0;
	virtual void Cleanup() = 0;

	// Call State Manager (AppEngine) Functions
	//---------------------------------------
	void ChangeState(AppEngine* app, AppState* state) {
		app->ChangeState(state); // Call function in state manager
	}

	// On Update
	//----------
	virtual void HandleEvents() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	//variables

	
protected:
	AppState() { } // Singleton Pattern
};