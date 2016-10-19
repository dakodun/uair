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

#include "signeddistancefield.hpp"

#include <cmath>
#include <limits>
#include <glm/gtx/fast_square_root.hpp>

#include "openglstates.hpp"

namespace uair {
std::pair<std::vector<unsigned char>, glm::ivec2> SignedDistanceField::GenerateFromFile(const std::string& textureFile) {
	// create and initialise the texture using the image file
	Texture tex;
	tex.AddFromFile(textureFile);
	tex.CreateTexture();
	
	return GenerateFromTexture(&tex, 0); // call the generation function using the newly created texture (and default layer 0)
}

std::pair<std::vector<unsigned char>, glm::ivec2> SignedDistanceField::GenerateFromTexture(Texture* texture, const unsigned int& layer) {
	if (texture->GetWidth() == 0u || texture->GetHeight() == 0u || texture->GetDepth() == 0u) { // if input is invalid...
		throw std::runtime_error("invalid input texture"); // throw exception and break
	}
	
	std::vector<unsigned char> inputData; // the input texture data as unsigned chars (in the form RGBA)
	std::vector<unsigned char> outputData; // the final texture data (of the distance field) in the range of 0 ... 255
	unsigned int textureWidth = texture->GetWidth(); // the width of the image
	unsigned int textureHeight = texture->GetHeight(); // the height of the image
	float distAdj = 1.0f; // the distance between 2 pixels in the cardinal directions
	float distDiag = glm::fastSqrt((distAdj * distAdj) + (distAdj * distAdj)); // the distance between 2 pixels in the intercardinal directions, calculated from the adjacent distance
	
	// the input image (bitmap) in binary form
		// the input image is converted to binary form by converting anything that isn't rgb(255, 255, 255) (or white) to 1 (true)
	std::vector<bool> binaryMap(textureWidth * textureHeight);
	
	// a 2d image (signed distance field) represented as signed floats
		// values range from -n.f ... 0.0f ... +m.f and will be normalised to 0 to 255 (unsigned chars) before returning
	std::vector<float> distanceField(textureWidth * textureHeight);
	
	// an array of ivec2 denoting to the closest border point (in the image) for each pixel
		// a border point is a pixel that is inside an object with at least one of its adjacent (cardinal) pixels outside (and vice-versa)
	std::vector<glm::ivec2> borderPointMap(textureWidth * textureHeight);
	
	// lamda function to check convert (x, y) coordinates to an index in a one-dimensional vector
	auto lambdaGetIndex = [&textureWidth](const size_t& x, const size_t& y)->size_t {
		return (x + (y * textureWidth)); // convert the 2d coordinates into 1d coordinates
	};
	
	// lamda function to check the four (cardinal) adjacent pixels of the passed pixel in the binary map to see if any differ
	// (returns true denoting the passed pixel is a border point, otherwise false)
	auto lambdaCheckAdjacent = [&lambdaGetIndex, &binaryMap, &textureWidth, &textureHeight](const size_t& x, const size_t& y)->bool {
		// check each adjacent pixel in the binary map to find a non-match
		// in c++, logical operators && and || use short-circuit evaluation meaning if the adjacent check fails (i'e', the current pixel
		// lies along the edge of the image e.g., (0, 0)) the bounds check then it is skipped before evaluation (assumed true for a match)
		if (	binaryMap.at(lambdaGetIndex(x, y))																		  &&
				((x > 0                 && binaryMap.at(lambdaGetIndex(x - 1, y)) != binaryMap.at(lambdaGetIndex(x, y)))  || // check left pixel
				(x < textureWidth - 1   && binaryMap.at(lambdaGetIndex(x + 1, y)) != binaryMap.at(lambdaGetIndex(x, y)))  || // check right pixel
				(y < textureHeight - 1  && binaryMap.at(lambdaGetIndex(x, y + 1)) != binaryMap.at(lambdaGetIndex(x, y)))  || // check pixel above
				(y > 0                  && binaryMap.at(lambdaGetIndex(x, y - 1)) != binaryMap.at(lambdaGetIndex(x, y))))) { // check pixel below
			
			return true; // if we reach here passed pixel is a border point due to a differing adjacent pixel
		}
		
		return false; // if we reach here then all adjacent pixels (that exist) match and thus passed pixel is not a border point
	};
	
	// lambda function to propagate scalar values sweeping through the input image in order to calculate the distance transform
	// this is the meat of the dead reckoning algorithm
	auto lambdaPropagate = [&lambdaGetIndex, &distanceField, &borderPointMap, &textureWidth, &textureHeight](const size_t& x, const size_t& y, const char& dx, const char& dy, const float& dist) {
		float& thisDistance = distanceField.at(lambdaGetIndex(x, y)); // get a reference to the distance value for current pixel
		glm::ivec2& thisBorderPoint = borderPointMap.at(lambdaGetIndex(x, y)); // get a reference to the nearest border point for current pixel
		
		// check if the adjacent pixel is valid (neccesary when current pixel is an edge pixel) and the distance is closer than ours
		if ((x + dx >= 0 && x + dx <= textureWidth - 1) && (y + dy >= 0 && y + dy <= textureHeight - 1) && // perform validity check (short-circuited)
				distanceField.at(lambdaGetIndex(x + dx, y + dy)) + dist < thisDistance) { // check if distance is closer than current
			
			thisBorderPoint = borderPointMap.at(lambdaGetIndex(x + dx, y + dy)); // set closest border point to the adjacent's closest border point
			float distx = static_cast<float>(x) - thisBorderPoint.x; // x component of the new distance value
			float disty = static_cast<float>(y) - thisBorderPoint.y; // y component of the new distance value
			thisDistance = glm::fastSqrt((distx * distx) + (disty * disty)); // calculate and update the distance value using the new x and y components
		}
	};
	
	// initialisation:
	// get the data for the desired layer from the texture
	{
		OpenGLStates::BindTexture(texture->GetTextureID());
		
		size_t layerSize = textureWidth * textureHeight * 4; // the number of values that make up a texture layer (RGBA)
		std::vector<unsigned char> texData(layerSize * texture->GetDepth()); // create an array big enough to hold all texture layers
		glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texData[0]); // read the texture data from opengl into the array
		inputData.insert(inputData.begin(), texData.begin() + (layerSize * layer), texData.begin() + (layerSize * (layer + 1))); // copy the specified layer into the array
	}
	
