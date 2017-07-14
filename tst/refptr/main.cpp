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
#include <list>
#include <functional>

#include "uair/uair.hpp"

class Object : public uair::RefType {
	public :
		Object() = default;
		
		// copying objects would be ill-formed
		Object(const Object& other) = delete;
		
		Object(Object&& other) : Object() {
			swap(*this, other);
		}
		
		~Object() {
			
		}
		
		Object& operator=(Object other) {
			swap(*this, other);
			
			return *this;
		}
		
		friend void swap(Object& first, Object& second) {
			using std::swap;
			
			// invoke the base class swap to swap base members first
			swap(static_cast<uair::RefType&>(first),
					static_cast<uair::RefType&>(second));
			
			swap(first.mUID, second.mUID);
		}
	
	public :
		unsigned int mUID = 0u;
};

class TestBed {
	public :
		TestBed() {
			mTests.emplace(std::make_pair('a', [](){
				Object obj1; obj1.mUID = 1u;
				Object obj2; obj2.mUID = 2u;
				
				std::cout << &obj1 << " " << obj1 << " / " <<
						&obj2 << " " << obj2 << std::endl;
				
				uair::RefPtr<Object> ptr;
				ptr.Set(&obj1);
				
				std::cout << &obj1 << " " << obj1 << " / " <<
						&obj2 << " " << obj2 << " / " << ptr << std::endl;
				
				ptr.Set(&obj2);
				
				std::cout << &obj1 << " " << obj1 << " / " <<
						&obj2 << " " << obj2 << " / " << ptr << std::endl;
				
				Object* objPtr = ptr.Get();
				const Object* cobjPtr = ptr.Get();
				if ((objPtr && cobjPtr && ptr) && (objPtr == cobjPtr &&
						objPtr == &(*ptr))) {
					
					std::cout << &obj1 << " " << obj1 << " / " <<
						&obj2 << " " << obj2 << " / " << ptr << std::endl;
				}
				else {
					std::cout << "error" << std::endl;
				}
				
				ptr.Unset();
				objPtr = ptr.Get();
				cobjPtr = ptr.Get();
				if (objPtr || cobjPtr || ptr) {
					std::cout << "error" << std::endl;
				}
				else {
					std::cout << &obj1 << " " << obj1 << " / " <<
						&obj2 << " " << obj2 << " / " << ptr << std::endl;
				}
				
				ptr.Set(&obj1);
				
				std::cout << &obj1 << " " << obj1 << " / " <<
						&obj2 << " " << obj2 << " / " << ptr << std::endl;
				
				std::cout << *ptr << " / " << ptr->mUID << std::endl;
			}));
			
			mTests.emplace(std::make_pair('b', [](){
				uair::RefPtr<Object> ptr1;
				uair::RefPtr<Object> ptr2;
				
				{
					Object obj1; obj1.mUID = 1u;
					Object obj2; obj2.mUID = 2u;
					ptr1.Set(&obj1);
					ptr2.Set(&obj2);
					
					std::cout << &obj1 << " " << obj1 << " / " <<
							&obj2 << " " << obj2 << std::endl;
					std::cout << "    " << ptr1 << " / " <<
							ptr2 << std::endl;
					
					Object obj3 = std::move(obj1);
					
					std::cout << &obj1 << " " << obj1 << " / " <<
							&obj2 << " " << obj2 << " / " <<
							&obj3 << " " << obj3 << std::endl;
					std::cout << "    " << ptr1 << " / " <<
							ptr2 << std::endl;
					
					Object obj4; obj4 = std::move(obj2);
					
					std::cout << &obj1 << " " << obj1 << " / " <<
							&obj2 << " " << obj2 << " / " <<
							&obj3 << " " << obj3 << " / " <<
							&obj4 << " " << obj4 << std::endl;
					std::cout << "    " << ptr1 << " / " <<
							ptr2 << std::endl;
					
					using std::swap;
					swap(obj3, obj4);
					
					std::cout << &obj1 << " " << obj1 << " / " <<
							&obj2 << " " << obj2 << " / " <<
							&obj3 << " " << obj3 << " / " <<
							&obj4 << " " << obj4 << std::endl;
					std::cout << "    " << ptr1 << " / " <<
							ptr2 << std::endl;
				}
				
				std::cout << "    " << ptr1 << " / " <<
						ptr2 << std::endl;
			}));
			
			mTests.emplace(std::make_pair('c', [](){
				Object obj1; obj1.mUID = 1u;
				Object obj2; obj2.mUID = 2u;
				
				{
					uair::RefPtr<Object> ptr1;
					std::cout << ptr1 << std::endl;
					std::cout << "    " << &obj1 << " " << obj1 << " / " <<
							&obj2 << " " << obj2 << std::endl;
					
					ptr1.Set(&obj1);
					uair::RefPtr<Object> ptr2(&obj2);
					std::cout << ptr1 << " / " <<
							ptr2 << std::endl;
					std::cout << "    " << &obj1 << " " << obj1 << " / " <<
							&obj2 << " " << obj2 << std::endl;
					
					uair::RefPtr<Object> ptr3 = ptr1;
					std::cout << ptr1 << " / " <<
							ptr2 << " / " <<
							ptr3 << std::endl;
					std::cout << "    " << &obj1 << " " << obj1 << " / " <<
							&obj2 << " " << obj2 << std::endl;
					
					uair::RefPtr<Object> ptr4; ptr4 = ptr2;
					std::cout << ptr1 << " / " <<
							ptr2 << " / " <<
							ptr3 << " / " <<
							ptr4 << std::endl;
					std::cout << "    " << &obj1 << " " << obj1 << " / " <<
							&obj2 << " " << obj2 << std::endl;
					
					uair::RefPtr<Object> ptr5 = std::move(ptr1);
					std::cout << ptr1 << " / " <<
							ptr2 << " / " <<
							ptr3 << " / " <<
							ptr4 << " / " <<
							ptr5 << std::endl;
					std::cout << "    " << &obj1 << " " << obj1 << " / " <<
							&obj2 << " " << obj2 << std::endl;
					
					uair::RefPtr<Object> ptr6; ptr6 = std::move(ptr2);
					std::cout << ptr1 << " / " <<
							ptr2 << " / " <<
							ptr3 << " / " <<
							ptr4 << " / " <<
							ptr5 << " / " <<
							ptr6 << std::endl;
					std::cout << "    " << &obj1 << " " << obj1 << " / " <<
							&obj2 << " " << obj2 << std::endl;;
					
					using std::swap;
					swap(ptr5, ptr6);
					std::cout << ptr1 << " / " <<
							ptr2 << " / " <<
							ptr3 << " / " <<
							ptr4 << " / " <<
							ptr5 << " / " <<
							ptr6 << std::endl;
					std::cout << "    " << &obj1 << " " << obj1 << " / " <<
							&obj2 << " " << obj2 << std::endl;
				}
				
				std::cout << "    " << &obj1 << " " << obj1 << " / " <<
						&obj2 << " " << obj2 << std::endl;
			}));
		}
		
