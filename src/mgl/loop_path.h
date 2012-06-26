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
	template <typename BASE>
	class iterator_gen {
	public:
		typedef BASE iterator;
		iterator_gen(BASE b) : base(b) {};

		PointType& operator*() { return *base; }
		PointType* operator->() { return &*base; }
		iterator operator&() const { return base; }
		// ++iterator
		iterator_gen<BASE>& operator++() {
			++base;
			return *this;
		}
		// iterator++
		iterator_gen<BASE> operator++(int) {
			iterator_gen<BASE> iter_copy = *this;
			++*this;
			return iter_copy;
		}
		iterator_gen<BASE>& operator+=(int off) {
			base += off;
			return *this;
		};
		iterator_gen<BASE> operator+(int off) {
			iterator_gen<BASE> iter_copy = *this;
			return iter_copy+=off;
		}
		// --iterator
		iterator_gen<BASE>& operator--() {
			--base;
			return *this;
		}
		// iterator--
		iterator_gen<BASE> operator--(int) {
			iterator_gen<BASE> iter_copy = *this;
			--*this;
			return iter_copy;
		}
		iterator_gen<BASE>& operator-=(int off) {
			return base += (-off);
		}
		iterator_gen<BASE> operator-(int off) {
			return *this + (-off);
		}
		bool operator==(iterator_gen<BASE> other) {
			return base == other.base;
		}

	private:
		BASE base;
	};

public:
	typedef iterator_gen<PointList::iterator> iterator;
	typedef iterator_gen<PointList::reverse_iterator> reverse_iterator;
	typedef iterator entry_iterator;

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
	template <typename ITER>
	void appendPoints(ITER first, ITER end) {
		for (; first != end; ++first)
			appendPoint(*first);
	}

	/*! Add a single point to the end of a path. This copies the point
	 *  /param point value to be appended
	 */
	void appendPoint(const PointType &point) {
		points.push_back(point);
	}

	/*! Add points to the beginning of a path. This makes coppies of the points.
	 *  /param first iterator to the first point to append
	 *  /param end stop iterating here
	 */
	template <typename ITER>
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
	 *  /return The line segment starting with the point at location, or a zero
	 *          length segment if this is the last point in the path
	 */
	template <typename ITER>
	libthing::LineSegment2 segmentAfterPoint(ITER &beginning) {
		ITER endpoint = beginning;
		++endpoint;

		if (isEnd(endpoint))
			//0 len segment if we're at the last point
			return libthing::LineSegment2(*beginning, *beginning);

		else
			return libthing::LineSegment2(*beginning, *endpoint);
	}

	/*! Find points you can start extrusion on for this path.  For an
	 *  OpenPath, this gives you the endpoints.
	 *  /return iterator for all the valid staring points
	 */
	entry_iterator getEntryPoints() {
		setEndPoints();
		return iterator(endpoints.begin());
	}

	PointType& getExitPoint(PointType entry) {
		setEndPoints();
		if (endpoints[0] == entry) {
			return endpoints[1];
		}
		else if (endpoints[1] == entry) {
			return endpoints[0];
		}
		else {
			throw Exception("Not a valid entry point");
		}
	}

	/*! Find points that are suspended by material underneath.
	 *  This is not implemented as the suspended property is not implemented.
	 *  /return An iterator to retrieve all suspended points, currently
	            retrieves all points
	 */
	iterator getSuspendedPoints() { return fromStart(); }; //stub
	
private:
	bool isEnd(iterator i) {
		return i == end();
	}

	bool isEnd(reverse_iterator i) {
		return i == rend();
	}

	void setEndPoints() {
		endpoints[0] = points.front();
		endpoints[1] = points.back();
	}		

	PointList points;
	PointList endpoints;
};


/*! /brief A closed loop
 *  A 2d path that has no beginning or end, connects around to itself, and has
 *  an interior.
 */
class Loop {
public:
	class PointNormal;
	typedef std::vector<PointNormal> PointNormalList;
private:
	template <typename BASE>
	class iterator_gen {
	public:
		typedef BASE iterator;
		iterator_gen(BASE i, BASE b, BASE e) : base(i), begin(b), end(e) {};
		
		PointNormal& operator*() { return *base; }
		PointNormal* operator->() { return &*base; }
		iterator operator&() const { return base; }
		// ++iterator
		iterator_gen<BASE>& operator++() {
			++base;
			if (base == end) {
				base = begin;
			}
			
			return *this;
		}
		// iterator++
		iterator_gen<BASE> operator++(int) {
			iterator_gen<BASE> iter_copy = *this;
			return ++iter_copy;
		}
		// --iterator
		iterator_gen<BASE>& operator--() {
			if(base == begin){
				base = end;
			}
			--base;
			return *this;
		}
		// iterator--
		iterator_gen<BASE> operator--(int) {
			iterator_gen<BASE> iter_copy = *this;
			return --iter_copy;
		}
		
