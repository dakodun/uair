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

#ifndef TESTER_HPP
#define TESTER_HPP

#include <algorithm>
#include <cassert>

struct Verifier {
    ~Verifier();
};

extern Verifier VERIFIER;


class Tester {
	friend class Verifier;
	
	public :
		Tester(const int& i);
		Tester();
		Tester(const Tester& other);
		Tester(Tester&& other) noexcept;
		~Tester();
		
		Tester& operator=(Tester other);
		
		friend void swap(Tester& first, Tester& second) {
			using std::swap;
			
			swap(first.mI, second.mI);
			assert(first.mSelf == &first && second.mSelf == &second);
		}
		
		void Func() const;
		void Func();
		
		int GetI() const;
	
	public :
		static int mAlive;
		const Tester* mSelf;
		int mI;
};

#endif
