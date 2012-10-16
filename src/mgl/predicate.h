#ifndef MGL_PREDICATE_H
#define	MGL_PREDICATE_H

#include <vector>

namespace mgl {

template <typename T>
class abstract_predicate {
public:
    typedef T value_type;
    virtual ~abstract_predicate() {}
    bool operator ()(const value_type& lhs, const value_type& rhs) const {
        return compare(lhs, rhs) == BETTER;
    }
protected:
    static const int BETTER = 1;
    static const int WORSE = -1;
    static const int SAME = 0;
    virtual int compare(const value_type& lhs, const value_type& rhs) const = 0;
};

template <typename T>
class abstract_predicate_collection : public abstract_predicate<T> {
    typedef std::vector<const abstract_predicate<T>*> predicate_collection;
    typedef typename abstract_predicate<T>::value_type value_type;
public:
    virtual void register_predicate(const abstract_predicate<T>* predicate) {
        predicates.push_back(predicate);
    }
protected:
    virtual int compare(const value_type& lhs, const value_type& rhs) {
        for(typename predicate_collection::const_iterator iter = predicates.begin(); 
                iter != predicates.end(); 
                ++iter) {
            int decision = iter->compare(lhs, rhs);
            if(decision)
                return decision;
        }
        return abstract_predicate<T>::SAME;
    }
private:
    predicate_collection predicates;
};

}

#endif	/* MGL_PREDICATE_H */

