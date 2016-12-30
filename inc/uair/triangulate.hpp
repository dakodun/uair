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

#ifndef UAIRTRIANGULATE_HPP
#define UAIRTRIANGULATE_HPP

#ifndef CALLBACK
#define CALLBACK
#endif

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "init.hpp"

namespace uair {
enum class WindingRule: unsigned int {
	Odd = 100130u,
	NonZero,
	Positive,
	Negative,
	AbsGeqTwo
};

class EXPORTDLL Triangulate {
	public :
		class Vertex {
			public :
				glm::vec2 mPoint;
				std::vector<unsigned int> mNeighbourIndices;
				std::vector<GLfloat> mNeighbourWeights;
		};
		
		typedef std::pair< std::vector<Vertex>, std::vector<unsigned int> > Result;
	private :
		class VertexData {
			public :
				explicit VertexData(const glm::vec2& vertex, const unsigned int& index);
				
				GLdouble& operator[](const size_t& index);
			public :
				GLdouble mVertex[3];
				unsigned int mIndex = 0u;
		};
	public :
		Triangulate();
		Triangulate(const Triangulate& other);
		Triangulate(Triangulate&& other);
		~Triangulate();
		
		Triangulate& operator=(Triangulate other);
		
		friend void swap(Triangulate& first, Triangulate& second);
		
		Result Process(const WindingRule& windingRule = WindingRule::Odd);
		void AddContour(const std::vector<glm::vec2>& contour);
		void AddContours(const std::vector< std::vector<glm::vec2> >& contours);
		void Clear();
	protected :
		static CALLBACK void HandleBegin(GLenum which);
		static CALLBACK void HandleEnd();
		static CALLBACK void HandleVertexData(void *vertex_data, void *polygon_data);
		static CALLBACK void HandleCombineData(GLdouble coords[3], void *vertex_data[4], GLfloat weight[4], void **outData, void *polygon_data);
		static CALLBACK void HandleErrorData(GLenum errno, void *polygon_data);
		static CALLBACK void HandleEdgeFlag(GLboolean flag);
	private :
		void FreeNewVertices();
	private :
		GLUtesselator* mTesselator; // handle to glu tesselator object
		
		std::vector< std::vector<VertexData> > mContours; // contours to be triangulated
		unsigned int mVertexCount = 0u; // total number of vertices (including newly created vertices)
		
		std::vector<Vertex> mVertices; // new vertices created from contour intersections
		std::vector<unsigned int> mIndices; // triangle indices describing triangulation of input contours
		std::vector<VertexData*> mNewVertices; // new vertices created from contour intersections allocated (temporarily) on the heap during triangulation
};
}

#endif
