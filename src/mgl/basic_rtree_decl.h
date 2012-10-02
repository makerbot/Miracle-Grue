/* 
 * File:   basic_rtree_decl.h
 * Author: Dev
 *
 * Created on October 1, 2012, 10:00 AM
 */

#ifndef MGL_BASIC_RTREE_DECL_H
#define	MGL_BASIC_RTREE_DECL_H

#include "mgl.h"
#include "spacial_data.h"
#include <sstream>

namespace mgl {

class TreeException : public Exception {
public:
    template <typename T>
    TreeException(const T& arg) : Exception(arg) {}
};

static const size_t RTREE_DEFAULT_BRANCH = 4;

template <typename T, size_t C = RTREE_DEFAULT_BRANCH>
class basic_rtree {
public:
    typedef T value_type;
    typedef std::allocator<basic_rtree> tree_alloc_t;
    typedef typename tree_alloc_t::template rebind<value_type>::other value_alloc_t;
    
    class iterator{
    public:
        iterator() {}
        template <typename U>
        iterator(const U&) {  }
    };
    typedef iterator const_iterator;
    
    basic_rtree();
    basic_rtree(const basic_rtree& other);
    basic_rtree& operator =(const basic_rtree& other);
    ~basic_rtree();
    
    iterator insert(const value_type& value);
    void erase(iterator iter);
    template <typename COLLECTION, typename FILTER>
    void search(COLLECTION& result, const FILTER& filt);
    
    iterator begin() { return iterator(); }
    iterator end() { return iterator(); }
    
    void repr(std::ostream& out, size_t recursionLevel = 0);
    
private:
    explicit basic_rtree(const value_type& value);
    basic_rtree(bool canReproduce);
    
    static const size_t CAPACITY = C;
    
    void insert(basic_rtree* child);
    void insertIntelligently(basic_rtree* child);   //here is rtree logic
    void split(basic_rtree* child);   //clone child, distribute its children
    
    void adopt(basic_rtree* from);
    void growTree();
    
    bool isLeaf() const { return myData; }
    bool isFull() const { return size() >= capacity(); }
    bool isEmpty() const { return !isLeaf() && !hasChildren(); }
    bool hasChildren() const { return size(); }
    size_t size() const { return myChildrenCount; }
    size_t capacity() const { return CAPACITY; }
    
    bool splitMyself;   //true for root
    
    AABBox myBounds;
    basic_rtree* myChildren[CAPACITY];
    size_t myChildrenCount;
    
    value_type* myData;
    
    tree_alloc_t myTreeAllocator;
    value_alloc_t myDataAllocator;
        
};

}

#endif	/* BASIC_RTREE_DECL_H */

