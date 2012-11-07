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
 
 Interface methods:
 
 void insert(const value_type& value);  //store a copy of value in this index
 template <typename CONTAINER, typename FILTER>
 void search(CONTAINER& result, const FILTER& filt); //query container
 void swap(spacial_index& other);   //fast swap implementation
 
 */

template <typename T>
class basic_boxlist {
public:
    typedef T value_type;
    typedef std::pair<value_type, AABBox> value_bounds;
    
    typedef std::vector<value_bounds> internal_container;
    
    template <typename BASE>
    class basic_iterator {
        friend class basic_boxlist;
    public:
        basic_iterator& operator++() { ++m_base; return *this; }
        basic_iterator operator++(int) { basic_iterator copy = *this; ++*this; return copy; }
        bool operator== (const basic_iterator& rhs) { return m_base ==  rhs.m_base; }
        bool operator!= (const basic_iterator& rhs) { return !(*this==rhs); }
        const typename BASE::value_type::first_type& operator* () { return m_base->first; }
        const typename BASE::value_type::first_type* operator-> () { return &**this; }
    private:
        basic_iterator(BASE base) : m_base(base) {}
        BASE m_base;
    };
    
    typedef basic_iterator<typename internal_container::iterator> iterator;
    typedef basic_iterator<typename internal_container::const_iterator> const_iterator;
    //internal_container::iterator::reference::first_type
    
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
        for(typename internal_container::const_iterator iter = data.begin(); 
                iter != data.end(); 
                ++iter) {
            if(filt.filter(iter->second))
                result.push_back(iter->first);
        }
    }
    /*!Swap contents of this object with that of another
     @other: The object with which to swap contents
     In general, this should be a constant time swap that involves 
     no copying of data elements*/
    void swap(basic_boxlist& other) {
        data.swap(other.data);
    }
    /*!Not universally implemented, do not use!*/
    iterator begin() { return data.begin(); }
    /*!Not universally implemented, do not use!*/
    iterator end() { return data.end(); }
    const_iterator begin() const { return data.begin(); }
    const_iterator end() const { return data.end(); }
    
private:
    internal_container data;
};

}

#endif	/* MGL_BASIC_BOXLIST_H */

