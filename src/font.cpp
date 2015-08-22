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

#include "font.hpp"

#include <iostream>
#include <map>
#include <algorithm>

#include "game.hpp"
#include "openglstates.hpp"
#include "polygon.hpp"
#include "renderbatch.hpp"
#include "exception.hpp"
#include "pbo.hpp"
#include "signeddistancefield.hpp"
#include "file.hpp"

namespace uair {
Font::Font(const unsigned int& textureSize) {
	mTextureSize = util::NextPowerOf2(textureSize); // ensure the texture width and height is a power of 2
	
	std::vector<unsigned char> textureData(mTextureSize * mTextureSize * 4, 0u); // create a new blank texture layer
	for (auto iter = textureData.begin(); iter != textureData.end(); iter += 4) { // for all pixels in the new layer...
		*(iter + 3) = 255u; // set the alpha to full
	}
	
	mTexture.AddFromMemory(textureData, mTextureSize, mTextureSize); // add the new layer to the font's texture
	mTexture.CreateTexture();
	
	mRenderBuffer.CreateRenderBuffer(GL_DEPTH24_STENCIL8, mTextureSize, mTextureSize); // create the associated render buffer
	
	// bind teh fbo, attach the render buffer and map the draw buffers
	OpenGLStates::BindFBO(mFBO.GetFBOID());
	mFBO.AttachRenderBuffer(&mRenderBuffer, GL_DEPTH_STENCIL_ATTACHMENT);
	mFBO.MapBuffers({GL_COLOR_ATTACHMENT0});
	
	// set up the packing area array
	mRectangles.emplace_back();
	mRectangles.back().emplace_back(std::make_pair(glm::ivec2(0, 0), glm::ivec2(mTextureSize, mTextureSize)));
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
	std::swap(first.mFontSize, second.mFontSize);
	std::swap(first.mLineHeight, second.mLineHeight);
	
	std::swap(first.mGlyphs, second.mGlyphs);
	std::swap(first.mKernMap, second.mKernMap);
	
	std::swap(first.mTextureSize, second.mTextureSize);
	std::swap(first.mFBO, second.mFBO);
	std::swap(first.mTexture, second.mTexture);
	std::swap(first.mRenderBuffer, second.mRenderBuffer);
	
	std::swap(first.mRectangles, second.mRectangles);
	
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
}

void Font::CreateCache(const std::string& cacheFilename) {
	File cacheFile;
	
	cacheFile.mBuffer.push_back("UaFCa"); // add the cache file tag
	
	{ // add the font dimensions
		std::string outString;
		outString = util::ToString(mFontSize) + " " + util::ToString(mLineHeight) + " " + util::ToString(mTexture.GetWidth()) + " " +
				util::ToString(mTexture.GetHeight()) + " " + util::ToString(mTexture.GetDepth());
		cacheFile.mBuffer.push_back(outString);
	}
	
	{ // add the font's texture data
		std::string outString;
		std::vector<unsigned char> textureData((mTexture.GetWidth() * mTexture.GetHeight() * mTexture.GetDepth() * 4));
		OpenGLStates::BindTexture(mTexture.GetTextureID());
		glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, &textureData[0]);
		
		for (auto data = textureData.begin(); data != textureData.end(); data += 4) {
			outString += util::ToString(static_cast<int>(*data)) + " ";
		}
		
		cacheFile.mBuffer.push_back(outString);
	}
	
