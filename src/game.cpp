/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 20XX Iain M. Crawford
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
	mSceneManager->ChangeScene(); // perform initial scene change if required so we can capture events immediately
	
	while (mWindow) { // whilst we have a valid window...
		mWindow->Process(); // process the window
		
		while (!mWindow->mEventQueue.empty()) { // whilst there are events waiting in the queue...
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

uniform mat4 vertModel;
uniform mat4 vertView;
uniform mat4 vertProj;

in vec4 vertPos;
in vec3 vertNormal;
in vec4 vertColour;
in vec3 vertTexCoord;
in float vertType;
in float[2] vertExtra;

flat out vec4 fragColour;
smooth out vec3 fragTexCoord;
flat out float fragType;
flat out float[2] fragExtra;

void main() {
	mat4 mvp = vertProj * vertView * vertModel;
	gl_Position = mvp * vertPos;
	
	vec3 normal = vec3((vertView * vertModel) * vec4(vertNormal, 0.0));
	
	fragColour = vertColour;
	fragTexCoord = vertTexCoord;
	fragType = vertType;
	
	fragExtra = vertExtra;
}
	)";
	mDefaultShader.VertexFromString(vertStr);
	
	// 
	std::string fragStr = R"(
#version 330

uniform sampler2DArray fragBaseTex;

flat in vec4 fragColour;
smooth in vec3 fragTexCoord;
flat in float fragType;
flat in float[2] fragExtra;

vec4 finalColour;
out vec4 fragData;

const float smoothing = 64.0f;

void main() {
	if (fragType < 0.5f) {
		vec4 texColour = clamp(texture(fragBaseTex, fragTexCoord) + (1.0 - fragExtra[0]), 0.0, 1.0);
		vec4 colColour = vec4(fragColour);
		
		finalColour = texColour * colColour;
	}
	else if (fragType < 1.5f) {
		float mask = texture(fragBaseTex, fragTexCoord).r; // get the mask value from the red channel of the texture
		float edgeWidth = clamp(smoothing * (abs(dFdx(fragTexCoord.x)) + abs(dFdy(fragTexCoord.y))), 0.0f, 0.5f); // get the width of the edge (for smooth edges) depending on glyph size (clamped to 0.0f ... 0.5f)
		float alpha = smoothstep(0.5f - edgeWidth, 0.5f + edgeWidth, mask); // interpolate the mask value between the lower and upper edges
		finalColour = vec4(fragColour.x, fragColour.y, fragColour.z, alpha);
		
		// outline
		/* vec4 outlineColour = vec4(1.0f, 0.0f, 0.0f, 1.0f);
		if (mask < 0.5f) {
			finalColour = mix(finalColour, outlineColour, smoothstep(0.1f, 0.3f, mask));
		}
		else if (mask < 0.55f) {
			finalColour = mix(finalColour, outlineColour, smoothstep(0.1f, 0.9f, mask));
		} */
		
		// drop shadow
		/* if (finalColour.a < 0.9f) {
			vec3 shadowColour = vec3(1.0f, 0.0f, 0.0f);
			vec2 texel = vec2(2.0f / texture_width, 2.0f / texture_height);
			vec3 coordOffset = vec3(fragTexCoord.x - texel.x, fragTexCoord.y + texel.y, fragTexCoord.z);
			float maskOffset = texture(fragBaseTex, coordOffset).r;
			if (maskOffset > 0.5f) {
				float alphaOffset = smoothstep(0.0f, 1.0f, maskOffset);
				finalColour = mix(vec4(shadowColour, alphaOffset), finalColour, 0.4f);
			}
		} */
	}
	
	fragData = finalColour;
}
	)";
	mDefaultShader.FragmentFromString(fragStr);
	
	mDefaultShader.LinkProgram();
	SetShader();
}

