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

#ifndef UAIRRENDERBATCH_HPP
#define UAIRRENDERBATCH_HPP

#include <string>
#include <vector>

#include "vbo.hpp"

namespace uair {
// render data from a renderable that is to be sent to vbo
struct RenderBatchData {
	std::vector<VBOVertex> mVertData; // vertices to send to vbo
	std::vector<VBOIndex> mIndData; // indices to send to vbo
	
	GLuint mTexID = 0; // the texture id to bind in vbo
	std::string mTag = ""; // the tag (type) of the underlying renderable
};

// a segment of vertices in vbo that share the same texture
struct SegmentInfo {
	SegmentInfo() = default;
	SegmentInfo(const GLuint & texID, const GLuint & start, const GLuint & end) :
			mTexID(texID), mStart(start), mEnd(end) {
		
		
	}
	
	GLuint mTexID = 0; // the texture id of the segment
	GLuint mStart = 0; // the start index of the segment
	GLuint mEnd = 0; // the final index of the segment
};

class Renderable; // forward declare the renderable class

// a batch of renderable objects that is processed and uploaded to a vbo
class RenderBatch {
	public :
		void Add(const Renderable & renderable); // add a renderable to the batch
		void Upload(); // upload the batch to vbo
		void Draw(); // draw the batch to the context
	private :
		std::vector<RenderBatchData> mRenderData; // the processed data of the added renderables
		
		VBO mVBO; // vertex buffer object (vbo) used for rendering
};
}

#endif
