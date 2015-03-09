/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2014 Iain M. Crawford
**
**	This software is provided 'as-is', without any express or
**	implied warranty. In no event will the authors be held liable
**	for any damages arising from the use of this software.
**
**	Permission is granted to anyone to use this software for any
**	purpose, including commercial applications, and to alter it
**	and redistribute it freely, subject to the following
**	restrictions:
** 
**		1. The origin of this software must not be misrepresented;
**		   you must not claim that you wrote the original
**		   software. If you use this software in a product, an
**		   acknowledgment in the product documentation would be
**		   appreciated but is not required.
**
**		2. Altered source versions must be plainly marked as such,
**		   and must not be misrepresented as being the original
**		   software.
**
**		3. This notice may not be removed or altered from any
**		   source distribution.
** **************************************************************** */

#include "game.hpp"

#include <iostream>

#include "openglstates.hpp"

namespace uair {
bool Game::mDefaultShaderExists = false;

Game::Game() {
	mSceneManager = std::make_shared<SceneManager>();
	mInputManager = std::make_shared<InputManager>();
	mResourceManager = std::make_shared<ResourceManager>();
	
	FT_Error ftError = FT_Init_FreeType(&mFTLibrary);
	if (ftError != 0) {
		std::cout << "unable to initiate the freetype2 library: " << ftError << std::endl;
	}
}

Game::~Game() {
	Clear();
}

void Game::Run() {
	while (mWindow) { // whilst we have a valid window...
		mWindow->Process(); // process the window
		
		while (!mWindow->mEventQueue.empty()) { // whilst there are events waiting in the queue...
			/* WindowEvent e = mWindow->mEventQueue.front(); // get the event at the front of the window's queue
			mEventQueue.push_back(e); // add to our event queue
			mWindow->mEventQueue.pop_front(); // remove it from the window's queue
			
			HandleEventQueue(e); // handle the event queue */
			
			HandleEventQueue(mWindow->mEventQueue.front());
			mWindow->mEventQueue.pop_front();
		}
		
		for (unsigned int i = 0u; i < mRenderPasses; ++i) {
			Render(i); // handle rendering for the current pass
		}
		
		mWindow->Display(); // draw the window
		Input(); // handle all user input
		mInputManager->Process(); // process the input manager (to update states)
		
		{ // handle process timing
			unsigned int processed = 0u; // the number of process calls this frame
			
			double prevFrameTime = mTimer.GetElapsedTime(); // the time since the prvious frame
			mTimer.Reset(); // reset the frame timer
			
			if (prevFrameTime > mFrameUpperLimit) { // if our time is longer than the upper frame limit...
				prevFrameTime = mFrameUpperLimit; // cap it at the upper limit
			}
			
			mAccumulator += prevFrameTime; // increase the accumulated time
			
			while (mAccumulator >= mFrameLowerLimit) { // whilst the accumulated time is above the lower frame limit...
				Process(); // handle all processing
				++processed; // increase the process call count
				
				// mTotalFrameTime += mFrameLowerLimit; // increase the total frame time
				mAccumulator -= mFrameLowerLimit; // decrease the accumulated time
				
				if (mOpen == false) { // if we are to quit the game...
					mWindow->Quit();
					Clear();
					break;
				}
				
				if (mSceneManager->mNextScene) { // if there is a scene change waiting...
					break; // stop processing this scene, saving the accumulated time leftover
				}
			}
			
			if (processed > 0u) { // if we have had at least 1 process call...
				PostProcess(processed); // handle all post processing
				// mEventQueue.clear(); // empty the event queue
			}
		}
		
		if (mSceneManager->ChangeScene() == true) { // if we're changing scene...
			mTimer.Reset(); // reset the frame timer
		}
		
		if (mOpen == false) { // if the game is no longer open...
			if (mSceneManager->mCurrScene) { // if we have a current scene...
				mOpen = !(mSceneManager->mCurrScene->OnQuit()); // handle scene quitting logic, and see if we are still to quit
				
				if (mOpen == false) { // if we are still to quit...
					mWindow->Quit();
					Clear();
					break;
				}
			}
		}
	}
}

void Game::Input() {
	if (mSceneManager->mCurrScene) {
		mSceneManager->mCurrScene->Input();
	}
}

void Game::Process() {
	if (mSceneManager->mCurrScene) {
		mSceneManager->mCurrScene->Process();
	}
}

void Game::PostProcess(const unsigned int & processed) {
	if (mSceneManager->mCurrScene) {
		mSceneManager->mCurrScene->PostProcess(processed);
	}
}

void Game::Render(const unsigned int & pass) {
	if (mSceneManager->mCurrScene) {
		mSceneManager->mCurrScene->Render(pass);
	}
	
	glFlush();
}

void Game::Init() {
	AddWindow(); // add a default window
	AddContext(mWindow); // add a context using the window
	MakeCurrent(mWindow, mContext); // make the window/context combo current
	
	GLint width = static_cast<GLint>(mWindow->GetWidth());
	GLint height = static_cast<GLint>(mWindow->GetWidth());
	if (height == 0) {
		height = 1;
	}
	
	glViewport(0, 0, width, height);
}

void Game::Init(const std::string & windowTitle, const WindowDisplaySettings & settings,
		const unsigned long & windowStyle) {
	
	AddWindow(windowTitle, settings, windowStyle); // add a custom window
	AddContext(mWindow); // add a context using the window
	MakeCurrent(mWindow, mContext); // make the window/context combo current
	
	GLint width = static_cast<GLint>(mWindow->GetWidth());
	GLint height = static_cast<GLint>(mWindow->GetWidth());
	if (height == 0) {
		height = 1;
	}
	
	glViewport(0, 0, width, height);
}

WindowPtr Game::GetWindow() {
	return mWindow;
}

OpenGLContextPtr Game::GetContext() {
	return mContext;
}

void Game::AddWindow() {
	mWindow.reset(); // remove any current window
	mWindow = WindowPtr(new Window); // add a new default window
}

void Game::AddWindow(const std::string & windowTitle, const WindowDisplaySettings & settings,
		const unsigned long & windowStyle) {
	
	mWindow.reset(); // remove any current window
	mWindow = WindowPtr(new Window(windowTitle, settings, windowStyle)); // add a new custom window
}

void Game::AddContext() {
	mContext.reset(); // remove any current context
	mContext = OpenGLContextPtr(new OpenGLContext()); // add a new default context
}

void Game::AddContext(WindowPtr windowPtr) {
	mContext.reset(); // remove any current context
	mContext = OpenGLContextPtr(new OpenGLContext(*windowPtr)); // add a new custom context
}

void Game::MakeCurrent(WindowPtr windowPtr, OpenGLContextPtr contextPtr) {
	if (windowPtr && contextPtr) { // if we have a window and a context...
		windowPtr->MakeCurrent(*contextPtr); // make the window/context combo current
		
		if (mDefaultShaderExists == false) { // if we don't yet have a default shader...
			CreateDefaultShader(); // create a default shader
			mDefaultShaderExists = true; // indicate that a default shader now exists
		}
	}
}

void Game::Quit() {
	mOpen = false;
}

void Game::Clear() {
	mDefaultShader.Clear(); // remove the default shader
	mSceneManager->Clear(); // clear the scene manager
	mResourceManager->Clear(); // clear the resource manager
	
	mContext.reset(); // remove the context
	mWindow.reset(); // remove the window
}

void Game::CreateDefaultShader() {
	//
	std::string vertStr = R"(
#version 330

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProj;

in vec4 vertPos;
in vec3 normalIn;
in vec4 colourIn;
in vec3 texCoordIn;
in float textureExistsIn;

out vec4 colourOut;
smooth out vec3 texCoordOut;
out float textureExistsOut;

void main() {
	mat4 vModelViewProj = vProj * vView * vModel;
	gl_Position = vModelViewProj * vertPos;
	
	vec3 normal = vec3((vView * vModel) * vec4(normalIn, 0.0));
	
	colourOut = colourIn;
	texCoordOut = texCoordIn;
	textureExistsOut = textureExistsIn;
}
	)";
	mDefaultShader.VertexFromString(vertStr);
	
