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

#include "tester.hpp"

Verifier VERIFIER;

Verifier::~Verifier() {
	assert(Tester::mAlive == 0);
}


int Tester::mAlive = 0;

Tester::Tester(const int& i) : mSelf(this), mI(i) {
	++mAlive;
}

Tester::Tester() : Tester(0) {
	
}

Tester::Tester(const Tester& other) : mSelf(this), mI(other.mI) {
	++mAlive;
}

Tester::Tester(Tester&& other) noexcept : Tester() {
	swap(*this, other);
}

Tester::~Tester() {
	assert(mSelf == this);
	--mAlive;
}

Tester& Tester::operator=(Tester other) {
	swap(*this, other);
	
	return *this;
}

void Tester::Func() const {
	assert(mSelf == this);
}

void Tester::Func() {
	assert(mSelf == this);
}
