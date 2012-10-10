#ifndef LOOP_PATH_OPENPATH_IMPL_H
#define	LOOP_PATH_OPENPATH_IMPL_H

#include <vector>

#include "loop_path_decl.h"
#include "loop_path_iter_impl.h"

namespace mgl {

OpenPath::OpenPath() {
	endpoints.resize(2);
}

OpenPath::iterator OpenPath::fromStart() { 
	return iterator(points.begin()); 
}

OpenPath::const_iterator OpenPath::fromStart() const { 
	return const_iterator(points.begin()); 
}

OpenPath::reverse_iterator OpenPath::fromEnd() { 
	return reverse_iterator(points.rbegin()); 
}

OpenPath::const_reverse_iterator OpenPath::fromEnd() const {
	return const_reverse_iterator(points.rbegin()); 
}

OpenPath::iterator OpenPath::end() { 
	return iterator(points.end()); 
}

OpenPath::const_iterator OpenPath::end() const { 
	return const_iterator(points.end()); 
}

OpenPath::reverse_iterator OpenPath::rend() {
	return reverse_iterator(points.rend());
}

OpenPath::const_reverse_iterator OpenPath::rend() const {
	return const_reverse_iterator(points.rend());
}

void OpenPath::appendPoint(const Point2Type& point) {
	points.insert(points.end(), point);
}

void OpenPath::prependPoint(const Point2Type& point) {
	points.insert(points.begin(), point);
}

OpenPath::entry_iterator OpenPath::entryBegin() {
	setEndPoints();
	return entry_iterator(endpoints.begin());
}

OpenPath::const_entry_iterator OpenPath::entryBegin() const {
	setEndPoints();
	return const_entry_iterator(endpoints.begin());
}

OpenPath::entry_iterator OpenPath::entryEnd() {
	setEndPoints();
	return entry_iterator(endpoints.end());
}

OpenPath::const_entry_iterator OpenPath::entryEnd() const {
	setEndPoints();
	return const_entry_iterator(endpoints.end());
}

Point2Type& OpenPath::getExitPoint(Point2Type entry) {
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

const Point2Type& OpenPath::getExitPoint(Point2Type entry) const {
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

bool OpenPath::empty() const {
	return points.empty();
}

bool OpenPath::isEnd(iterator i) const {
	return i == end();
}

bool OpenPath::isEnd(reverse_iterator i) const {
	return i == rend();
}

bool OpenPath::isEnd(const_iterator i) const {
	return i == end();
}

bool OpenPath::isEnd(const_reverse_iterator i) const {
	return i == rend();
}

void OpenPath::setEndPoints() const {
	endpoints.resize(2);
	endpoints[0] = points.front();
	endpoints[1] = points.back();
}


}


#endif	/* LOOP_PATH_OPENPATH_IMPL_H */