	// 
	std::string fragStr = R"(
#version 330

uniform sampler2DArray baseTex;

in vec4 colourOut;
smooth in vec3 texCoordOut;
in float textureExistsOut;

out vec4 fragColour;

void main() {
	vec4 texColour = clamp(texture(baseTex, texCoordOut) + (1.0 - textureExistsOut), 0.0, 1.0);
	vec4 colColour = vec4(colourOut);
	
	vec4 finalColour = texColour * colColour;
	fragColour = finalColour;
}
	)";
	mDefaultShader.FragmentFromString(fragStr);
	
	mDefaultShader.LinkProgram();
	SetShader();
}

void Game::SetShader() {
	OpenGLStates::mVertexLocation = glGetAttribLocation(mDefaultShader.GetProgramID(), "vertPos");
	OpenGLStates::mNormalLocation = glGetAttribLocation(mDefaultShader.GetProgramID(), "normalIn");
	OpenGLStates::mColourLocation = glGetAttribLocation(mDefaultShader.GetProgramID(), "colourIn");
	OpenGLStates::mTexCoordLocation = glGetAttribLocation(mDefaultShader.GetProgramID(), "texCoordIn");
	OpenGLStates::mTexExistsLocation = glGetAttribLocation(mDefaultShader.GetProgramID(), "textureExistsIn");
	OpenGLStates::mTexLocation = glGetUniformLocation(mDefaultShader.GetProgramID(), "baseTex");
	glUniform1i(OpenGLStates::mTexLocation, 0);
	
	OpenGLStates::mProjectionMatrixLocation = glGetUniformLocation(mDefaultShader.GetProgramID(), "vProj");
	OpenGLStates::mViewMatrixLocation = glGetUniformLocation(mDefaultShader.GetProgramID(), "vView");
	OpenGLStates::mModelMatrixLocation = glGetUniformLocation(mDefaultShader.GetProgramID(), "vModel");
	
	glBindFragDataLocation(mDefaultShader.GetProgramID(), 0, "fragColour");
	
	mDefaultShader.UseProgram();
}