void Game::SetShader() {
	OpenGLStates::mVertexLocation = glGetAttribLocation(mDefaultShader.GetProgramID(), "vertPos");
	OpenGLStates::mNormalLocation = glGetAttribLocation(mDefaultShader.GetProgramID(), "vertNormal");
	OpenGLStates::mColourLocation = glGetAttribLocation(mDefaultShader.GetProgramID(), "vertColour");
	OpenGLStates::mTexCoordLocation = glGetAttribLocation(mDefaultShader.GetProgramID(), "vertTexCoord");
	OpenGLStates::mTypeLocation = glGetAttribLocation(mDefaultShader.GetProgramID(), "vertType");
	OpenGLStates::mExtraLocation = glGetAttribLocation(mDefaultShader.GetProgramID(), "vertExtra");
	OpenGLStates::mTexLocation = glGetUniformLocation(mDefaultShader.GetProgramID(), "fragBaseTex");
	
	OpenGLStates::mProjectionMatrixLocation = glGetUniformLocation(mDefaultShader.GetProgramID(), "vertProj");
	OpenGLStates::mViewMatrixLocation = glGetUniformLocation(mDefaultShader.GetProgramID(), "vertView");
	OpenGLStates::mModelMatrixLocation = glGetUniformLocation(mDefaultShader.GetProgramID(), "vertModel");
	
	glBindFragDataLocation(mDefaultShader.GetProgramID(), 0, "fragData");
	
	mDefaultShader.UseProgram();
	glUniform1i(OpenGLStates::mTexLocation, 0);
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

bool Game::DeviceExists(const unsigned int& deviceID) const {
	return mInputManager->DeviceExists(deviceID);
}

const InputManager::InputDevice& Game::GetDevice(const unsigned int& deviceID) const {
	return mInputManager->GetDevice(deviceID);
}

unsigned int Game::GetDeviceButtonCount(const int& deviceID) const {
	return mInputManager->GetDeviceButtonCount(deviceID);
}

bool Game::GetDeviceButtonDown(const int& deviceID, const unsigned int& button) const {
	return mInputManager->GetDeviceButtonDown(deviceID, button);
}

bool Game::GetDeviceButtonPressed(const int& deviceID, const unsigned int& button) const {
	return mInputManager->GetDeviceButtonPressed(deviceID, button);
}

bool Game::GetDeviceButtonReleased(const int& deviceID, const unsigned int& button) const {
	return mInputManager->GetDeviceButtonReleased(deviceID, button);
}

unsigned int Game::GetDeviceControlCount(const int& deviceID) const {
	return mInputManager->GetDeviceControlCount(deviceID);
}

bool Game::DeviceHasControl(const int& deviceID, const Device& control) const {
	return mInputManager->DeviceHasControl(deviceID, control);
}

int Game::GetDeviceControl(const int& deviceID, const Device& control) const {
	return mInputManager->GetDeviceControl(deviceID, control);
}

int Game::GetDeviceControlScaled(const int& deviceID, const Device& control, std::pair<int, int> range) const {
	return mInputManager->GetDeviceControlScaled(deviceID, control, range);
}

std::pair<int, int> Game::GetDeviceControlRange(const int& deviceID, const Device& control) const {
	return mInputManager->GetDeviceControlRange(deviceID, control);
}

std::vector<Device> Game::GetDeviceLinkedDevices(const int& deviceID, const unsigned int& collectionID) const {
	return mInputManager->GetDeviceLinkedDevices(deviceID, collectionID);
}

unsigned int Game::GetDeviceLinkID(const int& deviceID, const Device& control) const {
	return mInputManager->GetDeviceLinkID(deviceID, control);
}

void Game::HandleEventQueue(const WindowEvent& e) {
	switch (e.mType) {
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
			mInputManager->HandleKeyboardKeys(e.mKeyboardKey.mKey, e.mKeyboardKey.mType);
			break;
		}
		case WindowEvent::MouseButtonType : {
			mInputManager->HandleMouseButtons(e.mMouseButton.mButton, e.mMouseButton.mType);
			break;
		}
		case WindowEvent::MouseWheelType : {
			mInputManager->mMouseWheel += e.mMouseWheel.mAmount;
			break;
		}
		case WindowEvent::MouseMoveType : {
			mInputManager->HandleMouseMove(glm::ivec2(e.mMouseMove.mLocalX, e.mMouseMove.mLocalY),
					glm::ivec2(e.mMouseMove.mGlobalX, e.mMouseMove.mGlobalY));
			break;
		}
		case WindowEvent::DeviceChangedType : { // an input device has been connect or disconnected
			if (e.mDeviceChanged.mStatus) { // if the device was connected...
				mInputManager->AddDevice(e.mDeviceChanged.mID, e.mDeviceChanged.mButtonCount, e.mDeviceChanged.mControlCount, e.mDeviceChanged.mCaps);
			}
			else {
				mInputManager->RemoveDevice(e.mDeviceChanged.mID);
			}
			
			break;
		}
		case WindowEvent::DeviceButtonType : { // a button on an input device has been pressed or released
			mInputManager->HandleDeviceButtons(e.mDeviceButton.mButton, e.mDeviceButton.mType, e.mDeviceButton.mID);
			break;
		}
		case WindowEvent::DeviceControlType : { // the value of a control on an input device has changed
			mInputManager->HandleDeviceControls(e.mDeviceControl.mControl, e.mDeviceControl.mValue, e.mDeviceControl.mID);
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
