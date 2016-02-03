/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2016, Iain M. Crawford
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

#include "resourcemanager.hpp"

namespace uair {
ResourcePtr<Texture> ResourceManager::AddTexture(const std::string& name, Texture&& res) {
	return mTextureStore.AddResource(name, std::forward<Texture>(res));
}

void ResourceManager::RemoveTexture(const std::string& name) {
	mTextureStore.RemoveResource(name);
}

ResourcePtr<Texture> ResourceManager::GetTexture(const std::string& name) {
	return mTextureStore.GetResource(name);
}


ResourcePtr<RenderBuffer> ResourceManager::AddRenderBuffer(const std::string& name, RenderBuffer&& res) {
	return mRenderBufferStore.AddResource(name, std::forward<RenderBuffer>(res));
}

void ResourceManager::RemoveRenderBuffer(const std::string& name) {
	mRenderBufferStore.RemoveResource(name);
}

ResourcePtr<RenderBuffer> ResourceManager::GetRenderBuffer(const std::string& name) {
	return mRenderBufferStore.GetResource(name);
}


ResourcePtr<Font> ResourceManager::AddFont(const std::string& name, Font&& res) {
	return mFontStore.AddResource(name, std::forward<Font>(res));
}

void ResourceManager::RemoveFont(const std::string& name) {
	mFontStore.RemoveResource(name);
}

ResourcePtr<Font> ResourceManager::GetFont(const std::string& name) {
	return mFontStore.GetResource(name);
}


ResourcePtr<SoundBuffer> ResourceManager::AddSoundBuffer(const std::string& name, SoundBuffer&& res) {
	return mSoundBufferStore.AddResource(name, std::forward<SoundBuffer>(res));
}

void ResourceManager::RemoveSoundBuffer(const std::string& name) {
	mSoundBufferStore.RemoveResource(name);
}

ResourcePtr<SoundBuffer> ResourceManager::GetSoundBuffer(const std::string& name) {
	return mSoundBufferStore.GetResource(name);
}


void ResourceManager::Clear() {
	mTextureStore.Clear();
	mRenderBufferStore.Clear();
	mFontStore.Clear();
	mSoundBufferStore.Clear();
}
}