	{ // add the glyph object data
		std::string outString;
		
		for (auto glyph = mGlyphs.begin(); glyph != mGlyphs.end(); ++glyph) {
			std::stringstream strStream;
			strStream << std::hex << glyph->first;
			outString += strStream.str() + " ";
			
			outString += util::ToString(glyph->second.mDimensions.x) + " " + util::ToString(glyph->second.mDimensions.y) + " ";
			
			outString += util::ToString(glyph->second.mTexCoords.at(0).x) + " " + util::ToString(glyph->second.mTexCoords.at(0).y) + " ";
			outString += util::ToString(glyph->second.mTexCoords.at(1).x) + " " + util::ToString(glyph->second.mTexCoords.at(1).y) + " ";
			outString += util::ToString(glyph->second.mTexCoords.at(2).x) + " " + util::ToString(glyph->second.mTexCoords.at(2).y) + " ";
			outString += util::ToString(glyph->second.mTexCoords.at(3).x) + " " + util::ToString(glyph->second.mTexCoords.at(3).y) + " ";
			outString += util::ToString(glyph->second.mLayer) + " ";
			
			outString += util::ToString(glyph->second.mAdvance) + " ";
			outString += util::ToString(glyph->second.mDrop) + " ";
			outString += util::ToString(glyph->second.mBearing.x) + " " + util::ToString(glyph->second.mBearing.y) + " ";
		}
		
		cacheFile.mBuffer.push_back(outString);
	}
	
	{ // add the kerning data
		std::string outString;
		
		if (!mKernMap.empty()) {
			for (auto kerningData = mKernMap.begin(); kerningData != mKernMap.end(); ++kerningData) {
				std::stringstream strStream;
				strStream << std::hex << kerningData->first.first << " " << kerningData->first.second;
				outString += strStream.str() + " ";
				outString += kerningData->second + " ";
			}
		}
		else {
			outString += "0 ";
		}
		
		cacheFile.mBuffer.push_back(outString);
	}
	
	{ // add the packing data
		std::string outString;
		
		for (auto layer = mRectangles.begin(); layer != mRectangles.end(); ++layer) {
			outString += util::ToString(layer->size()) + " ";
			for (auto rectangle = layer->begin(); rectangle != layer->end(); ++rectangle) {
				outString +=  util::ToString(rectangle->first.x) + " " +  util::ToString(rectangle->first.y) + " ";
				outString += util::ToString(rectangle->second.x) + " " + util::ToString(rectangle->second.y) + " ";
			}
		}
		
		cacheFile.mBuffer.push_back(outString);
	}
	
	// [!] encode the file
	
	cacheFile.SaveToFile(cacheFilename + ".ufc"); // save the cache file to disk
}

