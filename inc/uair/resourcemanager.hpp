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

#ifndef UAIRRESOURCEMANAGER_HPP
#define UAIRRESOURCEMANAGER_HPP

#include <memory>

#include "resourcestore.hpp"
#include "texture.hpp"
#include "renderbuffer.hpp"
#include "font.hpp"
#include "soundbuffer.hpp"

namespace uair {
class ResourceManager;
typedef std::shared_ptr<ResourceManager> ResourceManagerPtr;

class ResourceManager {
	friend class Game;
	
	public :
		template<typename ...Ts>
		ResourcePtr<Texture> AddTexture(const std::string& name, const Ts&... params) {
			return mTextureStore.AddResource(name, params...);
		}
		
		ResourcePtr<Texture> AddTexture(const std::string& name, Texture&& res);
		void RemoveTexture(const std::string& name);
		ResourcePtr<Texture> GetTexture(const std::string& name);
		
		
		template<typename ...Ts>
		ResourcePtr<RenderBuffer> AddRenderBuffer(const std::string& name, const Ts&... params) {
			return mRenderBufferStore.AddResource(name, params...);
		}
		
		ResourcePtr<RenderBuffer> AddRenderBuffer(const std::string& name, RenderBuffer&& res);
		void RemoveRenderBuffer(const std::string& name);
		ResourcePtr<RenderBuffer> GetRenderBuffer(const std::string& name);
		
		
		template<typename ...Ts>
		ResourcePtr<Font> AddFont(const std::string& name, const Ts&... params) {
			return mFontStore.AddResource(name, params...);
		}
		
		ResourcePtr<Font> AddFont(const std::string& name, Font&& res);
		void RemoveFont(const std::string& name);
		ResourcePtr<Font> GetFont(const std::string& name);
		
		
		template<typename ...Ts>
		ResourcePtr<SoundBuffer> AddSoundBuffer(const std::string& name, const Ts&... params) {
			return mSoundBufferStore.AddResource(name, params...);
		}
		
		ResourcePtr<SoundBuffer> AddSoundBuffer(const std::string& name, SoundBuffer&& res);
		void RemoveSoundBuffer(const std::string& name);
		ResourcePtr<SoundBuffer> GetSoundBuffer(const std::string& name);
		
		void Clear();
	public :
		ResourceStore<Texture> mTextureStore;
		ResourceStore<RenderBuffer> mRenderBufferStore;
		ResourceStore<Font> mFontStore;
		ResourceStore<SoundBuffer> mSoundBufferStore;
};
}

#endif
