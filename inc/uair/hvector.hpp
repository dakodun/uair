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

#ifndef UAIRHVECTOR_HPP
#define UAIRHVECTOR_HPP

#include <memory>

#include "init.hpp"

namespace uair {
// from "aligned_buffer.h" from libstdc++
// an aligned memory buffer for a data member object of type T
template <class T>
struct alignedMembuf {
	public :
		// ensure buffer is aligned as a class member
		struct Ta {
			T mT;
		};
	
	public :
		alignedMembuf() = default;
		
		// prevent mStorage being zeroed when value-initialised
		alignedMembuf(std::nullptr_t);
		
		// return the address of the start of the buffer
		void* addr() noexcept;
		const void* addr() const noexcept;
		
		// return a pointer to the stored object
		T* ptr() noexcept;
		const T* ptr() const noexcept;
	
	public :
		// create an array of unsigned chars big enough to hold an object
		// of type T and align it
		alignas(alignof(Ta::mT)) unsigned char
				mStorage[sizeof(T)];
};
// ...


// from "allocataed_ptr.h" from libstdc++
// managed pointer for pointers obtained from allocators
template <class A>
struct allocatedPtr {
	public :
		using pointer = typename std::allocator_traits<A>::pointer;
		using value_type = typename std::allocator_traits<A>::value_type;
		
		// only defined if P and value_type match
		template <typename P>
		using Require = typename std::enable_if<std::is_same<P,
				value_type*>::value >::type;
	
	public :
		// store allocator and take ownership of associated pointer
		allocatedPtr(A& alloc, pointer ptr) noexcept;
		
		// store allocator, convert associated pointer to allocator's
		// pointer type (if valid) and then take ownership of it
		template < typename P, typename Req = Require<P> >
		allocatedPtr(A& alloc, P ptr);
		
		// transfer ownership to other
		allocatedPtr(allocatedPtr&& other) noexcept;
		
		// deallocate the owned pointer
		~allocatedPtr();
		
		// release ownership of the owned pointer
		allocatedPtr& operator=(std::nullptr_t) noexcept;
		
		// get the address that the owned pointer refers to
		value_type* get();
	private:
		// return the raw pointer (address)
		value_type* rawPtr(value_type* ptr);
		
		template <typename P>
		auto rawPtr(P ptr)->decltype(rawPtr(ptr.operator->()));
	
	private :
		A* mAlloc;
		pointer mPtr;
};
// ...


// namespace holding internal implementation details
namespace impl {
// base class for an entry in a hvector that contains non-throwing logic
class EXPORTDLL hvectorEntryBase {
	public :
		hvectorEntryBase();
	
	public :
		bool mActive;
		unsigned int mCounter;
		
		hvectorEntryBase* mPrev;
		hvectorEntryBase* mNext;
};
// ...


// an entry in a hvector that holds an object (T) and entry information
template <class T>
class hvectorEntry : public hvectorEntryBase {
	public :
		hvectorEntry();
		
		// return a pointer to the stored object (membuf)
		T* valptr();
		const T* valptr() const;
	
	public :
		alignedMembuf<T> mStorage;
};
// ...


// 
template <class T>
class hvectorIterator {
	public :
		typedef hvectorIterator<T> self;
		typedef hvectorEntry<T> entry;
		
		typedef std::ptrdiff_t difference_type;
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
	
	public :
		hvectorIterator() noexcept;
		explicit hvectorIterator(hvectorEntryBase* baseEntry) noexcept;
		
		// input iterator
			// return a reference to the object in the entry's buffer
			reference operator*() const noexcept;
			
			// return a pointer to the object in the entry's buffer
			pointer operator->() const noexcept;
		//
		
		// forward iterator
			self& operator++() noexcept;
			self operator++(int) noexcept;
		//
		
		// bidirectional iterator
			self& operator--() noexcept;
			self operator--(int) noexcept;
		//
		
		bool operator==(const self& other) const noexcept;
		bool operator!=(const self& other) const noexcept;
	