bool Font::LoadFromCache(const std::string& cacheFilename) {
	if (mFTFace) { // if we already have a font face...
		FT_Done_Face(mFTFace); // clean it up
		mFTFace = nullptr; // reset the pointer
	}
	
	File cacheFile;
	unsigned int fontSize, width, height, depth;
	int lineHeight;
	std::vector<unsigned char> textureData;
	std::vector< std::pair<char32_t, Glyph> > glyphPairs;
	std::map<std::pair<char32_t, char32_t>, int> kerningData;
	std::list< std::list<Rectangle> > packingRects;
	
	try {
		cacheFile.LoadFromFile(cacheFilename + ".ufc");
	} catch (std::exception& e) {
		return false;
	}
	
	// [!] decode the file
	
	try {
		{ // verify the tag is correct
			std::string tag = cacheFile.mBuffer.at(0);
			if (tag != "UaFCa") {
				throw UairException("invalid cache file: invalid tag " + tag);
			}
		}
		
		{ // load the font dimensions
			std::string dimensionsStr = cacheFile.mBuffer.at(1);
			std::vector<std::string> parts = util::SplitString(dimensionsStr, ' ');
			fontSize   = util::FromString<unsigned int>(parts.at(0));
			lineHeight = util::FromString<unsigned int>(parts.at(1));
			width      = util::FromString<unsigned int>(parts.at(2));
			height     = util::FromString<unsigned int>(parts.at(3));
			depth      = util::FromString<unsigned int>(parts.at(4));
		}
		
		{ // load the font's texture data
			std::string dataStr = cacheFile.mBuffer.at(2);
			std::vector<std::string> parts = util::SplitString(dataStr, ' ');
			textureData.reserve(width * height * depth * 4u);
			
			for (unsigned int i = 0u; i < parts.size(); ++i) {
				int value = util::FromString<int>(parts.at(i));
				unsigned char rgbValue = static_cast<unsigned char>(value);
				textureData.push_back(rgbValue); textureData.push_back(rgbValue);
				textureData.push_back(rgbValue); textureData.push_back(255u);
			}
		}
		
		{ // load the glyph object data
			std::string glyphStr = cacheFile.mBuffer.at(3);
			std::vector<std::string> parts = util::SplitString(glyphStr, ' ');
			
			for (unsigned int i = 0u; i < parts.size(); i += 16) {
				Glyph glyph;
				
				std::istringstream iStrStream(parts.at(i));
				int32_t charCodeIn;
				iStrStream >> std::hex >> charCodeIn;
				char32_t charCode = static_cast<char32_t>(charCodeIn);
				
				glyph.mDimensions = glm::ivec2(util::FromString<int>(parts.at(i + 1)), util::FromString<int>(parts.at(i + 2)));
				
				glyph.mTexCoords.emplace_back(util::FromString<float>(parts.at(i +  3)), util::FromString<float>(parts.at(i +  4)));
				glyph.mTexCoords.emplace_back(util::FromString<float>(parts.at(i +  5)), util::FromString<float>(parts.at(i +  6)));
				glyph.mTexCoords.emplace_back(util::FromString<float>(parts.at(i +  7)), util::FromString<float>(parts.at(i +  8)));
				glyph.mTexCoords.emplace_back(util::FromString<float>(parts.at(i +  9)), util::FromString<float>(parts.at(i + 10)));
				glyph.mLayer = util::FromString<unsigned int>(parts.at(i + 11));
				
				glyph.mAdvance = util::FromString<int>(parts.at(i + 12));
				glyph.mDrop    = util::FromString<int>(parts.at(i + 13));
				glyph.mBearing = glm::ivec2(util::FromString<int>(parts.at(i + 14)), util::FromString<int>(parts.at(i + 15)));
				
				glyphPairs.emplace_back(std::move(charCode), std::move(glyph));
			}
		}
		
		{ // load the kerning data
			std::string kerningStr = cacheFile.mBuffer.at(4);
			std::vector<std::string> parts = util::SplitString(kerningStr, ' ');
			
			if (!parts.empty() && parts.at(0) != "0") {
				for (unsigned int i = 0u; i < parts.size(); i += 3) {
					std::pair<char32_t, char32_t> charPair;
					
					std::istringstream iStrStreamFirst(parts.at(i));
					int32_t charInFirst;
					iStrStreamFirst >> std::hex >> charInFirst;
					charPair.first = static_cast<char32_t>(charInFirst);
					
					std::istringstream iStrStreamSecond(parts.at(i + 1));
					int32_t charInSecond;
					iStrStreamSecond >> std::hex >> charInSecond;
					charPair.second = static_cast<char32_t>(charInSecond);
					
					int kerning = util::FromString<int>(parts.at(i + 2));
					
					kerningData.emplace(std::move(charPair), std::move(kerning));
				}
			}
		}
		
		{ // load the packing data
			std::string dataStr = cacheFile.mBuffer.at(5);
			std::vector<std::string> parts = util::SplitString(dataStr, ' ');
			
			for (unsigned int i = 0u; i < parts.size(); ++i) {
				size_t rectCount = util::FromString<size_t>(parts.at(i));
				packingRects.emplace_back();
				++i;
				
				for (unsigned int j = 0u; j < rectCount; ++j, i += 4) {
					packingRects.back().emplace_back(glm::ivec2(util::FromString<int>(parts.at(i)), util::FromString<int>(parts.at(i + 1))),
						glm::ivec2(util::FromString<int>(parts.at(i + 2)), util::FromString<int>(parts.at(i + 3))));
				}
			}
		}
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		return false;
	}
	
	{ // create the texture and set up the fbo and render buffer
		mTextureSize = width;
		
		OpenGLStates::BindTexture(mTexture.GetTextureID());
		
		mTexture.AddFromMemory(textureData, width, height, false);
		mTexture.CreateTexture();
		
		mRenderBuffer.CreateRenderBuffer(GL_DEPTH24_STENCIL8, mTextureSize, mTextureSize);
		
		OpenGLStates::BindFBO(mFBO.GetFBOID());
		mFBO.AttachRenderBuffer(&mRenderBuffer, GL_DEPTH_STENCIL_ATTACHMENT);
		mFBO.MapBuffers({GL_COLOR_ATTACHMENT0});
	}
	
	// create the glyph objects
	for (auto glyphPair = glyphPairs.begin(); glyphPair != glyphPairs.end(); ++glyphPair) {
		auto result = mGlyphs.insert(*glyphPair);
		if (result.second) {
			Glyph& glyphRef = result.first->second;
			
			Shape shape;
			shape.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(glyphRef.mDimensions.x, 0.0f),
					glm::vec2(glyphRef.mDimensions.x, glyphRef.mDimensions.y), glm::vec2(0.0f, glyphRef.mDimensions.y)}));
			shape.AddFrameCoords(&mTexture, glyphRef.mLayer, glyphRef.mTexCoords);
			
			std::swap(glyphRef.mBaseShape, shape);
		}
	}
	
	std::swap(mKernMap, kerningData); // set the kerning data
	std::swap(mRectangles, packingRects); // set the packing data
	mFontSize   = std::max(1u, fontSize); // set the font size (and ensure it's valid)
	mLineHeight = lineHeight; // set the new line vertical offset
	
	return true;
}