	// initialise the binary map from the texture data
	{
		size_t index = 0u; // the current index in the binary map (1d)
		for (auto iter = inputData.begin(); iter != inputData.end(); iter += 4) { // for all pixels (RGBA) in the texture data
			if (*(iter) < 255u || *(iter + 1) < 255u || *(iter + 2) < 255u) { // if any of the values (RGB) are not 255 (i.e., pixel is not white)...
				binaryMap.at(index) = true; // indicate the pixel is on (true)
			}
			
			++index; // increment the current binary map index
		}
	}
	
	// initialise the distance field and the border point array in a single pass
	for (size_t y = 0u; y < textureHeight; ++y) { // for all rows...
		for (size_t x = 0u; x < textureWidth; ++x) { // for all columns...
			if (lambdaCheckAdjacent(x, y)) { // if pixel is a border point...
				distanceField.at(lambdaGetIndex(x, y)) = 0.0f; // set the distance field value to 0.0f (border point)
				borderPointMap.at(lambdaGetIndex(x, y)) = glm::ivec2(x, y); // set closest border point to self
			}
			else { // otherwise initialise values to maxima (or minima)
				distanceField.at(lambdaGetIndex(x, y)) = std::numeric_limits<float>::max(); // set the distance field value to a high value
				borderPointMap.at(lambdaGetIndex(x, y)) = glm::ivec2(-1, -1); // set closest border point to (-1, -1)
			}
		}
	}
	