		bool operator==(iterator_gen<BASE> other) {
			return base == other.base;
		};
	private:
		BASE base;
		BASE begin;
		BASE end;
	};

public:
	
	class PointNormal{
	public:
		
		typedef iterator_gen<PointNormalList::iterator> myIteratorType;
		PointNormal(const PointType& point) : point(point), normalDirty(true), 
				myIteratorPointer(NULL) {}
		PointNormal() : normalDirty(true), myIteratorPointer(NULL) {}
		PointNormal(const PointNormal& orig) : point(orig.point), 
				normalDirty(orig.normalDirty), normal(orig.normal),
				myIteratorPointer(NULL) {
			if(orig.myIteratorPointer)
				myIteratorPointer = new 
						myIteratorType(*orig.myIteratorPointer);
		}
		PointNormal& operator=(const PointNormal& orig) {
			if(&orig == this)
				return *this;
			point = orig.point;
			normalDirty = orig.normalDirty;
			normal = orig.normal;
			delete myIteratorPointer;
			if(orig.myIteratorPointer)
				myIteratorPointer = new 
						myIteratorType(*orig.myIteratorPointer);
		}
		~PointNormal() {
			delete myIteratorPointer;
		}
		operator PointType() const { return point; }
		const PointType& getPoint() const { return point; }
		const PointType& getNormal() const { 
			if(normalDirty)
				recalculateNormal();
			return normal;
		}
		void setPoint(const PointType& npoint) {
			point = npoint;
			normalDirty = true;
			if(myIteratorPointer){
				myIteratorType Bi = *myIteratorPointer;
				myIteratorType Ai = Bi;
				myIteratorType Ci = Bi;
				--Ai;	//point to previous
				++Ci;	//point to next
				Ai->normalDirty = true;
				Ci->normalDirty = true;
			}
		}
		void setIterator(const myIteratorType& iter) {
			if(myIteratorPointer)
				*myIteratorPointer = iter;
			else
				myIteratorPointer = new myIteratorType(iter);
		}
		
	private:
		void recalculateNormal() const{
			normalDirty = false;
			/* A------B------C
			 * Assume we are point B. Normal is (B-A).rotate(90 degrees cw)
			 * normalized average with (C-B).rotate(90 degrees cw) normalized
			 * Then normalize the average
			 */
			if(myIteratorPointer){
				myIteratorType Bi = *myIteratorPointer;
				myIteratorType Ai = Bi;
				myIteratorType Ci = Bi;
				--Ai;	//point to previous
				++Ci;	//point to next
				recalculateNormal(*Ai, *Bi);
			} else {
				normal = PointType();
			}
		}
		void recalculateNormal(const PointNormal& A, const PointNormal& C) const {
			// A------B------C
			// this is B
			PointType ba = point-A;
			PointType cb = static_cast<PointType>(C)-point;
			ba = ba.rotate2d(M_PI_2).unit();
			cb = cb.rotate2d(M_PI_2).unit();
			normal = (ba+cb).unit();
		}
		PointType point;
		//true when need to recalculate normal
		mutable bool normalDirty;
		mutable PointType normal;
		myIteratorType* myIteratorPointer;
	};
	
	typedef iterator_gen<PointNormalList::iterator> cw_iterator;
	typedef iterator_gen<PointNormalList::reverse_iterator> ccw_iterator;
	typedef cw_iterator entry_iterator;

	Loop() { }
	Loop(const PointType &first) { pointNormals.push_back(first); }
	/*! Insert a point into the loop at a specific location.
	 *  The iterator passed to after is not guaranteed valid when this operation
	 *  is done
	 *  /param point value to be inserted
	 *  /param iterator location for this to be inserted after
	 *  /return iterator for the location of the new point
	 */
	template <typename ITER>
	cw_iterator insertPoint(const PointType &point, ITER after){
		typename ITER::iterator afterbase = &after;
		afterbase = pointNormals.insert(afterbase, point);
		return cw_iterator(afterbase, pointNormals.begin(), pointNormals.end());
	}