bool Font::LoadFromFile(const std::string& filename, const unsigned int& pointSize) {
	if (mFTFace) { // if we already have a font face...
		FT_Done_Face(mFTFace); // clean it up
		mFTFace = nullptr; // reset pointer
	}
	
	mFontSize = std::max(1u, pointSize); // ensure font size is at least 1
	FT_Error ftError = FT_New_Face(GAME.GetFTLibrary(), filename.c_str(), 0, &mFTFace); // get pointer to face object within the font file
	if (ftError != 0) {
		mFTFace = nullptr;
		std::cout << "error loading face: " << ftError << std::endl;
		return false;
	}
	
	// set character size and dpi (convert size into 26.6 fixed format by bit shifting (multiplying) and scale up 8 times for sdf generation
	ftError = FT_Set_Char_Size(mFTFace, mFontSize << 9, 0, 72, 0);
	if (ftError != 0) { // if an error occured...
		FT_Done_Face(mFTFace); // clean up font face object
		mFTFace = nullptr; // set pointer to null
		std::cout << "error setting size: " << ftError << std::endl; // output error message
		return false; // return failure
	}
	
	mLineHeight = mFTFace->size->metrics.height >> 9;
	return true; // return success
}

void Font::LoadGlyph(const char32_t& charCode) {
	LoadGlyphs({charCode});
}

void Font::LoadGlyphs(const std::vector<char32_t>& charCodes) {
	std::vector< std::pair<char32_t, Shape> > glyphs;
	Glyph glyph;
	
	for (auto charCode = charCodes.begin(); charCode != charCodes.end(); ++charCode) { // for all character codes to be added...
		auto result = mGlyphs.insert(std::make_pair(*charCode, glyph)); // add the character code to the map
		if (result.second) { // if the character code didn't already exist...
			try {
				glyphs.push_back(std::make_pair(*charCode, CreateGlyphShape(*charCode, (result.first->second)))); // create and add the glyph shape to the store
			} catch(UairException& e) {
				mGlyphs.erase(*charCode); // remove the newly added character code
			}
		}
	}
	
	if (!glyphs.empty()) { // if we have at least one new glyph...
		UpdateTexture(glyphs); // update the font's texture to add the new glyphs
		UpdateKerningMap(glyphs); // update the kerning table for the new glyphs and old
	}
}

Font::Glyph Font::GetGlyph(const char32_t& codePoint) {
	auto result = mGlyphs.find(codePoint); // search the map for the character code
	
	if (result != mGlyphs.end()) { // if the character code exists in the map...
		return result->second; // return the associated glyph object
	}
	
	throw UairException("font: code point not found");
}

int Font::GetKerning(const char32_t& firstCodePoint, const char32_t& secondCodePoint) {
	auto result = mKernMap.find(std::make_pair(firstCodePoint, secondCodePoint)); // search the map for the kerning pair
	if (result != mKernMap.end()) { // if the kerning pair exists in the map...
		return result->second; // return the kerning value
	}
	
	return 0; // return kerning value of 0 (no kerning)
}

