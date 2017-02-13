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

#include "sdffont.hpp"

#include "signeddistancefield.hpp"
#include "util.hpp"
#include "openglstates.hpp"
#include "renderbatch.hpp"

namespace uair {
SDFFont::SDFFont() : FontBase(512u, glm::uvec2(0u, 255u)) {
	mCacheTag = "UFC2";
}

SDFFont::SDFFont(const unsigned int& textureSize) : FontBase(textureSize, glm::uvec2(0u, 255u)) {
	mCacheTag = "UFC2";
}

SDFFont::SDFFont(SDFFont&& other) : SDFFont() {
	swap(*this, other);
}

SDFFont& SDFFont::operator=(SDFFont other) {
	swap(*this, other);
	return *this;
}

void swap(SDFFont& first, SDFFont& second) {
	std::swap(first.mFTFace, second.mFTFace);
	std::swap(first.mFontSize, second.mFontSize);
	std::swap(first.mLineHeight, second.mLineHeight);
	
	std::swap(first.mAdvanceMax, second.mAdvanceMax);
	std::swap(first.mDropMax, second.mDropMax);
	std::swap(first.mBearingMax, second.mBearingMax);
	
	std::swap(first.mGlyphs, second.mGlyphs);
	std::swap(first.mKernMap, second.mKernMap);
	
	std::swap(first.mTextureSize, second.mTextureSize);
	std::swap(first.mFBO, second.mFBO);
	std::swap(first.mTexture, second.mTexture);
	std::swap(first.mRenderBuffer, second.mRenderBuffer);
	
	std::swap(first.mRectangles, second.mRectangles);
	
	std::swap(first.mCacheTag, second.mCacheTag);
	
	// ensure glyph shapes point to the correct font texture
		for (auto glyph = first.mGlyphs.begin(); glyph != first.mGlyphs.end(); ++glyph) {
			Shape shp;
			shp.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(glyph->second.mDimensions.x, 0.0f),
					glm::vec2(glyph->second.mDimensions.x, glyph->second.mDimensions.y), glm::vec2(0.0f, glyph->second.mDimensions.y)}));
			shp.AddFrameCoords(&first.mTexture, glyph->second.mLayer, glyph->second.mTexCoords);
			
			glyph->second.mBaseShape = std::move(shp);
		}
		
		for (auto glyph = second.mGlyphs.begin(); glyph != second.mGlyphs.end(); ++glyph) {
			Shape shp;
			shp.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(glyph->second.mDimensions.x, 0.0f),
					glm::vec2(glyph->second.mDimensions.x, glyph->second.mDimensions.y), glm::vec2(0.0f, glyph->second.mDimensions.y)}));
			shp.AddFrameCoords(&second.mTexture, glyph->second.mLayer, glyph->second.mTexCoords);
			
			glyph->second.mBaseShape = std::move(shp);
		}
	//
}

unsigned int SDFFont::GetTypeID() {
	return static_cast<unsigned int>(Resources::SDFFont);
}

unsigned int SDFFont::GetType() const {
	return 1u;
}

bool SDFFont::SetSize(const unsigned int& fontSize) {
	// set character size and dpi (convert size into 26.6 fixed format by bit shifting (multiplying) and scale up 8 times for sdf generation
	FT_Error ftError = FT_Set_Char_Size(mFTFace, fontSize << 9, 0, 72, 0);
	if (ftError != 0) { // if an error occured...
		FT_Done_Face(mFTFace); // clean up font face object
		mFTFace = nullptr; // set pointer to null
		std::cout << "error setting size: " << ftError << std::endl; // output error message
		return false; // return failure
	}
	
	mLineHeight = mFTFace->size->metrics.height >> 9;
	
	return true;
}

