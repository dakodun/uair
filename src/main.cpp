/* **************************************************************** **
**	Uair Engine
**	Copyright (c) 2013 Iain M. Crawford
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

/** 
* \file		main.cpp
* \brief	Defines WinMain entry point if OS is Windows.
**/

#if defined(_WIN32) || defined(__WIN32__)
	#include <windows.h>
	
	extern int main(int argc, char* argv[]);
	
	/** 
	* \brief	Windows entry point.
	* \details	By defining WinMain like this we will be able to use
	* 			main as our entry point across various systems
	*			(including Windows), allowing for better portability.
	**/
	int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT) {
		return main(__argc, __argv);
	}
#endif