unsigned int Font::GetFontSize() const {
	return mFontSize;
}

int Font::GetLineHeight() const {
	return mLineHeight;
}

unsigned int Font::GetTextureID() const {
	return mTexture.GetTextureID(); // return the assigned id of the font's texture
}

unsigned int Font::GetTextureWidth() const {
	return mTexture.GetWidth();
}

unsigned int Font::GetTextureHeight() const {
	return mTexture.GetHeight();
}

Shape Font::CreateGlyphShape(const char32_t& charCode, Glyph& glyphObject) {
	if (mFTFace) {
		FT_Load_Char(mFTFace, charCode, FT_LOAD_NO_BITMAP); // load the glyph into the face
		FT_Outline outline = mFTFace->glyph->outline; // retrieve the outline for the glyph
		if (mFTFace->glyph->format != FT_GLYPH_FORMAT_OUTLINE) { // if the outline is not valid...
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
			
			Shape shp;
			shp.AddContours(glyphPoly.GetContours()); // convert the polygon to a shape
			
			// transform metrics from 26.6 format ("<< 6" or "/ 64") and scale down to correct size ("<< 3" or "/ 8")
			glyphObject.mAdvance = mFTFace->glyph->metrics.horiAdvance >> 9; 
			glyphObject.mDrop = (mFTFace->glyph->metrics.height >> 9) - (mFTFace->glyph->metrics.horiBearingY >> 9);
			glyphObject.mBearing = glm::ivec2(mFTFace->glyph->metrics.horiBearingX >> 9, mFTFace->glyph->metrics.horiBearingY >> 9);
			
			return shp;
		}
	}
	
	throw UairException("font: invalid face object");
}

