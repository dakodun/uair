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

namespace impl {
void TestCopyAssignEmpty() {
	std::cout << "  --" << std::endl;
	int alive = Tester::mAlive;
	
	if (!gFAILED) { // test an empty hvector being copy assigned to another
		// empty hvector the expected result is that both hvectors remain
		// empty and both still have a zero capacity (no allocated memory)
		
		std::cout << "    empty = empty: ";
		uair::hvector<Tester> hvec;
		
		uair::hvector<Tester> hvec2;
		
		hvec2 = hvec;
		if (hvec.empty() && hvec.count() == 0u &&
				hvec.size() == 0u && hvec.capacity() == 0u &&
				
				hvec2.empty() && hvec2.count() == 0u &&
				hvec2.size() == 0u && hvec2.capacity() == 0u &&
				
				(Tester::mAlive - alive) == 0) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
	
	if (!gFAILED) { // test an empty hvector being copy assigned to a now
		// empty hvector; the expected result is that both hvectors are
		// empty and both retain their capacities (since the destination
		// vector doesn't need to reallocate less memory
		// than it had previously)
		
		std::cout << "    empty (pop) = empty: ";
		uair::hvector<Tester> hvec; 
		
		uair::hvector<Tester> hvec2; hvec2.reserve(4);
		auto h1 = hvec2.emplace(1); auto h2 = hvec2.emplace(2);
		hvec2.pop(h1); hvec2.pop(h2);
		
		hvec2 = hvec;
		if (hvec.empty() && hvec.count() == 0u &&
				hvec.size() == 0u && hvec.capacity() == 0u &&
				
				hvec2.empty() && hvec2.count() == 0u &&
				hvec2.size() == 0u && hvec2.capacity() == 4u &&
				
				(Tester::mAlive - alive) == 0) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
	
	if (!gFAILED) { // test a now empty hvector being copy assigned to an
		// empty hvector the expected result is that neither hvector is
		// empty (both contain entries that hold non-default counter values
		// - that is it used to hold an object but now doesn't) and the
		// origin hvector retains its capacity whilst the destination
		// hvector has a capacity equal to the amount of removed elements
		// (since it now holds entries that have non-default counter values)
		
		std::cout << "    empty = empty (pop): ";
		uair::hvector<Tester> hvec; hvec.reserve(4);
		auto h1 = hvec.emplace(1); auto h2 = hvec.emplace(2);
		hvec.pop(h1); hvec.pop(h2);
		
		uair::hvector<Tester> hvec2;
		
		hvec2 = hvec;
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
			gFAILED = true;
		}
	}
	
	if (!gFAILED) { // test now empty hvectors being assign to each other
		// (one with fewer previous elements, the same amount and more)
		// the expected result is that none of the hvectors are empty (all
		// contain entries that hold non-default counter values) and
		// the capacities match the origin hvector (if original is lower)
		// or remain the same
		
		std::cout << "    empty (pop) = empty (pop): ";
		uair::hvector<Tester> hvec; hvec.reserve(2);
		auto h1 = hvec.emplace(1); auto h2 = hvec.emplace(2);
		hvec.pop(h1); hvec.pop(h2);
		
		uair::hvector<Tester> hvec2; hvec2.reserve(1);
		auto h3 = hvec2.emplace(3);
		hvec2.pop(h3);
		
		uair::hvector<Tester> hvec3; hvec3.reserve(2);
		auto h4 = hvec3.emplace(4); auto h5 = hvec3.emplace(5);
		hvec3.pop(h4); hvec3.pop(h5);
		
		uair::hvector<Tester> hvec4; hvec4.reserve(3);
		auto h6 = hvec4.emplace(6); auto h7 = hvec4.emplace(7);
				auto h8 = hvec4.emplace(8);
		hvec4.pop(h6); hvec4.pop(h7); hvec4.pop(h8);
		
		hvec2 = hvec;
		hvec3 = hvec;
		hvec4 = hvec;
		if (hvec.empty() && hvec.count() == 0u &&
				hvec.size() == 2u && hvec.capacity() == 2u &&
				hvec.get_handle(0u).get_counter() != 0u &&
				
				hvec2.empty() && hvec2.count() == 0u &&
				hvec2.size() == 2u && hvec2.capacity() == 2u &&
				hvec2.get_handle(0u).get_counter() != 0u &&
				
				hvec3.empty() && hvec3.count() == 0u &&
				hvec3.size() == 2u && hvec3.capacity() == 2u &&
				hvec3.get_handle(0u).get_counter() != 0u &&
				
				hvec4.empty() && hvec4.count() == 0u &&
				hvec4.size() == 2u && hvec4.capacity() == 3u &&
				hvec4.get_handle(0u).get_counter() != 0u &&
				
				(Tester::mAlive - alive) == 0) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
}

void TestCopyAssignEmptyFilled() {
	std::cout << "  --" << std::endl;
	int alive = Tester::mAlive;
	
	if (!gFAILED) { // test a filled hvector being copy assigned to an empty
		// hvector; the expected result is that neither hvector is empty,
		// the origin hvector's capacity is unchanged and destination
		// hvector's capacity matches the number of elements in
		// the origin hvector
		
		std::cout << "    empty = filled: ";
		uair::hvector<Tester> hvec; hvec.reserve(4);
		hvec.emplace(1); hvec.emplace(2);
		
		uair::hvector<Tester> hvec2;
		
		hvec2 = hvec;
		if (!hvec.empty() && hvec.count() == 2u &&
				hvec.size() == 2u && hvec.capacity() == 4u &&
				hvec.get_handle(0u).get_counter() == 0u &&
				
				!hvec2.empty() && hvec2.count() == 2u &&
				hvec2.size() == 2u && hvec2.capacity() == 2u &&
				hvec2.get_handle(0u).get_counter() == 0u &&
				
				(Tester::mAlive - alive) == 4) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
	
	if (!gFAILED) { // test an empty hvector being copy assigned to a filled
		// hvector; the expected result is that both hvectors are empty and
		// both capacities remain unchanged
		
		std::cout << "    filled = empty: ";
		uair::hvector<Tester> hvec; 
		
		uair::hvector<Tester> hvec2; hvec2.reserve(4);
		hvec2.emplace(1); hvec2.emplace(2);
		
		hvec2 = hvec;
		if (hvec.empty() && hvec.count() == 0u &&
				hvec.size() == 0u && hvec.capacity() == 0u &&
				
				hvec2.empty() && hvec2.count() == 0u &&
				hvec2.size() == 0u && hvec2.capacity() == 4u &&
				
				(Tester::mAlive - alive) == 0) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
	
	if (!gFAILED) { // test a now empty hvector being copy assigned to
		// filled hvectors; the expected result is that no hvector is empty,
		// the origin hvector's capacity is unchanged and the destination
		// hvectors either have a capacity to match (the number of entries)
		// in the origin hvector if theirs was lower or equal, otherwise it
		// remains unchanged
		
		std::cout << "    empty (pop) = filled: ";
		uair::hvector<Tester> hvec; hvec.reserve(4);
		hvec.emplace(1); hvec.emplace(2);
		
		uair::hvector<Tester> hvec2;
		auto h1 = hvec2.emplace(3);
		hvec2.pop(h1);
		
		uair::hvector<Tester> hvec3; hvec3.reserve(2);
		auto h2 = hvec3.emplace(4); auto h3 = hvec3.emplace(5);
		hvec3.pop(h2); hvec3.pop(h3);
		
		uair::hvector<Tester> hvec4; hvec4.reserve(6);
		auto h4 = hvec4.emplace(6); auto h5 = hvec4.emplace(7);
				auto h6 = hvec4.emplace(8); auto h7 = hvec4.emplace(9);
				auto h8 = hvec4.emplace(10); auto h9 = hvec4.emplace(11);
		hvec4.pop(h4); hvec4.pop(h5); hvec4.pop(h6);
				hvec4.pop(h7); hvec4.pop(h8); hvec4.pop(h9);
		
		hvec2 = hvec;
		hvec3 = hvec;
		hvec4 = hvec;
		if (!hvec.empty() && hvec.count() == 2u &&
				hvec.size() == 2u && hvec.capacity() == 4u &&
				hvec.get_handle(0u).get_counter() == 0u &&
				
				!hvec2.empty() && hvec2.count() == 2u &&
				hvec2.size() == 2u && hvec2.capacity() == 2u &&
				hvec2.get_handle(0u).get_counter() == 0u &&
				
				!hvec3.empty() && hvec3.count() == 2u &&
				hvec3.size() == 2u && hvec3.capacity() == 2u &&
				hvec3.get_handle(0u).get_counter() == 0u &&
				
				!hvec4.empty() && hvec4.count() == 2u &&
				hvec4.size() == 2u && hvec4.capacity() == 6u &&
				hvec4.get_handle(0u).get_counter() == 0u &&
				
				(Tester::mAlive - alive) == 8) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
	
	if (!gFAILED) { // test a now empty hvector being copy assigned to
		// filled hvectors; the expected result is that no hvector is empty,
		// the origin hvector's capacity is unchanged and the destination
		// hvectors either have a capacity to match (the number of entries)
		// in the origin hvector if theirs was lower or equal, otherwise it
		// remains unchanged
		
		std::cout << "    filled = empty (pop): ";
		uair::hvector<Tester> hvec; hvec.reserve(4);
		auto h1 = hvec.emplace(1); auto h2 = hvec.emplace(2);
		hvec.pop(h1); hvec.pop(h2);
		
		uair::hvector<Tester> hvec2;
		hvec2.emplace(3);
		
		uair::hvector<Tester> hvec3; hvec3.reserve(2);
		hvec3.emplace(4); hvec3.emplace(5);
		
		uair::hvector<Tester> hvec4; hvec4.reserve(6);
		hvec4.emplace(6); hvec4.emplace(7);
				hvec4.emplace(8); hvec4.emplace(9);
				hvec4.emplace(10); hvec4.emplace(11);
		
		hvec2 = hvec;
		hvec3 = hvec;
		hvec4 = hvec;
		if (hvec.empty() && hvec.count() == 0u &&
				hvec.size() == 2u && hvec.capacity() == 4u &&
				hvec.get_handle(0u).get_counter() != 0u &&
				
				hvec2.empty() && hvec2.count() == 0u &&
				hvec2.size() == 2u && hvec2.capacity() == 2u &&
				hvec2.get_handle(0u).get_counter() != 0u &&
				
				hvec3.empty() && hvec3.count() == 0u &&
				hvec3.size() == 2u && hvec3.capacity() == 2u &&
				hvec3.get_handle(0u).get_counter() != 0u &&
				
				hvec4.empty() && hvec4.count() == 0u &&
				hvec4.size() == 2u && hvec4.capacity() == 6u &&
				hvec4.get_handle(0u).get_counter() != 0u &&
				
				(Tester::mAlive - alive) == 0) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
}

void TestCopyAssignFilled() {
	std::cout << "  --" << std::endl;
	int alive = Tester::mAlive;
	
	if (!gFAILED) { // test a filled hvector being copy assigned to filled
		// hvectors; the expected result is that no hvector is empty,
		// the origin hvector's capacity is unchanged and the destination
		// hvectors either have a capacity to match (the number of entries)
		// in the origin hvector if theirs was lower or equal, otherwise it
		// remains unchanged
		
		std::cout << "    filled = filled: ";
		uair::hvector<Tester> hvec; hvec.reserve(4);
		hvec.emplace(1); hvec.emplace(2);
		
		uair::hvector<Tester> hvec2; hvec2.reserve(4);
		hvec2.emplace(3);
		
		uair::hvector<Tester> hvec3; hvec3.reserve(1);
		hvec3.emplace(4);
		
		uair::hvector<Tester> hvec4; hvec4.reserve(6);
		hvec4.emplace(6); hvec4.emplace(7);
				hvec4.emplace(8); hvec4.emplace(9);
				hvec4.emplace(10); hvec4.emplace(11);
		
		hvec2 = hvec;
		hvec3 = hvec;
		hvec4 = hvec;
		if (!hvec.empty() && hvec.count() == 2u &&
				hvec.size() == 2u && hvec.capacity() == 4u &&
				hvec.get_handle(0u).get_counter() == 0u &&
				
				!hvec2.empty() && hvec2.count() == 2u &&
				hvec2.size() == 2u && hvec2.capacity() == 4u &&
				hvec2.get_handle(0u).get_counter() == 0u &&
				
				!hvec3.empty() && hvec3.count() == 2u &&
				hvec3.size() == 2u && hvec3.capacity() == 2u &&
				hvec3.get_handle(0u).get_counter() == 0u &&
				
				!hvec4.empty() && hvec4.count() == 2u &&
				hvec4.size() == 2u && hvec4.capacity() == 6u &&
				hvec4.get_handle(0u).get_counter() == 0u &&
				
				(Tester::mAlive - alive) == 8) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
	
	if (!gFAILED) { // test a filled hvector with gaps being copy assigned
		// to a filled hvector with gaps; the expected result is that
		// neither hvector is empty and both capacities are unchanged
		// unless the destination hvector has a lower capacity in which
		// case it is increased to much the size of the origin hvector
		
		std::cout << "    filled (gaps) = filled (gaps): ";
		uair::hvector<Tester> hvec; hvec.reserve(8);
		hvec.emplace(1); auto h1 = hvec.emplace(2); hvec.emplace(3);
				auto h2 = hvec.emplace(4); hvec.emplace(5);
		hvec.pop(h1); hvec.pop(h2);
		
		uair::hvector<Tester> hvec2; hvec2.reserve(3);
		auto h3 = hvec2.emplace(6); hvec2.emplace(7);
				auto h4 = hvec2.emplace(8);
		hvec2.pop(h3); hvec2.pop(h4);
		
		hvec2 = hvec;
		if (!hvec.empty() && hvec.count() == 3u &&
				hvec.size() == 5u && hvec.capacity() == 8u &&
				hvec.get_handle(0u).get_counter() == 0u &&
				
				!hvec2.empty() && hvec2.count() == 3u &&
				hvec2.size() == 5u && hvec2.capacity() == 5u &&
				hvec2.get_handle(0u).get_counter() == 0u &&
				
				(Tester::mAlive - alive) == 6) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
}
}

void TestCopyAssign() {
	if (!gFAILED) {
		impl::TestCopyAssignEmpty();
	}
	
	if (!gFAILED) {
		impl::TestCopyAssignEmptyFilled();
	}
	
	if (!gFAILED) {
		impl::TestCopyAssignFilled();
	}
}
