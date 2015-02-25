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

#ifndef UAIRTRIANGULATE_HPP
#define UAIRTRIANGULATE_HPP

#include <ostream>
#include <vector>
#include <set>
#include <map>

#include <glm/glm.hpp>

#include "util.hpp"

namespace uair {
// class to triangulate non-intersecting polygon (with holes)
// algorithm from "Computational Geometry: Algorithms and Applications" by Mark de Berg, Otfried Cheong, Marc van Kreveld and Mark Overmars
// based on code from "polypartition" by Ivan Fratric (https://code.google.com/p/polypartition/)
// sweep line goes left to right instead of top to bottom and origin is top-left instead of bottom-right
class Triangulate {
	public :
		// special index used to create monotone polygons
		struct MonotoneIndex {
				enum { // various types index can be
					Regular = 0u,
					Start,
					End,
					Split,
					Merge
				};
				
				friend std::ostream& operator<<(std::ostream &out, const MonotoneIndex &monotoneIndex) { // output monotone index object as string
					out << "(" << monotoneIndex.mPrevious << " -> [" << monotoneIndex.mIndex << "] -> " << monotoneIndex.mNext << "): "; // output previous, current and next indices
					
					switch (monotoneIndex.mType) { // depending on type of index...
						case 0u :
							out << "Regular"; // output type in text form
							break;
						case 1u :
							out << "Start";
							break;
						case 2u :
							out << "End";
							break;
						case 3u :
							out << "Split";
							break;
						case 4u :
							out << "Merge";
							break;
					}
					
					return out;
				}
				
				unsigned int mIndex = 0u; // index of this index in index array
				unsigned int mPrevious = 0u; // index of previous index (same boundary) in index array
				unsigned int mNext = 0u; // index of next index (same boundary) in index array
				unsigned int mVertex = 0u; // index of this index in vertex array
				unsigned int mType = Regular; // type of index
				bool mProcessed = false; // has this index been processed yet (when compiling monotone polygons)
		};
		
		// edge (line segment of current to next) relating to monotone index
		struct Edge {
			Edge() = default;
			Edge(const glm::vec2& start, const glm::vec2& end, const unsigned int& helper) : mStart(start), mEnd(end), mHelper(helper) {
				
			}
			
			bool operator<(const Edge& other) const { // sorts left to right
				if (util::CompareFloats(other.mEnd.x, util::Equals, other.mStart.x)) { // if x-values of other edge's points match...
					if (util::CompareFloats(mEnd.x, util::Equals, mStart.x)) { // and if x-values of this edge's points match...
						if (util::CompareFloats(mEnd.x, util::LessThan, other.mEnd.x)) { // if this edge comes before other in terms of x-value...
							return true; // this edge comes before other
						}
						else {
							return false; // other edge comes before this
						}
					}
					
					if (util::IsConvex(mStart, mEnd, other.mEnd) > 0) { // if angle from other to this is less than 180...
						return true; // this edge comes before other
					}
					else {
						return false; // other edge comes before this
					}
				} 
				else if (util::CompareFloats(mEnd.x, util::Equals, mStart.x)) { // otherwise if only x-values of this edge's points match...
					if (util::IsConvex(other.mStart, other.mEnd, mEnd) > 0) { // if angle from this to other is less than 180...
						return false; // other edge comes before this
					}
					else {
						return true; // this edge comes before other
					}
				}
				else if (util::CompareFloats(mEnd.x, util::LessThan, other.mEnd.x)) { // otherwise if both edge's x-values differ and this edge comes before other in terms of x-value...
					if (util::IsConvex(other.mStart, other.mEnd, mEnd) > 0) { // if angle from this to other is less than 180...
						return false; // other edge comes before this
					}
					else {
						return true; // this edge comes before other
					}
				}
				else { // otherwise both edge's x-values differ and other edge comes before this in terms of x-value...
					if (util::IsConvex(mStart, mEnd, other.mEnd) > 0) { // if angle from other to this is less than 180...
						return true; // this edge comes before other
					}
					else {
						return false; // other edge comes before this
					}
				}
			}
			
			typedef std::set<Edge>::iterator Iterator; // easy typedef for iterator to set of edges
			
			glm::vec2 mStart; // first point of edge
			glm::vec2 mEnd; // second point of edge
			mutable unsigned int mHelper = 0u; // helper index, used when creating diagonals (mutable since it will be modified using const_iterator)
		};
		
		// triangulate valid polygon (one outer boundary and n inner boundaries, non-intersecting) and return indices forming triangles
		std::vector<unsigned int> Process(const std::vector<glm::vec2>& outerBoundary, const std::vector< std::vector<glm::vec2> >& innerBoundaries);
		
		// triangulate y-monotone polygon and return indices forming triangles
		void ProcessYMonotone(std::vector<unsigned int>& output, const std::vector<glm::vec2>& vertices, std::vector< std::pair<unsigned int, int> >& indices);
	protected :
		// returns true if first point is to right of second point (first using x-values, and if equal then y)
		bool IsRight(const glm::vec2& firstPt, const glm::vec2& secondPt);
		
		// adds diagonal into indices from first index to second index, updating other indices as neccesary
		void AddDiagonal(std::vector<MonotoneIndex>& indices, const unsigned int& indexFirst, const unsigned int& indexSecond);
};
}

#endif
