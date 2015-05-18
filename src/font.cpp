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
#include "openglstates.hpp"
#include "polygon.hpp"
#include "shape.hpp"
#include "renderbatch.hpp"
#include "exception.hpp"

namespace uair {
Font::Font(const unsigned int& textureSize) {
	mTextureSize = util::NextPowerOf2(textureSize);
	
	OpenGLStates::BindFBO(mFBO.GetFBOID());
	mFBO.AddTexture({GL_COLOR_ATTACHMENT0}, mTextureSize, mTextureSize);
	mFBO.AddRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, mTextureSize, mTextureSize);
	
	GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, attachments);
	
	glViewport(0, 0, mTextureSize, mTextureSize); // set the viewport to match the texture dimensions...
	OpenGLStates::mProjectionMatrix = glm::ortho(0.0f, static_cast<float>(mTextureSize), static_cast<float>(mTextureSize), 0.0f, 0.0f, -9999.0f); // ...as well as the projection matrix
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear colour to white...
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ...and clear the fbo texture
	
	mRectangles.emplace_back(std::make_pair(glm::ivec2(0, 0), glm::ivec2(mTextureSize, mTextureSize)));
}

Font::Font() : Font(512u) {
	
}

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

void Font::LoadGlyph(const char& charCode) {
	std::vector<Shape> glyphs;
	
	/* if (charCode not already loaded) {
		
	} */
	
	// return CreateGlyphShape(charCode);
	
	try {
		glyphs.push_back(CreateGlyphShape(charCode));
	} catch(UairException& e) {
		
	}
	
	UpdateTexture(glyphs);
}

void Font::LoadGlyphs(const std::vector<char>& charCodes) {
	std::vector<Shape> glyphs;
	
	for (auto charCode = charCodes.begin(); charCode != charCodes.end(); ++charCode) {
		/* if (*charCode not already loaded) {
			
		} */
		
		try {
			glyphs.push_back(CreateGlyphShape(*charCode));
		} catch(UairException& e) {
			
		}
	}
	
	UpdateTexture(glyphs);
}

Shape Font::CreateGlyphShape(const char& charCode) {
	if (mFTFace) {
		FT_Load_Char(mFTFace, charCode, FT_LOAD_NO_BITMAP);// | FT_LOAD_NO_SCALE);
		FT_Outline outline = mFTFace->glyph->outline;
		if (mFTFace->glyph->format != FT_GLYPH_FORMAT_OUTLINE) {
			std::cout << "glyph is not an outline" << std::endl;
			
			throw UairException("font: wrong glyph type");
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
			
			uair::Shape shp;
			shp.AddContours(glyphPoly.GetContours());
			return shp;
		}
	}
	
	throw UairException("font: invalid face object");
}

