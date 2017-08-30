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
typedef uair::hvector<Tester>::handle Handle;

void TestModify() {
	std::cout << "  --" << std::endl;
	int alive = Tester::mAlive;
	
	uair::hvector<Tester> hvec; hvec.reserve(3);
	uair::hvector<Tester>::handle h1, h2;
	
	if (!gFAILED) {
		std::cout << "    emplace: ";
		h1 = hvec.emplace(1u); hvec.emplace(2u);
		
		if (!hvec.empty() && hvec.count() == 2u &&
				hvec.size() == 2u && hvec.capacity() == 3u &&
				hvec.get_handle(0u).get_counter() == 0u &&
				
				(Tester::mAlive - alive) == 2) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
	
	if (!gFAILED) {
		std::cout << "    push: ";
		Tester t1(3u);
		h2 = hvec.push(t1);
		
		if (!hvec.empty() && hvec.count() == 3u &&
				hvec.size() == 3u && hvec.capacity() == 3u &&
				hvec.get_handle(0u).get_counter() == 0u &&
				
				(Tester::mAlive - alive) == 4) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
	
	if (!gFAILED) {
		std::cout << "    push (move): ";
		Tester t2(4u);
		hvec.push(std::move(t2));
		
		if (!hvec.empty() && hvec.count() == 4u &&
				hvec.size() == 4u && hvec.capacity() == 4u &&
				hvec.get_handle(0u).get_counter() == 0u &&
				
				(Tester::mAlive - alive) == 5) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
	
	if (!gFAILED) {
		std::cout << "    pop: ";
		hvec.pop(h1); hvec.pop(h2);
		
		if (!hvec.empty() && hvec.count() == 2u &&
				hvec.size() == 4u && hvec.capacity() == 4u &&
				hvec.get_handle(0u).get_counter() != 0u &&
				
				(Tester::mAlive - alive) == 2) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
	}
	
	if (!gFAILED) {
		std::cout << "    swap/clear: ";
		uair::hvector<Tester> hvec2;
		using std::swap;
		swap(hvec, hvec2);
		
		if (hvec.empty() && hvec.count() == 0u &&
				hvec.size() == 0u && hvec.capacity() == 0u &&
				
				!hvec2.empty() && hvec2.count() == 2u &&
				hvec2.size() == 4u && hvec2.capacity() == 4u &&
				
				(Tester::mAlive - alive) == 2) {
			
			std::cout << "success" << std::endl;
		}
		else {
			std::cout << "failed" << std::endl;
			gFAILED = true;
		}
		
		hvec2.clear();
	}
}

void TestAccess() {
	std::cout << "  --" << std::endl;
	int alive = Tester::mAlive;
	
	uair::hvector<Tester> hvec; hvec.reserve(6);
	
	{
		auto h1 = hvec.emplace(0u); auto h2 = hvec.emplace(1u); 
		auto h3 = hvec.emplace(2u); auto h4 = hvec.emplace(3u);
		auto h5 = hvec.emplace(4u);
		hvec.pop(h1); hvec.pop(h3); hvec.pop(h4); hvec.pop(h5);
		h1 = hvec.emplace(5u); h3 = hvec.emplace(6u);
		h4 = hvec.emplace(7u);
		hvec.pop(h1); hvec.pop(h4);
		hvec.emplace(8u);
	}
	
	if (!gFAILED) {
		std::cout << "    at/[]: ";
		
		if (!gFAILED) {
			try {
				Handle h(40, 0);
				hvec.at(h);
				
				std::cout << "failed" << std::endl;
				gFAILED = true;
			} catch (std::out_of_range& e) {
				
			} catch (...) {
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			try {
				Handle h(0, 40);
				hvec.at(h);
				
				std::cout << "failed" << std::endl;
				gFAILED = true;
			} catch (std::logic_error& e) {
				
			} catch (...) {
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			try {
				Handle h(0, 2);
				hvec.at(h);
			} catch (...) {
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			Handle h(2, 1);
			if (hvec[h].mI != 6u) {
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			std::cout << "success" << std::endl;
		}
	}
	
	if (!gFAILED) {
		std::cout << "    at/[] (const): ";
		
		const uair::hvector<Tester> hvecCRef = hvec;
		if (!gFAILED) {
			try {
				Handle h(40, 0);
				hvecCRef.at(h);
				
				std::cout << "failed" << std::endl;
				gFAILED = true;
			} catch (std::out_of_range& e) {
				
			} catch (...) {
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			try {
				Handle h(0, 40);
				hvecCRef.at(h);
				
				std::cout << "failed" << std::endl;
				gFAILED = true;
			} catch (std::logic_error& e) {
				
			} catch (...) {
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			try {
				Handle h(0, 2);
				hvecCRef.at(h);
			} catch (...) {
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			Handle h(2, 1);
			if (hvecCRef[h].mI != 6u) {
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			std::cout << "success" << std::endl;
		}
	}
	
	if (!gFAILED) {
		std::cout << "    get_handle: ";
		
		if (!gFAILED) {
			try {
				Handle h = hvec.get_handle(20);
				
				std::cout << "failed" << std::endl;
				gFAILED = true;
			} catch (std::out_of_range& e) {
				
			} catch (...) {
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			Handle h = hvec.get_handle(0);
			if (h.get_index() != 0u || h.get_counter() != 2u ||
					hvec.at(h).mI != 8) {
				
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			Handle h = hvec.get_handle(hvec.begin());
			if (h.get_index() != 0u || h.get_counter() != 2u ||
					hvec.at(h).mI != 8) {
				
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			Handle h = hvec.get_handle(hvec.cbegin());
			if (h.get_index() != 0u || h.get_counter() != 2u ||
					hvec.at(h).mI != 8) {
				
				std::cout << "failed" << std::endl;
				gFAILED = true;
			}
		}
		
		if (!gFAILED) {
			std::cout << "success" << std::endl;
		}
	}
}
}

void TestFunctions() {
	if (!gFAILED) {
		impl::TestModify();
	}
	
	if (!gFAILED) {
		impl::TestAccess();
	}
}
