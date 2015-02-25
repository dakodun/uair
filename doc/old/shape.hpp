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

#ifndef UAIRSHAPE_HPP
#define UAIRSHAPE_HPP

#include "polygon.hpp"
#include "renderable.hpp"
#include "resourceptr.hpp"
#include "texture.hpp"
#include "vbo.hpp"

namespace uair {
class RenderBatch;

struct AnimationFrame {
	ResourcePtr<Texture> mTexture;
	std::vector<glm::vec2> mTexCoords;
	float mLayer;
};

class Shape : public Polygon, public Renderable {
	friend class RenderBatch;
	
	public :
		void AddPoint(const glm::vec2& point, const unsigned int& coordinateSpace = CoordinateSpace::Local);
		void AddPoints(const std::vector<glm::vec2>& points, const unsigned int& coordinateSpace = CoordinateSpace::Local);
		
		void Clear();
		
		std::string GetTag() const;
		
		void Process();
		
		void AddFrame(ResourcePtr<Texture> texture, const std::vector<glm::vec2>& textureCoords, const unsigned int& layer = 0);
		void AddFrame(Texture* texture, const std::vector<glm::vec2>& textureCoords, const unsigned int& layer = 0);
		
		void SetAnimation(const float& speed, const unsigned int& start, const unsigned int& end, const int& loops = -1);
		
		// std::vector<glm::vec2> ClipRectangle(width, height, coordinatespace);
	protected :
		RenderBatchData Upload();
	private :
		unsigned int CreateVBOVertices(const std::vector<glm::vec2>& points, const glm::mat3& transform, RenderBatchData& batchData);
	private :
		std::vector< std::vector<VBOIndex> > mIndices = {{}, {}, {}, {}, {}};
		
		std::vector<AnimationFrame> mFrames;
		// unsigned int mWidth = 0;
		// unsigned int mHeight = 0;
		
		unsigned int mCurrentFrame = 0;
		
		bool mIsAnimated = false;
		int mAnimationDirection = 1;
		float mAnimationLimit = 0.0f;
		float mAnimationTimer = 0.0f;
		int mAnimationLoopCount = 0;
		unsigned int mAnimationStartFrame = 0;
		unsigned int mAnimationEndFrame = 0;
		
};
}

#endif
