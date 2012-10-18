#ifndef MGL_BASIC_QUADTREE_DECL_H
#define	MGL_BASIC_QUADTREE_DECL_H

#include "mgl.h"
#include "spacial_data.h"
#include <sstream>
#include <memory>
#include <utility>
#include <vector>
//#include "FSBAllocator.h"

namespace mgl {

class QuadTreeException : public Exception {
public:
    template <typename T>
    QuadTreeException(const T& arg) : Exception(arg) {}
};

template <typename T>
class basic_quadtree {
public:
    typedef T value_type;
    
    static value_type* DEFAULT_DATA_PTR() { return NULL; }
    static basic_quadtree* DEFAULT_CHILD_PTR() { return NULL; }
    
    typedef std::allocator<basic_quadtree> tree_alloc_t;
    typedef typename tree_alloc_t::template rebind<value_type>::other value_alloc_t;
    
    class iterator{
    public:
        iterator() {}
        template <typename U>
        iterator(const U&) {  }
    };
    typedef iterator const_iterator;
    
    basic_quadtree(AABBox bb);
    basic_quadtree(const basic_quadtree& other);
    basic_quadtree& operator =(const basic_quadtree& other);
    ~basic_quadtree();
    
    iterator insert(const value_type& value);
    void erase(iterator) {}
    template <typename COLLECTION, typename FILTER>
    void search(COLLECTION& result, const FILTER& filt) const;
    
    iterator begin() { return iterator(); }
    iterator end() { return iterator(); }
    const_iterator begin() const { return const_iterator(); }
    const_iterator end() const { return const_iterator(); }
    
    void repr(std::ostream& out, size_t recursionLevel = 0);
    void repr_svg(std::ostream& out, size_t recursionLevel = 0);
    
private:
    
    static const size_t CAPACITY = 4;
    
    enum QUADRANT {
        QUAD_BL = 0,
        QUAD_BR = 1, 
        QUAD_TL = 2, 
        QUAD_TR = 3
    };
    
    typedef std::pair<AABBox, value_type*> bounded_value;
    typedef std::vector<bounded_value> data_container;
    
    void insert(const bounded_value& bounded);
    bool tryPropagateInsert(const bounded_value& bounded);
    
    bool isLeaf() const { return !childrenExist; }
    bool hasChildren() const { return childrenExist; }
    bool hasData() const { return !myData.empty(); }
    
    void split();
    void split_and_redistribute();
    
    AABBox myBounds;
    basic_quadtree* myChildren[CAPACITY];
    
    data_container myData;
    
    bool childrenExist;
    
    tree_alloc_t myTreeAllocator;
    value_alloc_t myDataAllocator;
    
    
};

}

#endif	/* MGL_BASIC_QUADTREE_DECL_H */

