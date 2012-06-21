#ifndef PATH_H
#define PATH_H

#include <vector>
#include "libthing/Vector2.h"
#include "libthing/LineSegment2.h"

namespace mgl {

typedef std::vector<libthing::Vector2> PointList;
typedef std::vector<libthing::Vector2> VectorList;

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
		iterator_gen(BASE b) : base(b) {};

		libthing::Vector2& operator*() { return *base; };
		iterator_gen<BASE>& operator++() {
			base++;
			return *this;
		};
   		iterator_gen<BASE>& operator+(int off) {
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

	iterator end() { return iterator(points.end()); };
	reverse_iterator rend() { return reverse_iterator(points.rend()); };

	void connect(const PathIterator &connection);

	template <class ITER>
	void appendPoints(const ITER &first, const ITER &last);
	void appendPoint(const libthing::Vector2 &point);

	template <class ITER>
	void prependPoints(const ITER &first, const ITER &last);
	void prependPoint(const libthing::Vector2 &point);

	libthing::Vector2& operator[](int index);
	
private:
	PointList points;
};

class Loop {
public:
	template <class BASE>
	class iterator_gen {
	public:
		iterator_gen(BASE i, BASE b, BASE e) : base(i), begin(b), end(e) {};
		
		libthing::Vector2 &operator*() { return *base; };
			
		iterator_gen<BASE> operator++() {
			base++;
			if (base == end) {
				base = begin;
			}
			
			return *this;
		}

	private:
		BASE base;
		BASE begin;
		BASE end;
	};

	typedef iterator_gen<PointList::iterator> cw_iterator;
	typedef iterator_gen<PointList::reverse_iterator> ccw_iterator;

	Loop() {};
	Loop(const Loop &orig); 

	template <class ITER>
	cw_iterator insertPoint(const libthing::Vector2 &point, ITER after);

	cw_iterator clockwise(const libthing::Vector2 &startpoint);
	ccw_iterator counterClockwise(const libthing::Vector2 &startpoint);

	template <class ITER>
	libthing::LineSegment2 segmentAfterPoint(const ITER &location);
	template <class ITER>
	libthing::Vector2 normalAfterPoint(const ITER &location);

	friend class LoopPath;
private:
	PointList points;
	VectorList normals;
};


class LoopPath {
public:
	template <class BASE>
	class iterator_gen {
	public:
		iterator_gen(BASE i, LoopPath &p) : base(i), parent(p) {};

		libthing::Vector2& operator*() { return *base; }

		iterator_gen<BASE> operator++() {
			base++;
			if (base == parent.start()) {
				base == parent.end();
			}

			return base;
		}
	private:
		BASE base;
		LoopPath &parent;
	};

	LoopPath(Loop &p, Loop::cw_iterator s) : parent(p), start(s) {};

	typedef iterator_gen<Loop::cw_iterator> iterator;
	typedef iterator_gen<Loop::ccw_iterator> reverse_iterator;

	iterator end() {
		return iterator(Loop::cw_iterator(parent.points.end(),
										  parent.points.end(),
										  parent.points.end()),
						*this); };
	reverse_iterator rend() {
		return reverse_iterator(Loop::ccw_iterator(parent.points.end(),
												   parent.points.end(),
												   parent.points.end()),
								*this); };

	iterator fromStart();
	reverse_iterator fromEnd();

private:
	Loop &parent;
	Loop::cw_iterator start;
};

}
#endif