SceneManagerPtr Game::GetSceneManager() {
	return mSceneManager;
}

bool Game::RequestSceneChange(Scene* newScene, const bool & restore) {
	return mSceneManager->RequestSceneChange(newScene, restore);
}

bool Game::CurrentSceneExists() {
	return mSceneManager->CurrentSceneExists();
}

bool Game::NextSceneExists() {
	return mSceneManager->NextSceneExists();
}

ScenePtr Game::GetCurrentScene() {
	return mSceneManager->GetCurrentScene();
}

ScenePtr Game::GetNextScene() {
	return mSceneManager->GetNextScene();
}

InputManagerPtr Game::GetInputManager() {
	return mInputManager;
}

bool Game::GetKeyboardDown(const Keyboard & key) const {
	return mInputManager->GetKeyboardDown(key);
}

bool Game::GetKeyboardPressed(const Keyboard & key) const {
	return mInputManager->GetKeyboardPressed(key);
}

bool Game::GetKeyboardReleased(const Keyboard & key) const {
	return mInputManager->GetKeyboardReleased(key);
}

bool Game::GetMouseDown(const Mouse & button) const {
	return mInputManager->GetMouseDown(button);
}

bool Game::GetMousePressed(const Mouse & button) const {
	return mInputManager->GetMousePressed(button);
}

bool Game::GetMouseReleased(const Mouse & button) const {
	return mInputManager->GetMouseReleased(button);
}

int Game::GetMouseWheel() const {
	return mInputManager->GetMouseWheel();
}

glm::ivec2 Game::GetLocalMouseCoords() const {
	return mInputManager->GetLocalMouseCoords();
}

glm::ivec2 Game::GetGlobalMouseCoords() const {
	return mInputManager->GetGlobalMouseCoords();
}

void Game::HandleEventQueue(const WindowEvent& e) {
	switch (e.type) {
		case WindowEvent::CloseType : {
			mOpen = false;
			break;
		}
		case WindowEvent::LostFocusType : {
			mInputManager->Reset();
			break;
		}
		case WindowEvent::LostCaptureType : {
			mInputManager->Reset();
			break;
		}
		case WindowEvent::KeyboardKeyType : {
			mInputManager->HandleKeyboardKeys(e.keyboardKey.key, e.keyboardKey.type);
			break;
		}
		case WindowEvent::MouseButtonType : {
			mInputManager->HandleMouseButtons(e.mouseButton.button, e.mouseButton.type);
			break;
		}
		case WindowEvent::MouseWheelType : {
			mInputManager->mMouseWheel += e.mouseWheel.amount;
			break;
		}
		case WindowEvent::MouseMoveType : {
			mInputManager->HandleMouseMove(glm::ivec2(e.mouseMove.localX, e.mouseMove.localY),
					glm::ivec2(e.mouseMove.globalX, e.mouseMove.globalY));
			break;
		}
		default :
			break;
	}
	
	if (mSceneManager->mCurrScene) {
		mSceneManager->mCurrScene->HandleEventQueue(e);
	}
}

void Game::SetResourceManager(ResourceManager* resMan) {
	mResourceManager = std::make_shared<ResourceManager>();
}

ResourceManagerPtr Game::GetResourceManager() {
	return mResourceManager;
}

Game GAME;
}
