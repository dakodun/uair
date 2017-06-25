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

#ifndef UAIRUTIL_HPP
#define UAIRUTIL_HPP

#include <sstream>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include <queue>

#include "init.hpp"

#if defined(UAIRWIN32)
	#include "utilwin32.hpp"
#elif defined(UAIRLINUX)
	#include "utillinux.hpp"
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace uair {
namespace util {
EXPORTDLL extern const float EPSILON;
EXPORTDLL extern const double PI;
EXPORTDLL extern const double PIOVER180;
EXPORTDLL extern const double C180OVERPI;
EXPORTDLL extern unsigned int LOGLEVEL;
EXPORTDLL extern std::string LOGLOCATION;

enum {
	LessThan = 0u,
	LessThanEquals,
	Equals,
	GreaterThanEquals,
	GreaterThan
};

EXPORTDLL extern bool CompareFloats(const float& first, const unsigned int& comparison, const float& second, const float& variance = util::EPSILON);
EXPORTDLL extern int FloatToInt(const float& f);

EXPORTDLL extern glm::mat3 GetTranslationMatrix(const glm::vec2& translation);
EXPORTDLL extern glm::mat3 GetRotationMatrix(const float& angle);
EXPORTDLL extern glm::mat3 GetScalingMatrix(const glm::vec2& scale);
EXPORTDLL extern glm::mat3 GetSkewingMatrix(const glm::vec2& skew);

EXPORTDLL extern unsigned int NextPowerOf2(const unsigned int& input);
EXPORTDLL extern int IsConvex(const glm::vec2& pointA, const glm::vec2& pointB, const glm::vec2& pointC);
EXPORTDLL extern glm::vec2 ReflectPointByLine(const glm::vec2& pointA, const glm::vec2& pointB, const glm::vec2& pointC);
EXPORTDLL extern glm::vec3 RotatePointAroundAxis(const glm::vec3& pointA, const glm::vec3& axis, const float& angle);

EXPORTDLL extern float AngleBetweenVectors(const glm::vec2& vecA, const glm::vec2& vecB);

EXPORTDLL extern std::string GetGLErrorStatus();
EXPORTDLL extern void LogMessage(const unsigned int& level, const std::string& message);
EXPORTDLL extern void LogMessage(const unsigned int& level, const std::string& message, const std::string& filename);

EXPORTDLL extern std::vector<std::string> SplitString(const std::string& string, const char& delimiter);
EXPORTDLL extern std::u16string UTF8toUTF16(const std::string& input);
EXPORTDLL extern std::string UTF16toUTF8(const std::u16string& input);

template<typename T>
extern int SignOf(const T& input) {
	if (input < 0) {
		return -1;
	}
	else if (input > 0) {
		return 1;
	}
	
	return 0;
}

template <class T>
extern std::string ToString(const T& t, int precision = -1) {
	std::stringstream ss;
	
	if (precision >= 0) {
		ss.precision(precision);
	}
	
	ss << std::fixed << t;
	
	return ss.str();
}

template <class T>
extern T FromString(const std::string& s, std::ios_base& (*f)(std::ios_base&) = std::dec) {
	T t;
	
	std::istringstream iss(s);
	iss >> f >> t;
	
	return t;
}

template<typename F, typename... Ps, std::size_t... Is>
void IterateTuple(F func, std::tuple<Ps...>& tuple, std::index_sequence<Is...>) {
	using expander = int[]; // an alias allowing us to use brace list expansion
	
	// expand the tuple inside a brace list in order to iterate through its elements
	(void)expander{0, ((void)func(std::get<Is>(tuple)), 0)...};
}

template<typename F, typename... Ps, std::size_t... Is>
auto ExpandTuple(F func, std::tuple<Ps...>& tuple, std::index_sequence<Is...>) {
	return func(std::get<Is>(tuple)...); // pass the expanded tuple elements as arguments to the function
}

template <class T>
class HVector {
	public :
		// an entry into the store that holds the object as well as data to ensure validity
		class Entry {
			public :
				// create an entry (T) by copying or moving an exisiting object
				explicit Entry(const T& value);
				
				// create an entry (T) in place using constructor parameters
				template <typename ...Ps>
				explicit Entry(Ps&&... params);
				
				Entry(const Entry& other) = delete; // std::unique_ptr can't be copied
				Entry(Entry&& other); // std::unique_ptr can be moved
				
				Entry& operator=(Entry other);
				
				friend void swap(Entry& first, Entry& second) {
					using std::swap;
					
					swap(first.mActive, second.mActive);
					swap(first.mCounter, second.mCounter);
					swap(first.mData, second.mData);
				}
			
			public :
				bool mActive = true; // is the entry active (i.e., has it been removed)
				unsigned int mCounter = 0u; // the counter that ensures handles are still valid (i.e., its resource hasn't been removed and replaced)
				std::unique_ptr<T> mData; // a pointer to an object of the type that this store handles
		};
		
		// a handle that is used to refer to resources handled instead of a pointer
		class Handle {
			public :
				Handle() = default;
				
				explicit Handle(const unsigned int& index, const unsigned int& counter) : mIndex(index), mCounter(counter) {
					
				}
			public :
				unsigned int mIndex = 0u; // the index of the resource in the store
				unsigned int mCounter = 0u; // the counter value of the index used to validate the handle
		};
	
	public :
		HVector();
		explicit HVector(const unsigned int& reserveCap);
		HVector(const HVector& other) = delete; // std::unique_ptr can't be copied
		HVector(HVector&& other); // std::unique_ptr can be moved
		
		HVector& operator=(HVector other);
		
		friend void swap(HVector& first, HVector& second) {
			using std::swap;
			
			swap(first.mReserveCap, second.mReserveCap);
			
			swap(first.mStore, second.mStore);
			swap(first.mFreeIndices, second.mFreeIndices);
		}
		
		Handle Push(const T& value); // add an entry to the store via copying
		Handle Push(T&& value); // add an entry to the store using move semantics
		
		template <typename ...Ps>
		Handle Emplace(Ps&&... params); // construct an entry in the store
		
		void Pop(const Handle& handle); // remove an entry from the store
		T& Get(const Handle& handle); // return a reference to an entry in the store
		
		unsigned int Size() const;
	
	public :
		unsigned int mReserveCap = 100u; // how many slots to reserve in the store
	private :
		std::vector<Entry> mStore; // the resources handled by this store
		
		// a sequential list (ascending) of indices that were previously occupied but are now freed and available for re-use
		std::priority_queue<unsigned int, std::vector<unsigned int>, std::greater<unsigned int>> mFreeIndices;
};
}
}

#include "util.inl"
#endif
