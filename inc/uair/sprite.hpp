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

#ifndef UAIRSPRITE_HPP
#define UAIRSPRITE_HPP

#include <vector>
#include <memory>

#include "renderable.hpp"
#include "texture.hpp"

namespace uair {
struct SpriteFrame {
	ResourcePtr<Texture> mTexture;
	std::vector<glm::vec2> mTexCoords;
	float mLayer;
};

class Sprite : public Renderable {
	friend class RenderBatch; // allow a render batch to access this class
	
	public :
		std::string GetTag() const; // return the tag (type) of this renderable
		
		void Process();
		
		void AddFrame(ResourcePtr<Texture> texture, unsigned int layer = 0);
		void AddFrame(Texture* texture, unsigned int layer = 0);
		
		void AddFrameSheet(ResourcePtr<Texture> texture, const unsigned int& numFrames, const unsigned int& framesPerRow,
				unsigned int layer = 0);
		void AddFrameSheet(Texture* texture, const unsigned int& numFrames, const unsigned int& framesPerRow,
				unsigned int layer = 0);
		
		void SetAnimation(const float& speed, const unsigned int& start, const unsigned int& end,
				const int& loops = -1);
		
		unsigned int GetCurrentFrame() const;
		void SetCurrentFrame(const unsigned int& currentFrame);
		size_t GetFrameCount() const;
		
		Polygon GetClip() const; // return the clip polygon
		void SetClip(const Polygon& polyClip); // set the polygon to clip with the sprite, creating the clip polygon
	protected :
		void UpdateGlobalBoundingBox(); // update the sprite's global bounding box (according to it's transform)
		void UpdateGlobalMask(); // update the sprite's global mask (according to it's transform)
		void CreateLocalMask(); // create a default local mask for the sprite
		RenderBatchData Upload(); // upload the sprite to a render batch
	private :
		std::vector<SpriteFrame> mFrames;
		unsigned int mWidth = 0;
		unsigned int mHeight = 0;
		
		unsigned int mCurrentFrame = 0;
		
		bool mIsAnimated = false;
		int mAnimationDirection = 1;
		float mAnimationLimit = 0.0f;
		float mAnimationTimer = 0.0f;
		int mAnimationLoopCount = 0;
		unsigned int mAnimationStartFrame = 0;
		unsigned int mAnimationEndFrame = 0;
		
		bool mIsClipped = false;
		unsigned int mClippedWidth = 0;
		unsigned int mClippedHeight = 0;
		Polygon mClip;
};
}

#endif
