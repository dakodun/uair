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

void TestCopyConstruct() {
	std::cout << "  --" << std::endl;
	int alive = Tester::mAlive;
	
	if (!gFAILED) { // test an hvector being copy constructed from an empty hvector
		// the expected result is that the destination hvector matches
		// the origin vector and the origin hvector is unchanged
		
		std::cout << "    empty: ";
		uair::hvector<Tester> hvec;
		
		uair::hvector<Tester> hvec2 = hvec;
		if (hvec.empty() && hvec.count() == 0u &&
				hvec.size() == 0u && hvec.capacity() == 0u &&
				
				hvec2.empty() && hvec2.count() == 0u &&
				hvec2.size() == 0u && hvec2.capacity() == 0u &&
				
				(Tester::mAlive - alive) == 0) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
		}
	}
	
	if (!gFAILED) { // test an hvector being copy constructed from a now empty hvector
		// the expected result is that the destination hvector matches
		// the origin vector (except capacity which is set to the minimum
		// needed to hold all elements) and the origin hvector is unchanged
		
		std::cout << "    empty (pop): ";
		uair::hvector<Tester> hvec; hvec.reserve(4);
		auto h1 = hvec.emplace(1); auto h2 = hvec.emplace(2);
		hvec.pop(h1); hvec.pop(h2);
		
		uair::hvector<Tester> hvec2 = hvec;
		if (hvec.empty() && hvec.count() == 0u &&
				hvec.size() == 2u && hvec.capacity() == 4u &&
				hvec.get_handle(0u).get_counter() != 0u &&
				
				hvec2.empty() && hvec2.count() == 0u &&
				hvec2.size() == 2u && hvec2.capacity() == 2u &&
				hvec2.get_handle(0u).get_counter() != 0u &&
				
				(Tester::mAlive - alive) == 0) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
		}
	}
	
	if (!gFAILED) { // test an hvector being copy constructed from a filled hvector
		// the expected result is that the destination hvector matches
		// the origin vector and the origin hvector is unchanged
		
		std::cout << "    filled: ";
		uair::hvector<Tester> hvec;
		hvec.emplace(1); hvec.emplace(2); hvec.emplace(3);
		
		uair::hvector<Tester> hvec2 = hvec;
		if (!hvec.empty() && hvec.count() == 3u &&
				hvec.size() == 3u && hvec.capacity() == 3u &&
				hvec.begin()->mI == 1 &&
				hvec.get_handle(0u).get_counter() == 0u &&
				
				!hvec2.empty() && hvec2.count() == 3u &&
				hvec2.size() == 3u && hvec2.capacity() == 3u &&
				hvec2.begin()->mI == 1 &&
				hvec2.get_handle(0u).get_counter() == 0u &&
				
				(Tester::mAlive - alive) == 6) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
		}
	}
	
	if (!gFAILED) { // test an hvector being copy constructed from a filled hvector
		// the expected result is that the destination hvector matches
		// the origin vector and the origin hvector is unchanged
		
		std::cout << "    filled (gaps): ";
		uair::hvector<Tester> hvec; hvec.reserve(8);
		hvec.emplace(1); auto h1 = hvec.emplace(2); hvec.emplace(3);
				auto h2 = hvec.emplace(4); hvec.emplace(5);
		hvec.pop(h1); hvec.pop(h2);
		
		uair::hvector<Tester> hvec2 = hvec;
		if (!hvec.empty() && hvec.count() == 3u &&
				hvec.size() == 5u && hvec.capacity() == 8u &&
				hvec.begin()->mI == 1 &&
				hvec.get_handle(0u).get_counter() == 0u &&
				
				!hvec2.empty() && hvec2.count() == 3u &&
				hvec2.size() == 5u && hvec2.capacity() == 5u &&
				hvec2.begin()->mI == 1 &&
				hvec2.get_handle(0u).get_counter() == 0u &&
				
				(Tester::mAlive - alive) == 6) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
		}
	}
}