	public :
		// 
		hvectorEntryBase* mEntry;
};
// ...


//
template <class T>
class hvectorConstIterator {
	public :
		typedef hvectorConstIterator<T> self;
		typedef const hvectorEntry<T> entry;
		typedef hvectorIterator<T> iterator;
		
		typedef std::ptrdiff_t difference_type;
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef T value_type;
		typedef const T* pointer;
		typedef const T& reference;
	
	public :
		hvectorConstIterator() noexcept;
		explicit hvectorConstIterator(const hvectorEntryBase*
				baseEntry) noexcept;
		hvectorConstIterator(const iterator& iter) noexcept;
	  
		// input iterator
			// return a reference to the object in the entry's buffer
			reference operator*() const noexcept;
			
			// return a pointer to the object in the entry's buffer
			pointer operator->() const noexcept;
		//
		
		// forward iterator
			self& operator++() noexcept;
			self operator++(int) noexcept;
		//
		
		// bidirectional iterator
			self& operator--() noexcept;
			self operator--(int) noexcept;
		//
		
		bool operator==(const self& other) const noexcept;
		bool operator!=(const self& other) const noexcept;
	
	public :
		// 
		const hvectorEntryBase* mEntry;
};
// ...


// iterator comparison operators...
template <typename T>
inline bool operator==(const hvectorIterator<T>& iter,
		const hvectorConstIterator<T>& cIter) noexcept {
	
	return iter.mEntry == cIter.mEntry;
}

template <typename T>
inline bool operator!=(const hvectorIterator<T>& iter,
		const hvectorConstIterator<T>& cIter) noexcept {
	
	return iter.mEntry != cIter.mEntry;
}
// ...


// 
class EXPORTDLL hvectorHandle {
	public :
		hvectorHandle() = default;
		explicit hvectorHandle(const unsigned int& index,
				const unsigned int& counter) noexcept;
		unsigned int get_index() const noexcept;
		unsigned int get_counter() const noexcept;
	
	private :
		// the index of the resource in the store
		unsigned int mIndex;
		
		// the counter value of the index used to
		// validate the handle
		unsigned int mCounter;
};
// ...


// 
template <class T, class A>
class hvectorBase {
	public :
		typedef A allocType;
	protected :
		typedef typename std::allocator_traits<allocType>::template
				rebind_alloc<T> allocTypeT;
		typedef std::allocator_traits<allocTypeT> allocTraitsT;
		typedef typename allocTraitsT::template
				rebind_alloc<hvectorEntry<T>> allocTypeEntry;
		typedef std::allocator_traits<allocTypeEntry> allocTraitsEntry;
		
		typedef typename std::allocator_traits<allocTypeEntry>::pointer
				pointerEntry;
		typedef typename std::allocator_traits<allocTypeEntry>::
				const_pointer pointerEntryConst;
		
		// 
		struct hvectorImpl : public allocTypeEntry {
			public :
				hvectorImpl() noexcept;
				hvectorImpl(const allocTypeEntry& alloc) noexcept;
				hvectorImpl(allocTypeEntry&& alloc) noexcept;
				
				void swapWith(hvectorImpl& other) noexcept;
			
			public :
				// the start and end of the block of allocated memory
				pointerEntry mAllocStart;
				pointerEntry mAllocEnd;
				
				// the furthest initialised entry
				pointerEntry mInitLast;
				
				// holds the entry count for the hvector and also points
				// at the first and last entries in the hvector
				hvectorEntry<size_t> mEntry;
		};
	
	public :
		hvectorBase();
		hvectorBase(const allocType& alloc) noexcept;
		hvectorBase(const size_t& size, const allocType& alloc);
		hvectorBase(allocType&& alloc) noexcept;
		
		hvectorBase(hvectorBase&& other) noexcept;
		hvectorBase(hvectorBase&& other, const allocType& alloc);
		
		~hvectorBase() noexcept;
		
		allocTypeEntry& getAllocatorEntry() noexcept;
		const allocTypeEntry& getAllocatorEntry() const noexcept;
		
