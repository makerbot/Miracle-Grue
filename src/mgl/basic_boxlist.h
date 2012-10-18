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
    typedef typename internal_container::iterator iterator;
    typedef typename internal_container::const_iterator const_iterator;
    
    iterator insert(const value_type& value) {
        return data.insert(data.end(), value_bounds(value, 
                to_bbox<value_type>::bound(value)));
    }
    void erase (iterator iter) {
        data.erase(iter);
    }
    template <typename COLLECTION, typename FILTER>
    void search(COLLECTION& result, const FILTER& filt) const {
        for(const_iterator iter = data.begin(); 
                iter != data.end(); 
                ++iter) {
            if(filt.filter(iter->second))
                result.push_back(iter->first);
        }
    }
    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    
private:
    internal_container data;
};

}

#endif	/* MGL_BASIC_BOXLIST_H */