void SDFFont::CreateGlyph(const char16_t& charCode, std::vector< std::pair<char16_t, Glyph> >& glyphs,
		std::vector<Texture>& glyphTextures) {
	
	if (!mFTFace) {
		throw std::runtime_error("font: invalid face object");
	}
	
	FT_Load_Char(mFTFace, charCode, FT_LOAD_NO_BITMAP); // load the glyph into the face
	FT_Outline outline = mFTFace->glyph->outline; // retrieve the outline for the glyph
	if (mFTFace->glyph->format != FT_GLYPH_FORMAT_OUTLINE) { // if the outline is not valid...
		std::cout << "glyph is not an outline" << std::endl;
		
		throw std::runtime_error("font: wrong glyph type");
	}
	
	//
		Polygon glyphPoly; // final polygon representing whole glyph (one outer and zero to n inner boundaries)
		int start, end = 0; // start and end indices of current contour
		
		for (int i = 0; i < outline.n_contours; ++i) { // for all contours in glyph...
			Contour contour;
			end = outline.contours[i] + 1; // update end index
			std::vector<glm::vec2> bezier; // current bezier curve
			int previousPoint = -1; // tag representing previous point
			
			for (int j = start; j < end; ++j) { // for all indices in current contour...
				glm::vec2 pointCurr(outline.points[j].x / 64.0f, outline.points[j].y / 64.0f); // the current point converted from 26.6 format
				
				switch (outline.tags[j] & 3) { // check the tag type...
					case FT_CURVE_TAG_ON : // if current point is an on point...
						if (previousPoint == FT_CURVE_TAG_ON || previousPoint == -1) { // if previous point was also an on point or this is the first point...
							contour.AddPoint(pointCurr); // add the point to the contour
						}
						else { // otherwise previous point was a curve point...
							bezier.push_back(pointCurr); // add point as final bezier point
							contour.AddBezier(bezier); // add the finished bezier to the contour
							bezier.clear(); // reset the current bezier
						}
						
						break;
					case FT_CURVE_TAG_CONIC : // if current point is a curve point...
						if (previousPoint == FT_CURVE_TAG_ON) { // if previous point was an on point...
							bezier.push_back(pointCurr); // add this point to the current bezier
						}
						else if (previousPoint == FT_CURVE_TAG_CONIC || previousPoint == FT_CURVE_TAG_CUBIC) { // otherwise if previous point was a curve point...
							glm::vec2 pointPrev(outline.points[j - 1].x / 64.0f, outline.points[j - 1].y / 64.0f); // convert previous point from 26.6 format
							glm::vec2 midPoint((pointCurr.x + pointPrev.x) / 2, (pointCurr.y + pointPrev.y) / 2); // calculate the mid point of both curve points
							
							bezier.push_back(midPoint); // add the midpoint to the current bezier
							contour.AddBezier(bezier); // add the bezier to the contour
							
							bezier.clear(); // clear the current bezier
							bezier.push_back(pointCurr); // start a new bezier with this point
						}
						else { // otherwise this is the first point.
							glm::vec2 pointEnd(outline.points[end - 1].x / 64.0f, outline.points[end - 1].y / 64.0f); // convert the final point from 26.6 format
							int finalPoint = outline.tags[end - 1] & 3; // get the tags for the final point
							
							if (finalPoint == FT_CURVE_TAG_ON) { // if the final point is an on point...
								contour.AddPoint(pointEnd); // add the final point to the contour
								bezier.push_back(pointCurr); // add the current point to the bezier
							}
							else if (finalPoint == FT_CURVE_TAG_CONIC || finalPoint == FT_CURVE_TAG_CUBIC ) { // otherwise final point is a curve point...
								glm::vec2 midPoint((pointCurr.x + pointEnd.x) / 2, (pointCurr.y + pointEnd.y) / 2); // calculate the mid point of both curve points
								
								contour.AddPoint(midPoint); // add the midpoint to the contour
								bezier.push_back(pointCurr); // add the current point the bezier
							}
						}
						
						if (j == end - 1) { // if this is the final point...
							glm::vec2 pointStart(outline.points[start].x / 64.0f, outline.points[start].y / 64.0f); // convert the first point from 26.6 format
							int initialPoint = outline.tags[start] & 3; // get the tags for the first point
							
							if (initialPoint == FT_CURVE_TAG_ON) { // if the first point is an on point...
								bezier.push_back(pointStart); // add the first point to the bezier
								contour.AddBezier(bezier); // add the bezier to the contour
							}
							else if (initialPoint == FT_CURVE_TAG_CONIC || initialPoint == FT_CURVE_TAG_CUBIC ) { // otherwise first point is a curve point...
								glm::vec2 midPoint((pointCurr.x + pointStart.x) / 2, (pointCurr.y + pointStart.y) / 2); // calculate the mid point of both curve points
								
								bezier.push_back(midPoint); // add the mid point to the bezier
								contour.AddBezier(bezier); // add the bezier to the contour
							}
						}
						
						break;
					case FT_CURVE_TAG_CUBIC : // if current point is a cubic curver point...
						// [!] handle the rare case cubic
						
						break;
				}
				
				previousPoint = outline.tags[j] & 3; // update the previous point
			}
			
			glyphPoly.AddContour(contour, CoordinateSpace::Global); // add the contour to the polygon
			start = end; // update start index
		}
		
		Shape glyphOutline;
		glyphOutline.AddContours(glyphPoly.GetContours()); // convert the polygon to a shape
		glyphOutline.SetColour(glm::vec3(0.0f, 0.0f, 0.0f)); // set the colour of the shape to black
	//
	
	//
		std::vector<glm::vec2> bbox = glyphOutline.GetLocalBoundingBox(); 
		if (bbox.size() != 4) {
			bbox = {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
		}
		
		unsigned int padding = 20u; // the amount of padding around a glyph
		glyphOutline.SetScale(glm::vec2(1.0f, -1.0f)); // mirror the glyph over the x-axis
		glyphOutline.SetOrigin(glm::vec2(bbox.at(3).x - padding, bbox.at(3).y + padding)); // set the origin so that the glyph is at the top-left (plus any padding)
		
		unsigned int doublePadding = padding * 2u; // double padding value
		glm::ivec2 dimensions = bbox.at(2) - bbox.at(0); // create a rectangle with it
		unsigned int width = util::NextPowerOf2(dimensions.x + doublePadding); // calculate the width of the base glyph including padding, rounded to nearest power of two...
		unsigned int height = util::NextPowerOf2(dimensions.y + doublePadding); // ...and the same for height
		
		// create a blank texture of dimensions (width, height)
		Texture texture;
		texture.AddFromMemory({}, width, height);
		texture.CreateTexture();
		
		// create a blank render buffer of dimensions (width, height)
		RenderBuffer renderBuffer;
		renderBuffer.CreateRenderBuffer(GL_DEPTH24_STENCIL8, width, height);
		
		// create an fbo and attach the texture and render buffer
		FBO fbo;
		OpenGLStates::BindFBO(fbo.GetFBOID());
		fbo.AttachTexture(&texture, GL_COLOR_ATTACHMENT0, 0);
		fbo.AttachRenderBuffer(&renderBuffer, GL_DEPTH_STENCIL_ATTACHMENT);
		fbo.MapBuffers({GL_COLOR_ATTACHMENT0});
		
		// bind the fbo and set up the viewport and projection
		OpenGLStates::BindFBO(fbo.GetFBOID());
		glViewport(0, 0, width, height);
		OpenGLStates::mProjectionMatrix = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 0.0f, -9999.0f);
		
		// clear fbo texture to white and render the glyph
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		RenderBatch batch;
		batch.Add(glyphOutline);
		batch.Upload();
		batch.Draw(fbo, 0u);
		
		OpenGLStates::BindFBO(0);
		
		// create a signed distance field of our glyph texture and store it
		SignedDistanceField sdfGen;
		std::pair<std::vector<unsigned char>, glm::ivec2> sdfTexData = sdfGen.GenerateFromTexture(&texture, 0u);
		glyphTextures.emplace_back(); // add a new texture to the array
		glyphTextures.back().AddFromMemory(sdfTexData.first, sdfTexData.second.x, sdfTexData.second.y, false);
		glyphTextures.back().CreateTexture();
		
		//
			// rescale the dimensions to match the distance field
			glm::vec2 scaledDimensions = glm::vec2((dimensions.x + doublePadding) / 8u, (dimensions.y + doublePadding) / 8u);
			
			// add a contour matching the texture's dimensions
			Shape glyphShape; // a textured shape of the distance field of the glyph
			glyphShape.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(scaledDimensions.x, 0.0f),
					glm::vec2(scaledDimensions.x, scaledDimensions.y), glm::vec2(0.0f, scaledDimensions.y)})); 
			glyphShape.AddFrameRect(&glyphTextures.back(), 0u, {glm::vec2(0.0f, 0.0f), glm::vec2(scaledDimensions.x, scaledDimensions.y)}); // texture the new rectangle shape
			
			Glyph glyph;
			
			// transform metrics from 26.6 format ("<< 6" or "/ 64") and scale down to correct size ("<< 3" or "/ 8")
			glyph.mAdvance = mFTFace->glyph->metrics.horiAdvance >> 9;
			glyph.mDrop = (mFTFace->glyph->metrics.height >> 9) - (mFTFace->glyph->metrics.horiBearingY >> 9);
			glyph.mBearing = glm::ivec2(mFTFace->glyph->metrics.horiBearingX >> 9, mFTFace->glyph->metrics.horiBearingY >> 9);
			
			mAdvanceMax = std::max(mAdvanceMax, glyph.mAdvance);
			mDropMax = std::max(mDropMax, glyph.mDrop);
			mBearingMax = glm::vec2(std::max(mBearingMax.x, glyph.mBearing.x), std::max(mBearingMax.y, glyph.mBearing.y));
			glyph.mBaseShape = std::move(glyphShape);
			
			glyphs.push_back(std::make_pair(charCode, glyph));
		//
	//
}
}