		pointerEntry allocate(const size_t& size);
		void deallocate(const pointerEntry& ptr, const size_t& size);
	protected :
		// initialise the size and pointers (first and last) of
		// the hvector's entry
		void init();
		
		// swap the value and pointers (first and last entry) of
		// the hvector's entry with the other hvector's
		void updateEntry(hvectorBase&& other);
		
		// modify the value (entry count) of the hvector's entry
		size_t getSize() const;
		void setSize(size_t newSize);
		void incSize(size_t incAmount);
		void decSize(size_t decAmount);
	private :
		// allocate memory and set start, end and initialised pointers
		void create_storage(const size_t& size);
	
	public :
		hvectorImpl mImpl;
};
// ...


// 
// whether or not an allocator can be moved without throwing an exception
template <typename A>
static constexpr bool isAllocNoThrow() {
	typedef std::allocator_traits<A> alloc;
	
	// return true if allocator can be moved or is always
	// equal to other allocators
	return (alloc::propagate_on_container_move_assignment::value ||
			alloc::is_always_equal::value);
}

// copies an allocator (second) to another (first)
template <typename A>
inline void doAllocOnCopy(A& first, const A& second, std::true_type) {
	first = second;
}

// empty overload when allocators can't be copied 
template <typename A>
inline void doAllocOnCopy(A& first, const A& second, std::false_type) {

}

// copies an allocator (second) to another (first)
// if possible, otherwise does nothing
template <typename A>
inline void allocOnCopy(A& first, const A& second) {
	typedef std::allocator_traits<A> alloc;
	typedef typename alloc::propagate_on_container_copy_assignment prop;
	
	doAllocOnCopy(first, second, prop());
}

// moves an allocator (second) to another (first) via move assignment
template <typename A>
inline void doAllocOnMove(A& first, A& second, std::true_type) {
	first = std::move(second);
}

// empty overload when allocators can't be moved
template <typename A>
inline void doAllocOnMove(A& first, A& second, std::false_type) {

}

// moves an allocator (second) to another (first) via move assignment
// if possible, otherwise does nothing
template <typename A>
inline void allocOnMove(A& first, A& second) {
	typedef std::allocator_traits<A> alloc;
	typedef typename alloc::propagate_on_container_move_assignment prop;
	
	doAllocOnMove(first, second, prop());
}
// ...
}
// ...


//
template < class T, class A = std::allocator<T> >
class hvector : protected impl::hvectorBase<T, A> {
	private :
		typedef typename A::value_type allocValueType;
		
		typedef impl::hvectorBase<T, A> base;
		typedef typename base::allocTypeT allocTypeT;
		typedef typename base::allocTraitsT allocTraitsT;
		typedef typename base::allocTypeEntry allocTypeEntry;
		typedef typename base::allocTraitsEntry allocTraitsEntry;
		typedef typename base::pointerEntry pointerEntry;
		typedef typename base::pointerEntryConst pointerEntryConst;
	public :
		typedef T value_type;
		typedef typename allocTraitsT::pointer pointer;
		typedef typename allocTraitsT::const_pointer const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef impl::hvectorIterator<T> iterator;
		typedef impl::hvectorConstIterator<T> const_iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator>
				const_reverse_iterator;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef A allocator_type;
		
		typedef impl::hvectorHandle handle;
	protected :
		typedef impl::hvectorEntry<T> entry;
		
		using base::mImpl;
		
		// true if the iterator (InIter) is convertible to an input iterator
		template <typename InIter>
		using RequireInIter = typename std::enable_if<std::is_convertible<
				typename std::iterator_traits<InIter>::iterator_category,
				std::input_iterator_tag>::value>::type;
	
	public :
		// construct/copy/move/destroy
			hvector() noexcept(
					std::is_nothrow_default_constructible<A>::value);
			explicit hvector(const allocator_type& alloc) noexcept;
			
			hvector(const hvector& other);
			hvector(hvector&& other) noexcept;
			
			~hvector();
			
