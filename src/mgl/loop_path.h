#ifndef LOOP_PATH_H
#define LOOP_PATH_H

#include <vector>
#include <list>
#include <ostream>
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
		
		template <typename OTHERBASE>
		friend class iterator_gen;
		
		typedef BASE iterator;
		typedef typename iterator::value_type value_type;
		typedef typename iterator::reference reference;
		typedef typename iterator::pointer pointer;
		
		iterator_gen(BASE b = BASE());
		template <typename OTHERBASE>
		explicit iterator_gen(const iterator_gen<OTHERBASE>& orig);
		template <typename OTHERBASE>
		iterator_gen<BASE>& operator=(const iterator_gen<OTHERBASE>& orig);

		reference operator*();
		pointer operator->();
		iterator operator&() const;
		// ++iterator
		iterator_gen<BASE>& operator++();
		// iterator++
		iterator_gen<BASE> operator++(int);
		iterator_gen<BASE>& operator+=(int off);
		iterator_gen<BASE> operator+(int off);
		// --iterator
		iterator_gen<BASE>& operator--();
		// iterator--
		iterator_gen<BASE> operator--(int);
		iterator_gen<BASE>& operator-=(int off);
		iterator_gen<BASE> operator-(int off);
		template <typename OTHERBASE>
		bool operator==(const iterator_gen<OTHERBASE>& other) const;
		template <typename OTHERBASE>
		bool operator!=(const iterator_gen<OTHERBASE>& other) const;

	private:
		iterator base;
	};