void Font::UpdateTexture(const std::vector< std::pair<char32_t, Shape> >& newShapes) {
	RenderBatch batch; // render batch used to create individual glyph textures as well as font texture
	typedef std::pair<Rectangle, unsigned int> RectangleIndexPair; // define a rectangle and index type
	
	// lambda to sort glyphs by their size
	auto lambdaSortGlyphs = [](const RectangleIndexPair& first, const RectangleIndexPair& second)->bool {
		Rectangle firstRect = first.first; // get reference to first rectangle
		Rectangle secondRect = second.first; // get reference to second rectangle
		
		return ((firstRect.second.x - firstRect.first.x) < (secondRect.second.x - secondRect.first.x)); // return true if first glyph is smaller than the second
	};
	
	// create a priority queue sorted by area
	std::priority_queue<RectangleIndexPair, std::vector<RectangleIndexPair>, decltype(lambdaSortGlyphs)> pQueue(lambdaSortGlyphs);
	for (unsigned int i = 0; i < newShapes.size(); ++i) { // for all glyphs...
		Rectangle rect(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f));
		const std::vector<glm::vec2>& bbox = newShapes.at(i).second.GetLocalBoundingBox(); // get the bounding box of the glyph
		
		if (!bbox.empty()) {
			rect = Rectangle(bbox.at(0), bbox.at(2)); // create a rectangle with it
		}
		
		pQueue.emplace(std::make_pair(rect, i)); // add it to the priority queue
	}
	
	std::vector<Texture> textures; // array of individual textures for rendering glyphs
	textures.reserve(pQueue.size()); // reserve enough space for all glyphs to avoid reallocation and invalidation of pointer
	std::map<unsigned int, std::vector<Shape>> sdfShapes; // map of arrays of textured sdf rectangles
	unsigned int padding = 20u; // the amount of padding around a glyph
	unsigned int doublePadding = padding * 2u; // double padding value
	
	while (!pQueue.empty()) { // whilst there are still glyphs to process...
		char32_t glyph = newShapes.at(pQueue.top().second).first; // get a copy of the char code relating to the glyph
		Shape shp = newShapes.at(pQueue.top().second).second; // get a copy of the glyph shape
		shp.SetColour(glm::vec3(0.0f, 0.0f, 0.0f)); // set the colour of the shape to black
		Shape sdfShape; // a textured shape of the distance field of the glyph
		
		PositionBase(shp, padding); // position the shape at the origin (0, 0) with added padding
		batch.Add(shp);
		batch.Upload(); // upload the current glyph
		glm::ivec2 dimensions(pQueue.top().first.second - pQueue.top().first.first); // calculate the dimensions (width and height) of the base glyph
		textures.emplace_back(); // add a new texture to the array
		
		{
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
			batch.Draw(fbo, 0u);
			OpenGLStates::BindFBO(0);
			
			// create a signed distance field of our glyph texture and store it
			uair::SignedDistanceField sdfGen;
			std::pair<std::vector<unsigned char>, glm::ivec2> sdfTexData = sdfGen.GenerateFromTexture(&texture, 0u);
			textures.back().AddFromMemory(sdfTexData.first, sdfTexData.second.x, sdfTexData.second.y, false);
			textures.back().CreateTexture();
		}
		
		{
			// rescale the dimensions to match the distance field
			glm::vec2 scaledDimensions = glm::vec2((dimensions.x + doublePadding) / 8u, (dimensions.y + doublePadding) / 8u);
			
			// add a contour matching the texture's dimensions
			sdfShape.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(scaledDimensions.x, 0.0f),
					glm::vec2(scaledDimensions.x, scaledDimensions.y), glm::vec2(0.0f, scaledDimensions.y)})); 
			sdfShape.AddFrameRect(&textures.back(), 0u, {glm::vec2(0.0f, 0.0f), glm::vec2(scaledDimensions.x, scaledDimensions.y)}); // texture the new rectangle shape
		}
		
		try {
			Glyph glyphDetails = Pack(sdfShape); // pack the new rectangle shape into the font's texture and store the layer
			auto result = sdfShapes.emplace(glyphDetails.mLayer, std::initializer_list<Shape>{}); // add an array to the map for the current layer, or get the exisiting one
			result.first->second.push_back(std::move(sdfShape)); // add the glyph rectangle to the layer
			
			auto glyphIter = mGlyphs.find(glyph); // get the glyph object matching the char code from the store
			
			// create a shape representing the distance field of the glyph
			Shape shape;
			shape.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(glyphDetails.mDimensions.x, 0.0f),
					glm::vec2(glyphDetails.mDimensions.x, glyphDetails.mDimensions.y), glm::vec2(0.0f, glyphDetails.mDimensions.y)}));
			shape.AddFrameCoords(&mTexture, glyphDetails.mLayer, glyphDetails.mTexCoords);
			
			// set the glyph object's data
			std::swap(glyphIter->second.mBaseShape, shape);
			glyphIter->second.mDimensions = glyphDetails.mDimensions;
			glyphIter->second.mTexCoords = glyphDetails.mTexCoords;
			glyphIter->second.mLayer = glyphDetails.mLayer;
		} catch (UairException& e) {
			std::cout << "unable to pack glyph: skipping" << std::endl;
			mGlyphs.erase(glyph); // remove the glyph object from the store
		}
		
		pQueue.pop(); // remove the current glyph shape from the queue
	}
	
	// disable depth testing if it is enabled
	GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
	if (depthTest == GL_TRUE) {
		glDisable(GL_DEPTH_TEST);
	}
	
	for (auto vec = sdfShapes.begin(); vec != sdfShapes.end(); ++vec) {
		unsigned int layer = vec->first;
		for (auto shape =  vec->second.begin(); shape != vec->second.end(); ++shape) {
			batch.Add(*shape); // add shape to the batch
		}
		
		batch.Upload(); // upload the glyph rectangles
		
		OpenGLStates::BindFBO(mFBO.GetFBOID()); // bind the font's fbo
		mFBO.AttachTexture(&mTexture, GL_COLOR_ATTACHMENT0, layer);
		glViewport(0, 0, mTextureSize, mTextureSize); // set the viewport to match the texture dimensions...
		OpenGLStates::mProjectionMatrix = glm::ortho(0.0f, static_cast<float>(mTextureSize), static_cast<float>(mTextureSize), 0.0f, 0.0f, -9999.0f); // ...as well as the projection matrix
		batch.Draw(mFBO, 0u); // draw the glyphs to the texture
	}
	
	// re-enable depth testing if required
	if (depthTest == GL_TRUE) {
		glEnable(GL_DEPTH_TEST);
	}
	
	OpenGLStates::BindFBO(0); // ensure fbo is no longer bound
}

