#ifndef PATH_H
#define PATH_H

#include <vector>
#include "libthing/Vector2.h"

namespace mgl {

typedef std::vector<libthing::Vector2> PointList;

/*! /brief Interface for path iterators
 * A pure virtual interface to the iterators in Paths and LoopPaths
 */
class PathIterator {
public:
	virtual libthing::Vector2& operator*() = 0;
	virtual PathIterator& operator++() = 0;
	virtual PathIterator& operator+(int off) = 0;
};

/*!
 * /brief An open ended path
 * A path of points with a defined beginning and end, with no assumption of an
 * interior. 
 */
class OpenPath {
	template <class BASE>
	class iterator_gen : public PathIterator {
	public:
		iterator_gen(PointList::iterator &b) : base(b) {};

		libthing::Vector2& operator*() { return *base; };
		iterator_gen& operator++() {
			base++;
			return *this;
		};
		iterator_gen& operator+(int off) {
			base = base + off;
			return *this;
		};

	private:
		BASE base;
	};

public:
	typedef iterator_gen<PointList::iterator> iterator;
	typedef iterator_gen<PointList::reverse_iterator> reverse_iterator;

	OpenPath() {};
	OpenPath(const OpenPath &orig);

	iterator fromStart();
	reverse_iterator fromEnd();

	void connect(const PathIterator &connection);

	void appendPoints(const PathIterator &first, const PathIterator &last);
	void appendPoint(const libthing::Vector2 &point);

	void prependPoints(const PathIterator &first, const PathIterator &last);
	void prependPoint(const libthing::Vector2 &point);

	libthing::Vector2& operator[](int index);
	
private:
	PointList points;
};

class Loop2 {
public:
	template <class BASE>
	class iterator_gen {
	public:
		iterator_gen(Loop2 &p, PointList::iterator *i = NULL,
				 PointList::reverse_iterator *i = NULL);
		
		libthing::Vector2 &operator*() { return *base };
			
		iterator_gen operator++() {
			
		iterator_gen operator+(int off);

	private:
		BASE base;
	};

	typedef iterator_gen<PointList::iterator> cw_iterator;
	typedef iterator_gen<PointList::reverse_iterator> ccw_iterator;

	Loop2() {};
	Loop2(const Loop2 &orig); 

	iterator insertPoint(const libthing::Vector2 &point, iterator after);
	iterator insertPoint(const libthing::Vector2 &point, iterator after);

private:
	PointList points;
}

/*
class LoopPath {
public:
	class iterator {
	public:
		iterator(LoopPath &p, Loop2::iterator b) : parent(p), base_iter(b);

		libthing::Vector2& operator*();
		iterator operator++();
		iterator operator+(int off);
	}
private:
	Loop2 *base;
	Loop2::iterator *start;
}
*/
}
#endif
