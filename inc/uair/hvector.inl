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

namespace uair {
// alignedMembuf...
template <class T>
alignedMembuf<T>::alignedMembuf(std::nullptr_t) {
	
}

template <class T>
void* alignedMembuf<T>::addr() noexcept {
	return static_cast<void*>(&mStorage);
}

template <class T>
const void* alignedMembuf<T>::addr() const noexcept {
	return static_cast<const void*>(&mStorage);
}

template <class T>
T* alignedMembuf<T>::ptr() noexcept {
	return static_cast<T*>(addr());
}

template <class T>
const T* alignedMembuf<T>::ptr() const noexcept {
	return static_cast<const T*>(addr());
}
// ...


// allocatedPtr...
template <class A>
allocatedPtr<A>::allocatedPtr(A& alloc, pointer ptr) noexcept
		: mAlloc(std::addressof(alloc)), mPtr(ptr) {
	
	
}

template <class A>
template <typename P, typename Req>
allocatedPtr<A>::allocatedPtr(A& alloc, P ptr) :
		mAlloc(std::addressof(alloc)),
		mPtr(std::pointer_traits<pointer>::pointer_to(*ptr)) {
	
	
}

template <class A>
allocatedPtr<A>::allocatedPtr(allocatedPtr&& other) noexcept :
		mAlloc(other.mAlloc), mPtr(other.mPtr) {
	
	other.mPtr = nullptr; // release ownership
}

template <class A>
allocatedPtr<A>::~allocatedPtr() {
	if (mPtr != nullptr) { // if we own a pointer...
		// we're in charge of cleaning up
		std::allocator_traits<A>::deallocate(*mAlloc, mPtr, 1);
	}
}

template <class A>
allocatedPtr<A>& allocatedPtr<A>::operator=(std::nullptr_t) noexcept {
	mPtr = nullptr; // release ownership
	return *this;
}

template <class A>
typename allocatedPtr<A>::value_type* allocatedPtr<A>::get() {
	return rawPtr(mPtr);
}

template <class A>
typename allocatedPtr<A>::value_type* allocatedPtr<A>::rawPtr(value_type* ptr) {
	return ptr;
}

template <class A>
template <typename P>
auto allocatedPtr<A>::rawPtr(P ptr)->decltype(rawPtr(ptr.operator->())) {
	return rawPtr(ptr.operator->());
}
// ...


namespace impl {
// hvectorEntry...
template <class T>
hvectorEntry<T>::hvectorEntry() : hvectorEntryBase() {
	
}

template <class T>
T* hvectorEntry<T>::valptr() {
	return mStorage.ptr();
}

template <class T>
const T* hvectorEntry<T>::valptr() const {
	return mStorage.ptr();
}
// ...


// hvectorIterator...
template <class T>
hvectorIterator<T>::hvectorIterator() noexcept : mEntry() {
	
}

template <class T>
hvectorIterator<T>::hvectorIterator(hvectorEntryBase* baseEntry)
		noexcept : mEntry(baseEntry) {
	
	
}

template <class T>
typename hvectorIterator<T>::reference
		hvectorIterator<T>::operator*() const noexcept {
	
	return *(static_cast<entry*>(mEntry)->valptr());
}

template <class T>
typename hvectorIterator<T>::pointer
		hvectorIterator<T>::operator->() const noexcept {
	
	return static_cast<entry*>(mEntry)->valptr();
}

template <class T>
typename hvectorIterator<T>::self&
		hvectorIterator<T>::operator++() noexcept {
	
	mEntry = mEntry->mNext;
	return *this;
}

template <class T>
typename hvectorIterator<T>::self
		hvectorIterator<T>::operator++(int) noexcept {
	
	self temp = *this;
	mEntry = mEntry->mNext;
	return temp;
}

template <class T>
typename hvectorIterator<T>::self&
		hvectorIterator<T>::operator--() noexcept {
	
	mEntry = mEntry->mPrev;
	return *this;
}

template <class T>
typename hvectorIterator<T>::self
		hvectorIterator<T>::operator--(int) noexcept {
	
	self temp = *this;
	mEntry = mEntry->mPrev;
	return temp;
}

template <class T>
bool hvectorIterator<T>::operator==(const self& other)
		const noexcept {
	
	return mEntry == other.mEntry;
}

template <class T>
bool hvectorIterator<T>::operator!=(const self& other)
		const noexcept {
	
	return mEntry != other.mEntry;
}
// ...


// hvectorConstIterator...
template <class T>
hvectorConstIterator<T>::hvectorConstIterator() noexcept : mEntry() {
	
}

template <class T>
hvectorConstIterator<T>::hvectorConstIterator(const hvectorEntryBase*
		baseEntry) noexcept : mEntry(baseEntry) {
	
	
}

template <class T>
hvectorConstIterator<T>::hvectorConstIterator(const iterator& iter) noexcept
		: mEntry(iter.mEntry) {
	
	
}

template <class T>
typename hvectorConstIterator<T>::reference
		hvectorConstIterator<T>::operator*() const noexcept {
	
	return *(static_cast<entry*>(mEntry)->valptr());
}

template <class T>
typename hvectorConstIterator<T>::pointer
		hvectorConstIterator<T>::operator->() const noexcept {
	
	return static_cast<entry*>(mEntry)->valptr();
}

template <class T>
typename hvectorConstIterator<T>::self&
		hvectorConstIterator<T>::operator++() noexcept {
	
	mEntry = mEntry->mNext;
	return *this;
}

template <class T>
typename hvectorConstIterator<T>::self
		hvectorConstIterator<T>::operator++(int) noexcept {
	
	self temp = *this;
	mEntry = mEntry->mNext;
	return temp;
}

template <class T>
typename hvectorConstIterator<T>::self&
		hvectorConstIterator<T>::operator--() noexcept {
	
	mEntry = mEntry->mPrev;
	return *this;
}

template <class T>
typename hvectorConstIterator<T>::self
		hvectorConstIterator<T>::operator--(int) noexcept {
	
	self temp = *this;
	mEntry = mEntry->mPrev;
	return temp;
}

template <class T>
bool hvectorConstIterator<T>::operator==(const self& other)
		const noexcept {
	
	return mEntry == other.mEntry;
}

template <class T>
bool hvectorConstIterator<T>::operator!=(const self& other)
		const noexcept {
	
	return mEntry != other.mEntry;
}
// ...


// hvectorBase::hvectorImpl...
template <class T, class A>
hvectorBase<T, A>::hvectorImpl::hvectorImpl() noexcept :
		allocTypeEntry(), mAllocStart(),
		mAllocEnd(), mInitLast(), mEntry() {
	
	
}

template <class T, class A>
hvectorBase<T, A>::hvectorImpl::hvectorImpl(const allocTypeEntry& alloc)
		noexcept : allocTypeEntry(alloc), mAllocStart(), mAllocEnd(),
		mInitLast(), mEntry() {
	
	
}

template <class T, class A>
hvectorBase<T, A>::hvectorImpl::hvectorImpl(allocTypeEntry&& alloc)
		noexcept : allocTypeEntry(std::move(alloc)), mAllocStart(),
		mAllocEnd(), mInitLast(), mEntry() {
	
	
}

template <class T, class A>
void hvectorBase<T, A>::hvectorImpl::swapWith(hvectorImpl& other) noexcept {
	std::swap(mAllocStart, other.mAllocStart);
	std::swap(mAllocEnd, other.mAllocEnd);
	
	std::swap(mInitLast, other.mInitLast);
}
// ...


// hvectorBase...
template <class T, class A>
hvectorBase<T, A>::hvectorBase() : mImpl() {
	init();
}

template <class T, class A>
hvectorBase<T, A>::hvectorBase(const allocType& alloc) noexcept
		: mImpl(alloc) {
	
	init();
}

template <class T, class A>
hvectorBase<T, A>::hvectorBase(const size_t& size, const allocType& alloc) :
		mImpl(alloc) {
	
	init();
	
	create_storage(size);
}

template <class T, class A>
hvectorBase<T, A>::hvectorBase(allocType&& alloc) noexcept
		: mImpl(std::move(alloc)) {
	
	init();
}

template <class T, class A>
hvectorBase<T, A>::hvectorBase(hvectorBase&& other) noexcept
		: mImpl(std::move(other.getAllocatorEntry())) {
	
	mImpl.swapWith(other.mImpl);
	updateEntry(std::move(other));
}

template <class T, class A>
hvectorBase<T, A>::hvectorBase(hvectorBase&& other, const allocType& alloc)
		: mImpl(alloc) {
	
	if (other.get_allocator() == alloc) { // if the allocator's match...
		mImpl.swapWith(other.mImpl);
		updateEntry(std::move(other));
	}
	else {
		init();
	}
}

template <class T, class A>
hvectorBase<T, A>::~hvectorBase() noexcept {
	deallocate(this->mImpl.mAllocStart, this->mImpl.mAllocEnd -
			this->mImpl.mAllocStart);
}

template <class T, class A>
typename hvectorBase<T, A>::allocTypeEntry&
		hvectorBase<T, A>::getAllocatorEntry() noexcept {
	
	return mImpl;
}

template <class T, class A>
const typename hvectorBase<T, A>::allocTypeEntry&
		hvectorBase<T, A>::getAllocatorEntry() const noexcept {
	
	return mImpl;
}

template <class T, class A>
typename hvectorBase<T, A>::pointerEntry
		hvectorBase<T, A>::allocate(const size_t& size) {
	
	typedef std::allocator_traits<allocTypeEntry> alloc;
	
	if (size != 0) {
		return alloc::allocate(mImpl, size);
	}
	else {
		return pointerEntry();
	}
}

template <class T, class A>
void hvectorBase<T, A>::deallocate(const pointerEntry& ptr,
		const size_t& size) {
	
	typedef std::allocator_traits<allocTypeEntry> alloc;
	
	if (ptr) {
		alloc::deallocate(mImpl, ptr, size);
	}
}

template <class T, class A>
void hvectorBase<T, A>::init() {
	setSize(0u);
	
	mImpl.mEntry.mNext = &mImpl.mEntry;
	mImpl.mEntry.mPrev = &mImpl.mEntry;
}

template <class T, class A>
void hvectorBase<T, A>::updateEntry(hvectorBase&& other) {
	if (other.mImpl.mEntry.mNext == &other.mImpl.mEntry) { // if the other
		// hvector is empty...
		
		// intialise this hvector to a default state
		init();
	}
	else {
		// set this hvector's entry to point to the first and
		// last entries of the other hvector
		mImpl.mEntry.mNext = other.mImpl.mEntry.mNext;
		mImpl.mEntry.mPrev = other.mImpl.mEntry.mPrev;
		
		// update the first and last entries to point back at this hvector
		mImpl.mEntry.mNext->mPrev = &mImpl.mEntry;
		mImpl.mEntry.mPrev->mNext = &mImpl.mEntry;
		
		// copy the entry count from the other hvector
		setSize(other.getSize());
		
		// reinitalise the other hvector back to a default state
		other.init();
	}
}

template <class T, class A>
size_t hvectorBase<T, A>::getSize() const {
	return *(mImpl.mEntry.valptr());
}

template <class T, class A>
void hvectorBase<T, A>::setSize(size_t newSize) {
	*(mImpl.mEntry.valptr()) = newSize;
}

template <class T, class A>
void hvectorBase<T, A>::incSize(size_t incAmount) {
	*(mImpl.mEntry.valptr()) += incAmount;
}

template <class T, class A>
void hvectorBase<T, A>::decSize(size_t decAmount) {
	*(mImpl.mEntry.valptr()) -= decAmount;
}

template <class T, class A>
void hvectorBase<T, A>::create_storage(const size_t& size) {
	this->mImpl.mAllocStart = this->allocate(size);
	this->mImpl.mAllocEnd = this->mImpl.mAllocStart + size;
	
	this->mImpl.mInitLast = this->mImpl.mAllocStart;
}
// ...
}

// hvector...
template <class T, class A>
hvector<T, A>::hvector() noexcept(std::is_nothrow_default_constructible<A>::
		value) : base() {
	
	
}

template <class T, class A>
hvector<T, A>::hvector(const allocator_type& alloc) noexcept :
		base(alloc) {
	
	
}

template <class T, class A>
hvector<T, A>::hvector(const hvector& other) : base(other.size(),
		allocTraitsEntry::select_on_container_copy_construction(
		other.getAllocatorEntry())) {
	
	uninitializedCopy(other.mImpl.mAllocStart, other.mImpl.mInitLast,
			this->mImpl.mAllocStart, this->getAllocatorEntry());
	
	// store the old distances
	size_type offset = other.size();
	size_type offsetNext;
	size_type offsetPrev;
	
	if (other.mImpl.mEntry.mNext != &other.mImpl.mEntry) { // if there
		// is at least one active entry...
		
		// store the offset from the beginning of the allocated memory
		// to the first and last entries
		offsetNext = (static_cast<impl::hvectorEntry<T>*>
				(other.mImpl.mEntry.mNext) - other.mImpl.mAllocStart);
		offsetPrev = (static_cast<impl::hvectorEntry<T>*>
				(other.mImpl.mEntry.mPrev) - other.mImpl.mAllocStart);
	}
	
	// update pointers to point to new memory block
	this->mImpl.mInitLast = this->mImpl.mAllocStart + offset;
	
	if (other.mImpl.mEntry.mNext != &other.mImpl.mEntry) { // if there
		// was at least one active entry in the container...
		
		// update the pointer to the first and last active entries to
		// point at the new memory location
		this->mImpl.mEntry.mNext = this->mImpl.mAllocStart + offsetNext;
		this->mImpl.mEntry.mPrev = this->mImpl.mAllocStart + offsetPrev;
		
		// update the fir and last active entries to point back to the
		// hvector's entry
		(this->mImpl.mEntry.mNext)->mPrev = &this->mImpl.mEntry;
		(this->mImpl.mEntry.mPrev)->mNext = &this->mImpl.mEntry;
		
		// 
		this->setSize(other.getSize());
	}
	else {
		// set the size to 0 and point the hvector's entry to itself
		this->init();
	}
}

template <class T, class A>
hvector<T, A>::hvector(hvector&& other) noexcept : base(std::move(other)) {
	
}

template <class T, class A>
hvector<T, A>::~hvector() {
	clear();
}

template <class T, class A>
hvector<T, A>& hvector<T, A>::operator=(const hvector& other) {
	if (&other != this) { // if we aren't self assigning...
		typedef typename allocTraitsEntry::
				propagate_on_container_copy_assignment prop;
		
		if (prop()) { // if we can replace this allocator with
			// other allocator...
			
			// if allocators don't equate...
			if (!allocTraitsEntry::is_always_equal::value &&
					this->getAllocatorEntry() !=
					other.getAllocatorEntry()) {
				
				// new allocator can't deallocate existing so
				// do it using the existing allocator
				clear();
				this->deallocate(this->mImpl.mAllocStart,
						this->mImpl.mAllocEnd - this->mImpl.mAllocStart);
				
				this->mImpl.mAllocStart = pointerEntry();
				this->mImpl.mInitLast = pointerEntry();
				this->mImpl.mAllocEnd = pointerEntry();
				this->init();
			}
			
			// copies other allocator to this
			allocOnCopy(this->getAllocatorEntry(),
					other.getAllocatorEntry());
		}
		
		size_type offsetNext;
		size_type offsetPrev;
		
		if (other.mImpl.mEntry.mNext != &other.mImpl.mEntry) { // if
			// there is at least one active entry...
			
			// store the offset from the beginning of the allocated
			// memory to the first and last entries
			offsetNext = (static_cast<impl::hvectorEntry<T>*>
					(other.mImpl.mEntry.mNext) -
					other.mImpl.mAllocStart);
			offsetPrev = (static_cast<impl::hvectorEntry<T>*>
					(other.mImpl.mEntry.mPrev) -
					other.mImpl.mAllocStart);
		}
		
		// store the size of the other hvector
		const size_type otherSize = other.size();
		if (otherSize > capacity()) { // if the other hvector
			// is too large to fit in the allocated memory
			// for this hvector...
			
			// allocate a new block of memory and
			// copy the other hvector into it
			pointerEntry newStart = allocateCopy(otherSize,
					other.mImpl.mAllocStart, other.mImpl.mInitLast);
			
			// destroy this and then free memory reserved
			// by old memory block
			clear();
			this->deallocate(this->mImpl.mAllocStart,
					this->mImpl.mAllocEnd - this->mImpl.mAllocStart);
			
			// update pointers to point to new memory block
			this->mImpl.mAllocStart = newStart;
			this->mImpl.mInitLast = this->mImpl.mAllocStart + otherSize;
			this->mImpl.mAllocEnd = this->mImpl.mAllocStart + otherSize;
		}
		else if (this->size() >= otherSize) { // otherwise if
			// this hvector contains the same amount of or more
			// entries than the other hvector...
			
			// copy the other hvector into this hvector, trim the
			// remaining entries and then update the size
			initializedCopy(other.mImpl.mAllocStart, other.mImpl.mInitLast,
					this->mImpl.mAllocStart, this->getAllocatorEntry());
			clearEntries(this->mImpl.mAllocStart + otherSize,
					this->mImpl.mInitLast, this->getAllocatorEntry());
		}
		else { // otherwise this hvector has enough allocated
			// memory and fewer entries than the other hvector
			
			// copy the other hvector into this hvector
			initializedCopy(other.mImpl.mAllocStart,
					other.mImpl.mAllocStart + this->size(),
					this->mImpl.mAllocStart, this->getAllocatorEntry());
			
			// copy the remaining values of hvector into
			// uninitialised memory
			uninitializedCopy(other.mImpl.mAllocStart + this->size(),
					other.mImpl.mInitLast, this->mImpl.mInitLast,
					this->getAllocatorEntry());
		}
		
		// update the pointer to the last allocate entry
		this->mImpl.mInitLast = this->mImpl.mAllocStart + otherSize;
		
		if (other.mImpl.mEntry.mNext != &other.mImpl.mEntry) { // if
			// there was at least one active entry in the container...
			
			// update the pointer to the first and last active entries
			// to point at the new memory location
			this->mImpl.mEntry.mNext =
					this->mImpl.mAllocStart + offsetNext;
			this->mImpl.mEntry.mPrev =
					this->mImpl.mAllocStart + offsetPrev;
			
			// update the fir and last active entries to point back to the
			// hvector's entry
			(this->mImpl.mEntry.mNext)->mPrev = &this->mImpl.mEntry;
			(this->mImpl.mEntry.mPrev)->mNext = &this->mImpl.mEntry;
			
			// 
			this->setSize(other.getSize());
		}
		else {
			// set the size to 0 and point the hvector's entry to itself
			this->init();
		}
	}
	
	return *this;
}

template <class T, class A>
hvector<T, A>& hvector<T, A>::operator=(hvector&& other) noexcept(
		impl::isAllocNoThrow<allocTypeEntry>()) {
	
