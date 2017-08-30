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

#include <iostream>
#include <string>

#include "uair/uair.hpp"
#include "init.hpp"
#include "tester.hpp"

#include "tests/functions.inl"
#include "tests/iterators.inl"
#include "tests/copyconstruct.inl"
#include "tests/moveconstruct.inl"
#include "tests/copyassign.inl"
#include "tests/moveassign.inl"

int main(int argc, char* argv[]) {
	if (!gFAILED) {
		std::cout << "testing functions..." << std::endl;
		TestFunctions();
		std::cout << std::endl;
	}
	
	if (!gFAILED) {
		std::cout << "testing iterators..." << std::endl;
		TestIterators();
		std::cout << std::endl;
	}
	
	if (!gFAILED) {
		std::cout << "testing copy construct..." << std::endl;
		TestCopyConstruct();
		std::cout << std::endl;
	}
	
	if (!gFAILED) {
		std::cout << "testing move construct..." << std::endl;
		TestMoveConstruct();
		std::cout << std::endl;
	}
	
	if (!gFAILED) {
		std::cout << "testing copy assignment..." << std::endl;
		TestCopyAssign();
		std::cout << std::endl;
	}
	
	if (!gFAILED) {
		std::cout << "testing move assignment..." << std::endl;
		TestMoveAssign();
		std::cout << std::endl;
	}
	
	// pause the console so we can see any output before quitting
	std::string pause;
	std::cout << "\nEnter to continue..." << std::endl;
	std::getline(std::cin, pause);
	
	return 0; // exit
}
