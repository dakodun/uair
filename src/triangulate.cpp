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

#include "triangulate.hpp"

namespace uair {
Triangulate::VertexData::VertexData(const glm::vec2& vertex, const unsigned int& index) {
	mVertex[0] = vertex.x;
	mVertex[1] = vertex.y;
	mVertex[2] = 0.0d;
	
	mIndex = index;
}

GLdouble& Triangulate::VertexData::operator[](const size_t& index) {
	return mVertex[index];
}

Triangulate::Triangulate(const Triangulate& other) : Triangulate() {
	mContours.insert(mContours.end(), other.mContours.begin(), other.mContours.end());
}

Triangulate::Triangulate(Triangulate&& other) : Triangulate() {
	swap(*this, other);
}

Triangulate::Triangulate() {
	mTesselator = gluNewTess();
	
	if (mTesselator) {
		gluTessNormal(mTesselator, 0.0d, 0.0d, 1.0d);
		
		gluTessCallback(mTesselator, GLU_TESS_BEGIN,        (Callback)HandleBegin);
		gluTessCallback(mTesselator, GLU_TESS_END,          (Callback)HandleEnd);
		gluTessCallback(mTesselator, GLU_TESS_VERTEX_DATA,  (Callback)HandleVertexData);
		gluTessCallback(mTesselator, GLU_TESS_COMBINE_DATA, (Callback)HandleCombineData);
		gluTessCallback(mTesselator, GLU_TESS_ERROR_DATA,   (Callback)HandleErrorData);
		gluTessCallback(mTesselator, GLU_TESS_EDGE_FLAG,    (Callback)HandleEdgeFlag);
	}
}

Triangulate& Triangulate::operator=(Triangulate other) {
	swap(*this, other);
	
	return *this;
}

void swap(Triangulate& first, Triangulate& second) {
	std::swap(first.mTesselator, second.mTesselator);
	
	std::swap(first.mContours, second.mContours);
	std::swap(first.mVertexCount, second.mVertexCount);
	
	std::swap(first.mVertices, second.mVertices);
	std::swap(first.mIndices, second.mIndices);
	std::swap(first.mNewVertices, second.mNewVertices);
}

Triangulate::~Triangulate() {
	if (mTesselator) { // if valid tesselator handle...
		gluDeleteTess(mTesselator); // delete tesselator handle
	}
	
	FreeNewVertices(); // ensure new vertices are cleaned up
}

Triangulate::Result Triangulate::Process(const WindingRule& windingRule) {
	if (!mTesselator) {
		std::cerr << "tesselation error: unable to create tesselator\n";
	}
	
	mVertices.clear(); // ensure no vertices exist from previous triangulation
	mIndices.clear(); // ensure no indices exist from previous triangulation
	FreeNewVertices(); // ensure no new vertices exist from previous triangulation
	
	gluTessProperty(mTesselator, GLU_TESS_BOUNDARY_ONLY, GL_FALSE);
	gluTessProperty(mTesselator, GLU_TESS_WINDING_RULE, static_cast<unsigned int>(windingRule));
	
	try {
		unsigned int iter = 0u;
		gluTessBeginPolygon(mTesselator, (void*)this);
			for (auto contour = mContours.begin(); contour != mContours.end(); ++contour) {
				gluTessBeginContour(mTesselator);
					for (auto point = contour->begin(); point != contour->end(); ++point) {
						gluTessVertex(mTesselator, &((*point)[0]), &(*point));
						++iter;
					}
				gluTessEndContour(mTesselator);
			}
		gluTessEndPolygon(mTesselator);
	} catch (std::exception& e) {
		
	}
	
	FreeNewVertices(); // clean up new vertices
	
	return std::make_pair(mVertices, mIndices);
}

void Triangulate::AddContour(const std::vector<glm::vec2>& contour) {
	std::vector<VertexData> vec;
	for (auto point = contour.begin(); point != contour.end(); ++point) {
		vec.emplace_back(*point, mVertexCount);
		++mVertexCount;
	}
	
	mContours.push_back(std::move(vec));
}

void Triangulate::AddContours(const std::vector< std::vector<glm::vec2> >& contours) {
	for (auto contour = contours.begin(); contour != contours.end(); ++contour) {
		AddContour(*contour);
	}
}

void Triangulate::Clear() {
	mContours.clear();
}

void Triangulate::HandleBegin(GLenum which) {
	
}

void Triangulate::HandleEnd() {
	
}

void Triangulate::HandleVertexData(void *vertex_data, void *polygon_data) {
	Triangulate& triangulate = *(static_cast<Triangulate*>(polygon_data));
	VertexData& vertex = *(static_cast<VertexData*>(vertex_data));
	triangulate.mIndices.push_back(vertex.mIndex);
}

void Triangulate::HandleCombineData(GLdouble coords[3], void *vertex_data[4], GLfloat weight[4], void **outData, void *polygon_data) {
	Triangulate& triangulate = *(static_cast<Triangulate*>(polygon_data));
	std::vector<VertexData*> vertices = {static_cast<VertexData*>(vertex_data[0]), static_cast<VertexData*>(vertex_data[1]),
			static_cast<VertexData*>(vertex_data[2]), static_cast<VertexData*>(vertex_data[3])};
	
	Vertex vertex;
	vertex.mPoint = glm::vec2(coords[0], coords[1]);
	vertex.mNeighbourIndices = {vertices.at(0)->mIndex, vertices.at(1)->mIndex, vertices.at(2)->mIndex, vertices.at(3)->mIndex};
	vertex.mNeighbourWeights = {weight[0], weight[1], weight[2], weight[3]};
	triangulate.mVertices.push_back(std::move(vertex));
	
	VertexData* newVertex = new VertexData(glm::vec2(coords[0], coords[1]), triangulate.mVertexCount);
	triangulate.mNewVertices.push_back(newVertex);
	++triangulate.mVertexCount;
	*outData = newVertex;
}

void Triangulate::HandleErrorData(GLenum errno, void *polygon_data) {
	std::cerr << "tesselation error: " << gluErrorString(errno) << '\n';
}

void Triangulate::HandleEdgeFlag(GLboolean flag) {
	
}

void Triangulate::FreeNewVertices() {
	for (auto vertex = mNewVertices.begin(); vertex != mNewVertices.end(); ++vertex) { // for all new vertices...
		delete *vertex; // delete vertex from heap
	}
	
	mNewVertices.clear(); // clear new vertex array
}
}
