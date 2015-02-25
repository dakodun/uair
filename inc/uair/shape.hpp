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
#include "triangulate.hpp"

namespace uair {
class RenderBatch;

struct AnimationFrame {
	ResourcePtr<Texture> mTexture;
	std::vector<glm::vec2> mTexCoords; // texture coordinates for the vertices in the shape
	std::vector<glm::vec2> mTexCoordsExtra; // texture coordinates for the extra vertices added by triangulation
	
	float mLayer;
	glm::vec2 mMinST;
	glm::vec2 mMaxST;
};

class Shape : public Polygon, public Renderable {
	friend class RenderBatch;
	
	public :
		Shape() = default;
		explicit Shape(const std::vector<Contour>& contours, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
		explicit Shape(const ClipperLib::Paths& clipperPaths);
		
		void AddContour(const Contour& contour, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
		void AddContours(const std::vector<Contour>& contours, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
		
		void Offset(const float& distance, const ClipperLib::JoinType& miterType = ClipperLib::jtRound, const double& miterLimit = 2.0d);
		
		void FromClipperPaths(const ClipperLib::Paths& clipperPaths);
		
		// return a copy of this shape with its transformations applied directly to its contours (transformations of new shape are reset back to default)
		Shape GetTransformed() const;
		
		std::string GetTag() const;
		
		void Process();
		
		// set the winding rule for filled drawing (forces retriangulation)
		void SetWindingRule(const WindingRule& windingRule);
		
		// add one or more animation frames to the shape from a sepecified texture or resource pointer
		void AddFrame(ResourcePtr<Texture> texture, const unsigned int& layer, const std::vector<glm::vec2>& textureRect = {});
		void AddFrame(Texture* texture, const unsigned int& layer, const std::vector<glm::vec2>& textureRect = {});
		void AddFrames(ResourcePtr<Texture> texture, const unsigned int& layer, const unsigned int& numFrames, const unsigned int& numPerRow,
				const unsigned int& numPerCol, const glm::ivec2& offset = glm::ivec2(0, 0));
		void AddFrames(Texture* texture, const unsigned int& layer, const unsigned int& numFrames, const unsigned int& numPerRow,
				const unsigned int& numPerCol, const glm::ivec2& offset = glm::ivec2(0, 0));
		
		// set the animation attributes
		void SetAnimation(const float& speed, const unsigned int& start, const unsigned int& end, const int& loops = -1);
	protected :
		RenderBatchData Upload();
	private :
		void CalculateTexCoords(const std::vector<glm::vec2>& points, const std::vector<glm::vec2>& texCoordsMax, std::vector<glm::vec2>& texCoordsLocation);
		void CreateVBOVertices(const std::vector<glm::vec2>& points, const glm::mat3& transform, RenderBatchData& batchData, std::vector<glm::vec2> texCoords);
	private :
		WindingRule mWindingRule = WindingRule::Odd;
		std::vector< std::vector<VBOIndex> > mIndices = {{}, {}, {}, {}, {}}; // indices used to render this shape in various styles (line, fill, etc) stored for efficiency
		std::vector<glm::vec2> mVertices; // any vertices added during the triangulation process stored for efficiency
		
		std::vector<AnimationFrame> mFrames;
		unsigned int mCurrentFrame = 0u;
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
