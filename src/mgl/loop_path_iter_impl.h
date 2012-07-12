#ifndef LOOP_PATH_IMPL_H
#define	LOOP_PATH_IMPL_H

#include "loop_path_decl.h"

namespace mgl {

template <typename BASE>
OpenPath::iterator_gen<BASE>::iterator_gen(BASE b) : base(b) {}

template <typename BASE> template <typename OTHERBASE>
OpenPath::iterator_gen<BASE>::iterator_gen(const iterator_gen<OTHERBASE>& orig) : 
		base(orig.iterator_gen<OTHERBASE>::base) {}

template <typename BASE> template <typename OTHERBASE>
OpenPath::iterator_gen<BASE>& OpenPath::iterator_gen<BASE>::operator=(
		const iterator_gen<OTHERBASE>& orig) {
	base = orig.iterator_gen<OTHERBASE>::base;
	return *this;
}

template <typename BASE>
typename OpenPath::iterator_gen<BASE>::reference 
		OpenPath::iterator_gen<BASE>::operator*() {
	return *base;
}

template <typename BASE>
typename OpenPath::iterator_gen<BASE>::pointer 
		OpenPath::iterator_gen<BASE>::operator->() {
	return &*base;
}

template <typename BASE>
typename OpenPath::iterator_gen<BASE>::iterator 
		OpenPath::iterator_gen<BASE>::operator&() const {
	return base;
}

template <typename BASE>
OpenPath::iterator_gen<BASE>& OpenPath::iterator_gen<BASE>::operator ++() {
	++base;
	return *this;
}

template <typename BASE>
OpenPath::iterator_gen<BASE> OpenPath::iterator_gen<BASE>::operator ++(int) {
	iterator_gen<BASE> iter_copy = *this;
	++*this;
	return iter_copy;
}

template <typename BASE>
OpenPath::iterator_gen<BASE>& OpenPath::iterator_gen<BASE>::operator --() {
	--base;
	return *this;
}

template <typename BASE>
OpenPath::iterator_gen<BASE> OpenPath::iterator_gen<BASE>::operator --(int) {
	iterator_gen<BASE> iter_copy = *this;
	--*this;
	return iter_copy;
}

template <typename BASE>
OpenPath::iterator_gen<BASE>& OpenPath::iterator_gen<BASE>::operator +=(int off) {
	iterator_gen<BASE> iter_copy = *this;
	return iter_copy+=off;
}

template <typename BASE>
OpenPath::iterator_gen<BASE> OpenPath::iterator_gen<BASE>::operator +(int off) {
	iterator_gen<BASE> iter_copy = *this;
	return iter_copy+=off;
}

template <typename BASE>
OpenPath::iterator_gen<BASE>& OpenPath::iterator_gen<BASE>::operator -=(int off) {
	return base += (-off);
}

template <typename BASE>
OpenPath::iterator_gen<BASE> OpenPath::iterator_gen<BASE>::operator -(int off) {
	return *this + (-off);
}

template <typename BASE> template <typename OTHERBASE>
bool OpenPath::iterator_gen<BASE>::operator ==(
		const iterator_gen<OTHERBASE>& other) const {
	return base == other.iterator_gen<OTHERBASE>::base;
}

template <typename BASE> template <typename OTHERBASE>
bool OpenPath::iterator_gen<BASE>::operator !=(
		const iterator_gen<OTHERBASE>& other) const {
	return !(*this==other);
}

template <typename BASE>
Loop::iterator_gen<BASE>::iterator_gen() 
		: base(iterator()), begin(iterator()), end(iterator()) {}

template <typename BASE>
Loop::iterator_gen<BASE>::iterator_gen(iterator i, iterator b, iterator e) : 
		base(i), begin(b), end(e) {}

template <typename BASE> template <typename OTHERBASE>
Loop::iterator_gen<BASE>::iterator_gen(const iterator_gen<OTHERBASE>& orig) : 
				base(orig.iterator_gen<OTHERBASE>::base), 
				begin(orig.iterator_gen<OTHERBASE>::begin), 
				end(orig.iterator_gen<OTHERBASE>::end) {}

template <typename BASE>
Loop::iterator_gen<BASE>& Loop::iterator_gen<BASE>::operator =(
		const iterator_gen<BASE>& orig) {
	base = orig.base;
	begin = orig.begin;
	end = orig.end;
}

template <typename BASE>
typename Loop::iterator_gen<BASE>::reference 
		Loop::iterator_gen<BASE>::operator *() {
	return *base;
}

template <typename BASE>
typename Loop::iterator_gen<BASE>::pointer 
		Loop::iterator_gen<BASE>::operator ->() {
	return &*base;
}

template <typename BASE>
typename Loop::iterator_gen<BASE>::iterator 
		Loop::iterator_gen<BASE>::operator &() const {
	return base;
}

template <typename BASE>
Loop::iterator_gen<BASE>& Loop::iterator_gen<BASE>::operator ++() {
	if(base != end) {
		++base;
		if (base == end) {
			base = begin;
		}
	}
	return *this;
}

template <typename BASE>
Loop::iterator_gen<BASE> Loop::iterator_gen<BASE>::operator ++(int) {
	iterator_gen<BASE> iter_copy = *this;
	return ++iter_copy;
}

template <typename BASE>
Loop::iterator_gen<BASE>& Loop::iterator_gen<BASE>::operator --() {
	if(base != end) {
		if(base == begin){
			base = end;
		}
		--base;
	}
	return *this;
}

template <typename BASE>
Loop::iterator_gen<BASE> Loop::iterator_gen<BASE>::operator --(int) {
	iterator_gen<BASE> iter_copy = *this;
	return --iter_copy;
}

template <typename BASE>
Loop::iterator_gen<BASE> Loop::iterator_gen<BASE>::makeBegin() const {
	return iterator_gen(begin, begin, end); 
}

template <typename BASE>
Loop::iterator_gen<BASE> Loop::iterator_gen<BASE>::makeEnd() const {
	return iterator_gen(end, begin, end); 
}

template <typename BASE> template <typename OTHERBASE>
bool Loop::iterator_gen<BASE>::operator ==(
		const iterator_gen<OTHERBASE>& other) const {
	return base == other.iterator_gen<OTHERBASE>::base;
}

template <typename BASE> template <typename OTHERBASE>
bool Loop::iterator_gen<BASE>::operator !=(
		const iterator_gen<OTHERBASE>& other) const {
	return !(*this == other);
}

template <typename BASE>
bool Loop::iterator_gen<BASE>::isBegin() const { return base == begin; }

template <typename BASE>
bool Loop::iterator_gen<BASE>::isEnd() const { return base == end; }

template <typename BASE>
Loop::iterator_finite_gen<BASE>::iterator_finite_gen() : 
		iterator_gen<BASE>() {}

template <typename BASE>
Loop::iterator_finite_gen<BASE>::iterator_finite_gen(iterator i, 
		iterator b, iterator e) : 
		iterator_gen<BASE>(i, b, e) {}

template <typename BASE> template <typename OTHERBASE>
Loop::iterator_finite_gen<BASE>::iterator_finite_gen(
		const iterator_gen<OTHERBASE>& orig) : 
		iterator_gen<BASE>(
		orig.iterator_gen<OTHERBASE>::base, 
		orig.iterator_gen<OTHERBASE>::begin, 
		orig.iterator_gen<OTHERBASE>::end) {}

template <typename BASE> template <typename OTHERBASE>
Loop::iterator_finite_gen<BASE>& Loop::iterator_finite_gen<BASE>::operator =( 
		const iterator_gen<OTHERBASE>& orig) {
	iterator_gen<BASE>::base = 
			orig.iterator_gen<OTHERBASE>::base;
	iterator_gen<BASE>::begin = 
			orig.iterator_gen<OTHERBASE>::begin;
	iterator_gen<BASE>::end = 
			orig.iterator_gen<OTHERBASE>::end;
}

template <typename BASE>
Loop::iterator_finite_gen<BASE>& Loop::iterator_finite_gen<BASE>::operator ++() {
	++iterator_gen<BASE>::base;
	return *this;
}

template <typename BASE>
Loop::iterator_finite_gen<BASE> Loop::iterator_finite_gen<BASE>::operator ++(int) {
	iterator_finite_gen<BASE> iter_copy = *this;
	++*this;
	return iter_copy;
}

template <typename BASE>
Loop::iterator_finite_gen<BASE>& Loop::iterator_finite_gen<BASE>::operator --() {
	--iterator_gen<BASE>::base;
	return *this;
}

template <typename BASE>
Loop::iterator_finite_gen<BASE> Loop::iterator_finite_gen<BASE>::operator --(int) {
	iterator_finite_gen<BASE> iter_copy = *this;
	--*this;
	return iter_copy;
}

template <typename BASE>
LoopPath::iterator_gen<BASE>::iterator_gen(const BASE& i, const LoopPath& p) : 
		base(i), parent(p), hasLooped(false) {}

template <typename BASE> template <typename OTHERBASE>
LoopPath::iterator_gen<BASE>::iterator_gen(const iterator_gen<OTHERBASE>& orig) : 
		base(orig.iterator_gen<OTHERBASE>::base), 
		parent(orig.iterator_gen<OTHERBASE>::parent), 
		hasLooped(orig.iterator_gen<OTHERBASE>::hasLooped) {}

template <typename BASE>
const PointType& LoopPath::iterator_gen<BASE>::operator *() {
	return base->getPoint();
}

template <typename BASE>
const PointType* LoopPath::iterator_gen<BASE>::operator ->() {
	return &(**this);
}

template <typename BASE>
typename LoopPath::iterator_gen<BASE>::iterator 
		LoopPath::iterator_gen<BASE>::operator &() const {
	return base;
}

template <typename BASE>
LoopPath::iterator_gen<BASE>& LoopPath::iterator_gen<BASE>::operator ++() {
	if(hasLooped) {
		base = base.makeEnd();
	} else {
		++base;
		if (parent.isBegin(base)) {
			hasLooped = true;
		}
	}
	return *this;
}

template <typename BASE>
LoopPath::iterator_gen<BASE> LoopPath::iterator_gen<BASE>::operator ++(int) {
	iterator_gen<BASE> iter_copy = *this;
	++*this;
	return iter_copy;
}

template <typename BASE>
LoopPath::iterator_gen<BASE>& LoopPath::iterator_gen<BASE>::operator --() {
	if(parent.isBegin(base)) {
		base = end();
	} else {
		--base;
	}
	return *this;
}

template <typename BASE>
LoopPath::iterator_gen<BASE> LoopPath::iterator_gen<BASE>::operator --(int) {
	iterator_gen<BASE> iter_copy = *this;
	--*this;
	return iter_copy;
}

template <typename BASE>
bool LoopPath::iterator_gen<BASE>::operator ==(
		const iterator_gen<BASE>& other) const {
	return base == other.base;
}

template <typename BASE>
bool LoopPath::iterator_gen<BASE>::operator !=(
		const iterator_gen<BASE>& other) const {
	return !((*this)==other);
}

}


#endif	/* LOOP_PATH_IMPL_H */

