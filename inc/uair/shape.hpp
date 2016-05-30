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

#ifndef UAIRSHAPE_HPP
#define UAIRSHAPE_HPP

#include "polygon.hpp"
#include "renderable.hpp"
#include "resourceptr.hpp"
#include "texture.hpp"
#include "vbo.hpp"
#include "triangulate.hpp"

namespace uair {
class Game;
class RenderBatch;

struct AnimationFrame {
	ResourcePtr<Texture> mTexture; // resource pointer to the texture source for this frame
	std::vector<glm::vec2> mTexCoords; // texture coordinates for the vertices in the shape
	std::vector<glm::vec2> mTexCoordsExtra; // texture coordinates for the extra vertices added by triangulation
	
	float mLayer; // the layer of the source texture
	glm::vec2 mMinST; // the top-left-most texture coord
	glm::vec2 mMaxST; // the bottom-right-most texture coord
};

class Shape : public Polygon, public Renderable {
	friend class RenderBatch;
	friend class RenderString;
	
	public :
		Shape() = default;
		explicit Shape(const std::vector<Contour>& contours, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
		explicit Shape(const ClipperLib::Paths& clipperPaths);
		
		// add contours to the shape
		void AddContour(const Contour& contour, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
		void AddContours(const std::vector<Contour>& contours, const CoordinateSpace& coordinateSpace = CoordinateSpace::Local);
		
		// offset the shape by an amount (in/out)
		void Offset(float distance, const ClipperLib::JoinType& miterType = ClipperLib::jtRound, const double& miterLimit = 2.0d);
		
		// adjust the contours so the lower bound is at the origin (0, 0)
		void PositionContoursAtOrigin();
		
		// create the shape from paths from the external clipper library
		void FromClipperPaths(const ClipperLib::Paths& clipperPaths);
		
		// return a copy of this shape with its transformations applied directly to its contours (transformations of new shape are reset back to default)
		Shape GetTransformed() const;
		
		// return the renderable type tag ("Shape")
		std::string GetTag() const;
		
		// process the shape's animation
		void Process();
		
		// set the winding rule for filled drawing (forces retriangulation)
		void SetWindingRule(const WindingRule& windingRule);
		
		// add custom per-vertex colouring
		void AddColour(const std::vector<glm::vec4>& colour);
		
		// add one or more animation frames to the shape from a sepecified texture or resource pointer
		void AddFrame(ResourcePtr<Texture> texture, const unsigned int& layer);
		void AddFrameCoords(ResourcePtr<Texture> texture, const unsigned int& layer, std::vector<glm::vec2> textureCoords);
		void AddFrameRect(ResourcePtr<Texture> texture, const unsigned int& layer, const std::vector<glm::vec2>& textureRect);
		void AddFrameStrip(ResourcePtr<Texture> texture, const unsigned int& layer, const unsigned int& numFrames, const unsigned int& numPerRow,
				const unsigned int& numPerCol, const glm::ivec2& offset = glm::ivec2(0, 0));
		
		void AddFrame(Texture* texture, const unsigned int& layer);
		void AddFrameCoords(Texture* texture, const unsigned int& layer, std::vector<glm::vec2> textureCoords);
		void AddFrameRect(Texture* texture, const unsigned int& layer, const std::vector<glm::vec2>& textureRect);
		void AddFrameStrip(Texture* texture, const unsigned int& layer, const unsigned int& numFrames, const unsigned int& numPerRow,
				const unsigned int& numPerCol, const glm::ivec2& offset = glm::ivec2(0, 0));
		
		// set the animation attributes
		void SetAnimation(const float& speed, const unsigned int& start, const unsigned int& end, const int& loops = -1);
	protected :
		// transform the shape into the correct format for rendering
		std::list<RenderBatchData> Upload();
	private :
		// calculate the texture coordinates of vertices from supplied texture rectangle (min and max st coordinates) and store result in supplied array
		void CalculateTexCoords(const std::vector<glm::vec2>& points, const std::vector<glm::vec2>& texRect, std::vector<glm::vec2>& texCoordsLocation);
		
		// create vertices for rendering from the vertices, transform matrix, and texture coords and colours supplied
		void CreateVBOVertices(RenderBatchData& batchData, const std::vector<glm::vec2>& vertices, std::vector<glm::vec2> texCoords,
				std::vector<glm::vec4> colours);
		
		// recalculate the colours for extra vertices added during triangulation
		void CalculateExtraColour();
		
		// recalculate the texture coords for extra vertices added during triangulation
		void CalculateExtraTexCoords(AnimationFrame& frame);
	
	public :
		static float mFrameLowerLimit;
	private :
		WindingRule mWindingRule = WindingRule::Odd; // winding rule for triangulation
		std::vector< std::vector<VBOIndex> > mIndices = {{}, {}, {}, {}, {}}; // indices used to render this shape in various styles (line, fill, etc) stored for efficiency
		std::vector<Triangulate::Vertex> mVertices; // any vertices added during the triangulation process stored for efficiency
		
		std::vector<glm::vec4> mColourArray; // per-vertex colours
		std::vector<glm::vec4> mColourArrayExtra; // per-vertex colours for extra vertices
		
		std::vector<AnimationFrame> mFrames; // animation frame data
		unsigned int mCurrentFrame = 0u; // index of the current animation frame (if any)
		bool mIsAnimated = false; // is shape animated
		int mAnimationDirection = 1; // direction of animation (forward (1) or reverse (1))
		float mAnimationLimit = 0.0f; // limit of animation (essentially it's speed)
		float mAnimationTimer = 0.0f; // timer before switching to next frame
		int mAnimationLoopCount = 0; // how many times to loop the animation (-1 implies infinitely)
		unsigned int mAnimationStartFrame = 0; // frame to begin the animation on
		unsigned int mAnimationEndFrame = 0; // frame to end the animation on
};
}

#endif