	/*! Get an iterator that traverses around the loop clockwise.  There is no
	 *  end, the iterator will continue around the loop indefinitely.
	 *  /param startpoint Point value to start on
	 *  /return clockwise iterator from the start point
	 */
	cw_iterator clockwise(const PointType &startpoint) {
		for (PointNormalList::iterator i = pointNormals.begin();
			 i != pointNormals.end(); i++) {
			if (static_cast<PointType>(*i) == startpoint)
				return cw_iterator(i, pointNormals.begin(), 
						pointNormals.end());
		}
	}
	/*! Get an iterator that traverses around the loop clockwise from an
	 *  arbitrary start point.  There is no end, the iterator will continue
	 *  around the loop indefinitely.
	 *  /return clockwise iterator from the start point
	 */
	cw_iterator clockwise() { return clockwise(pointNormals.front()); };

	/*! Get an iterator that traverses around the loop counter clockwise.
	 *  There is no end, the iterator will continue around the loop
	 *  indefinitely.
	 *  /param startpoint Point value to start on
	 *  /return counter clockwise iterator from the start point
	 */
	ccw_iterator counterClockwise(const PointType &startpoint) {
		for (PointNormalList::reverse_iterator i = pointNormals.rbegin();
			 i != pointNormals.rend(); i++) {
			if (static_cast<PointType>(*i) == startpoint)
				return ccw_iterator(i, pointNormals.rbegin(), 
						pointNormals.rend());
		}
	}

	/*! Get an iterator that traverses around the loop counter clockwise from an
	 *  arbitrary start point. There is no end, the iterator will continue
	 *  around the loop indefinitely.
	 *  /return counter clockwise iterator from the start point
	 */
	ccw_iterator counterClockwise() {
		return counterClockwise(pointNormals.front());
	};

	/*! Retrieve the LineSegment2 in the path that starts with a provided point.
	 *  This creates a new LineSegment2 value
	 *  /param location iterator pointing to the first endpoint of the segment
	 *  /return The line segment starting with the point at location
	 */
	template <typename ITER>
	libthing::LineSegment2 segmentAfterPoint(const ITER &location);

	/*! Retrieve the normal vector in a loop at a provided point.
	 *  Normals point toward the interior of a loop.
	 *  /param location iterator pointing to the point before the normal
	 *  /return The normal vector at this location
	 */
	template <typename ITER>
	PointType normalAfterPoint(const ITER &location);

	/*! Find points you can start extrusion on for this path.  For a
	 *  Loop, this gives you every point in the loop.
	 *  /return iterator for all the valid staring points
	 */
	cw_iterator getEntryPoints() {
		return cw_iterator(pointNormals.begin(), 
				pointNormals.begin(), pointNormals.end());
	};

	/*! Find points that are suspended by material underneath.
	 *  This is not implemented as the suspended property is not implemented.
	 *  /return An iterator to retrieve all suspended points, currently
	            retrieves all points
	 */
	cw_iterator getSuspendedPoints() { 
		return clockwise(pointNormals.front()); 
	};

	friend class LoopPath;
private:
	
	void refreshIteratorRefs(){
		for(PointNormalList::iterator it = pointNormals.begin(); 
				it != pointNormals.end(); 
				++it){
			it->setIterator(PointNormal::myIteratorType(it, 
					pointNormals.begin(), 
					pointNormals.end()));
		}
	}

	PointList points;
	VectorList normals;
	PointNormalList pointNormals;
};

/*! /brief Adapter to make a Loop look like an OpenPath
 *  Provides an interface to a loop that functions similar to the read only
 *  operation of OpenPath.  
 */
class LoopPath {
	template <typename BASE>
	class iterator_gen {
	public:
		typedef BASE iterator;
		iterator_gen(BASE i, LoopPath &p) : base(i), parent(p) {};

		PointType& operator*() { return *base; }
		PointType* operator->() { return &*base; }
		iterator operator&() const { return base; }
		// ++iterator
		iterator_gen<BASE>& operator++() {
			++base;
			if (base == parent.start()) {
				base = parent.end();
			}
			return *this;
		}
		// iterator++
		iterator_gen<BASE> operator++(int) {
			iterator_gen<BASE> iter_copy = *this;
			return ++iter_copy;
		}
		// --iterator
		iterator_gen<BASE>& operator--() {
			if(base == parent.start){
				base = end;
			} else {
				--base;
			}
			return *this;
		}
		// iterator--
		iterator_gen<BASE> operator--(int) {
			iterator_gen<BASE> iter_copy = *this;
			return --iter_copy;
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
		return iterator(Loop::cw_iterator(parent.pointNormals.end(),
										  parent.pointNormals.end(),
										  parent.pointNormals.end()),
						*this); };

	/*! Reverse iterator after the end of the list. */
	reverse_iterator rend() {
		return reverse_iterator(Loop::ccw_iterator(parent.pointNormals.rend(),
												   parent.pointNormals.rend(),
												   parent.pointNormals.rend()),
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
