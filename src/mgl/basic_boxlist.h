/* 
 * File:   basic_boxlist.h
 * Author: Dev
 *
 * Created on September 28, 2012, 4:27 PM
 */

#ifndef MGL_BASIC_BOXLIST_H
#define	MGL_BASIC_BOXLIST_H

#include "spacial_data.h"
#include <vector>
#include <utility>

namespace mgl {

/*
 basic_boxlist implements the interface for a spacial index.
 A spacial index is a class that contains two-dimensional objects and 
 allows queries for these objects based on their location and geometry.
 
 Internally, spacial data structures work on AABBox (axis aligned 
 bounding box). They must be able to construct a bounding box for 
 the type that they contain. This is done by calling the static function
 AABBox boundingBox = to_bbox<value_type>::bound(value);
 
 YOU MUST PROVIDE A TEMPLATE SPECIALIZATION FOR to_bbox<>::bound(...);
 following this form:
 template <>
 struct to_bbox<YourType> {
    static AABBox bound(const YourType&) {
        //see spacial_data.h for interface to AABBox
    }
 }
 
 Spacial indexes are safe to copy construct and assign. They store copies 
 of all things inserted into them and fill search results with copies of 
 their contents. You may have an index of pointers if you implement 
 the correct specialization of to_bbox.
 
 Spacial data structures may be queried by calling search(collection, filter)
 collection is any object that supports push_back(const value_type&). This 
 would usually be a list or vector of the same type as the spacial index.
 filter is any object with a function bool object::filter(const AABBox&) const; 
 This function should return true for bounding boxes that meet the criteria of 
 what you wish to search for, and false otherwise.
 
 Removal from spacial indexes is not universally implemented yet.
 
 */

template <typename T>
class basic_boxlist {
public:
    typedef T value_type;
    typedef std::pair<value_type, AABBox> value_bounds;
    
    typedef std::vector<value_bounds> internal_container;
    typedef typename internal_container::iterator iterator;
    typedef typename internal_container::const_iterator const_iterator;
    
    /*!Insert a value into the spacial index
     @value: a const reference of what should be inserted.
     a copy of this will be stored.
     @return: an iterator to what you just inserted (not implemented)*/
    iterator insert(const value_type& value) {
        return data.insert(data.end(), value_bounds(value, 
                to_bbox<value_type>::bound(value)));
    }
    /*!Not universally implemented, do not use!*/
    void erase (iterator iter) {
        data.erase(iter);
    }
    /*!Search for values that meet criteria of filt.filter(AABBox)
     @result: Object supporting push_back(...) where output is placed
     @filter: object supporting filter(...) that defines the criteria
     Contents of index are not modified, copies of values that pass 
     the filter are placed in result.*/
    template <typename COLLECTION, typename FILTER>
    void search(COLLECTION& result, const FILTER& filt) const {
        for(const_iterator iter = data.begin(); 
                iter != data.end(); 
                ++iter) {
            if(filt.filter(iter->second))
                result.push_back(iter->first);
        }
    }
    /*!Not universally implemented, do not use!*/
    iterator begin() { return data.begin(); }
    /*!Not universally implemented, do not use!*/
    iterator end() { return data.end(); }
    
private:
    internal_container data;
};

}

#endif	/* MGL_BASIC_BOXLIST_H */

