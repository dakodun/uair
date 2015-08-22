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

#ifndef UAIRRENDERBATCH_HPP
#define UAIRRENDERBATCH_HPP

#include <string>
#include <map>
#include <vector>

#include "vbo.hpp"

namespace uair {
// render data from a renderable that is to be sent to vbo
struct RenderBatchData {
	std::vector<VBOVertex> mVertData; // vertices to send to vbo
	std::vector<VBOIndex> mIndData; // indices to send to vbo
	
	GLuint mTexID = 0u; // the texture id to bind in vbo
	GLenum mRenderMode = GL_TRIANGLES;
	std::string mTag = ""; // the tag (type) of the underlying renderable
};

// a segment of vertices in vbo that share the same texture
struct SegmentInfo {
	SegmentInfo() = default;
	SegmentInfo(const GLuint& texID, const GLenum& renderMode, const GLuint& count, const GLuint& offset, const GLuint& min, const GLuint& max) :
			mTexID(texID), mRenderMode(renderMode), mIndicesCount(count), mIndicesOffset(offset), mMinIndex(min), mMaxIndex(max) {
		
		
	}
	
	GLuint mTexID = 0u; // the texture id of the segment
	GLenum mRenderMode = GL_TRIANGLE_FAN;
	
	GLuint mIndicesCount = 0u;
	GLuint mIndicesOffset = 0u;
	GLuint mMinIndex = 0u;
	GLuint mMaxIndex = 0u;
};

class Renderable; // forward declare the renderable class

// a batch of renderable objects that is processed and uploaded to a vbo
class RenderBatch {
	public :
		void Add(Renderable& renderable, const unsigned int& pass = 0); // add a renderable to the batch for the specified render pass
		void Upload(); // upload the batch to vbo
		void Draw(const unsigned int& pass = 0); // draw the specified pass to the default framebuffer
		void Draw(const FBO& fbo, const unsigned int& pass); // draw the specified pass to the specified framebuffer
	private :
		typedef std::pair< unsigned int, std::vector<RenderBatchData> > IndexedRenderBatchData; // unsigned int/render batch data vector pair
		typedef std::pair< unsigned int, std::vector<SegmentInfo> > IndexedSegmentInfo; // unsigned int/segment info vector pair
		
		std::map< unsigned int, std::vector<RenderBatchData> > mRenderData;
		
		VBO mVBO; // vertex buffer object (vbo) used for rendering
};
}

#endif
