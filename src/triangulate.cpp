/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2010 - 2017, Iain M. Crawford
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

#include "util.hpp"

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
		
		gluTessCallback(mTesselator, GLU_TESS_BEGIN,        (void (CALLBACK *)())HandleBegin);
		gluTessCallback(mTesselator, GLU_TESS_END,          (void (CALLBACK *)())HandleEnd);
		gluTessCallback(mTesselator, GLU_TESS_VERTEX_DATA,  (void (CALLBACK *)())HandleVertexData);
		gluTessCallback(mTesselator, GLU_TESS_COMBINE_DATA, (void (CALLBACK *)())HandleCombineData);
		gluTessCallback(mTesselator, GLU_TESS_ERROR_DATA,   (void (CALLBACK *)())HandleErrorData);
		gluTessCallback(mTesselator, GLU_TESS_EDGE_FLAG,    (void (CALLBACK *)())HandleEdgeFlag);
	}
}

Triangulate& Triangulate::operator=(Triangulate other) {
	swap(*this, other);
	
	return *this;
}

void swap(Triangulate& first, Triangulate& second) {
	using std::swap;
	
	swap(first.mTesselator, second.mTesselator);
	
	swap(first.mContours, second.mContours);
	swap(first.mVertexCount, second.mVertexCount);
	
	swap(first.mVertices, second.mVertices);
	swap(first.mIndices, second.mIndices);
	swap(first.mNewVertices, second.mNewVertices);
}

Triangulate::~Triangulate() {
	if (mTesselator) { // if valid tesselator handle...
		gluDeleteTess(mTesselator); // delete tesselator handle
	}
	
	FreeNewVertices(); // ensure new vertices are cleaned up
}

Triangulate::Result Triangulate::Process(const WindingRule& windingRule) {
	if (!mTesselator) {
		util::LogMessage(1, "tesselation error: unable to create tesselator"); // log the error
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
	mContours.clear(); // remove any contours added to triangulator
}

void Triangulate::HandleBegin(GLenum which) {
	// needed for glutess (unused)
}

void Triangulate::HandleEnd() {
	// needed for glutess (unused)
}

void Triangulate::HandleVertexData(void *vertex_data, void *polygon_data) {
	Triangulate& triangulate = *(static_cast<Triangulate*>(polygon_data)); // cast instance handle back
	VertexData& vertex = *(static_cast<VertexData*>(vertex_data)); // cast vertex back
	triangulate.mIndices.push_back(vertex.mIndex); // add new vertex's index to instance handle's index array
}

void Triangulate::HandleCombineData(GLdouble coords[3], void *vertex_data[4], GLfloat weight[4], void **outData, void *polygon_data) {
	Triangulate& triangulate = *(static_cast<Triangulate*>(polygon_data)); // cast instance handle back
	
	Vertex vertex; // create a new vertex
	vertex.mPoint = glm::vec2(coords[0], coords[1]); // set coords of the new vertex
	
	for (unsigned int i = 0u; i < 4; ++i) { // for all possible neighbours...
		VertexData* vertexData = static_cast<VertexData*>(vertex_data[i]); // cast the neighbour data back
		if (vertexData) { // if the neighbour data is valid...
			vertex.mNeighbourIndices.push_back(vertexData->mIndex); // store the neighbours index
			vertex.mNeighbourWeights.push_back(weight[i]); // store the neighbours weight (influence on new vertex)
		}
	}
	
	triangulate.mVertices.push_back(std::move(vertex)); // store new vertex
	
	VertexData* newVertex = new VertexData(glm::vec2(coords[0], coords[1]), triangulate.mVertexCount); // create vertex data for the new vertex (cleaned up later)
	triangulate.mNewVertices.push_back(newVertex); // add new vertex data to store
	++triangulate.mVertexCount; // increment the total vertex count
	*outData = newVertex; // set the output to the new vertex data
}

void Triangulate::HandleErrorData(GLenum errno, void *polygon_data) {
	const GLubyte* err = gluErrorString(errno); // get error string
	if (err) { // if error string exists...
		std::string errString = (const char*)err; // convert the error string
		util::LogMessage(1, "tesselation error: " + errString); // log the error
	}
	else { // otherwise no error string exists...
		util::LogMessage(1, "tesselation error: unknown error"); // log unknown error
	}
}

void Triangulate::HandleEdgeFlag(GLboolean flag) {
	// needed for glutess (unused)
}

void Triangulate::FreeNewVertices() {
	for (auto vertex = mNewVertices.begin(); vertex != mNewVertices.end(); ++vertex) { // for all new vertex data...
		delete *vertex; // free vertex data memory
	}
	
	mNewVertices.clear(); // clear new vertex data array
}
}
