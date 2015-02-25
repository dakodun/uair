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

#include <iostream>
#include <map>
#include <algorithm>

namespace uair {
std::vector<unsigned int> Triangulate::Process(const std::vector<glm::vec2>& outerBoundary, const std::vector< std::vector<glm::vec2> >& innerBoundaries) {
	std::vector<unsigned int> output; // final array of indices froming triangles (CW ordering)
	
	std::vector<glm::vec2> vertices(outerBoundary.begin(), outerBoundary.end()); // add outer boundary vertices to vertex array
	std::vector< const std::vector<glm::vec2>* > boundaries; // array of references to all boundaries
	boundaries.push_back(&outerBoundary); // add reference to outer boundary
	for (auto iter = innerBoundaries.begin(); iter != innerBoundaries.end(); ++iter) { // for all iner boundaries...
		boundaries.push_back(&(*iter)); // add boundary to boundaries array
		vertices.insert(vertices.end(), iter->begin(), iter->end()); // add boundary vertices to vertex array
	}
	
	std::vector<MonotoneIndex> indices; // indices for all vertices in vertex array
	std::vector<unsigned int> sortedIndices; // as above but sorted by x and then y value
	std::set<Edge> edges; // edges that should be currently considered
	std::map<unsigned int, Edge::Iterator> edgeIters; // const iterators to edges above for faster access and deletion
	bool isMonotone = true; // assume polygon is already monotone
	
	unsigned int count = 0u; // count of indices (and subsequently, vertices)
	for (auto iter = boundaries.begin(); iter != boundaries.end(); ++iter) { // for all boundaries in boundary array...
		for (unsigned int i = 0u; i < (*iter)->size(); ++i) { // for all vertices in boundary...
			MonotoneIndex monotoneIndex; // index relating to current vertex
			monotoneIndex.mVertex = i + count; // set current vertex index to current vertex plus count offset
			monotoneIndex.mIndex = i + count; // set current index to current vertex plus count offset
			
			if (i == 0u) { // if this is first vertex in boundary...
				monotoneIndex.mPrevious = ((*iter)->size() - 1u) + count; // set previous to last vertex in boundary
			}
			else {
				monotoneIndex.mPrevious = (i - 1u) + count; // otherwise set previous to previous vertex in boundary
			}
			
			if (i == (*iter)->size() - 1u) { // if this is last vertex in boundary...
				monotoneIndex.mNext = 0u + count; // set next to first vertex in boundary
			}
			else {
				monotoneIndex.mNext = (i + 1u) + count; // otherwise set next to next vertex in boundary
			}
			
			if (IsRight(vertices.at(monotoneIndex.mPrevious), vertices.at(monotoneIndex.mIndex)) &&
					IsRight(vertices.at(monotoneIndex.mNext), vertices.at(monotoneIndex.mIndex))) { // if both previous and next vertices lie to the right...
				
				if (util::IsConvex(vertices.at(monotoneIndex.mPrevious), vertices.at(monotoneIndex.mIndex), vertices.at(monotoneIndex.mNext)) > 0) { // and if angle between them is less than 180...
					monotoneIndex.mType = MonotoneIndex::Start; // type of vertex is start
				}
				else { // otherwise angle is reflex (greater than 180)...
					monotoneIndex.mType = MonotoneIndex::Split; // type of vertex is split
					isMonotone = false; // split vertex indicates polygon is not monotone
				}
			}
			else if (!IsRight(vertices.at(monotoneIndex.mPrevious), vertices.at(monotoneIndex.mIndex)) &&
					!IsRight(vertices.at(monotoneIndex.mNext), vertices.at(monotoneIndex.mIndex))) { // otherwise if both previous and next vertices lie to the left...
				
				if (util::IsConvex(vertices.at(monotoneIndex.mPrevious), vertices.at(monotoneIndex.mIndex), vertices.at(monotoneIndex.mNext)) > 0) { // and if angle between them is less than 180...
					monotoneIndex.mType = MonotoneIndex::End; // type of vertex is end
				}
				else {
					monotoneIndex.mType = MonotoneIndex::Merge; // type of vertex is merge
					isMonotone = false; // merge vertex indicates polygon is not monotone
				}
			}
			else { // otherwise both previous and next vertices lie to opposite sides...
				monotoneIndex.mType = MonotoneIndex::Regular; // type of vertex is regular
			}
			
			indices.push_back(std::move(monotoneIndex)); // add index to index array
		}
		
		count += (*iter)->size(); // add number of vertices in current boundary to total vertex count
	}
	
	if (!isMonotone) { // if polygon isn't already monotone...
		{ // sort by x then y value of vertices they point to
			sortedIndices.assign(indices.size(), 0u); // assign indices to sorted index array
			std::iota(sortedIndices.begin(), sortedIndices.end(), 0);
			
			auto xSort = [vertices](unsigned int firstInd, unsigned int secondInd)->bool { // lambda to sort vertices by x then y
				if (util::CompareFloats(vertices.at(firstInd).x, util::LessThan, vertices.at(secondInd).x)) { // if first x is less than second x...
					return true; // vertex pointed to by firstInd comes BEFORE vertex pointed to by secondInd
				}
				else if (util::CompareFloats(vertices.at(firstInd).x, util::Equals, vertices.at(secondInd).x)) { // otherwise if x values match...
					if (util::CompareFloats(vertices.at(firstInd).y, util::LessThan, vertices.at(secondInd).y)) { // if first y is less than second y...
						return true; // vertex pointed to by firstInd comes BEFORE vertex pointed to by secondInd
					}
				}
				
				return false; // vertex pointed to by firstInd comes AFTER vertex pointed to by secondInd
			};
			
			std::sort(sortedIndices.begin(), sortedIndices.end(), xSort); // sort indices using lambda function above
		}
		
		for (auto iter = sortedIndices.begin(); iter != sortedIndices.end(); ++iter) { // for all sorted indices...
			MonotoneIndex& curr = indices.at(*iter); // current index
			MonotoneIndex& prev = indices.at(curr.mPrevious); // index before current in terms of boundary order
			MonotoneIndex& next = indices.at(curr.mNext); // index after current in terms of boundary order
			
			switch (curr.mType) { // depending on index type...
				case MonotoneIndex::Regular :
					// from "Computational Geometry: Algorithms and Applications":
					// 1. if the interior of P lies to the right of v(i)
						// 2. then if helper(e(i-1)) is a merge vertex
							// 3. then insert the diagonal connecting v(i) to helper(e(i-1)) in D
						// 4. delete e(i-1) from T
						// 5. insert e(i) in T and set helper(e(i)) to v(i)
					// 6. else search in T to find the edge e(j) directly left of v(i)
						// 7. if helper(e(j)) is a merge vertex
							// 8. then insert the diagonal connecting v(i) to helper(e(j)) in D
						// 9. helper(e(j)) <-- v(i)
					
					if (IsRight(vertices.at(curr.mVertex), vertices.at(prev.mVertex))) { // if interior lies below...
						unsigned int edgeIndex = curr.mIndex; // the index of the current edge
						
						Edge::Iterator& edge = edgeIters.at(curr.mPrevious); // get iterator for edge with index mPrevious
						MonotoneIndex& helper = indices.at(edge->mHelper); // get helper of edge
						if (helper.mType == MonotoneIndex::Merge) { // if type of helper is merge...
							AddDiagonal(indices, curr.mIndex, helper.mIndex); // add diagonal to index array (note: not sorted index array)
							edgeIndex = indices.size() - 2; // set the edge index to our first new index
						}
						
						edges.erase(edge); // remove edge from set
						edgeIters.erase(curr.mPrevious); // remove iterator from map
						
						Edge insert(vertices.at(curr.mVertex), vertices.at(next.mVertex), edgeIndex); // create new edge for current index
						edgeIters.insert(std::make_pair(edgeIndex, edges.insert(insert).first)); // add edge to set, and add iterator return to map
					}
					else {
						Edge search(vertices.at(curr.mVertex), vertices.at(curr.mVertex), 0u); // create degenerate edge to search for
						Edge::Iterator edge = edges.lower_bound(search); // get edge after search edge
						if (edge == edges.begin()) { // if edge is start of set...
							std::cout << "triangulation error: failed to find nearest edge" << std::endl; // error occured
							return output; // return empty output
						}
						
						--edge; // get edge before search edge
						MonotoneIndex& helper = indices.at(edge->mHelper); // get helper of edge
						if (helper.mType == MonotoneIndex::Merge) { // if type of helper is merge...
							AddDiagonal(indices, curr.mIndex, helper.mIndex); // add diagonal to index array (note: not sorted index array)
						}
						
						edge->mHelper = curr.mIndex; // update helper of edge to current index
					}
					
					break;
				case MonotoneIndex::Start :
					// from "Computational Geometry: Algorithms and Applications":
					// 1. insert e(i) in T and set helper(e(i)) to v(i)
					
					{
						Edge insert(vertices.at(curr.mVertex), vertices.at(next.mVertex), curr.mIndex); // create new edge for current index
						edgeIters.insert(std::make_pair(curr.mIndex, edges.insert(insert).first)); // add edge to set, and add iterator return to map
					}
					
					break;
				case MonotoneIndex::End : {
					// from "Computational Geometry: Algorithms and Applications":
					// 1. if helper(e(i-1)) is a merge vertex
						// 2. then insert the diagonal connecting v(i) to helper(e(i-1)) in D
					// 3. delete e(i-1) from T
					
					{
						Edge::Iterator& edge = edgeIters.at(curr.mPrevious); // get iterator for edge with index mPrevious
						MonotoneIndex& helper = indices.at(edge->mHelper); // get helper of edge
						if (helper.mType == MonotoneIndex::Merge) { // if type of helper is merge...
							AddDiagonal(indices, curr.mIndex, helper.mIndex); // add diagonal to index array (note: not sorted index array)
						}
						
						edges.erase(edge); // remove edge from set
						edgeIters.erase(curr.mPrevious); // remove iterator from map
					}
					
					break;
				}
				case MonotoneIndex::Split :
					// from "Computational Geometry: Algorithms and Applications":
					// 1. search in T to find the edge e(j) directly left of v(i)
					// 2. insert the diagonal connecting v(i) to helper(e(j)) in D
					// 3. helper(e(j)) <-- v(i)
					// 4. insert e(i) in T and set helper(e(i)) to v(i)
					
					{
						Edge search(vertices.at(curr.mVertex), vertices.at(curr.mVertex), 0u); // create degenerate edge to search for
						Edge::Iterator edge = edges.lower_bound(search); // get edge after search edge
						if (edge == edges.begin()) { // if edge is start of set...
							std::cout << "triangulation error: failed to find nearest edge" << std::endl; // error occured
							return output; // return empty output
						}
						
						--edge; // get edge before search edge
						AddDiagonal(indices, curr.mIndex, edge->mHelper); // add diagonal to index array (note: not sorted index array)
						
						edge->mHelper = curr.mIndex; // update helper of edge to current index
						
						Edge insert(vertices.at(curr.mVertex), vertices.at(next.mVertex), indices.size() - 2); // create new edge for first new index
						edgeIters.insert(std::make_pair(indices.size() - 2, edges.insert(insert).first)); // add edge to set, and add iterator return to map
					}
					
					break;
				case MonotoneIndex::Merge :
					// from "Computational Geometry: Algorithms and Applications":
					// 1. if helper(e(i-1)) is a merge vertex
						// 2. then insert the diagonal connecting v(i) to helper(e(i-1)) in D
					// 3. delete e(i-1) from T
					// 4. search in T to find the edge e(j) directly left of v(i)
					// 5. if helper(e(j)) is a merge vertex
						// 6. then insert the diagonal connecting v(i) to helper(e(j)) in D
					// 7. helper(e(j)) <-- v(i)
					
					{
						Edge::Iterator& edge = edgeIters.at(curr.mPrevious); // get iterator for edge with index mPrevious
						MonotoneIndex& helper = indices.at(edge->mHelper); // get helper of edge
						if (helper.mType == MonotoneIndex::Merge) { // if type of helper is merge...
							AddDiagonal(indices, curr.mIndex, helper.mIndex); // add diagonal to index array (note: not sorted index array)
						}
						
						edges.erase(edge); // remove edge from set
						edgeIters.erase(curr.mPrevious); // remove iterator from map
					}
					
					{
						Edge search(vertices.at(curr.mVertex), vertices.at(curr.mVertex), 0u); // create degenerate edge to search for
						Edge::Iterator edge = edges.lower_bound(search); // get edge after search edge
						if (edge == edges.begin()) { // if edge is start of set...
							std::cout << "triangulation error: failed to find nearest edge" << std::endl; // error occured
							return output; // return empty output
						}
						
						--edge; // get edge before search edge
						MonotoneIndex& helper = indices.at(edge->mHelper); // get helper of edge
						if (helper.mType == MonotoneIndex::Merge) { // if type of helper of edge is merge...
							AddDiagonal(indices, curr.mIndex, helper.mIndex); // add diagonal to index array (note: not sorted index array)
						}
						
						edge->mHelper = curr.mIndex; // update helper of edge to current index
					}
					
					break;
			}
		}
	}
	
	std::vector< std::vector< std::pair<unsigned int, int> > > monotonePolygons; // array of monotone polygons (index and chain id)
	for (auto iter = indices.begin(); iter != indices.end(); ++iter) { // for all indices of polygon...
		if (iter->mProcessed == true) { // if current index already processed...
			continue; // skip
		}
		
		monotonePolygons.emplace_back(); // add new monotone polygon
		auto iterNext = iter; // set next iterator to current
		float area = 0.0f; // initialise area of polygon to 0
		
		do { // while next iterator hasn't looped back to current iterator
			glm::vec2& first = vertices.at(iterNext->mVertex); // get current vertex
			
			monotonePolygons.back().push_back(std::make_pair(iterNext->mVertex, -1)); // add index and chain id to polygon (assume bottom chain (-1) initially)
			iterNext->mProcessed = true; // indicate index has been processed
			iterNext = indices.begin() + iterNext->mNext; // set next iterator to next index of polygon
			
			glm::vec2& second = vertices.at(iterNext->mVertex); // get next vertex
			area += (second.x - first.x) * (second.y + first.y); // add to current area
		} while (iterNext != iter);
		
		if (!monotonePolygons.back().empty()) { // if we have at polygon isn't empty...
			if (util::CompareFloats(area, util::GreaterThan, 0.0f)) { // if area of polygon is positive...
				std::reverse(monotonePolygons.back().begin() + 1, monotonePolygons.back().end()); // reverse winding of polygon
			}
			
			auto left = monotonePolygons.back().begin(); // set iterator to left-most index to first index initially
			auto right = monotonePolygons.back().begin(); // set iterator to right-most index to first index initially
			for (auto iter = monotonePolygons.back().begin() + 1; iter != monotonePolygons.back().end(); ++iter) { // for all indices except first...
				if (IsRight(vertices.at(iter->first), vertices.at(right->first))) { // if current index is to right of right-most...
					right = iter; // set right-most index to current
				}
				
				if (!IsRight(vertices.at(iter->first), vertices.at(left->first))) { // if current index is to left of left-most...
					left = iter; // set left-most index to current
				}
			}
			
			left->second = 0; // set chain id to 0 (indicating start index)
			right->second = 0; // set chain id to 0 (indicating end index)
			auto iter = left; // set iterator to left-most index
			while (iter != right) { // while we haven't reached right-most index...
				iter->second = 1; // set chain id to top (1)
				++iter; // increment iterator
				
				if (iter == monotonePolygons.back().end()) { // if we have reached end of polygon index array...
					iter = monotonePolygons.back().begin(); // set iterator to start of polygon index array
				}
			}
		}
		else {
			monotonePolygons.pop_back(); // remove empty polygon from array of monotone polygons
		}
	}
	
	for (auto iter = monotonePolygons.begin(); iter != monotonePolygons.end(); ++iter) { // for all monotone polygons...
		ProcessYMonotone(output, vertices, *iter); // triangulate and store triangle indices in output
	}
	
	return output; // return array indices of triangles
}

void Triangulate::ProcessYMonotone(std::vector<unsigned int>& output, const std::vector<glm::vec2>& vertices, std::vector< std::pair<unsigned int, int> >& indices) {
	if (indices.size() < 3) { // if polygon is already a triangle (or degenerate)...
		return; // stop processing
	}
	
	{ // sort by x then y value of vertices they point to
		auto xSort = [vertices](std::pair<unsigned int, int> firstInd, std::pair<unsigned int, int> secondInd)->bool { // lambda to sort vertices by x then y
			if (util::CompareFloats(vertices.at(firstInd.first).x, util::LessThan, vertices.at(secondInd.first).x)) { // if first x is less than second x...
				return true; // vertex pointed to by firstInd comes BEFORE vertex pointed to by secondInd
			}
			else if (util::CompareFloats(vertices.at(firstInd.first).x, util::Equals, vertices.at(secondInd.first).x)) { // otherwise if x values match...
				if (util::CompareFloats(vertices.at(firstInd.first).y, util::LessThan, vertices.at(secondInd.first).y)) { // if first y is less than second y...
					return true; // vertex pointed to by firstInd comes BEFORE vertex pointed to by secondInd
				}
			}
			
			return false; // vertex pointed to by firstInd comes AFTER vertex pointed to by secondInd
		};
		
		std::sort(indices.begin(), indices.end(), xSort); // sort indices using lambda function above
	}
	
	std::vector< std::pair<unsigned int, int> > open(indices.begin(), indices.begin() + 2); // indices that are still to be considered
	
	for (auto iter = indices.begin() + 2; iter != indices.end() - 1; ++iter) { // for all indices except first two and last one...
		if (iter->second != (open.end() - 1)->second) { // if indices lie on opposite sides of boundary...
			for (auto jter = open.begin(); jter < open.end() - 1; ++jter) { // for all indices in open list except last...
				if (iter->second == 1) { // if index is on top monotone chain...
					output.push_back(iter->first); // add current index
					output.push_back((jter + 1)->first); // add next open index
					output.push_back(jter->first); // add current open index
				}
				else { // otherwise index is on bottom monotone chain...
					output.push_back(iter->first); // add current index
					output.push_back(jter->first); // add current open index
					output.push_back((jter + 1)->first); // add next open index
				}
			}
			
			std::vector< std::pair<unsigned int, int> > newOpen = {open.back(), *iter}; // create new open list with last index of previous and current index
			std::swap(open, newOpen); // swap new open list with current
		}
		else { // otherwise indices lie on same side of boundary...
			int isConvex;
			while (open.size() > 1) {
				if (iter->second == 1) { // if index is on top monotone chain...
					// get value detailing if angle between second last open index, last open index and current index is less than, equal to or greater than 180
					isConvex = util::IsConvex(vertices.at((open.end() - 2)->first), vertices.at((open.end() - 1)->first), vertices.at(iter->first));
				}
				else { // otherwise index is on bottom monotone chain...
					// get value detailing if angle between second last open index, last open index and current index is less than, equal to or greater than 180
					isConvex = util::IsConvex(vertices.at(iter->first), vertices.at((open.end() - 1)->first), vertices.at((open.end() - 2)->first));
				}
				
				if (isConvex == -1) { // if angle is greater than 180...
					break; // stop processing
				}
				else if (isConvex > 0) { // otherwise if angle is not 180 or 0...
					if (iter->second == 1) { // if index is on top monotone chain...
						output.push_back((open.end() - 2)->first); // add second last open index
						output.push_back((open.end() - 1)->first); // add last open index
						output.push_back(iter->first); // add current index
					}
					else { // otherwise index is on bottom monotone chain...
						output.push_back(iter->first); // add current index
						output.push_back((open.end() - 1)->first); // add last open index
						output.push_back((open.end() - 2)->first); // add second last open index
					}
				}
				
				open.pop_back(); // remove last index in open list
			}
			
			open.push_back(*iter); // add current index to open list
		}
	}
	
	for (auto iter = open.begin(); iter != open.end() - 1; ++iter) { // for all indices still in open list...
		if ((iter + 1)->second == 1) { // if index is on top monotone chain...
			output.push_back((indices.end() - 1)->first); // add final index
			output.push_back(iter->first); // add current open index
			output.push_back((iter + 1)->first); // add next open index
		}
		else { // otherwise index is on bottom monotone chain...
			output.push_back((indices.end() - 1)->first); // add final index
			output.push_back((iter + 1)->first); // add next open index
			output.push_back(iter->first); // add current open index
		}
	}
}

bool Triangulate::IsRight(const glm::vec2& firstPt, const glm::vec2& secondPt) {
	if (util::CompareFloats(firstPt.x, util::GreaterThan, secondPt.x)) { // if first point's x-value is greater than second's...
		return true; // first point is to right of second
	}
	else if (util::CompareFloats(firstPt.x, util::Equals, secondPt.x)) { // otherwise if first point's x-value matches second's...
		if (util::CompareFloats(firstPt.y, util::GreaterThan, secondPt.y)) { // if first point's y-value is greater than second's...
			return true; // first point is to right of second
		}
	}
	
	return false; // first point is to left of second (or same)
}

void Triangulate::AddDiagonal(std::vector<MonotoneIndex>& indices, const unsigned int& indexFirst, const unsigned int& indexSecond) {
	MonotoneIndex monotoneIndex; // new monotone indices to insert
	
	monotoneIndex = indices.at(indexFirst); // make a copy of the first index
	monotoneIndex.mIndex = indices.size(); // update the index in index array
	indices.push_back(monotoneIndex); // add new index to polygon indices (not sorted)
	MonotoneIndex& newFirst = indices.back(); // get reference to new index
	
	monotoneIndex = indices.at(indexSecond); // make a copy of the second index
	monotoneIndex.mIndex = indices.size(); // update the index in index array
	indices.push_back(monotoneIndex); // add new index to polygon indices (not sorted)
	MonotoneIndex& newSecond = indices.back(); // get reference to new index
	
	newFirst.mPrevious = newSecond.mIndex; // set the previous of the first new index to the second new index
	indices.at(newFirst.mNext).mPrevious = newFirst.mIndex; // set the previous of the new first index's next index to the new first index
	
	newSecond.mNext = newFirst.mIndex; // set the next of the second new index to the first new index
	indices.at(newSecond.mPrevious).mNext = newSecond.mIndex; // set the next of the new second index's previous index to the new second index
	
	indices.at(indexFirst).mNext = indexSecond; // set the next of the first index to the second index
	indices.at(indexSecond).mPrevious = indexFirst; // set the previous of the second index to the first index
}
}
