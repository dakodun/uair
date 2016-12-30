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

#ifndef UAIRRENDERBATCH_HPP
#define UAIRRENDERBATCH_HPP

#include <string>
#include <map>
#include <vector>

#include "init.hpp"
#include "vbo.hpp"

namespace uair {
// render data from a renderable that is to be sent to vbo
struct EXPORTDLL RenderBatchData {
	std::vector<VBOVertex> mVertexData; // vertices to send to vbo
	std::vector<VBOIndex> mIndexData; // indices to send to vbo
	
	unsigned int mPass = 0u;
	ResourcePtr<Shader> mShader;
	GLuint mTextureID = 0u; // the texture id to bind in vbo
	GLenum mRenderMode = GL_TRIANGLES; // the mode to use when rendering this renderable
	std::string mTag = ""; // the tag (type) of the underlying renderable
};

// 
struct EXPORTDLL Segment {
	unsigned int mPass;
	GLuint mVAOID;
	ResourcePtr<Shader> mShader;
	GLuint mTextureID;
	
	GLenum mRenderMode;
	GLuint mIndicesCount;
	GLuint mIndicesOffset;
	GLuint mMinIndex;
	GLuint mMaxIndex;
};


class Renderable; // forward declare the renderable class

// a batch of renderable objects that is processed and uploaded to a vbo
class EXPORTDLL RenderBatch {
	public :
		void Add(Renderable& renderable, const unsigned int& pass = 0u); // add a renderable to the batch for the specified render pass
		void Add(std::list<RenderBatchData> rbd, const unsigned int& pass = 0u);
		void Upload(); // upload the batch to vbo
		void Draw(const unsigned int& pass = 0u); // draw the specified pass to the default framebuffer
		void Draw(const FBO& fbo, const unsigned int& pass); // draw the specified pass to the specified framebuffer
	private :
		static bool RenderDataSort(const RenderBatchData& first, const RenderBatchData& second);
	
	private :
		std::vector<RenderBatchData> mRenderData; // holds the data to be rendered for each pass
		
		VBO mVBO; // vertex buffer object (vbo) used for rendering
};
}

#endif
