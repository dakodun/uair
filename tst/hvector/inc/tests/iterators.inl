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

#include <vector>

namespace impl {
typedef uair::hvector<Tester>::iterator Iter;
typedef uair::hvector<Tester>::const_iterator CIter;
typedef uair::hvector<Tester>::reverse_iterator RIter;
typedef uair::hvector<Tester>::const_reverse_iterator CRIter;

void Test() {
	int alive = Tester::mAlive;
	std::cout << "  --" << std::endl;
	
	uair::hvector<Tester> hvec; hvec.reserve(8);
	hvec.emplace(1); auto h1 = hvec.emplace(2); hvec.emplace(3);
			auto h2 = hvec.emplace(4); hvec.emplace(5);
	hvec.pop(h1); hvec.pop(h2);
	
	std::vector<int> expected = {1, 3, 5};
	unsigned int i;
	
	if (!gFAILED) {
		std::cout << "    begin/end: ";
		
		i = 0u;
		for (Iter iter = hvec.begin(); iter != hvec.end(); ++iter) {
			if (iter->mI == expected.at(i)) {
				iter->mI = (++iter->mI) - 1;
				++i;
			}
			else {
				break;
			}
		}
		
		if (i != 3u) {
			std::cout << "failed" << std::endl;
		}
		else {
			std::cout << "success" << std::endl;
		}
	}
	
	if (!gFAILED) {
		std::cout << "    begin/end (const): ";
		
		i = 0u;
		for (CIter citer = hvec.begin(); citer != hvec.end(); ++citer) {
			if (citer->mI == expected.at(i) &&
					std::is_const<CIter::entry>::value) {
				
				++i;
			}
			else {
				break;
			}
		}
		
		if (i != 3u) {
			std::cout << "failed" << std::endl;
		}
		else {
			std::cout << "success" << std::endl;
		}
	}
	
	if (!gFAILED) {
		std::cout << "    cbegin/cend: ";
		
		i = 0u;
		for (auto citer = hvec.cbegin(); citer != hvec.cend(); ++citer) {
			if (citer->mI == expected.at(i) &&
					std::is_const<decltype(citer)::entry>::value) {
				
				++i;
			}
			else {
				break;
			}
		}
		
		if (i != 3u) {
			std::cout << "failed" << std::endl;
		}
		else {
			std::cout << "success" << std::endl;
		}
	}
}

void TestReverse() {
	int alive = Tester::mAlive;
	std::cout << "  --" << std::endl;
	
	uair::hvector<Tester> hvec; hvec.reserve(8);
	hvec.emplace(1); auto h1 = hvec.emplace(2); hvec.emplace(3);
			auto h2 = hvec.emplace(4); hvec.emplace(5);
	hvec.pop(h1); hvec.pop(h2);
	
	std::vector<int> expected = {5, 3, 1};
	unsigned int i;
	
	if (!gFAILED) {
		std::cout << "    rbegin/rend: ";
		
		i = 0u;
		for (RIter riter = hvec.rbegin(); riter != hvec.rend(); ++riter) {
			if (riter->mI == expected.at(i)) {
				riter->mI = (++riter->mI) - 1;
				++i;
			}
			else {
				break;
			}
		}
		
		if (i != 3u) {
			std::cout << "failed" << std::endl;
		}
		else {
			std::cout << "success" << std::endl;
		}
	}
	
	if (!gFAILED) {
		std::cout << "    rbegin/rend (const): ";
		
		i = 0u;
		for (CRIter criter = hvec.rbegin(); criter != hvec.rend();
				++criter) {
			
			if (criter->mI == expected.at(i) &&
					std::is_const<CIter::entry>::value) {
				
				++i;
			}
			else {
				break;
			}
		}
		
		if (i != 3u) {
			std::cout << "failed" << std::endl;
		}
		else {
			std::cout << "success" << std::endl;
		}
	}
	
	if (!gFAILED) {
		std::cout << "    crbegin/crend: ";
		
		i = 0u;
		for (auto criter = hvec.crbegin(); criter != hvec.crend();
				++criter) {
			
			if (criter->mI == expected.at(i) &&
					std::is_const<decltype(hvec.cbegin())::entry>::value) {
				
				++i;
			}
			else {
				break;
			}
		}
		
		if (i != 3u) {
			std::cout << "failed" << std::endl;
		}
		else {
			std::cout << "success" << std::endl;
		}
	}
}
}

void TestIterators() {
	if (!gFAILED) {
		impl::Test();
	}
	
	if (!gFAILED) {
		impl::TestReverse();
	}
}
