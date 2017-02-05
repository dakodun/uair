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

#include "fontbase.hpp"

#include <iostream>
#include <map>
#include <algorithm>

#include "game.hpp"
#include "openglstates.hpp"
#include "polygon.hpp"
#include "renderbatch.hpp"
#include "pbo.hpp"
#include "file.hpp"
#include "util.hpp"
#include "openglstates.hpp"

namespace uair {
namespace charset {
const std::vector<char16_t> BASICLATIN = {32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
		58, 59, 60, 61, 62, 63, 64,
		65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
		91, 92, 93, 94, 95, 96,
		97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
		123, 124, 125, 126};
}

FT_Library* FontBase::mFTLibraryPtr = nullptr;

FontBase::FontBase(const unsigned int& textureSize, const glm::uvec2& texColour) : mTextureColour(texColour) {
	mTextureSize = util::NextPowerOf2(textureSize); // ensure the texture width and height is a power of 2
	
	std::vector<unsigned char> textureData(mTextureSize * mTextureSize * 4, mTextureColour.x); // create a new blank texture layer
	for (auto iter = textureData.begin(); iter != textureData.end(); iter += 4) { // for all pixels in the new layer...
		*(iter + 3) = mTextureColour.y; // set the alpha to full
	}
	
	mTexture.AddFromMemory(textureData, mTextureSize, mTextureSize); // add the new layer to the font's texture
	mTexture.CreateTexture();
	
	mRenderBuffer.CreateRenderBuffer(GL_DEPTH24_STENCIL8, mTextureSize, mTextureSize); // create the associated render buffer
	
	// bind the fbo, attach the render buffer and map the draw buffers
	OpenGLStates::BindFBO(mFBO.GetFBOID());
	mFBO.AttachRenderBuffer(&mRenderBuffer, GL_DEPTH_STENCIL_ATTACHMENT);
	mFBO.MapBuffers({GL_COLOR_ATTACHMENT0});
	
	// set up the packing area array
	mRectangles.emplace_back();
	mRectangles.back().emplace_back(std::make_pair(glm::ivec2(0, 0), glm::ivec2(mTextureSize, mTextureSize)));
}

FontBase::FontBase(FontBase&& other) : FontBase(0u, glm::uvec2(0u, 0u)) {
	swap(*this, other);
}

FontBase::~FontBase() {
	if (mFTFace) {
		FT_Done_Face(mFTFace);
	}
}

void swap(FontBase& first, FontBase& second) {
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

void FontBase::CreateCache(const std::string& cacheFilename) {
	File cacheFile;
	
	cacheFile.mBuffer.push_back(mCacheTag); // add the cache file tag
	
	{ // add the font dimensions
		std::string outString;
		outString = util::ToString(mFontSize) + " " + util::ToString(mLineHeight) + " " + util::ToString(mTexture.GetWidth()) + " " +
				util::ToString(mTexture.GetHeight()) + " " + util::ToString(mTexture.GetDepth()) + " " +
				util::ToString(mAdvanceMax) + " " + util::ToString(mDropMax) + " " + util::ToString(mBearingMax.x) + " " +
				util::ToString(mBearingMax.y);
		cacheFile.mBuffer.push_back(outString);
	}
	
	{ // add the font's texture data
		std::string outString;
		std::vector<unsigned char> textureData((mTexture.GetWidth() * mTexture.GetHeight() * mTexture.GetDepth() * 4));
		OpenGLStates::BindTexture(mTexture.GetTextureID());
		glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, &textureData[0]);
		
		for (auto data = textureData.begin(); data != textureData.end(); data += 4) {
			outString += util::ToString(static_cast<int>(*data)) + " ";
			outString += util::ToString(static_cast<int>(*(data + 3))) + " ";
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
				strStream << std::hex << kerningData->first.first << " " << kerningData->first.second << " " << std::dec << kerningData->second;
				outString += strStream.str() + " ";
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

bool FontBase::LoadFromCache(const std::string& cacheFilename) {
	if (mFTFace) { // if we already have a font face...
		FT_Done_Face(mFTFace); // clean it up
		mFTFace = nullptr; // reset the pointer
	}
	
	File cacheFile;
	unsigned int fontSize, width, height, depth;
	int lineHeight, advanceMax, dropMax;
	glm::ivec2 bearingMax;
	std::vector<unsigned char> textureData;
	std::vector< std::pair<char16_t, Glyph> > glyphPairs;
	std::map<std::pair<char16_t, char16_t>, int> kerningData;
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
			if (tag != mCacheTag) {
				throw std::runtime_error("invalid cache file: invalid tag " + tag);
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
			
			advanceMax = util::FromString<int>(parts.at(5));
			dropMax    = util::FromString<int>(parts.at(6));
			bearingMax = glm::ivec2(util::FromString<int>(parts.at(7)), util::FromString<int>(parts.at(8)));
		}
		
		{ // load the font's texture data
			std::string dataStr = cacheFile.mBuffer.at(2);
			std::vector<std::string> parts = util::SplitString(dataStr, ' ');
			textureData.reserve(width * height * depth * 4u);
			
			for (unsigned int i = 0u; i < parts.size(); i += 2) {
				int value = util::FromString<int>(parts.at(i));
				unsigned char rgbValue = static_cast<unsigned char>(value);
				textureData.push_back(rgbValue); textureData.push_back(rgbValue);
				textureData.push_back(rgbValue);
				
				textureData.push_back(static_cast<unsigned char>(util::FromString<int>(parts.at(i + 1))));
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
				char16_t charCode = static_cast<char16_t>(charCodeIn);
				
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
					std::pair<char16_t, char16_t> charPair;
					
					std::istringstream iStrStreamFirst(parts.at(i));
					int32_t charInFirst;
					iStrStreamFirst >> std::hex >> charInFirst;
					charPair.first = static_cast<char16_t>(charInFirst);
					
					std::istringstream iStrStreamSecond(parts.at(i + 1));
					int32_t charInSecond;
					iStrStreamSecond >> std::hex >> charInSecond;
					charPair.second = static_cast<char16_t>(charInSecond);
					
					int kerning = util::FromString<int>(parts.at(i + 2));
					
					kerningData.emplace(std::move(charPair), std::move(kerning));
				}
			}
		}
		
		{ // load the packing data
			std::string dataStr = cacheFile.mBuffer.at(5);
			std::vector<std::string> parts = util::SplitString(dataStr, ' ');
			
			for (unsigned int i = 0u; i < parts.size();) {
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
	
	mAdvanceMax = advanceMax; // set the longest horizontal advance of all glyphs
	mDropMax    = dropMax; // set the biggest drop of all glyphs
	mBearingMax = bearingMax; // set the widest and highest bearings of all glyphs
	
	return true;
}

bool FontBase::LoadFromFile(const std::string& filename, const unsigned int& pointSize) {
	if (mFTLibraryPtr == nullptr) {
		std::cout << "pointer to freetype library is invalid" << std::endl;
		return false;
	}
	
	if (mFTFace) { // if we already have a font face...
		FT_Done_Face(mFTFace); // clean it up
		mFTFace = nullptr; // reset pointer
	}
	
	FT_Error ftError = FT_New_Face(*mFTLibraryPtr, filename.c_str(), 0, &mFTFace); // get pointer to face object within the font file
	if (ftError != 0) {
		mFTFace = nullptr;
		std::cout << "error loading face: " << ftError << std::endl;
		return false;
	}
	
	mFontSize = std::max(1u, pointSize); // ensure font size is at least 1
	mLineHeight = mFTFace->size->metrics.height >> 9;
	
	return SetSize(mFontSize);
}

void FontBase::LoadGlyph(const char16_t& charCode) {
	LoadGlyphs({charCode});
}

void FontBase::LoadGlyphs(const std::vector<char16_t>& charCodes) {
	// hold new glyphs and associated textures until they can be added into the main store
	// size to the maximum potential size to prevent reallocation and thus pointer invalidation
	std::vector< std::pair<char16_t, Glyph> > glyphs; glyphs.reserve(charCodes.size());
	std::vector<Texture> glyphTextures; glyphTextures.reserve(charCodes.size());
	
	for (auto charCode = charCodes.begin(); charCode != charCodes.end(); ++charCode) { // for all character codes to be added...
		auto result = mGlyphs.find(*charCode); // attempt to retrieve the matching glyph
		if (result == mGlyphs.end()) { // if the glyph doesn't already exist...
			try {
				CreateGlyph(*charCode, glyphs, glyphTextures); // create the glyph and texture
			} catch (std::exception& e) {
				std::cout << e.what() << ": skipping " << std::endl;
			}
		}
	}
	
	if (!glyphs.empty()) { // if at least on new glyph was added...
		UpdateTexture(glyphs); // pack the glyph texture into the main texture and update the glyph
		UpdateKerning(glyphs); // update the kerning between new glyphs and existing glyphs
	}
}

FontBase::Glyph FontBase::GetGlyph(const char16_t& codePoint) {
	auto result = mGlyphs.find(codePoint); // search the map for the character code
	
	if (result != mGlyphs.end()) { // if the character code exists in the map...
		return result->second; // return the associated glyph object
	}
	
	throw std::runtime_error("font: code point not found");
}

int FontBase::GetKerning(const char16_t& firstCodePoint, const char16_t& secondCodePoint) {
	auto result = mKernMap.find(std::make_pair(firstCodePoint, secondCodePoint)); // search the map for the kerning pair
	if (result != mKernMap.end()) { // if the kerning pair exists in the map...
		return result->second; // return the kerning value
	}
	
	return 0; // return kerning value of 0 (no kerning)
}

unsigned int FontBase::GetFontSize() const {
	return mFontSize;
}

int FontBase::GetLineHeight() const {
	return mLineHeight;
}

unsigned int FontBase::GetTextureID() const {
	return mTexture.GetTextureID(); // return the assigned id of the font's texture
}

unsigned int FontBase::GetTextureWidth() const {
	return mTexture.GetWidth();
}

unsigned int FontBase::GetTextureHeight() const {
	return mTexture.GetHeight();
}

int FontBase::GetAdvanceMax() const {
	return mAdvanceMax;
}

int FontBase::GetDropMax() const {
	return mDropMax;
}

glm::ivec2 FontBase::GetBearingMax() const {
	return mBearingMax;
}

void FontBase::UpdateTexture(const std::vector< std::pair<char16_t, Glyph> >& glyphs) {
	std::priority_queue<RectangleIndexPair, std::vector<RectangleIndexPair>, decltype(&SortGlyph)> pQueue(SortGlyph);
	
	for (unsigned int i = 0; i < glyphs.size(); ++i) { // for all glyphs...
		// get the bounding box of the glyph
		std::vector<glm::vec2> bbox = glyphs.at(i).second.mBaseShape.GetLocalBoundingBox(); 
		
		if (bbox.size() != 4) {
			bbox = {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
		}
		
		Rectangle rect = Rectangle(bbox.at(0), bbox.at(2)); // create a rectangle with it
		pQueue.emplace(std::make_pair(rect, i)); // add it to the priority queue
	}
	
	std::map<unsigned int, std::vector<Shape>> glyphShapes;
	
	while (!pQueue.empty()) { // whilst there are still glyphs to process...
		auto storedGlyph = mGlyphs.insert(std::make_pair(glyphs.at(pQueue.top().second).first,
				glyphs.at(pQueue.top().second).second));
		Glyph& glyph = storedGlyph.first->second;
		
		Shape packedShape = PackGlyph(glyph); // pack the glyph into the texture
		
		// add a new layer or retrieve the exisiting one and then add a pointer to the shape to it
		auto layer = glyphShapes.insert(std::make_pair(glyph.mLayer, std::initializer_list<Shape>{}));
		layer.first->second.push_back(packedShape);
		
		// create a new shape using the font's texture atlas and swap it with the glyph objects shape
		Shape modifiedBase;
		modifiedBase.AddContour(Contour({glm::vec2(0.0f, 0.0f), glm::vec2(glyph.mDimensions.x, 0.0f),
				glm::vec2(glyph.mDimensions.x, glyph.mDimensions.y), glm::vec2(0.0f, glyph.mDimensions.y)}));
		modifiedBase.AddFrameCoords(&mTexture, glyph.mLayer, glyph.mTexCoords);
		glyph.mBaseShape = std::move(modifiedBase);
		
		pQueue.pop();
	}
	
	//
		RenderBatch batch;
		
		// disable depth testing if it is enabled
		GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
		if (depthTest == GL_TRUE) {
			glDisable(GL_DEPTH_TEST);
		}
		
		for (auto layer = glyphShapes.begin(); layer != glyphShapes.end(); ++layer) {
			for (auto shape = layer->second.begin(); shape != layer->second.end(); ++shape) {
				batch.Add(*shape); // add shape to the batch
			}
			
			batch.Upload(); // upload the glyph rectangles
			
			OpenGLStates::BindFBO(mFBO.GetFBOID()); // bind the font's fbo
			mFBO.AttachTexture(&mTexture, GL_COLOR_ATTACHMENT0, layer->first);
			glViewport(0, 0, mTextureSize, mTextureSize); // set the viewport to match the texture dimensions...
			OpenGLStates::mProjectionMatrix = glm::ortho(0.0f, static_cast<float>(mTextureSize), static_cast<float>(mTextureSize), 0.0f, 0.0f, -9999.0f); // ...as well as the projection matrix
			batch.Draw(mFBO, 0u); // draw the glyphs to the texture
		}
		
		// re-enable depth testing if required
		if (depthTest == GL_TRUE) {
			glEnable(GL_DEPTH_TEST);
		}
		
		OpenGLStates::BindFBO(0); // ensure fbo is no longer bound
	//
}

void FontBase::UpdateKerning(const std::vector< std::pair<char16_t, Glyph> >& glyphs) {
	if (FT_HAS_KERNING(mFTFace)) { // if the font has kerning data...
		for (auto newGlyph = glyphs.begin(); newGlyph != glyphs.end(); ++newGlyph) { // for all newly added glyphs...
			for (auto oldGlyph = mGlyphs.begin(); oldGlyph != mGlyphs.end(); ++oldGlyph) { // for all existing glyphs...
				char16_t newChar = newGlyph->first; // the charcode of the newly added glyph
				char16_t oldChar = oldGlyph->first; // the charcode of the existing glyph
				FT_Vector kerning;
				
				// get kerning data for character pair
				FT_Get_Kerning(mFTFace, FT_Get_Char_Index(mFTFace, newChar),
						FT_Get_Char_Index(mFTFace, oldChar), FT_KERNING_DEFAULT, &kerning);
				kerning.x = kerning.x >> 9; // transform from 26.6 format and scale down by 8
				if (kerning.x != 0) { // if kerning exists for character pair (prevents unneccesary data in map and subsequently cache)...
					mKernMap.emplace(std::make_pair(newChar, oldChar), kerning.x); // add it to the kerning map
				}
				
				if (newChar != oldChar) { // if the characters differ...
					// repeat process for other order (i.e, AV and VA)
					FT_Get_Kerning(mFTFace, FT_Get_Char_Index(mFTFace, oldChar),
							FT_Get_Char_Index(mFTFace, newChar), FT_KERNING_DEFAULT, &kerning);
					kerning.x = kerning.x >> 9;
					if (kerning.x != 0) {
						mKernMap.emplace(std::make_pair(oldChar, newChar), kerning.x);
					}
				}
			}
		}
	}
}

bool FontBase::SortGlyph(const RectangleIndexPair& first, const RectangleIndexPair& second) {
	Rectangle firstRect = first.first; // get reference to first rectangle
	Rectangle secondRect = second.first; // get reference to second rectangle
	
	// return true if first glyph is smaller than the second
	return ((firstRect.second.x - firstRect.first.x) < (secondRect.second.x - secondRect.first.x));
}

Shape FontBase::PackGlyph(Glyph& glyph) {
	std::vector<glm::vec2> bbox = glyph.mBaseShape.GetGlobalBoundingBox(); // get the transformed bounding box of the glyph
	if (bbox.size() != 4) { // if the bounding box is not valid...
		// set bounding box to a default valid value
		bbox = {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
	}
	
	int glyphWidth = util::FloatToInt(bbox.at(2).x - bbox.at(0).x); // calculate the width of the glyph
	int glyphHeight = util::FloatToInt(bbox.at(2).y - bbox.at(0).y); // calculate the height of the glyph
	size_t layerCount = 0; // what layer of the texture we're currently on
	
	for (auto layer = mRectangles.begin(); layer != mRectangles.end(); ++layer) { // for all texture layers...
		for (auto rectangle = layer->begin(); rectangle != layer->end(); ++rectangle) { // for all areas in the current layer...
			int rectWidth = rectangle->second.x - rectangle->first.x; // calculate the width of the layer
			int rectHeight = rectangle->second.y - rectangle->first.y; // calculate the height of the layer
			
			if (glyphWidth <= rectWidth && glyphHeight <= rectHeight) { // if the glyph fits in the area...
				if (glyphWidth != rectWidth) { // if there is any leftover horizontal space...
					// add a new rectangle to the store
					layer->insert(rectangle, std::make_pair(glm::ivec2(rectangle->first.x + glyphWidth, rectangle->first.y), glm::ivec2(rectangle->second.x, rectangle->first.y + glyphHeight)));
				}
				
				if (glyphHeight != rectHeight) { // if there is any leftover vertical space...
					layer->insert(rectangle, std::make_pair(glm::ivec2(rectangle->first.x, rectangle->first.y + glyphHeight), glm::ivec2(rectangle->second.x, rectangle->second.y)));
				}
				
				// calculate the new texture coordinates that represent the glyph in the font's texture atlas
				float texSize = static_cast<float>(mTextureSize);
				std::pair<glm::vec2, glm::vec2> texCoords = std::make_pair(glm::vec2(rectangle->first.x / texSize, rectangle->first.y / texSize),
						glm::vec2((rectangle->first.x + glyphWidth) / texSize, (rectangle->first.y + glyphHeight) / texSize));
				
				// create a shape representing the glyph in the font's texture atlas
				Shape shape = glyph.mBaseShape;
				shape.SetPosition(rectangle->first);
				
				// update the glyph objects attributes to point to the font's texture atlas instead of its individual texture
				glyph.mDimensions = glm::ivec2(glyphWidth, glyphHeight);
				glyph.mTexCoords = {texCoords.first, glm::vec2(texCoords.second.x, texCoords.first.y), texCoords.second, glm::vec2(texCoords.first.x, texCoords.second.y)};
				glyph.mLayer = layerCount;
				
				layer->erase(rectangle); // remove the current (now occupied) area
				
				return shape;
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
		
		std::vector<unsigned char> newLayerData(mTexture.GetWidth() * mTexture.GetHeight() * 4, mTextureColour.x); // create a new blank texture layer
		for (auto iter = newLayerData.begin(); iter != newLayerData.end(); iter += 4) { // for all pixels in the new layer...
			*(iter + 3) = mTextureColour.y; // set the alpha to full
		}
		std::vector<unsigned char> textureData((mTexture.GetWidth() * mTexture.GetHeight() * mTexture.GetDepth() * 4));
		glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, &textureData[0]); // get all current texture data 
		
		mTexture.AddFromMemory(textureData, mTexture.GetWidth(), mTexture.GetHeight(), false); // add old layers to texture
		mTexture.AddFromMemory(newLayerData, mTexture.GetWidth(), mTexture.GetHeight()); // add new layer to texture
		mTexture.CreateTexture();
		
		OpenGLStates::BindTexture(0);
		
		mRectangles.emplace_back();
		
		// as above except for the new texture layer
		if (glyphWidth != rectWidth) {
			mRectangles.back().emplace_back(std::make_pair(glm::ivec2(newRect.first.x + glyphWidth, newRect.first.y), glm::ivec2(newRect.second.x, newRect.first.y + glyphHeight)));
		}
		
		if (glyphHeight != rectHeight) {
			mRectangles.back().emplace_back(std::make_pair(glm::ivec2(newRect.first.x, newRect.first.y + glyphHeight), glm::ivec2(newRect.second.x, newRect.second.y)));
		}
		
		float texSize = static_cast<float>(mTextureSize);
		std::pair<glm::vec2, glm::vec2> texCoords = std::make_pair(glm::vec2(newRect.first.x / texSize, newRect.first.y / texSize),
				glm::vec2((newRect.first.x + glyphWidth) / texSize, (newRect.first.y + glyphHeight) / texSize));
		
		Shape shape = glyph.mBaseShape;
		shape.SetPosition(newRect.first);
		
		glyph.mDimensions = glm::ivec2(glyphWidth, glyphHeight);
		glyph.mTexCoords = {texCoords.first, glm::vec2(texCoords.second.x, texCoords.first.y), texCoords.second, glm::vec2(texCoords.first.x, texCoords.second.y)};
		glyph.mLayer = layerCount;
		
		return shape;
	}
	
	throw std::runtime_error("unable to pack glyph");
}
}
