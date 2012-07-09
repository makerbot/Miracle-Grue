#ifndef LOOP_PATH_IMPL_H
#define	LOOP_PATH_IMPL_H

#include "loop_path.h"

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


}


#endif	/* LOOP_PATH_IMPL_H */

