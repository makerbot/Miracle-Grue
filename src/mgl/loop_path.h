#ifndef LOOP_PATH_H
#define LOOP_PATH_H

#include <vector>
#include "libthing/Vector2.h"
#include "libthing/LineSegment2.h"

namespace mgl {

typedef libthing::Vector2 PointType;

typedef std::vector<PointType> PointList;
typedef std::vector<PointType> VectorList;


/*!
 * /brief An open ended path
 * A 2d path of points with a defined beginning and end, with no assumption of
 * an interior. Intended to be an extruded path in real space with suspended
 * points.
 *
 * This is a read-write object, meant for both constructing a path as well as
 * following it.
 */
class OpenPath {
	/*!
	 * /brief A shallow wrapper around underlying container iterators
	 */
	template <class BASE>
	class iterator_gen {
	public:
		iterator_gen(BASE b) : base(b) {};

		PointType& operator*() { return *base; };
		iterator_gen<BASE> operator++() {
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

	OpenPath() : endpoints(2) {};
	/*! Get an iterator from the first point of the path */
	iterator fromStart() { return iterator(points.begin()); };
	/*! Get an iterator from the last point of the path	 */
	reverse_iterator fromEnd() { return reverse_iterator(points.rbegin()); };

	iterator end() { return iterator(points.end()); };
	reverse_iterator rend() { return reverse_iterator(points.rend()); };

	/*! Add points to the end of a path.  This makes copies of the points.
	 *  /param first iterator to the first point to append
	 *  /param end stop iterating here
	 */
	template <class ITER>
	void appendPoints(ITER first, ITER end) {
		for (; first != end; ++first)
			appendPoint(*first);
	}

	/*! Add a single point to the end of a path. This copies the point
	 *  /param point value to be appended
	 */
	void appendPoint(const PointType &point) {
		points.push_back(point);
	};

	/*! Add points to the beginning of a path. This makes coppies of the points.
	 *  /param first iterator to the first point to append
	 *  /param end stop iterating here
	 */
	template <class ITER>
	void prependPoints(ITER first, ITER end) {
		for (; first != end; ++first) 
			prependPoint(*first);
	}
		
	/*! Add a single point to the beginning of a path. This copies the point
	 *  /param point value to be appended
	 */
	void prependPoint(const PointType &point) {
		points.insert(points.begin(), point);
	}

	/*! Retrieve the LineSegment2 in the path that starts with a provided point.
	 *  This creates a new LineSegment2 value
	 *  /param location iterator pointing to the first endpoint of the segment
	 *  /return The line segment starting with the point at location
	 */
	template <class ITER>
	libthing::LineSegment2 segmentAfterPoint(const ITER &location);

	/*! Find points you can start extrusion on for this path.  For an
	 *  OpenPath, this gives you the endpoints.
	 *  /return iterator for all the valid staring points
	 */
	iterator getEntryPoints() {
		endpoints[0] = points[0];
		endpoints[1] = points.back();
		return endpoints.begin();
	};

	/*! Find points that are suspended by material underneath.
	 *  This is not implemented as the suspended property is not implemented.
	 *  /return An iterator to retrieve all suspended points, currently
	            retrieves all points
	 */
	iterator getSuspendedPoints() { return fromStart(); }; //stub
	
private:
	PointList points;
	PointList endpoints;
};


/*! /brief A closed loop
 *  A 2d path that has no beginning or end, connects around to itself, and has
 *  an interior.
 */
class Loop {
	template <class BASE>
	class iterator_gen {
	public:
		iterator_gen(BASE i, BASE b, BASE e) : base(i), begin(b), end(e) {};
		
		PointType &operator*() { return *base; };
			
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

public:
	typedef iterator_gen<PointList::iterator> cw_iterator;
	typedef iterator_gen<PointList::reverse_iterator> ccw_iterator;

	Loop() { };
	Loop(const PointType &first) { points.push_back(first); };
	Loop(const Loop &orig); 

	/*! Insert a point into the loop at a specific location
	 *  /param point value to be inserted
	 *  /param iterator location for this to be inserted after
	 *  /return iterator for the location of the new point
	 */
	template <class ITER>
	cw_iterator insertPoint(const PointType &point, ITER after);

	/*! Get an iterator that traverses around the loop clockwise.  There is no
	 *  end, the iterator will continue around the loop indefinitely.
	 *  /param startpoint Point value to start on
	 *  /return clockwise iterator from the start point
	 */
	cw_iterator clockwise(const PointType &startpoint);

