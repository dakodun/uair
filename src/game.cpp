/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2017, Iain M. Crawford
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
#include "shape.hpp"
#include "sdffont.hpp"
#include "font.hpp"
#include "soundbuffer.hpp"
#include "gui.hpp"
#include "windowmessages.hpp"

namespace uair {
Game::Game() {
	mSceneManager = std::make_shared<SceneManager>();
	mInputManager = std::make_shared<InputManager>();
	
	try {
		RegisterResourceType<Texture>();
		RegisterResourceType<SDFFont>();
		RegisterResourceType<Font>();
		RegisterResourceType<RenderBuffer>();
		RegisterResourceType<Shader>();
		RegisterResourceType<SoundBuffer>();
	} catch (std::exception& e) {
		
	}
	
	FT_Error ftError = FT_Init_FreeType(&mFTLibrary);
	if (ftError != 0) {
		std::cout << "unable to initiate the freetype2 library: " << ftError << std::endl;
	}
	
	FontBase::mFTLibraryPtr = &mFTLibrary; // set the freetype library used by the font class to the newly initialised one
	GUI::mInputManagerPtr = mInputManager;
	GUI::mMessageQueuePtr = &mMessageQueue;
}

Game::~Game() {
	Clear();
}

void Game::Run() {
	mSceneManager->ChangeScene(); // perform initial scene change if required so we can capture events immediately
	
	while (mWindow) { // whilst we have a valid window...
		mWindow->Process(); // process the window
		
		mMessageQueue += mWindow->mMessageQueue; // add the window's message queue to the end of this
		mWindow->mMessageQueue.ClearQueue(); // clear the window's message queue for next loop
		while (!mMessageQueue.IsEmpty()) { // while there are messages to process...
			MessageQueue::Entry e = mMessageQueue.GetMessage(); // retrieve the message entry from the queue
			HandleMessageQueue(e); // handle the message
			mMessageQueue.PopMessage(); // remove the message from the front of the queue
		}
		
		while (!mEntitySystem.mMessageQueue.IsEmpty()) {
			MessageQueue::Entry e = mEntitySystem.mMessageQueue.GetMessage();
			HandleMessageQueue(e);
			mEntitySystem.mMessageQueue.PopMessage();
		}
		
		for (unsigned int i = 0u; i < mRenderPasses; ++i) {
			Render(i); // handle rendering for the current pass
		}
		
		mWindow->Display(); // draw the window
		Input(); // handle all user input
		mInputManager->Process(); // process the input manager (to update states)
		
		{ // handle process timing
			double prevFrameTime = mTimer.GetElapsedTime(); // the time since the prvious frame
			mTimer.Reset(); // reset the frame timer
			
			if (prevFrameTime > mFrameLowerLimit) {
				unsigned int processed = 0u; // the number of process calls this frame
				double accum = prevFrameTime;
				while (accum > mFrameLowerLimit) {
					Process(mFrameLowerLimit); // handle all processing
					++processed; // increase the process call count
					accum -= mFrameLowerLimit; // decrease the accumulated time
					
					if (mFrameSkip || processed >= mMaxFrames) {
						break;
					}
					
					if (mOpen == false) { // if we are to quit the game...
						mWindow->Quit();
						Clear();
						break;
					}
					
					if (mSceneManager->mNextScene) { // if there is a scene change waiting...
						break; // stop processing this scene, saving the accumulated time leftover
					}
				}
				
				PostProcess(processed, prevFrameTime); // handle all post processing
			}
			else {
				Process(prevFrameTime);
				PostProcess(1u, prevFrameTime);
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

void Game::Process(float deltaTime) {
	if (mSceneManager->mCurrScene) {
		mSceneManager->mCurrScene->Process(deltaTime);
	}
}

void Game::PostProcess(const unsigned int & processed, float deltaTime) {
	if (mSceneManager->mCurrScene) {
		mSceneManager->mCurrScene->PostProcess(processed, deltaTime);
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
	InputManager::mWindowPtr = mWindow;
}

void Game::AddWindow(const std::string & windowTitle, const WindowDisplaySettings & settings,
		const unsigned long & windowStyle) {
	
	mWindow.reset(); // remove any current window
	mWindow = WindowPtr(new Window(windowTitle, settings, windowStyle)); // add a new custom window
	InputManager::mWindowPtr = mWindow;
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
			VBO::mDefaultShader.Set(&mDefaultShader);
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
	mResourceManager = std::move(Manager<Resource>());
	mEntitySystem = std::move(EntitySystem());
	
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

layout(location = 0) in vec3 vertXYZ;
layout(location = 1) in vec4 vertNormal;
layout(location = 2) in vec2 vertST;
layout(location = 3) in vec4 vertLWTT;
layout(location = 4) in vec4 vertSTBounds;
layout(location = 5) in vec4 vertRGBA;
layout(location = 6) in vec2 vertScale;

smooth out vec3 fragSTL;
flat out uint fragWrap;
flat out uint fragType;
flat out float fragIsTextured;
flat out vec2 fragSTMin;
flat out vec2 fragSTMax;
smooth out vec4 fragRGBA;
flat out vec2 fragScale;

void main() {
	mat4 mvp = vertProj * vertView * vertModel;
	gl_Position = mvp * vec4(vertXYZ, 1.0f);
	
	vec3 normal = vec3((vertView * vertModel) * vec4(vertNormal.xyz, 1.0f));
	
	fragSTL = vec3(vertST, vertLWTT.x);
	fragWrap = uint(vertLWTT.y + 0.5f);
	fragType = uint(vertLWTT.z + 0.5f);
	fragIsTextured = vertLWTT.w;
	fragSTMin = vertSTBounds.xy;
	fragSTMax = vertSTBounds.zw;
	fragRGBA = vertRGBA;
	fragScale = vertScale;
}
	)";
	mDefaultShader.VertexFromString(vertStr);
	
	// 
	std::string fragStr = R"(
#version 330

uniform sampler2DArray fragBaseTex;

smooth in vec3 fragSTL;
flat in uint fragWrap;
flat in uint fragType;
flat in float fragIsTextured;
flat in vec2 fragSTMin;
flat in vec2 fragSTMax;
smooth in vec4 fragRGBA;
flat in vec2 fragScale;

vec4 finalColour;
layout(location = 0) out vec4 fragData;

const float smoothing = 64.0f;

void main() {
	if (fragType == 0u) {
		vec3 texCoords = fragSTL;
		
		if (uint(fragWrap & 1u) != 0u) { // clip s
			texCoords.x = ((texCoords.x - fragSTMin.x) * fragScale.x) + fragSTMin.x;
			
			if (texCoords.x > fragSTMax.x) {
				discard;
			}
		}
		else if (uint(fragWrap & 4u) != 0u) { // repeat s
			texCoords.x = ((texCoords.x - fragSTMin.x) * fragScale.x) + fragSTMin.x;
			texCoords.x = mod(texCoords.x, fragSTMax.x);
		}
		
		if (uint(fragWrap & 2u) != 0u) { // clip t
			texCoords.y = (((1.0f - texCoords.y) - fragSTMin.y) * fragScale.y) + fragSTMin.y;
			
			if (texCoords.y > fragSTMax.y) {
				discard;
			}
			
			texCoords.y = 1.0f - texCoords.y;
		}
		else if (uint(fragWrap & 8u) != 0u) { // repeat t
			texCoords.y = (((1.0f - texCoords.y) - fragSTMin.y) * fragScale.y) + fragSTMin.y;
			texCoords.y = mod(texCoords.y, fragSTMax.y);
			texCoords.y = 1.0f - texCoords.y;
		}
		
		vec4 texColour = clamp(texture(fragBaseTex, texCoords) + (1.0 - fragIsTextured), 0.0f, 1.0f);
		finalColour = texColour * fragRGBA;
	}
	else if (fragType == 1u) {
		float mask = texture(fragBaseTex, fragSTL).r; // get the mask value from the red channel of the texture
		float edgeWidth = clamp(smoothing * (abs(dFdx(fragSTL.x)) + abs(dFdy(fragSTL.y))), 0.0f, 0.5f); // get the width of the edge (for smooth edges) depending on glyph size (clamped to 0.0f ... 0.5f)
		float alpha = smoothstep(0.5f - edgeWidth, 0.5f + edgeWidth, mask); // interpolate the mask value between the lower and upper edges
		finalColour = vec4(fragRGBA.x, fragRGBA.y, fragRGBA.z, alpha);
		
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
	mDefaultShader.InitCallback();
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

unsigned int Game::GetKeyboardState(const Keyboard& key) const {
	return mInputManager->GetKeyboardState(key);
}

std::u16string Game::GetInputString() const {
	return mInputManager->GetInputString();
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

unsigned int Game::GetMouseState(const Mouse& button) const {
	return mInputManager->GetMouseState(button);
}

int Game::GetMouseWheel() const {
	return mInputManager->GetMouseWheel();
}

void Game::SetMouseCoords(const glm::ivec2& newCoords, const CoordinateSpace& coordinateSpace) {
	mInputManager->SetMouseCoords(newCoords, coordinateSpace);
}

glm::ivec2 Game::GetMouseCoords(const CoordinateSpace& coordinateSpace) const {
	return mInputManager->GetMouseCoords(coordinateSpace);
}

bool Game::DeviceExists(const int& deviceID) const {
	return mInputManager->DeviceExists(deviceID);
}

const InputManager::InputDevice& Game::GetDevice(const int& deviceID) const {
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

unsigned int Game::GetDeviceButtonState(const int& deviceID, const unsigned int& button) const {
	return mInputManager->GetDeviceButtonState(deviceID, button);
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

//
	Manager<Resource>& Game::GetResourceManager() {
		return mResourceManager;
	}

	void Game::RemoveResource(const ResourceHandle& handle) {
		try {
			mResourceManager.Remove(handle);
		} catch (std::exception& e) {
			throw;
		}
	}

	void Game::RemoveResource(const std::string& name) {
		try {
			mResourceManager.Remove(name);
		} catch (std::exception& e) {
			throw;
		}
	}
//

void Game::HandleMessageQueue(const MessageQueue::Entry& e) {
	using namespace WindowMessage;
	switch (e.GetTypeID()) {
		case CloseMessage::GetTypeID() : {
			mOpen = false; // indicate that the application should close
			break;
		}
		case LostFocusMessage::GetTypeID() : {
			mInputManager->Reset(); // reset any input states (such as keys that are down)
			break;
		}
		case LostCaptureMessage::GetTypeID() : {
			mInputManager->ResetMouse();
			break;
		}
		case KeyboardKeyMessage::GetTypeID() : {
			KeyboardKeyMessage msg = e.Deserialise<KeyboardKeyMessage>();
			mInputManager->HandleKeyboardKeys(msg.mKey, msg.mType);
			break;
		}
		case TextInputMessage::GetTypeID() : {
			TextInputMessage msg = e.Deserialise<TextInputMessage>();
			mInputManager->HandleTextInput(msg.mInput);
			break;
		}
		case MouseButtonMessage::GetTypeID() : {
			MouseButtonMessage msg = e.Deserialise<MouseButtonMessage>();
			mInputManager->HandleMouseButtons(msg.mButton, msg.mType);
			break;
		}
		case MouseWheelMessage::GetTypeID() : {
			MouseWheelMessage msg = e.Deserialise<MouseWheelMessage>();
			mInputManager->mMouseWheel += msg.mAmount;
			break;
		}
		case DeviceChangedMessage::GetTypeID() : { // an input device has been connect or disconnected
			DeviceChangedMessage msg = e.Deserialise<DeviceChangedMessage>();
			
			if (msg.mStatus) { // if the device was connected...
				mInputManager->AddDevice(msg.mID, msg.mButtonCount, msg.mControlCount, msg.mCaps);
			}
			else {
				mInputManager->RemoveDevice(msg.mID);
			}
			
			break;
		}
		case DeviceButtonMessage::GetTypeID() : { // a button on an input device has been pressed or released
			DeviceButtonMessage msg = e.Deserialise<DeviceButtonMessage>();
			mInputManager->HandleDeviceButtons(msg.mButton, msg.mType, msg.mID);
			break;
		}
		case DeviceControlMessage::GetTypeID() : { // the value of a control on an input device has changed
			DeviceControlMessage msg = e.Deserialise<DeviceControlMessage>();
			mInputManager->HandleDeviceControls(msg.mControl, msg.mValue, msg.mID);
			break;
		}
		default :
			break;
	}
	
	if (mSceneManager->mCurrScene) { // if a current scene exists...
		mSceneManager->mCurrScene->HandleMessageQueue(e); // forward the message to the scene
	}
}

// begin entity system helpers...
	EntitySystem& Game::GetEntitySystem() {
		return mEntitySystem;
	}
	
	EntitySystem::EntityHandle Game::AddEntity(const std::string& name) {
		try {
			return mEntitySystem.AddEntity(name);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	void Game::RemoveEntity(const EntitySystem::EntityHandle& handle) {
		try {
			mEntitySystem.RemoveEntity(handle);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	void Game::RemoveEntities(const std::string& name) {
		try {
			mEntitySystem.RemoveEntities(name);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	void Game::RemoveEntities() {
		try {
			mEntitySystem.RemoveEntities();
		} catch (std::exception& e) {
			throw;
		}
	}
	
	Entity& Game::GetEntity(const EntitySystem::EntityHandle& handle) {
		try {
			return mEntitySystem.GetEntity(handle);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	std::list< std::reference_wrapper<Entity> > Game::GetEntities(const std::string& name) {
		try {
			return mEntitySystem.GetEntities(name);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	std::list< std::reference_wrapper<Entity> > Game::GetEntities() {
		try {
			return mEntitySystem.GetEntities();
		} catch (std::exception& e) {
			throw;
		}
	}
	
	std::list<EntitySystem::EntityHandle> Game::GetEntityHandles(const std::string& name) {
		try {
			return mEntitySystem.GetEntityHandles(name);
		} catch (std::exception& e) {
			throw;
		}
	}
	
	std::list<EntitySystem::EntityHandle> Game::GetEntityHandles() {
		try {
			return mEntitySystem.GetEntityHandles();
		} catch (std::exception& e) {
			throw;
		}
	}
	
	void Game::PushMessageString(const unsigned int& messageTypeID, const std::string& messageString) {
		mEntitySystem.PushMessageString(messageTypeID, messageString);
	}

	MessageQueue::Entry Game::GetMessage() const {
		return mEntitySystem.GetMessage();
	}

	void Game::PopMessage() {
		mEntitySystem.PopMessage();
	}

	void Game::ClearQueue() {
		mEntitySystem.ClearQueue();
	}

	bool Game::IsEmpty() const {
		return mEntitySystem.IsEmpty();
	}

	unsigned int Game::GetMessageCount() const {
		return mEntitySystem.GetMessageCount();
	}
// ...end entity system helpers

//
	#ifndef UAIRNOLUA
	LuaAPI& Game::GetLuaAPI() {
		return mLuaAPI;
	}
	#endif
//
}
