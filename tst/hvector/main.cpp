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

class Test {
	public :
		Test() = default;
		Test(const int& i) : mI(i) {
			
		}
		
		Test(const Test& other) : mI(other.mI) {
			
		}
		
		Test(Test&& other) : Test() {
			swap(*this, other);
		}
		
		Test& operator=(Test other) {
			swap(*this, other);
			
			return *this;
		}
		
		friend void swap(Test& first, Test& second) {
			using std::swap;
			
			swap(first.mI, second.mI);
		}
	
	public :
		int mI = 0;
};

int main(int argc, char* argv[]) {	
	{
		typedef uair::util::HVector<Test>::Handle TestHandle;
		uair::util::HVector<Test> store(10);
		
		// test the four methods of insertion (emplace (default/custom constructor) and push (copy/move))
		// on a fresh store
			// create a test resource in the store using the default constructor (no parameters) and store the handle
			// then return a reference to it using the handle
			TestHandle h1;
			try {
				std::cout << ".--- Emplace Default" << std::endl;
				
				h1 = store.Emplace();
				Test& t = store.Get(h1);
				
				std::cout << "| " << "Test (" << t.mI << ")" << std::endl;
				std::cout << "| " << "Handle (" << h1.mIndex << ", " << h1.mCounter << ")" << std::endl;
				std::cout << "| " << "Store Size: " << store.Size() << std::endl;
				std::cout << "'---------" << std::endl << std::endl;
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			//
			
			// create a test resource in the store supplying an int and store the handle
			// then return a reference to it using the handle 
			TestHandle h2;
			try {
				std::cout << ".--- Emplace Custom" << std::endl;
				
				h2 = store.Emplace(4);
				Test& t = store.Get(h2);
				
				std::cout << "| " << "Test (" << t.mI << ")" << std::endl;
				std::cout << "| " << "Handle (" << h2.mIndex << ", " << h2.mCounter << ")" << std::endl;
				std::cout << "| " << "Store Size: " << store.Size() << std::endl;
				std::cout << "'---------" << std::endl << std::endl;
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			//
			
			// create a test resource, add it to the store (copy) and store the handle
			// then return a reference to it using the handle and change it
			TestHandle h3;
			try {
				std::cout << ".--- Push Copy" << std::endl;
				
				Test t1(13);
				h3 = store.Push(t1);
				Test& t2 = store.Get(h3);
				int temp = t2.mI;
				t2.mI = 44;
				Test& t3 = store.Get(h3);
				
				std::cout << "| " << "Test (" << t1.mI << " : " << temp << " -> " << t3.mI << ")" << std::endl;
				std::cout << "| " << "Handle (" << h3.mIndex << ", " << h3.mCounter << ")" << std::endl;
				std::cout << "| " << "Store Size: " << store.Size() << std::endl;
				std::cout << "'---------" << std::endl << std::endl;
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			//
			
			// create a test resource, add it to the store (move) and store the handle
			// then return a reference to it using the handle and change it
			TestHandle h4;
			try {
				std::cout << ".--- Push Move" << std::endl;
				
				Test t1(28);
				h4 = store.Push(std::move(t1));
				Test& t2 = store.Get(h4);
				int temp = t2.mI;
				t2.mI = 93;
				Test& t3 = store.Get(h4);
				
				std::cout << "| " << "Test (" << t1.mI << " : " << temp << " -> " << t3.mI << ")" << std::endl;
				std::cout << "| " << "Handle (" << h4.mIndex << ", " << h4.mCounter << ")" << std::endl;
				std::cout << "| " << "Store Size: " << store.Size() << std::endl;
				std::cout << "'---------" << std::endl << std::endl;
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			//
		//
		
		
		// remove test resources via their handles
		try {
			std::cout << ".--- Pop" << std::endl;
			
			store.Pop(h1);
			store.Pop(h2);
			store.Pop(h3);
			store.Pop(h4);
			
			std::cout << "| " << "Handle (" << h1.mIndex << ", " << h1.mCounter << ")" << std::endl;
			std::cout << "| " << "Handle (" << h2.mIndex << ", " << h2.mCounter << ")" << std::endl;
			std::cout << "| " << "Handle (" << h3.mIndex << ", " << h3.mCounter << ")" << std::endl;
			std::cout << "| " << "Handle (" << h4.mIndex << ", " << h4.mCounter << ")" << std::endl;
			std::cout << "| " << "Store Size: " << store.Size() << std::endl;
			std::cout << "'---------" << std::endl << std::endl;
		} catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		//
		
		
		// test the four methods of insertion (emplace (default/custom constructor) and push (copy/move))
		// on a fresh store
			// create a test resource in the store using the default constructor (no parameters) and store the handle
			// then return a reference to it using the handle
			try {
				std::cout << ".--- Emplace Default" << std::endl;
				
				TestHandle h = store.Emplace();
				Test& t = store.Get(h);
				
				std::cout << "| " << "Test (" << t.mI << ")" << std::endl;
				std::cout << "| " << "Handle (" << h.mIndex << ", " << h.mCounter << ")" << std::endl;
				std::cout << "| " << "Store Size: " << store.Size() << std::endl;
				std::cout << "'---------" << std::endl << std::endl;
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			//
			
			// create a test resource in the store supplying an int and store the handle
			// then return a reference to it using the handle 
			try {
				std::cout << ".--- Emplace Custom" << std::endl;
				
				h2 = store.Emplace(4);
				Test& t = store.Get(h2);
				
				std::cout << "| " << "Test (" << t.mI << ")" << std::endl;
				std::cout << "| " << "Handle (" << h2.mIndex << ", " << h2.mCounter << ")" << std::endl;
				std::cout << "| " << "Store Size: " << store.Size() << std::endl;
				std::cout << "'---------" << std::endl << std::endl;
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			//
			
			// create a test resource, add it to the store (copy) and store the handle
			// then return a reference to it using the handle and change it
			try {
				std::cout << ".--- Push Copy" << std::endl;
				
				Test t1(13);
				h3 = store.Push(t1);
				Test& t2 = store.Get(h3);
				int temp = t2.mI;
				t2.mI = 44;
				Test& t3 = store.Get(h3);
				
				std::cout << "| " << "Test (" << t1.mI << " : " << temp << " -> " << t3.mI << ")" << std::endl;
				std::cout << "| " << "Handle (" << h3.mIndex << ", " << h3.mCounter << ")" << std::endl;
				std::cout << "| " << "Store Size: " << store.Size() << std::endl;
				std::cout << "'---------" << std::endl << std::endl;
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			//
			
			// create a test resource, add it to the store (move) and store the handle
			// then return a reference to it using the handle and change it
			try {
				std::cout << ".--- Push Move" << std::endl;
				
				Test t1(28);
				h4 = store.Push(std::move(t1));
				Test& t2 = store.Get(h4);
				int temp = t2.mI;
				t2.mI = 93;
				Test& t3 = store.Get(h4);
				
				std::cout << "| " << "Test (" << t1.mI << " : " << temp << " -> " << t3.mI << ")" << std::endl;
				std::cout << "| " << "Handle (" << h4.mIndex << ", " << h4.mCounter << ")" << std::endl;
				std::cout << "| " << "Store Size: " << store.Size() << std::endl;
				std::cout << "'---------" << std::endl << std::endl;
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}
			//
		//
		
		
		// remove a test resource via its handle
		try {
			std::cout << ".--- Pop" << std::endl;
			
			store.Pop(h2);
			
			std::cout << "| " << "Handle (" << h2.mIndex << ", " << h2.mCounter << ")" << std::endl;
			std::cout << "| " << "Store Size: " << store.Size() << std::endl;
			std::cout << "'---------" << std::endl << std::endl;
		} catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		//
		
		
		// attempt to return a reference to a test resource via an invalid handle
			try {
				std::cout << ".--- Invalid Handle (Free Index)" << std::endl;
				std::cout << "| " << "Handle (" << h2.mIndex << ", " << h2.mCounter << ")" << std::endl;
				std::cout << "| " << "Store Size: " << store.Size() << std::endl;
				
				store.Get(h2);
			} catch (std::exception& e) {
				std::cout << "| E: " << e.what() << std::endl;
				std::cout << "'---------" << std::endl << std::endl;
			}
			
			try {
				std::cout << ".--- Invalid Handle (Re-used Index)" << std::endl;
				std::cout << "| " << "Handle (" << h1.mIndex << ", " << h1.mCounter << ")" << std::endl;
				std::cout << "| " << "Store Size: " << store.Size() << std::endl;
				
				store.Get(h1);
			} catch (std::exception& e) {
				std::cout << "| E: " << e.what() << std::endl;
				std::cout << "'---------" << std::endl << std::endl;
			}
			
			try {
				TestHandle h(100u, 0u);
				
				std::cout << ".--- Invalid Handle (Out of Bounds)" << std::endl;
				std::cout << "| " << "Handle (" << h.mIndex << ", " << h.mCounter << ")" << std::endl;
				std::cout << "| " << "Store Size: " << store.Size() << std::endl;
				
				store.Get(h);
			} catch (std::exception& e) {
				std::cout << "| E: " << e.what() << std::endl;
				std::cout << "'---------" << std::endl << std::endl;
			}
		//
		
		// 
		try {
			std::cout << ".--- Moved and Swappped Store" << std::endl;
			
			using std::swap;
			
			uair::util::HVector<Test> store2 = std::move(store);
			uair::util::HVector<Test> store3;
			swap(store3, store2);
			
			Test& t = store3.Get(h3);
			
			std::cout << "| " << "Test (" << t.mI << ")" << std::endl;
			std::cout << "| " << "Handle (" << h3.mIndex << ", " << h3.mCounter << ")" << std::endl;
			std::cout << "| " << "Store Size: " << store.Size() << " " << store2.Size() << " " << store3.Size() << std::endl;
			std::cout << "'---------" << std::endl << std::endl;
		} catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		//
	}
	
	// pause the console so we can see any output before quitting
	std::string pause;
	std::cout << "\nEnter to continue..." << std::endl;
	std::getline(std::cin, pause);
	
	return 0; // Exit
}
