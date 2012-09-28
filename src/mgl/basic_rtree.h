#ifndef MGL_BASIC_RTREE_H
#define	MGL_BASIC_RTREE_H

#include "mgl.h"
#include "spacial_data.h"

namespace mgl {

template <typename T, size_t C = 3>
class basic_rtree {
public:
    typedef T value_type;
    typedef std::allocator<basic_rtree> alloc_t;
    
    
    //class node_iterator{};
    class iterator{};
    class const_iterator{};
    
    //typedef std::pair<node_iterator, leaf_iterator> iter_pair;
    ~basic_rtree();
    
    iterator insert(const value_type& value);
    void erase(iterator iter);
    template <typename COLLECTION, typename FILTER>
    void search(COLLECTION& result, const FILTER& filt);
    
private:
    static const size_t CAPACITY = C;
    
    void split(basic_rtree* child);   //redistribute my children
    bool isLeaf() const { return myData; }
    AABBox myBounds;
    basic_rtree* children[CAPACITY];
    
    value_type* myData;
        
};

}


#endif	/* MGL_BASIC_RTREE_H */

