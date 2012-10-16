#ifndef MGL_PREDICATE_H
#define	MGL_PREDICATE_H

#include <vector>

namespace mgl {

static const int BETTER = 1;
static const int WORSE = -1;
static const int SAME = 0;

template <typename T>
class abstract_predicate {
public:
    typedef T value_type;
    
    template <typename U>
    friend class abstract_predicate;
    
    virtual ~abstract_predicate() {}
    bool operator ()(const value_type& lhs, const value_type& rhs) const {
        return compare(lhs, rhs) == BETTER;
    }
    virtual int compare(const value_type& lhs, const value_type& rhs) const = 0;
};

template <typename T, int COMP = 0>
class tiebreaker_predicate : public abstract_predicate<T> {
public:
    typedef typename abstract_predicate<T>::value_type value_type;
    int compare(const value_type&, const value_type&) const {
        return COMP;
    }
};

template <typename T, typename U, typename V>
class composite_predicate : public abstract_predicate<T> {
public:
    typedef typename abstract_predicate<T>::value_type value_type;
    composite_predicate(const U& u = U(), const V& v = V())
            : m_u(u), m_v(v) {}
    int compare(const value_type& lhs, const value_type& rhs) const {
        int ur = m_u.compare(lhs, rhs);
        if(ur)
            return ur;
        return m_v.compare(lhs, rhs);
    }
protected:
    U m_u;
    V m_v;
};

template <typename T>
class abstract_predicate_collection : public abstract_predicate<T> {
    typedef std::vector<const abstract_predicate<T>*> predicate_collection;
public:
    typedef typename abstract_predicate<T>::value_type value_type;
    virtual void register_predicate(const abstract_predicate<T>* predicate) {
        predicates.push_back(predicate);
    }
    virtual int compare(const value_type& lhs, const value_type& rhs) const {
        for(typename predicate_collection::const_iterator iter = predicates.begin(); 
                iter != predicates.end(); 
                ++iter) {
            int decision = iter->compare(lhs, rhs);
            if(decision)
                return decision;
        }
        SAME;
    }
private:
    predicate_collection predicates;
};

}

#endif	/* MGL_PREDICATE_H */