void Font::UpdateTexture(const std::vector<Shape>& newShapes) {
	uair::RenderBatch batch; // render batch used to create individual glyph textures as well as font texture
	typedef std::pair<Rectangle, unsigned int> RectangleIndexPair; // define a rectangle and index type
	
	// lambda to sort glyphs by their area (boundinx box)
	auto lambdaSortByArea = [](const RectangleIndexPair& first, const RectangleIndexPair& second)->bool {
		Rectangle firstRect = first.first; // get reference to first rectangle
		Rectangle secondRect = second.first; // get reference to second rectangle
		
		return ((firstRect.first.x * firstRect.first.y) < (secondRect.first.x * secondRect.first.y)); // return true if first area is smaller than second area
	};
	
	// create a priority queue sorted by area
	std::priority_queue<RectangleIndexPair, std::vector<RectangleIndexPair>, decltype(lambdaSortByArea)> pQueue(lambdaSortByArea);
	for (unsigned int i = 0; i < newShapes.size(); ++i) { // for all glyphs...
		const std::vector<glm::vec2>& bbox = newShapes.at(i).GetLocalBoundingBox(); // get the bounding box of the glyph
		Rectangle rect(bbox.at(0), bbox.at(2)); // create a rectangle with it
		pQueue.emplace(std::make_pair(rect, i)); // add it to the priority queue
	}
	
	std::vector<FBO> fbos; // array of individual fbos for rendering individual glyphs
	std::vector<uair::Shape> glyphShapes; // array of textured glyph rectangles
	float outerOffsetCount = 9; // number of times to offset outwards
	float offsetInc = 2.0f; // amount to offset each time
	
	while (!pQueue.empty()) { // whilst there are still glyphs to process...
		Shape shp = newShapes.at(pQueue.top().second); // get a copy of the glyph shape...
		
		PositionBase(shp, outerOffsetCount, 2.0f); // position the shape at the origin (0, 0) accounting for outsetting
		std::vector<uair::Shape> shapes = CreateGradient(shp, 9, outerOffsetCount, offsetInc); // create the gradients shapes by offsetting out and in
		for (auto shp2 = shapes.begin(); shp2 != shapes.end(); ++shp2) { // for all gradient shapes...
			batch.Add(*shp2); // add the gradient shape to the batch
		}
		
		batch.Upload(); // upload the gradients shapes
		fbos.emplace_back(); // add a new fbo to the array
		float offsetOffset = (outerOffsetCount * offsetInc) * 2; // calculate the outward offset distance
		glm::ivec2 dimensions(pQueue.top().first.second - pQueue.top().first.first); // calculate the dimensions (width and height) of the base glyph
		
		{ // render gradient to fbo
			OpenGLStates::BindFBO(fbos.back().GetFBOID()); // bind the new fbo
			unsigned int width = util::NextPowerOf2(dimensions.x + offsetOffset); // calculate the width of the base glyph plus offset rounded to nearest power of two...
			unsigned int height = util::NextPowerOf2(dimensions.y + offsetOffset);// ...and the same for height
			fbos.back().AddTexture({GL_COLOR_ATTACHMENT0}, width, height); // add a texture to the new fbo...
			fbos.back().AddRenderBuffer(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8, width, height); // ...and a matching render buffer
			
			// set up fbo shader attachments
			GLuint attachments[1] = {GL_COLOR_ATTACHMENT0}; 
			glDrawBuffers(1, attachments);
			
			RenderToFBO(fbos.back(), batch, width, height); // render the gradient to the new fbo
		}
		
		{
			glyphShapes.emplace_back(); // add an empty glyph rectangle shape to the array
			
			// add an appropiately sized contour to the new rectangle shape
			glyphShapes.back().AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(dimensions.x + offsetOffset, 0.0f),
					glm::vec2(dimensions.x + offsetOffset, dimensions.y + offsetOffset), glm::vec2(0.0f, dimensions.y + offsetOffset)}));
			glyphShapes.back().SetScale(glm::vec2(0.25f, 0.25f)); // scale the rectangle glyph by 1/4 (linearly interpolated)
			
			uair::ResourcePtr<uair::Texture> texPtr = fbos.back().GetTextures().front(); // get a pointer to the gradient fbo texture
			glyphShapes.back().AddFrameRect(texPtr, 0u, {glm::vec2(0.0f, 0.0f), glm::vec2(dimensions.x + offsetOffset, dimensions.y + offsetOffset)}); // texture the new rectangle shape
		}
		
		Pack(glyphShapes.back()); // pack the new rectangle shape into the font's texture [!] store which layer to render to
		
		pQueue.pop(); // remove the current glyph shape from the queue
	}
	
	for (auto shape = glyphShapes.begin(); shape != glyphShapes.end(); ++shape) { // for all glyph rectangle shapes...
		batch.Add(*shape); // add shape to the batch
	}
	
	batch.Upload(); // upload the glyph rectangles
	
	OpenGLStates::BindFBO(mFBO.GetFBOID()); // bind the font's fbo
	glViewport(0, 0, mTextureSize, mTextureSize); // set the viewport to match the texture dimensions...
	OpenGLStates::mProjectionMatrix = glm::ortho(0.0f, static_cast<float>(mTextureSize), static_cast<float>(mTextureSize), 0.0f, 0.0f, -9999.0f); // ...as well as the projection matrix
	batch.Draw(mFBO, 0u); // draw the glyphs to the texture
}