	/*! Get an iterator that traverses around the loop clockwise from an
	 *  arbitrary start point.  There is no end, the iterator will continue
	 *  around the loop indefinitely.
	 *  /return clockwise iterator from the start point
	 */
	cw_iterator clockwise() { return clockwise(points.front()); };

	/*! Get an iterator that traverses around the loop counter clockwise.
	 *  There is no end, the iterator will continue around the loop
	 *  indefinitely.
	 *  /param startpoint Point value to start on
	 *  /return counter clockwise iterator from the start point
	 */
	ccw_iterator counterClockwise(const PointType &startpoint);

	/*! Get an iterator that traverses around the loop counter clockwise from an
	 *  arbitrary start point. There is no end, the iterator will continue
	 *  around the loop indefinitely.
	 *  /return counter clockwise iterator from the start point
	 */
	ccw_iterator counterClockwise() {
		return counterClockwise(points.front());
	};

	/*! Retrieve the LineSegment2 in the path that starts with a provided point.
	 *  This creates a new LineSegment2 value
	 *  /param location iterator pointing to the first endpoint of the segment
	 *  /return The line segment starting with the point at location
	 */
	template <class ITER>
	libthing::LineSegment2 segmentAfterPoint(const ITER &location);

	/*! Retrieve the normal vector in a loop at a provided point.
	 *  Normals point toward the interior of a loop.
	 *  /param location iterator pointing to the point before the normal
	 *  /return The normal vector at this location
	 */
	template <class ITER>
	PointType normalAfterPoint(const ITER &location);

	/*! Find points you can start extrusion on for this path.  For a
	 *  Loop, this gives you every point in the loop.
	 *  /return iterator for all the valid staring points
	 */
	cw_iterator getEntryPoints() {
		return cw_iterator(points.begin(), points.begin(), points.end());
	};

	/*! Find points that are suspended by material underneath.
	 *  This is not implemented as the suspended property is not implemented.
	 *  /return An iterator to retrieve all suspended points, currently
	            retrieves all points
	 */
	cw_iterator getSuspendedPoints() { return clockwise(points.front()); };

	friend class LoopPath;
private:

	PointList points;
	VectorList normals;
};

/*! /brief Adapter to make a Loop look like an OpenPath
 *  Provides an interface to a loop that functions similar to the read only
 *  operation of OpenPath.  
 */
class LoopPath {
	template <class BASE>
	class iterator_gen {
	public:
		iterator_gen(BASE i, LoopPath &p) : base(i), parent(p) {};

		PointType& operator*() { return *base; }

		iterator_gen<BASE> operator++() {
			base++;
			if (base == parent.start()) {
				base = parent.end();
			}

			return *this;
		}
	private:
		BASE base;
		LoopPath &parent;
	};
public:
	typedef iterator_gen<Loop::cw_iterator> iterator;
	typedef iterator_gen<Loop::ccw_iterator> reverse_iterator;

	/*! Constructor, every LoopPath has a Loop and a start point.
	 *  /param parent The Loop this LoopPath follows
	 *  /param start The clockwise start point
	 *  /param rstart The same start point, counter clockwise
	 */
	LoopPath(Loop &p, Loop::cw_iterator s, Loop::ccw_iterator r)
        : parent(p), start(s), rstart(r) {};

	/*! Iterator after the end of the list. */
	iterator end() {
		return iterator(Loop::cw_iterator(parent.points.end(),
										  parent.points.end(),
										  parent.points.end()),
						*this); };

	/*! Reverse iterator after the end of the list. */
	reverse_iterator rend() {
		return reverse_iterator(Loop::ccw_iterator(parent.points.rend(),
												   parent.points.rend(),
												   parent.points.rend()),
								*this); };

	/*! Get an iterator from the start point
	 *  Will proceed clockwise and end at the same point
	 */
	iterator fromStart() { return iterator(start, *this); };

	/*! Get an iterator from the start point
	 *  Will proceed counter clockwise and end at the same point
	 */
	reverse_iterator fromEnd() { return reverse_iterator(rstart, *this); }

	/*! Find points that are suspended by material underneath.
	 *  This is not implemented as the suspended property is not implemented.
	 *  /return An iterator to retrieve all suspended points, currently
	            retrieves all points
	 */
	iterator getSuspendedPoints() { return fromStart(); }; //stub

private:
	Loop &parent;
	Loop::cw_iterator start;
	Loop::ccw_iterator rstart;
};

}
#endif