	// forward pass:
	// perform first propagation pass (bottom-left, bottom, bottom-right, left)
	for (size_t y = 0u; y < textureHeight; ++y) { // for all rows...
		for (size_t x = 0u; x < textureWidth; ++x) { // for all columns...
			lambdaPropagate(x, y, -1, -1, distDiag); // bottom-left
			 lambdaPropagate(x, y,  0, -1, distAdj); // bottom
			lambdaPropagate(x, y,  1, -1, distDiag); // bottom-right
			 lambdaPropagate(x, y, -1,  0, distAdj); // left
		}
	}
	
	// backwards pass:
	// perform second propagation pass (right, top-left, top, top-right)
	for (int y = textureHeight - 1; y >= 0; --y) { // for all rows (use signed int as unsigned would overflow after 0)...
		for (int x = textureWidth - 1; x >= 0; --x) { // for all columns...
			// std::cout << "  " << x << " " << y << std::endl;
			 lambdaPropagate(x, y,  1, 0, distAdj); // right
			lambdaPropagate(x, y, -1, 1, distDiag); // top-left
			 lambdaPropagate(x, y,  0, 1, distAdj); // top
			lambdaPropagate(x, y,  1, 1, distDiag); // top-right
		}
	}
	
	// normalisation:
	// negate distance values to indicate if they are inside or outside
	for (size_t y = 0u; y < textureHeight; ++y) { // for all rows...
		for (size_t x = 0u; x < textureWidth; ++x) { // for all columns...
			float& thisDistance = distanceField.at(lambdaGetIndex(x, y)); // get a reference to the distance value for current pixel
			
			if (!binaryMap.at(lambdaGetIndex(x, y))) { // if the pixel is deemed outside...
				thisDistance = -thisDistance; // then negate the distance value
			}
		}
	}
	
	std::vector<float> distanceFieldScaled; // the final signed distance field scaled down to size
	unsigned int scale = 8u; // the scale factor of our downsized distance field
	unsigned int scaleSquared = scale * scale; // the scale factor squared used to find the mean
	unsigned int scaledWidth = textureWidth / scale; // the width of the scaled distance field
	unsigned int scaledHeight = textureHeight / scale; // the height of the scaled distance field
	
	for (size_t y = 0u; y < textureHeight; y += scale) { // for all rows...
		for (size_t x = 0u; x < textureWidth; x += scale) { // for all columns...
			float accum = 0.0f; // the sum of all sampled distance values
			for (size_t dy = 0u; dy < scale; ++dy) { // for all distance values to be sampled in the y direction...
				for (size_t dx = 0u; dx < scale; ++dx) { // for all distance values to be sampled in the x direction...
					accum += distanceField.at(lambdaGetIndex(x + dx, y + dy)); // add the distance value to the accumulator
				}
			}
			
            distanceFieldScaled.push_back(accum / scaleSquared); // get the mean of the accumulated distance values
		}
	}
	
	float normFactor = 16.0f;
	for (size_t y = 0u; y < scaledHeight; ++y) { // for all rows...
		for (size_t x = 0u; x < scaledWidth; ++x) { // for all columns...
			float thisDistance = distanceFieldScaled.at(x + (y * scaledWidth)); // get the distance value for the current pixel
            float clampDist = std::max(-normFactor, std::min(thisDistance, normFactor)); // clamp the distance between positive and negative normal factor
            float scaledDist = clampDist / normFactor; // scale into the range -1 to 1
            unsigned char finalDistance = (((scaledDist + 1.0f) / 2.0f) * 255.0f) + 0.5f; // scale into the range 0 ... 255
			
			outputData.push_back(finalDistance); // R
			outputData.push_back(finalDistance); // G
			outputData.push_back(finalDistance); // B
			         outputData.push_back(255u); // A
		}
	}
	
	return std::make_pair(outputData, glm::ivec2(scaledWidth, scaledHeight)); // return the new texture data representing the signed distance field, and the texture dimensions
}
}