void Font::Pack(Shape& shape) {
	bool done = false;
	std::list<Rectangle>::iterator storedIter;
	Rectangle rectangleA, rectangleB;
	
	const std::vector<glm::vec2>& bbox = shape.GetGlobalBoundingBox();
	glm::ivec2 glyph(bbox.at(2) - bbox.at(0));
	
	for (storedIter = mRectangles.begin(); storedIter != mRectangles.end(); ++storedIter) {
		int rectWidth = storedIter->second.x - storedIter->first.x;
		int rectHeight = storedIter->second.y - storedIter->first.y;
		
		if (glyph.x <= rectWidth && glyph.y <= rectHeight) {
			if (glyph.x >= glyph.y) {
				rectangleA = std::make_pair(glm::ivec2(storedIter->first.x + glyph.x, storedIter->first.y), glm::ivec2(storedIter->second.x, storedIter->first.y + glyph.y));
				rectangleB = std::make_pair(glm::ivec2(storedIter->first.x, storedIter->first.y + glyph.y), glm::ivec2(storedIter->second.x, storedIter->second.y));
			}
			else {
				rectangleA = std::make_pair(glm::ivec2(storedIter->first.x, storedIter->first.y + glyph.y), glm::ivec2(storedIter->first.x + glyph.x, storedIter->second.y));
				rectangleB = std::make_pair(glm::ivec2(storedIter->first.x + glyph.x, storedIter->first.y), glm::ivec2(storedIter->second.x, storedIter->second.y));
			}
			
			done = true;
			break;
		}
	}
	
	if (done) {
		shape.SetPosition(storedIter->first);
		mRectangles.erase(storedIter);
		mRectangles.push_back(rectangleA); mRectangles.push_back(rectangleB);
	}
	
	// unable to pack, create new rectangle and texture layer
}

void Font::PositionBase(Shape& baseShape, const unsigned int& outerOffsetCount, const float& offsetInc) {
	baseShape.SetScale(glm::vec2(1.0f, -1.0f));
	
	const std::vector<glm::vec2>& points = baseShape.GetGlobalBoundingBox();
	if (points.size() > 3) {
		float offsetOffset = outerOffsetCount * offsetInc;
		baseShape.SetOrigin(glm::vec2(points.at(3).x - offsetOffset, points.at(3).y + offsetOffset));
	}
}

std::vector<Shape> Font::CreateGradient(const Shape& baseShape, const unsigned int& innerOffsetCount, const unsigned int& outerOffsetCount, const float& offsetInc) {
	std::vector<Shape> shapes;
	
	float offset = -(offsetInc * innerOffsetCount);
	float colour = 0.0f; float colourInc = 1.0f / (innerOffsetCount + outerOffsetCount + 1u);
	float depth = 0;
	
	for (unsigned int i = innerOffsetCount; i > 0u; --i) {
		Shape shape = baseShape;
		
		shape.Offset(offset, ClipperLib::jtRound, 2.0d);
		shape.SetColour(glm::vec3(colour));
		shape.SetDepth(depth);
		shapes.push_back(shape);
		
		offset += offsetInc;
		colour += colourInc;
		++depth;
	}
	
	{
		Shape shape = baseShape;
		
		shape.SetColour(glm::vec3(colour));
		shape.SetDepth(depth);
		shapes.push_back(shape);
		
		offset += offsetInc;
		colour += colourInc;
		++depth;
	}
	
	for (unsigned int i = 0u; i < outerOffsetCount; ++i) {
		Shape shape = baseShape;
		
		shape.Offset(offset, ClipperLib::jtRound, 2.0d);
		shape.SetColour(glm::vec3(colour));
		shape.SetDepth(depth);
		shapes.push_back(shape);
		
		offset += offsetInc;
		colour += colourInc;
		++depth;
	}
	
	return shapes;
}

void Font::RenderToFBO(FBO& fbo, RenderBatch& batch, const unsigned int& width, const unsigned int& height) {
	OpenGLStates::BindFBO(fbo.GetFBOID());
	glViewport(0, 0, width, height);
	OpenGLStates::mProjectionMatrix = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 0.0f, -9999.0f);
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	batch.Draw(fbo, 0u);
}
}