public:
	typedef iterator_gen<PointList::iterator> iterator;
	typedef iterator_gen<PointList::const_iterator> const_iterator;
	typedef iterator_gen<PointList::reverse_iterator> reverse_iterator;
	typedef iterator_gen<PointList::const_reverse_iterator> const_reverse_iterator;
	typedef iterator entry_iterator;
	typedef const_iterator const_entry_iterator;

	OpenPath() : endpoints(2) {};
	/*! Get an iterator from the first point of the path */
	iterator fromStart() { return iterator(points.begin()); };
	const_iterator fromStart() const { return const_iterator(points.begin()); }
	/*! Get an iterator from the last point of the path	 */
	reverse_iterator fromEnd() { return reverse_iterator(points.rbegin()); };
	const_reverse_iterator fromEnd() const { 
		return const_reverse_iterator(points.rbegin()); 
	}

	iterator end() { return iterator(points.end()); };
	const_iterator end() const { return const_iterator(points.end()); };
	reverse_iterator rend() { return reverse_iterator(points.rend()); };
	const_reverse_iterator rend() const { return const_reverse_iterator(points.rend()); };

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
		return entry_iterator(endpoints.begin());
	}
	
	const_entry_iterator getEntryPoints() const {
		setEndPoints();
		return const_entry_iterator(endpoints.begin());
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
	
	const PointType& getExitPoint(PointType entry) const {
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
	const_iterator getSuspendedPoints() const { return fromStart(); }; //stub
	
	bool empty() const { return points.empty(); }
	
private:
	bool isEnd(iterator i) {
		return i == end();
	}
	bool isEnd(reverse_iterator i) {
		return i == rend();
	}
	bool isEnd(const_iterator i) const {
		return i == end();
	}
	bool isEnd(const_reverse_iterator i) const {
		return i == rend();
	}

	void setEndPoints() const {
		endpoints[0] = points.front();
		endpoints[1] = points.back();
	}		

	PointList points;
	mutable PointList endpoints;
};

//std::ostream& operator<<(std::ostream& out, OpenPath& openpath) {
//	for(OpenPath::iterator it = openpath.fromStart(); 
//			it != openpath.end(); 
//			++it)
//		out << *it << std::endl;
//	return out;
//}


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
	class iterator_gen;
	template <typename BASE>
	class iterator_finite_gen;
	
	template <typename BASE>
	class iterator_gen {
	public:
		
		template <typename OTHERBASE>
		friend class iterator_gen;
		template <typename OTHERBASE>
		friend class iterator_finite_gen;
		
		typedef BASE iterator;
		typedef typename iterator::value_type value_type;
		typedef typename iterator::reference reference;
		typedef typename iterator::pointer pointer;
		
		iterator_gen(iterator i, iterator b, iterator e);
		template <typename OTHERBASE>
		explicit iterator_gen(const iterator_gen<OTHERBASE>& orig);
		iterator_gen<BASE>& operator=(const iterator_gen<BASE>& orig);
		
		reference operator*();
		pointer operator->();
		iterator operator&() const;
		// ++iterator
		iterator_gen<BASE>& operator++();
		// iterator++
		iterator_gen<BASE> operator++(int);
		// --iterator
		iterator_gen<BASE>& operator--();
		// iterator--
		iterator_gen<BASE> operator--(int);
		iterator_gen<BASE> makeBegin() const;
		iterator_gen<BASE> makeEnd() const;
		template <typename OTHERBASE>
		bool operator==(const iterator_gen<OTHERBASE>& other) const;
		template <typename OTHERBASE>
		bool operator!=(const iterator_gen<OTHERBASE>& other) const;
		bool isBegin() const;
		bool isEnd() const;
	protected:
		iterator base;
		iterator begin;
		iterator end;
	};
	
	/*! Specialization of iterator_gen for entry points
	 *
	 */
	template <typename BASE>
	class iterator_finite_gen : public iterator_gen<BASE> {
		typedef typename iterator_gen<BASE>::iterator iterator;
		typedef typename iterator_gen<BASE>::reference reference;
		typedef typename iterator_gen<BASE>::pointer pointer;
		typedef typename iterator_gen<BASE>::value_type value_type;
	public:
		
		template <typename OTHERBASE>
		friend class iterator_finite_gen;
		template <typename OTHERBASE>
		friend class iterator_gen;
		
		iterator_finite_gen(iterator i, iterator b, iterator e);
		template <typename OTHERBASE>
		explicit iterator_finite_gen(const iterator_gen<OTHERBASE>& orig);
		template <typename OTHERBASE>
		iterator_finite_gen<BASE>& operator=(
				const iterator_gen<OTHERBASE>& orig);
		// ++iterator	
		iterator_finite_gen<BASE>& operator++();
		// iterator++
		iterator_finite_gen<BASE> operator++(int);
		// --iterator
		iterator_finite_gen<BASE>& operator--();
		// iterator--
		iterator_finite_gen<BASE> operator--(int);
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
		bool operator==(const PointNormal& rhs) const {
			return static_cast<PointType>(*this) == static_cast<PointType>(rhs);
		}
		bool operator!=(const PointNormal& rhs) const {
			return !(*this == rhs);
		}
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
				/* If we have a valid iterator, use it to find the point
				 * before and after, then call the computation on those
				 *
				 */
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
			// rotate and normalize both vectors
			ba = ba.rotate2d(M_PI_2).unit();
			cb = cb.rotate2d(M_PI_2).unit();
			// normal is the unit vector of the sum (same as unit of average)
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
	typedef iterator_finite_gen<PointNormalList::const_iterator> entry_iterator;
	typedef iterator_finite_gen<PointNormalList::iterator> finite_cw_iterator;
	typedef iterator_finite_gen<PointNormalList::reverse_iterator> finite_ccw_iterator;
	typedef iterator_gen<PointNormalList::const_iterator> const_cw_iterator;
	typedef iterator_gen<PointNormalList::const_reverse_iterator> const_ccw_iterator;
	typedef iterator_finite_gen<PointNormalList::const_iterator> const_finite_cw_iterator;
	typedef iterator_finite_gen<PointNormalList::const_reverse_iterator> const_finite_ccw_iterator;

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
		typename ITER::iterator afterbase = &(++after);
		afterbase = pointNormals.insert(afterbase, point);
		return cw_iterator(afterbase, pointNormals.begin(), pointNormals.end());
	}
	/*! Insert points into the loop at a specific location.
	 *  The iterator passed to position is not guaranteed valid when this
	 *  operation is done
	 *  /param iterator location for this to be inserted at
	 *  /return iterator for the new position
	 */
	template <typename ITER, typename OTHERITER>
	ITER insertPoints(ITER position, OTHERITER first, OTHERITER last) {
		typename ITER::iterator at = &position;
		typename ITER::iterator ret = pointNormals.insert(at, &first, &last);
		return ITER(ret, position.makeBegin(), position.makeEnd());
	}

	/*! Get an iterator that traverses around the loop clockwise.  There is no
	 *  end, the iterator will continue around the loop indefinitely.
	 *  /param startpoint Point value to start on
	 *  /return clockwise iterator from the start point
	 */
	cw_iterator clockwise(const PointType &startpoint) {
		for (PointNormalList::iterator i = pointNormals.begin();
			 i != pointNormals.end(); i++) {
			if (i->getPoint() == startpoint)
				return cw_iterator(i, pointNormals.begin(), 
						pointNormals.end());
		}
		return clockwiseEnd();
	}
	const_cw_iterator clockwise(const PointType &startpoint) const {
		for (PointNormalList::const_iterator i = pointNormals.begin();
			 i != pointNormals.end(); i++) {
			if (i->getPoint() == startpoint)
				return const_cw_iterator(i, pointNormals.begin(), 
						pointNormals.end());
		}
		return clockwiseEnd();
	}
	/*! Get an iterator that traverses around the loop clockwise from an
	 *  arbitrary start point.  There is no end, the iterator will continue
	 *  around the loop indefinitely.
	 *  /return clockwise iterator from the start point
	 */
	cw_iterator clockwise() { 
		return cw_iterator(pointNormals.begin(), pointNormals.begin(), 
				pointNormals.end());
	}
	const_cw_iterator clockwise() const { 
		return const_cw_iterator(pointNormals.begin(), pointNormals.begin(), 
				pointNormals.end());
	}

	finite_cw_iterator clockwiseFinite() {
		return finite_cw_iterator(clockwise());
	}
	const_finite_cw_iterator clockwiseFinite() const {
		return const_finite_cw_iterator(clockwise());
	}
	
	/*! Get an iterator that represents an end of the loop.
	 *  This is not a point on the loop, but is returned upon failure to
	 *  find a point using clockwise(const PointType&)
	 *  /return cw_iterator representing the "end"
	 */
	cw_iterator clockwiseEnd() { return cw_iterator(pointNormals.end(), 
			pointNormals.begin(), pointNormals.end()); };
	const_cw_iterator clockwiseEnd() const { return const_cw_iterator(
			pointNormals.end(), pointNormals.begin(), pointNormals.end()); 
	}

	/*! Get an iterator that traverses around the loop counter clockwise.
	 *  There is no end, the iterator will continue around the loop
	 *  indefinitely.
	 *  /param startpoint Point value to start on
	 *  /return counter clockwise iterator from the start point
	 */
	ccw_iterator counterClockwise(const PointType &startpoint) {
		for (PointNormalList::reverse_iterator i = pointNormals.rbegin();
			 i != pointNormals.rend(); i++) {
			if (i->getPoint() == startpoint)
				return ccw_iterator(i, pointNormals.rbegin(), 
						pointNormals.rend());
		}
		return counterClockwiseEnd();
	}
	const_ccw_iterator counterClockwise(const PointType& startpoint) const {
		for (PointNormalList::const_reverse_iterator i = pointNormals.rbegin();
			 i != pointNormals.rend(); i++) {
			if (i->getPoint() == startpoint)
				return const_ccw_iterator(i, pointNormals.rbegin(), 
						pointNormals.rend());
		}
		return counterClockwiseEnd();
	}

	/*! Get an iterator that traverses around the loop counter clockwise from an
	 *  arbitrary start point. There is no end, the iterator will continue
	 *  around the loop indefinitely.
	 *  /return counter clockwise iterator from the start point
	 */
	ccw_iterator counterClockwise() {
		return ccw_iterator(pointNormals.rbegin(), pointNormals.rbegin(), 
				pointNormals.rend());
	}
	const_ccw_iterator counterClockwise() const {
		return const_ccw_iterator(pointNormals.rbegin(), pointNormals.rbegin(), 
				pointNormals.rend());
	}
		
	finite_ccw_iterator counterClockwiseFinite() {
		return finite_ccw_iterator(counterClockwise());
	}
	const_finite_ccw_iterator counterClockwiseFinite() const {
		return const_finite_ccw_iterator(counterClockwise());
	}
	
	/*! Get an iterator that represents an end of the loop.
	 *  This is not a point on the loop, but is returned upon failure to
	 *  find a point using counterClockwise(const PointType&)
	 *  /return ccw_iterator representing the "end"
	 */
	ccw_iterator counterClockwiseEnd() { return ccw_iterator(pointNormals.rend(), 
			pointNormals.rbegin(), pointNormals.rend()); }
	const_ccw_iterator counterClockwiseEnd() const { 
		return const_ccw_iterator(pointNormals.rend(), 
			pointNormals.rbegin(), pointNormals.rend()); 
	}

	/*! Retrieve the LineSegment2 in the path that starts with a provided point.
	 *  This creates a new LineSegment2 value
	 *  /param location iterator pointing to the first endpoint of the segment
	 *  /return The line segment starting with the point at location
	 */
	template <typename ITER>
	libthing::LineSegment2 segmentAfterPoint (ITER location) const {
		ITER second = location;
		++second;
		return libthing::LineSegment2(*location, *second);
	}
	template <typename BASE>
	libthing::LineSegment2 segmentAfterPoint (iterator_finite_gen<BASE> location) const {
		return segmentAfterPoint(static_cast<iterator_gen<BASE> >(location));
	}
	

	/*! Retrieve the normal vector in a loop at a provided point.
	 *  Normals point toward the interior of a loop.
	 *  /param location iterator pointing to the point before the normal
	 *  /return The normal vector at this location
	 */
	template <typename ITER>
	PointType normalAfterPoint(ITER location) const {
		ITER second = location;
		++second;
		PointType normals = (location->getNormal() + second->getNormal());
		return(normals.unit());
	}

	/*! Find points you can start extrusion on for this path.  For a
	 *  Loop, this gives you every point in the loop.
	 *  /return iterator for all the valid staring points
	 */
	entry_iterator entryBegin() const {
		return entry_iterator(pointNormals.begin(), 
				pointNormals.begin(), pointNormals.end());
	};
	
	/*! Get an iterator that represents the end of entry points
	 *  /return entry_iterator representing the "end".
	 */
	entry_iterator entryEnd() const {
		return entry_iterator(pointNormals.end(), 
				pointNormals.begin(), pointNormals.end());
	};
	
	/*! Get an exit point for a given entry point
	 *  /return PointType representing the "end".
	 */
	PointType getExitPoint(entry_iterator entry) const {
		return *entry;
	}
	/*! Find points that are suspended by material underneath.
	 *  This is not implemented as the suspended property is not implemented.
	 *  /return An iterator to retrieve all suspended points, currently
	            retrieves all points
	 */
	cw_iterator getSuspendedPoints() { 
		return clockwise(pointNormals.front()); 
	};
	const_cw_iterator getSuspendedPoints() const { 
		return clockwise(pointNormals.front()); 
	};
	
	bool empty() const { return pointNormals.empty(); }

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

//std::ostream& operator<<(std::ostream& out, Loop& loop) {
//	bool moved = false;
//	for(Loop::cw_iterator it = loop.clockwise(); 
//			!it.isBegin() || !moved; 
//			++it, moved = true)
//		out << *it << std::endl;
//	return out;
//}

/*! /brief Adapter to make a Loop look like an OpenPath
 *  Provides an interface to a loop that functions similar to the read only
 *  operation of OpenPath.  
 */
class LoopPath {
	template <typename BASE>
	class iterator_gen {
	public:
		
		template <typename OTHERBASE>
		friend class iterator_gen;
		
		typedef BASE iterator;
		iterator_gen(const iterator& i, const LoopPath &p);
		template <typename OTHERBASE>
		explicit iterator_gen(const iterator_gen<OTHERBASE>& orig);

		const PointType& operator*();
		const PointType* operator->();
		iterator operator&() const;
		// ++iterator
		iterator_gen<BASE>& operator++();
		// iterator++
		iterator_gen<BASE> operator++(int);
		// --iterator
		iterator_gen<BASE>& operator--();
		// iterator--
		iterator_gen<BASE> operator--(int);
		bool operator==(const iterator_gen<BASE>& other) const;
		bool operator!=(const iterator_gen<BASE>& other) const;

	private:
		iterator base;
		bool hasLooped;
		const LoopPath &parent;
	};
public:
	typedef iterator_gen<Loop::const_cw_iterator> iterator;
	typedef iterator_gen<Loop::const_ccw_iterator> reverse_iterator;
	typedef iterator_gen<Loop::const_cw_iterator> const_iterator;
	typedef iterator_gen<Loop::const_ccw_iterator> const_reverse_iterator;

	/*! Constructor, every LoopPath has a Loop and a start point.
	 *  /param parent The Loop this LoopPath follows
	 *  /param start The clockwise start point
	 *  /param rstart The same start point, counter clockwise
	 */
	LoopPath(const Loop& p, Loop::const_cw_iterator s, Loop::const_ccw_iterator r)
        : parent(&p), start(s), rstart(r) {}

	LoopPath(const Loop& p, Loop::cw_iterator s, Loop::ccw_iterator r)
        : parent(&p), start(s), rstart(r) {}
	
	LoopPath(const LoopPath& orig) 
		: parent(orig.parent), start(orig.start), rstart(orig.rstart) {}
	
	LoopPath& operator=(const LoopPath& orig) {
		if(this != &orig) {
			parent = orig.parent;
			start = orig.start;
			rstart = orig.rstart;
		}
		return *this;
	}


	/*! Iterator after the end of the list. */
	iterator end() {
		return iterator(parent->clockwiseEnd(),
						*this); }
	const_iterator end() const {
		return const_iterator(parent->clockwiseEnd(),
						*this); }

	/*! Reverse iterator after the end of the list. */
	reverse_iterator rend() {
		return reverse_iterator(parent->counterClockwiseEnd(),
								*this); }
	const_reverse_iterator rend() const {
		return const_reverse_iterator(
				parent->counterClockwiseEnd(),
								*this); }
	/*! Get an iterator from the start point
	 *  Will proceed clockwise and end at the same point
	 */
	iterator fromStart() { return iterator(start, *this); };
	const_iterator fromStart() const { 
		return const_iterator(iterator(start, *this)); 
	}

	/*! Get an iterator from the start point
	 *  Will proceed counter clockwise and end at the same point
	 */
	reverse_iterator fromEnd() { return reverse_iterator(rstart, *this); }
	const_reverse_iterator fromEnd() const { 
		return const_reverse_iterator(reverse_iterator(rstart, *this)); 
	}

	/*! Find points that are suspended by material underneath.
	 *  This is not implemented as the suspended property is not implemented.
	 *  /return An iterator to retrieve all suspended points, currently
	            retrieves all points
	 */
	iterator getSuspendedPoints() { return fromStart(); } //stub
	const_iterator getSuspendedPoints() const { return fromStart(); } //stub
	
	bool empty() const { return parent->empty(); }

private:
	const Loop* parent;
	Loop::const_cw_iterator start;
	Loop::const_ccw_iterator rstart;

	bool isBegin(Loop::cw_iterator i) const { return i == start; }
	bool isBegin(Loop::const_cw_iterator i) const { return i == start; }
	bool isBegin(Loop::ccw_iterator i) const { return i == rstart; }
	bool isBegin(Loop::const_ccw_iterator i) const { return i == rstart; }
};

typedef std::list<OpenPath> OpenPathList;
typedef std::list<Loop> LoopList;
typedef std::list<LoopPath> LoopPathList;

}

#include "loop_path_iter_impl.h"


#endif