void Font::UpdateKerningMap(const std::vector< std::pair<char32_t, Shape> >& newShapes) {
	if (FT_HAS_KERNING(mFTFace)) { // if the font has kerning data...
		for (auto newGlyph = newShapes.begin(); newGlyph != newShapes.end(); ++newGlyph) { // for all newly added glyphs...
			for (auto oldGlyph = mGlyphs.begin(); oldGlyph != mGlyphs.end(); ++oldGlyph) { // for all existing glyphs...
				char32_t firstChar  = newGlyph->first;
				char32_t secondChar = oldGlyph->first;
				FT_Vector kerning;
				
				// get kerning data for character pair
				FT_Get_Kerning(mFTFace, FT_Get_Char_Index(mFTFace, firstChar),
						FT_Get_Char_Index(mFTFace, secondChar), FT_KERNING_DEFAULT, &kerning);
				kerning.x = kerning.x >> 9; // transform from 26.6 format and scale down by 8
				if (kerning.x != 0) { // if kerning exists for character pair (prevents unneccesary data in map and subsequently cache)...
					mKernMap.emplace(std::make_pair(firstChar, secondChar), kerning.x); // add it to the kerning map
				}
				
				if (firstChar != secondChar) { // if the characters differ...
					// repeat process for other order (i.e, AV and VA)
					FT_Get_Kerning(mFTFace, FT_Get_Char_Index(mFTFace, secondChar),
							FT_Get_Char_Index(mFTFace, firstChar), FT_KERNING_DEFAULT, &kerning);
					kerning.x = kerning.x >> 9;
					if (kerning.x != 0) {
						mKernMap.emplace(std::make_pair(secondChar, firstChar), kerning.x);
					}
				}
			}
		}
	}
}

