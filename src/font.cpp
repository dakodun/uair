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

#include "font.hpp"

#include <iostream>
#include <algorithm>

#include "game.hpp"
#include "polygon.hpp"

namespace uair {
Font::Font(Font&& other) : Font() {
	swap(*this, other);
}

Font::~Font() {
	if (mFTFace) {
		FT_Done_Face(mFTFace);
	}
}

Font& Font::operator=(Font other) {
	swap(*this, other);
	return *this;
}

void swap(Font& first, Font& second) {
	std::swap(first.mFTFace, second.mFTFace);
}

bool Font::LoadFromFile(const std::string& filename, const unsigned int& pointSize) {
	FT_Error ftError = FT_New_Face(GAME.GetFTLibrary(), filename.c_str(), 0, &mFTFace);
	if (ftError != 0) {
		mFTFace = nullptr;
		std::cout << "error loading face: " << ftError << std::endl;
		return false;
	}
	
	ftError = FT_Set_Char_Size(mFTFace, pointSize * 64, 0, 72, 0);
	if (ftError != 0) {
		FT_Done_Face(mFTFace);
		mFTFace = nullptr;
		std::cout << "error setting size: " << ftError << std::endl;
		return false;
	}
	
	return true;
}

Polygon Font::LoadGlyph(const char& charCode) {
	if (mFTFace) {
		FT_Load_Char(mFTFace, charCode, FT_LOAD_NO_BITMAP);// | FT_LOAD_NO_SCALE);
		FT_Outline outline = mFTFace->glyph->outline;
		if (mFTFace->glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
			std::cout << "glyph is not an outline" << std::endl;
			return Polygon();
		}
		
		{
			Polygon glyphPoly; // final polygon representing whole glyph (one outer and zero to n inner boundaries)
			int start, end = 0; // start and end indices of current contour
			
			for (int i = 0; i < outline.n_contours; ++i) { // for all contours in glyph...
				Contour contour;
				end = outline.contours[i] + 1; // update end index
				std::vector<glm::vec2> bezier; // current bezier curve
				int previousPoint = -1; // tag representing previous point
				
				for (int j = start; j < end; ++j) { // for all indices in current contour...
					glm::vec2 pointCurr(outline.points[j].x / 64.0f, outline.points[j].y / 64.0f);
					
					switch (outline.tags[j] & 3) {
						case FT_CURVE_TAG_ON :
							if (previousPoint == FT_CURVE_TAG_ON || previousPoint == -1) {
								contour.AddPoint(pointCurr);
							}
							else {
								bezier.push_back(pointCurr);
								contour.AddBezier(bezier);
								bezier.clear();
							}
							
							break;
						case FT_CURVE_TAG_CONIC :
							if (previousPoint == FT_CURVE_TAG_ON) {
								bezier.push_back(pointCurr);
							}
							else if (previousPoint == FT_CURVE_TAG_CONIC || previousPoint == FT_CURVE_TAG_CUBIC) {
								glm::vec2 pointPrev(outline.points[j - 1].x / 64.0f, outline.points[j - 1].y / 64.0f);
								glm::vec2 midPoint((pointCurr.x + pointPrev.x) / 2, (pointCurr.y + pointPrev.y) / 2);
								
								bezier.push_back(midPoint);
								contour.AddBezier(bezier);
								
								bezier.clear();
								bezier.push_back(pointCurr);
							}
							else {
								glm::vec2 pointEnd(outline.points[end - 1].x / 64.0f, outline.points[end - 1].y / 64.0f);
								int finalPoint = outline.tags[end - 1] & 3;
								
								if (finalPoint == FT_CURVE_TAG_ON) {
									contour.AddPoint(pointEnd);
									bezier.push_back(pointCurr);
								}
								else if (finalPoint == FT_CURVE_TAG_CONIC || finalPoint == FT_CURVE_TAG_CUBIC ) {
									glm::vec2 midPoint((pointCurr.x + pointEnd.x) / 2, (pointCurr.y + pointEnd.y) / 2);
									
									contour.AddPoint(midPoint);
									bezier.push_back(pointCurr);
								}
							}
							
							if (j == end - 1) {
								glm::vec2 pointStart(outline.points[start].x / 64.0f, outline.points[start].y / 64.0f);
								int initialPoint = outline.tags[start] & 3;
								
								if (initialPoint == FT_CURVE_TAG_ON) {
									bezier.push_back(pointStart);
									contour.AddBezier(bezier);
								}
								else if (initialPoint == FT_CURVE_TAG_CONIC || initialPoint == FT_CURVE_TAG_CUBIC ) {
									glm::vec2 midPoint((pointCurr.x + pointStart.x) / 2, (pointCurr.y + pointStart.y) / 2);
									
									bezier.push_back(midPoint);
									contour.AddBezier(bezier);
								}
							}
							
							break;
						case FT_CURVE_TAG_CUBIC :
							std::cout << "CUBIC POINT DETECTED! LAUNCHING COUNTERMEASURES..." << std::endl;
							
							break;
					}
					
					previousPoint = outline.tags[j] & 3;
				}
				
				glyphPoly.AddContour(contour, CoordinateSpace::Global);
				start = end; // update start index
			}
			
			glyphPoly.SetScale(glm::vec2(1.0f, -1.0f));
			glyphPoly = glyphPoly.GetTransformed();
			
			// create shape;
			// create another shape with offset
			// render offset shape and regular shape to texture (set second contour colour to white, first contour to black)
				// need custom Upload for shape
			// return pointer to texture? nothing?
			
			// [TODO] GET GLYPH DRAWN TO TEXTURE
			// [TODO] GET GLYPH(S) ADDED TO EXISTINBG TEXTURE (PACKING/MULTIPLE LAYERS)
			// [TODO] GET METRICS AND TEXTURE INFO FOR GLYPHS SAVED
			
			/* {
				std::vector<Contour> contours;
				contours = p.GetContours();
				p.Offset(5.0f, ClipperLib::jtMiter, 16.0d);
				contours = p.GetContours();
				
				uair::Triangulate triangulate;
				
				for (auto iter = contours.begin(); iter != contours.end(); ++iter) {
					triangulate.AddContour(std::move(iter->GetPoints()));
				}
				
				Triangulate::Result result = triangulate.Process(mWindingRule); // WINDING RULE
				mIndices = result.second; // ADD INDICES TO A STORE
				
				for (auto vertex = result.first.begin(); vertex != result.first.end(); ++vertex) {
					mVertices.push_back(vertex->mPoint); // ADD VERTEX TO STORE
				}
				
				std::vector<glm::vec2> texCoords;
				
				{
					for (unsigned int i = 0; i < points.size(); ++i) {
						glm::vec3 pos = transform * glm::vec3(points.at(i), 1.0f);
						
						VBOVertex vert;
						vert.mX = pos.x; vert.mY = pos.y; vert.mZ = mDepth + 1000.5f;
						vert.mNX = 0.0f; vert.mNY = 0.0f; vert.mNZ = 1.0f;
						vert.mS = 0.0f; vert.mT = 0.0f; vert.mLayer = 0.0f;
						vert.mR = mColour.x; vert.mG = mColour.y; vert.mB = mColour.z; vert.mA = 1.0f;
						vert.mTex = 0.0f;
						
						batchData.mVertData.push_back(vert);
					}
				}
			} */
			
			// glyphPoly.Flip(glm::ivec2(0, 1));
			return glyphPoly;
		}
	}
	
	return Polygon();
}
}
