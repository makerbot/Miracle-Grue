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
    
    iterator insert(const value_type& value) {
        return data.insert(data.end(), value_bounds(value, 
                to_bbox<value_type>::bound(value)));
    }
    void erase (iterator iter) {
        data.erase(iter);
    }
    template <typename COLLECTION, typename FILTER>
    void search(COLLECTION& result, const FILTER& filt) {
        for(typename internal_container::iterator iter = data.begin(); 
                iter != data.end(); 
                ++iter) {
            if(filt.filter(iter->second))
                result.push_back(iter->first);
        }
    }
    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    const_iterator begin() const { return data.begin(); }
    const_iterator end() const { return data.end(); }
    
private:
    internal_container data;
};

}

#endif	/* MGL_BASIC_BOXLIST_H */