Font::Glyph Font::Pack(Shape& shape) {
	std::vector<glm::vec2> bbox = shape.GetGlobalBoundingBox(); // get the transformed bounding box of the glyph
	int glyphWidth = util::FloatToInt(bbox.at(2).x - bbox.at(0).x); // calculate the width of the glyph
	int glyphHeight = util::FloatToInt(bbox.at(2).y - bbox.at(0).y); // calculate the height of the glyph
	size_t layerCount = 0; // what layer of teh texture we're currently on
	
	for (auto layer = mRectangles.begin(); layer != mRectangles.end(); ++layer) { // for all texture layers...
		for (auto rectangle = layer->begin(); rectangle != layer->end(); ++rectangle) { // for all areas in the current layer...
			int rectWidth = rectangle->second.x - rectangle->first.x; // calculate the width of the layer
			int rectHeight = rectangle->second.y - rectangle->first.y; // calculate the height of the layer
			
			if (glyphWidth <= rectWidth && glyphHeight <= rectHeight) { // if the glyph fits in the area...
				if (glyphWidth != rectWidth) { // if there is any leftover space...
					// add a new rectangle to the store
					layer->insert(rectangle, std::make_pair(glm::ivec2(rectangle->first.x + glyphWidth, rectangle->first.y), glm::ivec2(rectangle->second.x, rectangle->first.y + glyphHeight)));
				}
				
				if (glyphHeight != rectHeight) {
					layer->insert(rectangle, std::make_pair(glm::ivec2(rectangle->first.x, rectangle->first.y + glyphHeight), glm::ivec2(rectangle->second.x, rectangle->second.y)));
				}
				
				shape.SetPosition(rectangle->first); // set the position of the glyph shape in the texture
				
				// create a new glyph object
				Glyph glyph;
				glyph.mDimensions = glm::ivec2(glyphWidth, glyphHeight);
				float texSize = static_cast<float>(mTextureSize);
				std::pair<glm::vec2, glm::vec2> texCoords = std::make_pair(glm::vec2(rectangle->first.x / texSize, rectangle->first.y / texSize),
						glm::vec2((rectangle->first.x + glyphWidth) / texSize, (rectangle->first.y + glyphHeight) / texSize));
				glyph.mTexCoords = {texCoords.first, glm::vec2(texCoords.second.x, texCoords.first.y), texCoords.second, glm::vec2(texCoords.first.x, texCoords.second.y)};
				glyph.mLayer = layerCount;
				
				layer->erase(rectangle); // remove the current (now occupied) area
				return glyph; // return the new glyph object
			}
		}
		
		++layerCount; // go to the next layer
	}
	
	// we only reach here if glyph shape didn't fit into current layer
	// add a new layer to area store and calculate width and height
	Rectangle newRect = std::make_pair(glm::ivec2(0, 0), glm::ivec2(mTextureSize, mTextureSize));
	int rectWidth = newRect.second.x - newRect.first.x;
	int rectHeight = newRect.second.y - newRect.first.y;
	
	if (glyphWidth <= rectWidth && glyphHeight <= rectHeight) {
		OpenGLStates::BindFBO(0); // unbind any bound FBOs
		OpenGLStates::BindTexture(mTexture.GetTextureID()); // bind the font's texture
		
		std::vector<unsigned char> newLayerData(mTexture.GetWidth() * mTexture.GetHeight() * 4, 0u); // create a new blank texture layer
		for (auto iter = newLayerData.begin(); iter != newLayerData.end(); iter += 4) { // for all pixels in the new layer...
			*(iter + 3) = 255u; // set the alpha to full
		}
		std::vector<unsigned char> textureData((mTexture.GetWidth() * mTexture.GetHeight() * mTexture.GetDepth() * 4));
		glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, &textureData[0]); // get all current texture data 
		
		mTexture.AddFromMemory(textureData, mTexture.GetWidth(), mTexture.GetHeight(), false); // add old layers to texture
		mTexture.AddFromMemory(newLayerData, mTexture.GetWidth(), mTexture.GetHeight()); // add new layer to texture
		mTexture.CreateTexture();
		
		OpenGLStates::BindTexture(0);
		
		mRectangles.emplace_back();
		if (glyphWidth != rectWidth) {
			mRectangles.back().emplace_back(std::make_pair(glm::ivec2(newRect.first.x + glyphWidth, newRect.first.y), glm::ivec2(newRect.second.x, newRect.first.y + glyphHeight)));
		}
		
		if (glyphHeight != rectHeight) {
			mRectangles.back().emplace_back(std::make_pair(glm::ivec2(newRect.first.x, newRect.first.y + glyphHeight), glm::ivec2(newRect.second.x, newRect.second.y)));
		}
		
		shape.SetPosition(newRect.first);
		
		Glyph glyph;
		glyph.mDimensions = glm::ivec2(glyphWidth, glyphHeight);
		float texSize = static_cast<float>(mTextureSize);
		std::pair<glm::vec2, glm::vec2> texCoords = std::make_pair(glm::vec2(newRect.first.x / texSize, newRect.first.y / texSize),
				glm::vec2((newRect.first.x + glyphWidth) / texSize, (newRect.first.y + glyphHeight) / texSize));
		glyph.mTexCoords = {texCoords.first, glm::vec2(texCoords.second.x, texCoords.first.y), texCoords.second, glm::vec2(texCoords.first.x, texCoords.second.y)};
		glyph.mLayer = layerCount;
		
		return glyph;
	}
	
	throw(UairException("unable to pack glyph"));
}

void Font::PositionBase(Shape& baseShape, const unsigned int& padding) {
	baseShape.SetScale(glm::vec2(1.0f, -1.0f)); // mirror the glyph over the x-axis
	
	const std::vector<glm::vec2>& points = baseShape.GetLocalBoundingBox(); // get the local bounding box of the glyph (no transforms)
	if (points.size() > 3) { // if the bounding box is valid...
		baseShape.SetOrigin(glm::vec2(points.at(3).x - padding, points.at(3).y + padding)); // set the origin so that the glyph is at the top-left (plus any padding)
	}
}
}