	typedef std::integral_constant<bool,
			impl::isAllocNoThrow<allocTypeEntry>()> moveStorage;
	
	moveAssign(std::move(other), moveStorage());
	
	return *this;
}

template <class T, class A>
typename hvector<T, A>::allocator_type
hvector<T, A>::get_allocator() const noexcept {
	return allocator_type(base::getAllocatorEntry());
}

template <class T, class A>
typename hvector<T, A>::iterator
hvector<T, A>::begin() noexcept {
	return iterator(this->mImpl.mEntry.mNext);
}

template <class T, class A>
typename hvector<T, A>::const_iterator
hvector<T, A>::begin() const noexcept {
	return const_iterator(this->mImpl.mEntry.mNext);
}

template <class T, class A>
typename hvector<T, A>::iterator
hvector<T, A>::end() noexcept {
	return iterator(&this->mImpl.mEntry);
}

template <class T, class A>
typename hvector<T, A>::const_iterator
hvector<T, A>::end() const noexcept {
	return const_iterator(&this->mImpl.mEntry);
}

template <class T, class A>
typename hvector<T, A>::reverse_iterator
hvector<T, A>::rbegin() noexcept {
	return reverse_iterator(end());
}

template <class T, class A>
typename hvector<T, A>::const_reverse_iterator
hvector<T, A>::rbegin() const noexcept {
	return const_reverse_iterator(end());
}

template <class T, class A>
typename hvector<T, A>::reverse_iterator
hvector<T, A>::rend() noexcept {
	return reverse_iterator(begin());
}

template <class T, class A>
typename hvector<T, A>::const_reverse_iterator
hvector<T, A>::rend() const noexcept {
	return const_reverse_iterator(begin());
}

template <class T, class A>
typename hvector<T, A>::const_iterator
hvector<T, A>::cbegin() noexcept {
	return const_iterator(this->mImpl.mEntry.mNext);
}

template <class T, class A>
typename hvector<T, A>::const_iterator
hvector<T, A>::cend() noexcept {
	return const_iterator(&this->mImpl.mEntry);
}

template <class T, class A>
typename hvector<T, A>::const_reverse_iterator
hvector<T, A>::crbegin() const noexcept {
	return const_reverse_iterator(end());
}

template <class T, class A>
typename hvector<T, A>::const_reverse_iterator
hvector<T, A>::crend() const noexcept {
	return const_reverse_iterator(begin());
}

template <class T, class A>
typename hvector<T, A>::size_type
hvector<T, A>::size() const noexcept {
	return (this->mImpl.mInitLast - this->mImpl.mAllocStart);
}

template <class T, class A>
typename hvector<T, A>::size_type
hvector<T, A>::count() const noexcept {
	return this->getSize();
}

template <class T, class A>
typename hvector<T, A>::size_type
hvector<T, A>::max_size() const noexcept {
	return allocTraitsEntry::max_size(this->getAllocatorEntry());
}

template <class T, class A>
typename hvector<T, A>::size_type
hvector<T, A>::capacity() const noexcept {
	return size_type(this->mImpl.mAllocEnd -
			this->mImpl.mAllocStart);
}

template <class T, class A>
bool hvector<T, A>::empty() const noexcept {
	return begin() == end();
}

template <class T, class A>
void hvector<T, A>::reserve(size_type vsize) {
	// if the requested size breaches the maximum
	// allowed by the implementation...
	if (vsize > this->max_size()) {
		throw std::length_error("unable to allocate");
	}
	
	// if requested size is larger than the current...
	if (this->capacity() < vsize) {
		// store the old distances
		size_type offset = this->mImpl.mInitLast - this->mImpl.mAllocStart;
		size_type offsetNext;
		size_type offsetPrev;
		
		if (this->mImpl.mEntry.mNext != &this->mImpl.mEntry) { // if there
			// is at least one active entry...
			
			// store the offset from the beginning of the allocated memory
			// to the first and last entries
			offsetNext = (static_cast<impl::hvectorEntry<T>*>
					(this->mImpl.mEntry.mNext) - this->mImpl.mAllocStart);
			offsetPrev = (static_cast<impl::hvectorEntry<T>*>
					(this->mImpl.mEntry.mPrev) - this->mImpl.mAllocStart);
		}
		
		// allocate and copy this hvector into a larger
		// block of memory and then destroy this
		pointerEntry newStart = allocateCopyAndDestroy(vsize,
				this->mImpl.mAllocStart,
				this->mImpl.mInitLast);
		
		// free memory reserved by old memory block
		this->deallocate(this->mImpl.mAllocStart, this->mImpl.mAllocEnd -
				this->mImpl.mAllocStart);
		
		// update pointers to point to new memory block
		this->mImpl.mAllocStart = newStart;
		this->mImpl.mInitLast = this->mImpl.mAllocStart + offset;
		this->mImpl.mAllocEnd = this->mImpl.mAllocStart + vsize;
		
		if (this->mImpl.mEntry.mNext != &this->mImpl.mEntry) { // if there
			// was at least one active entry in the container...
			
			// update the pointer to the first and last active entries to
			// point at the new memory location
			this->mImpl.mEntry.mNext = this->mImpl.mAllocStart + offsetNext;
			this->mImpl.mEntry.mPrev = this->mImpl.mAllocStart + offsetPrev;
			
			// update the fir and last active entries to point back to the
			// hvector's entry
			(this->mImpl.mEntry.mNext)->mPrev = &this->mImpl.mEntry;
			(this->mImpl.mEntry.mPrev)->mNext = &this->mImpl.mEntry;
		}
	}
}

template <class T, class A>
typename hvector<T, A>::reference
hvector<T, A>::operator[](const handle& h) noexcept {
	return *iterator(this->mImpl.mAllocStart + h.get_index());
}

template <class T, class A>
typename hvector<T, A>::const_reference
hvector<T, A>::operator[](const handle& h) const noexcept {
	return *iterator(this->mImpl.mAllocStart + h.get_index());
}

template <class T, class A>
typename hvector<T, A>::reference
hvector<T, A>::at(const handle& h) {
	pointerEntry ePtr = (this->mImpl.mAllocStart + h.get_index());
	
	if (h.get_index() >= this->mImpl.mInitLast - this->mImpl.mAllocStart) {
		throw std::out_of_range("handle index is out of bounds");
	}
	else if (h.get_counter() != ePtr->mCounter || !ePtr->mActive) {
		throw std::logic_error("handle counter mismatch or inactive entry");
	}
	
	return (*this)[h];
}

template <class T, class A>
typename hvector<T, A>::const_reference
hvector<T, A>::at(const handle& h) const {
	pointerEntry ePtr = (this->mImpl.mAllocStart + h.get_index());
	
	if (h.get_index() >= this->mImpl.mInitLast - this->mImpl.mAllocStart) {
		throw std::out_of_range("handle index is out of bounds");
	}
	else if (h.get_counter() != ePtr->mCounter || !ePtr->mActive) {
		throw std::logic_error("handle counter mismatch or inactive entry");
	}
	
	return (*this)[h];
}

template <class T, class A>
typename hvector<T, A>::handle
hvector<T, A>::get_handle(const size_type& index) const {
	if (static_cast<int>(index) >= this->mImpl.mInitLast - this->mImpl.mAllocStart) {
		throw std::out_of_range("index is out of bounds");
	}
	
	return handle(index, (this->mImpl.mAllocStart + index)->mCounter);
}

template <class T, class A>
typename hvector<T, A>::handle
hvector<T, A>::get_handle(const iterator& iter) const {
	size_type index = static_cast<impl::hvectorEntry<T>*>
			(iter.mEntry) - this->mImpl.mAllocStart;
	
	if (static_cast<int>(index) >=
			this->mImpl.mInitLast - this->mImpl.mAllocStart) {
		
		throw std::out_of_range("index is out of bounds");
	}
	
	return handle(index, iter.mEntry->mCounter);
}

template <class T, class A>
typename hvector<T, A>::handle
hvector<T, A>::get_handle(const const_iterator& cIter) const {
	size_type index = static_cast<const impl::hvectorEntry<T>*>
			(cIter.mEntry) - this->mImpl.mAllocStart;
	
	if (static_cast<int>(index) >=
			this->mImpl.mInitLast - this->mImpl.mAllocStart) {
		
		throw std::out_of_range("index is out of bounds");
	}
	
	return handle(index, cIter.mEntry->mCounter);
}

template <class T, class A>
template<typename... Ps>
typename hvector<T, A>::handle
hvector<T, A>::emplace(Ps&&... params) {
	return this->insertEntry(std::forward<Ps>(params)...);
}

template <class T, class A>
typename hvector<T, A>::handle
hvector<T, A>::push(const value_type& obj) {
	return this->insertEntry(obj);
}

template <class T, class A>
typename hvector<T, A>::handle
hvector<T, A>::push(value_type&& obj) {
	return this->insertEntry(std::move(obj));
}

template <class T, class A>
void hvector<T, A>::pop(const handle& h) {
	pointerEntry ePtr = (this->mImpl.mAllocStart + h.get_index());
	
	if (h.get_index() >= this->mImpl.mInitLast - this->mImpl.mAllocStart) {
		throw std::out_of_range("handle index is out of bounds");
	}
	else if (h.get_counter() != ePtr->mCounter || !ePtr->mActive) {
		throw std::logic_error("handle counter mismatch or inactive entry");
	}
	
	// update the entry data
	ePtr->mActive = false;
	++ePtr->mCounter;
	
	// decrement the number of active entries
	this->decSize(1u);
	
	if (this->count() == 0u) { // if there are no more active entries...
		this->mImpl.mEntry.mNext = &this->mImpl.mEntry;
		this->mImpl.mEntry.mPrev = &this->mImpl.mEntry;
	}
	else {
		if (ePtr->mPrev == &this->mImpl.mEntry) { // if pointer's previous
			// points to the hvector's entry then it is the
			// first active entry...
			
			// update the pointer to the first entry (now the entry
			// directly after the removed entry) and make the first
			// entry point to the hvector's entry
			this->mImpl.mEntry.mNext = ePtr->mNext;
			ePtr->mNext->mPrev = &this->mImpl.mEntry;
		}
		else if (ePtr->mNext == &this->mImpl.mEntry) { // otherwise if
			// pointer's next points to the hvector's entry then it
			// is the last active entry...
			
			// update the pointer to the last entry (now the entry
			// directly before the removed entry) and make the last
			// entry point to the hvector's entry
			this->mImpl.mEntry.mPrev = ePtr->mPrev;
			ePtr->mPrev->mNext = &this->mImpl.mEntry;
		}
		else { // otherwise the entry is somewhere in between...
			// link the entries adjacent to the removed entry together
			ePtr->mNext->mPrev = ePtr->mPrev;
			ePtr->mPrev->mNext = ePtr->mNext;
		}
	}
	
	// destroy the object
	entry* e = static_cast<entry*>(ePtr);
	value_type* val = e->valptr();
	allocTraitsEntry::destroy(this->getAllocatorEntry(), val);
}

template <class T, class A>
void hvector<T, A>::swapWith(hvector& other) noexcept {
	using std::swap;
	
	this->mImpl.swapWith(other.mImpl);
	
	impl::hvectorEntryBase* next = this->mImpl.mEntry.mNext;
	impl::hvectorEntryBase* prev = this->mImpl.mEntry.mPrev;
	
	if (other.mImpl.mEntry.mNext == &other.mImpl.mEntry) {
		this->mImpl.mEntry.mNext = &this->mImpl.mEntry;
		this->mImpl.mEntry.mPrev = &this->mImpl.mEntry;
	}
	else {
		this->mImpl.mEntry.mNext = other.mImpl.mEntry.mNext;
		this->mImpl.mEntry.mPrev = other.mImpl.mEntry.mPrev;
		
		(this->mImpl.mEntry.mNext)->mPrev = &this->mImpl.mEntry;
		(this->mImpl.mEntry.mPrev)->mNext = &this->mImpl.mEntry;
	}
	
	if (next == &this->mImpl.mEntry) {
		other.mImpl.mEntry.mNext = &other.mImpl.mEntry;
		other.mImpl.mEntry.mPrev = &other.mImpl.mEntry;
	}
	else {
		other.mImpl.mEntry.mNext = next;
		other.mImpl.mEntry.mPrev = prev;
		
		(other.mImpl.mEntry.mNext)->mPrev = &other.mImpl.mEntry;
		(other.mImpl.mEntry.mPrev)->mNext = &other.mImpl.mEntry;
	}
	
	size_t otherSize = other.getSize();
	other.setSize(this->getSize());
	this->setSize(otherSize);
	
	typedef typename allocTraitsEntry::
			propagate_on_container_swap pocs;
	
	doAllocOnSwap(this->getAllocatorEntry(),
			other.getAllocatorEntry(), pocs());
}

template <class T, class A>
void hvector<T, A>::clear() {
	clearEntries(this->getAllocatorEntry());
	this->init();
}

template <class T, class A>
template <typename... Ps>
typename hvector<T, A>::handle
hvector<T, A>::insertEntry(Ps&&... params) {
	bool reallocated = false;
	unsigned int index = 0u;
	unsigned int counter = 0u;
	
	// if we've used all of our allocated space...
	if (this->getSize() == capacity()) {
		reserve(capacity() + 1u);
		reallocated = true;
	}
	
	pointerEntry free = nullptr;
	if (this->mImpl.mEntry.mNext == &this->mImpl.mEntry) { // if there are
		// no active entries...
		
		// the new entry is at the very start of the allocated block
		free = this->mImpl.mAllocStart;
		
		// fill the entry's (T) buffer
		constructEntry(free, std::forward<Ps>(params)...);
		
		// 
		this->incSize(1u);
		
		// set the new entry's next and previous entry to point to
		// the hvector's entry
		free->mNext = &this->mImpl.mEntry;
		free->mPrev = &this->mImpl.mEntry;
		
		// set the hvector's entry to point at the new entry (first (next)
		// and last (previous) in store)
		this->mImpl.mEntry.mNext = free;
		this->mImpl.mEntry.mPrev = free;
		
		// 
		counter = free->mCounter;
	}
	else if (reallocated) { // otherwise if we performed a reallocation due
		// to reaching our memory limit...
		
		// the new entry is at the very end of the (newly) allocated block
		free = this->mImpl.mAllocStart + (capacity() - 1u);
		
		constructEntry(free, std::forward<Ps>(params)...);
		this->incSize(1u);
		
		// set the new entry's next entry to point to the hvector's entry
		// and previous entry to point at the previous last entry
		free->mNext = &this->mImpl.mEntry;
		free->mPrev = this->mImpl.mEntry.mPrev;
		
		// update the previous last entry and hvector's entry to point to
		// the new entry
		(this->mImpl.mEntry.mPrev)->mNext = free;
		this->mImpl.mEntry.mPrev = free;
		
		// 
		index = (capacity() - 1u);
		counter = free->mCounter;
	}
	else { // otherwise we have exisitng entries and didn't reallocate and
		// thus must attempt to create the new entry at the first free
		// (active == false) entry...
		
		pointerEntry iter = this->mImpl.mAllocStart;
		pointerEntry prev = nullptr;
		
		while (!free && iter != this->mImpl.mAllocEnd) { // while we haven't
			// found a free entry or reached the end of the
			// allocated memory block...
			
			if (iter >= this->mImpl.mInitLast || iter->mActive == false) {
				// if the current entry is uninitialised or inactive...
				
				free = iter;
			}
			else {
				if (iter->mActive == true) { // if the current entry
					// is active...
					
					prev = iter;
				}
				
				++iter;
			}
		}
		
		if (!free) { // if we didn't find a free entry...
			throw std::out_of_range("no free entry found");
		}
		
		constructEntry(free, std::forward<Ps>(params)...);
		this->incSize(1u);
		
		if (prev) { // if there is previous active entry...
			// set the new entry's previous and next pointers to point
			// to adjacent entries
			free->mNext = prev->mNext;
			free->mPrev = prev;
			
			// set adjacent entries to point back to the new entry
			(prev->mNext)->mPrev = free;
			prev->mNext = free;
		}
		else { // otherwise free entry is the first active entry...
			if (this->mImpl.mEntry.mNext == &this->mImpl.mEntry) {
				free->mNext = &this->mImpl.mEntry;
				free->mPrev = &this->mImpl.mEntry;
				
				this->mImpl.mEntry.mNext = free;
				this->mImpl.mEntry.mPrev = free;
			}
			else {
				free->mNext = this->mImpl.mEntry.mNext;
				free->mPrev = &this->mImpl.mEntry;
				
				(this->mImpl.mEntry.mNext)->mPrev = free;
				this->mImpl.mEntry.mNext = free;
			}
		}
		
		// 
		index = iter - this->mImpl.mAllocStart;
		counter = free->mCounter;
	}
	
	// if the latest added entry surpasses the previous in terms
	// of being the most advanced in memory...
	if ((free + 1u) > this->mImpl.mInitLast) {
		// update the "1-past-the-end" pointer
		this->mImpl.mInitLast = (free + 1u);
		
		// ensure the counter value is properly initialised
		counter = (free->mCounter = 0u);
	}
	
	return handle(index, counter);
}

template <class T, class A>
template <typename... Ps>
typename hvector<T, A>::entry*
hvector<T, A>::constructEntry(pointerEntry ptr, Ps&&... params) {
	auto& alloc = this->getAllocatorEntry();
	
	// convert to a managed pointer to take advantage of SFINAE to
	// ensure ptr is compatible with alloc
	allocatedPtr<allocTypeEntry> aPtr{alloc, ptr};
	
	// construct an object (T) in the entry's buffer, set the
	// entry as active and set its offset
	allocTraitsEntry::construct(alloc, ptr->valptr(),
			std::forward<Ps>(params)...);
	ptr->mActive = true;
	
	// reset the managed pointer to ensure it does not deallocate
	// the pointer it manages
	aPtr = nullptr;
	
	return ptr;
}

template <class T, class A>
template <typename FwdIter, typename Aa>
unsigned int hvector<T, A>::clearEntries(FwdIter start,
		FwdIter end, Aa& alloc) noexcept {
	
	unsigned int removedCount = 0u;
	
	// for all entries (that are or have been active)...
	auto iter = start;
	while (iter != end) {
		entry* e = static_cast<entry*>(iter);
		
		if (e->mActive) { // if the entry is active...
			// get the pointer to the entry's buffer and destroy
			// the object (T)
			value_type* val = e->valptr();
			
			allocTraitsEntry::destroy(alloc, val);
			
			e->mActive = false;
			++e->mCounter;
			++removedCount;
		}
		
		++iter;
	}
	
	return removedCount;
}

template <class T, class A>
template <typename Aa>
void hvector<T, A>::clearEntries(Aa& alloc) noexcept {
	auto iter = this->mImpl.mEntry.mNext;
	while (iter != &this->mImpl.mEntry) {
		entry* e = static_cast<entry*>(iter);
		
		value_type* val = e->valptr();
		
		allocTraitsEntry::destroy(alloc, val);
		
		e->mActive = false;
		++e->mCounter;
		
		iter = iter->mNext;
	}
}

template <class T, class A>
template <typename FwdIter>
typename hvector<T, A>::pointerEntry
hvector<T, A>::allocateCopyAndDestroy(size_type vsize, FwdIter start,
		FwdIter end) {
	
	// allocate a block of memory
	pointerEntry result = this->allocate(vsize);
	
	try {
		// copy into newly allocated space
		uninitializedCopyAndDestroy(start, end, result,
				this->getAllocatorEntry());
		
		return result;
	} catch (...) {
		// release the memory previously allocated
		this->deallocate(result, vsize);
		throw;
	}
}

template <class T, class A>
template <typename FwdIter>
typename hvector<T, A>::pointerEntry
hvector<T, A>::allocateCopy(size_type vsize, FwdIter start,
		FwdIter end) {
	
	pointerEntry result = this->allocate(vsize);
	
	try {
		uninitializedCopy(start, end, result,
				this->getAllocatorEntry());
		
		return result;
	} catch (...) {
		this->deallocate(result, vsize);
		throw;
	}
}

template <class T, class A>
template<typename InIter, typename FwdIter, typename Aa>
void hvector<T, A>::uninitializedCopyAndDestroy(InIter start, InIter end,
		FwdIter result, Aa& alloc) {
	
	auto iterOld = start;
	auto iterNew = result;
	
	try {
		while (iterOld != end) {
			entry* entryOld = static_cast<entry*>(iterOld);
			entry* entryNew = static_cast<entry*>(iterNew);
			
			entryNew->mActive = entryOld->mActive;
			entryNew->mCounter = entryOld->mCounter;
			
			// update the previous and next active entry pointers to
			// point at the entries in their new memory location
			// (first and last entries won't point to the correct
			// hvector entry and must be updated later)
			entryNew->mNext = result + (static_cast<impl::hvectorEntry<T>*>
					(entryOld->mNext) - start);
			entryNew->mPrev = result + (static_cast<impl::hvectorEntry<T>*>
					(entryOld->mPrev) - start);
			
			if (entryOld->mActive) { // if the entry is active...
				// get the pointer to the entry's buffer
				value_type* val = entryOld->valptr();
				
				// construct a new entry in the new memory block
				// by moving (since we're destroying the origin
				// object) if it's noexcept or copy constructor
				// isn't available, otherwise via copy constructor
				constructEntry(iterNew,
						std::move_if_noexcept(*val));
				
				// destroy the object in the buffer
				allocTraitsEntry::destroy(alloc, val);
			}
			
			++iterOld;
			++iterNew;
		}
	} catch (...) {
		// clean up anything we might have constructed
		auto iter = result;
		while (iter != end) {
			entry* e = static_cast<entry*>(iter);
			if (e->mActive) {
				value_type* val = e->valptr();
				allocTraitsEntry::destroy(alloc, val);
			}
			
			++iter;
		}
		
		throw;
	}
}

template <class T, class A>
template<typename InIter, typename FwdIter, typename Aa>
void hvector<T, A>::uninitializedCopy(InIter start, InIter end,
		FwdIter result, Aa& alloc) {
	
	auto iterOld = start;
	auto iterNew = result;
	
	try {
		while (iterOld != end) {
			entry* entryOld = static_cast<entry*>(iterOld);
			entry* entryNew = static_cast<entry*>(iterNew);
			
			entryNew->mActive = entryOld->mActive;
			entryNew->mCounter = entryOld->mCounter;
			
			entryNew->mNext = result + (static_cast<impl::hvectorEntry<T>*>
					(entryOld->mNext) - start);
			entryNew->mPrev = result + (static_cast<impl::hvectorEntry<T>*>
					(entryOld->mPrev) - start);
			
			if (entryOld->mActive) {
				value_type* val = entryOld->valptr();
				constructEntry(iterNew, *val);
			}
			
			++iterOld;
			++iterNew;
		}
	} catch (...) {
		auto iter = result;
		while (iter != end) {
			entry* e = static_cast<entry*>(iter);
			if (e->mActive) {
				value_type* val = e->valptr();
				allocTraitsEntry::destroy(alloc, val);
			}
			
			++iter;
		}
		
		throw;
	}
}

template <class T, class A>
template<typename InIter, typename FwdIter, typename Aa>
void hvector<T, A>::initializedCopy(InIter start, InIter end,
		FwdIter result, Aa& alloc) {
	
	auto iterOld = start;
	auto iterNew = result;
	
	try {
		while (iterOld != end) {
			entry* entryOld = static_cast<entry*>(iterOld);
			entry* entryNew = static_cast<entry*>(iterNew);
			
			// store the value of the entry being allocated to
			// to decide how we want to assign the new value
			bool newActive = entryNew->mActive;
			entryNew->mActive = entryOld->mActive;
			entryNew->mCounter = entryOld->mCounter;
			
			entryNew->mNext = result + (static_cast<impl::hvectorEntry<T>*>
					(entryOld->mNext) - start);
			entryNew->mPrev = result + (static_cast<impl::hvectorEntry<T>*>
					(entryOld->mPrev) - start);
			
			if (entryOld->mActive) {
				if (newActive) { // if the entry being assigned to
					// already has an active object...
					
					// use assignment operator to set the new value
					value_type* valOld = entryOld->valptr();
					value_type* valNew = entryNew->valptr();
					*valNew = *valOld;
				}
				else { // otherwise the entry being assigned to
					// doesn't have an active object...
					
					// use copy constructor to set the new value
					value_type* val = entryOld->valptr();
					constructEntry(iterNew, *val);
				}
			}
			else if (newActive) { // otherwise if there is no
				// object to be assign yet we already have an
				// active object...
				
				// destroy the existing object (nothing to assign)
				value_type* val = entryNew->valptr();
				allocTraitsEntry::destroy(alloc, val);
			}
			
			++iterOld;
			++iterNew;
		}
	} catch (...) {
		auto iter = result;
		while (iter != end) {
			entry* e = static_cast<entry*>(iter);
			if (e->mActive) {
				value_type* val = e->valptr();
				allocTraitsEntry::destroy(alloc, val);
			}
			
			++iter;
		}
		
		throw;
	}
}

template <class T, class A>
void hvector<T, A>::doAllocOnSwap(allocTypeEntry& first,
		allocTypeEntry& second, std::true_type) {
	
	using std::swap;
	swap(first, second);
}

template <class T, class A>
void hvector<T, A>::doAllocOnSwap(allocTypeEntry& first,
		allocTypeEntry& second, std::false_type) {
	
	
}

template <class T, class A>
void hvector<T, A>::moveAssign(hvector&& other, std::true_type) noexcept {
	// create a blank hvector (using this hvector's allocator)
	hvector temp(get_allocator());
	
	// swap this hvector with the new blank hvector
	this->mImpl.swapWith(temp.mImpl);
	temp.updateEntry(std::move(*this));
	
	// swap this (now blank) hvector with the other hvector
	// (other hvector is now blank)
	this->mImpl.swapWith(other.mImpl);
	this->updateEntry(std::move(other));
	
	// moves other allocator to this
	allocOnMove(this->getAllocatorEntry(),
			other.getAllocatorEntry());
}

template <class T, class A>
void hvector<T, A>::moveAssign(hvector&& other, std::false_type) {
	// if allocators equate with each other...
	if (other.getAllocatorEntry() == this->getAllocatorEntry()) {
		moveAssign(std::move(other), std::true_type());
	}
	else {
		// [!] implement moving for differing allocators
	}
}
// ...
}