		void Run() {
			std::string choice = " ";
			while (!choice.empty() && choice.front() != '0') {
				DisplayMenu();
				std::getline(std::cin, choice);
				std::cout << std::endl;
				PerformChoice(choice);
				std::cout << "\n\n\n" << std::endl;
			}
		}
	private :
		void DisplayMenu() {
			std::cout << ".--- run test" << std::endl;
			std::cout << "| a. (derived) reftype and refptr" << std::endl;
			std::cout << "| b. (derived) reftype operations" << std::endl;
			std::cout << "| c. refptr operations" << std::endl;
			std::cout << "| d. run all" << std::endl;
			std::cout << "| " << std::endl;
			std::cout << "| leave blank to exit" << std::endl;
			std::cout << "'---------" << std::endl;
			std::cout << "> ";
		}
		
		void PerformChoice(const std::string& choice) {
			if (!choice.empty()) {
				if (choice.front() == 'd') {
					for (auto& test: mTests) {
						test.second();
						std::cout << std::endl;
					}
				}
				else {
					auto test = mTests.find(choice.front());
					if (test != mTests.end()) {
						test->second();
					}
					else {
						std::cout << "invalid selection" << std::endl;
					}
				}
			}
		}
	
	private :
		std::map< char, std::function<void()> > mTests;
};

int main(int argc, char* argv[]) {
	TestBed tb;
	tb.Run();
	
	return 0; // exit
}