			hvector& operator=(const hvector& other);
			hvector& operator=(hvector&& other) noexcept(
					impl::isAllocNoThrow<allocTypeEntry>());
			
			allocator_type get_allocator() const noexcept;
		//
		
		// iterators
			iterator begin() noexcept;
			const_iterator begin() const noexcept;
			
			iterator end() noexcept;
			const_iterator end() const noexcept;
			
			reverse_iterator rbegin() noexcept;
			const_reverse_iterator rbegin() const noexcept;;
			
			reverse_iterator rend() noexcept;
			const_reverse_iterator rend() const noexcept;
			
			const_iterator cbegin() noexcept;
			const_iterator cend() noexcept;
			
			const_reverse_iterator crbegin() const noexcept;
			const_reverse_iterator crend() const noexcept;
		//
		
		// capacity
			size_type size() const noexcept;
			size_type count() const noexcept;
			size_type max_size() const noexcept;
			size_type capacity() const noexcept;
			bool empty() const noexcept;
			
			void reserve(size_type vsize);
		//
		
		// element access
			reference operator[](const handle& h) noexcept;
			const_reference operator[](const handle& h) const noexcept;
			
			reference at(const handle& h);
			const_reference at(const handle& h) const;
			
			handle get_handle(const size_type& index) const;
			handle get_handle(const iterator& iter) const;
			handle get_handle(const const_iterator& cIter) const;
		//
		
		// modifiers
			template<typename... Ps>
			handle emplace(Ps&&... params);
			
			handle push(const value_type& obj);
			handle push(value_type&& obj);
			
			void pop(const handle& h);
			
			void swapWith(hvector& other) noexcept;
			friend void swap(hvector& first, hvector& second)
					noexcept(noexcept(first.swapWith(second))) {
				
				first.swapWith(second);
			}
			
			void clear();
		//
	protected :
		template <typename... Ps>
		handle insertEntry(Ps&&... params);
		
		template <typename... Ps>
		entry* constructEntry(pointerEntry ptr, Ps&&... params);
		
		template <typename Aa>
		void clearEntries(Aa& alloc) noexcept;
		
		template <typename FwdIter, typename Aa>
		unsigned int  clearEntries(FwdIter start, FwdIter end,
				Aa& alloc) noexcept;
		
		// allocate space and then copy (or move) this hvector into it,
		// and then destroy original
		template <typename FwdIter>
		pointerEntry allocateCopyAndDestroy(size_type vsize, FwdIter start,
				FwdIter end);
		
		// as above but without destroying afterwards (and only copying)
		template <typename FwdIter>
		pointerEntry allocateCopy(size_type vsize, FwdIter start,
				FwdIter end);
		
		// copy (or move) range [start...end] into memory block start at
		// result using the supplied allocator and then destroy original
		template<typename InIter, typename FwdIter, typename Aa>
		void uninitializedCopyAndDestroy(InIter start, InIter end,
				FwdIter result, Aa& alloc);
		
		// as above but without destroying afterwards (and only copying)
		template<typename InIter, typename FwdIter, typename Aa>
		void uninitializedCopy(InIter start, InIter end,
				FwdIter result, Aa& alloc);
		
		// as above but also dealing with initialised memory
		template<typename InIter, typename FwdIter, typename Aa>
		void initializedCopy(InIter start, InIter end,
				FwdIter result, Aa& alloc);
		
		// swaps an allocator with another
		void doAllocOnSwap(allocTypeEntry& first,
				allocTypeEntry& second, std::true_type);
		
		// empty overload when allocators can't be swapped
		void doAllocOnSwap(allocTypeEntry& first,
				allocTypeEntry& second, std::false_type);
		
		// move a hvector (other) to this if allocators can
		// be moved without throwing an exception
		void moveAssign(hvector&& other, std::true_type) noexcept;
		
		// move a hvector (other) to this when allocators might
		// throw exceptions
		void moveAssign(hvector&& other, std::false_type);
};
// ...
}

#include "hvector.inl"

#endif
