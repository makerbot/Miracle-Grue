/* 
 * File:   labeled_path.h
 * Author: Dev
 *
 * Created on July 23, 2012, 4:47 PM
 */

#ifndef LABELED_PATH_H
#define	LABELED_PATH_H

#include "loop_path.h"
#include "mgl.h"

namespace mgl {

class PathLabel {
public:
	//is this an infill, inset, or outline
	enum TYPE {
		TYP_INFILL,
		TYP_INSET,
		TYP_OUTLINE,
		TYP_CONNECTION,
		TYP_INVALID
	};
	//this can and should change, not used by gcoder in this release
	enum OWN {
		OWN_MODEL,
		OWN_SUPPORT,
		OWN_INVALID
	};
	PathLabel(TYPE typ = TYP_INVALID, 
			OWN own = OWN_INVALID, 
			int val = -1) 
			: myType(typ), myOwner(own), myValue(val) {}
	bool operator==(const PathLabel& r) {
	return myType == r.myType && 
			myOwner == r.myOwner;
	}
	bool isInfill() const {
		return myType == TYP_INFILL;
	}
	bool isInset() const {
		return myType == TYP_INSET;
	}
	bool isOutline() const {
		return myType == TYP_OUTLINE;
	}
	bool isConnection() const {
		return myType == TYP_CONNECTION;
	}
	bool isSupport() const {
		return myOwner == OWN_SUPPORT;
	}
	bool isInvalid() const {
		return myType == TYP_INVALID || myOwner == OWN_INVALID;
	}
	bool isValid() const {
		return !isInvalid();
	}
	//what type of extrusion this is
	TYPE myType;
	//is this model or support?
	OWN myOwner;
	//value depends on above (for example, how deep an inset is)
	int myValue;
};

template <typename PATH>
class basic_labeled_path {
public:
	typedef PATH value_type;
	typedef PATH& reference;
	typedef const PATH& const_reference;
	typedef PATH* pointer;
	typedef const PATH* const_pointer;
	
	basic_labeled_path(const PathLabel& label = PathLabel(), 
			const_reference path = value_type())
			: myLabel(label), myPath(path) {}
	
	PathLabel myLabel;
	value_type myPath;
};

typedef basic_labeled_path<OpenPath> LabeledOpenPath;
typedef basic_labeled_path<Loop> LabeledLoop;
//don't use this one!
//typedef basic_labeled_path<LoopPath> LabeledLoopPath;

template <typename COMPARE = std::less<int> >
class basic_labelvalue_comparator {
public:
	basic_labelvalue_comparator(const COMPARE& compare = COMPARE())
			: myCompare(compare) {}
	bool operator ()(const PathLabel& lhs, const PathLabel& rhs) const {
		return myCompare(lhs.myValue, rhs.myValue);
	}
private:
	COMPARE myCompare;
};

template <typename PATH, typename COMPARE = basic_labelvalue_comparator<std::less<int> > >
class basic_labeled_path_comparator {
public:
	basic_labeled_path_comparator(const COMPARE& compare = COMPARE()) 
			: myCompare(compare) {}
	bool operator ()(const basic_labeled_path<PATH>& lhs, 
			const basic_labeled_path<PATH>& rhs) const {
		return myCompare(lhs.myLabel, rhs.myLabel);
	}
private:
	COMPARE myCompare;
};



}



#endif	/* LABELED_PATH_H */

