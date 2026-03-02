#ifndef PMERGEME_HPP
#define PMERGEME_HPP
#include <algorithm>    // for std::swap, std::distance
#include <cstddef>      // for std::size_t

#ifdef DEBUG
extern int num_comparisons;
#endif

template <typename Container>
class PmergeMe {
public:
    typedef Container container_type;
    typedef typename Container::value_type value_type;
    typedef typename Container::iterator iterator;
    typedef typename Container::const_iterator const_iterator;
    PmergeMe() {}
    PmergeMe(const PmergeMe& rhs) { *this = rhs; }
    PmergeMe& operator=(const PmergeMe& rhs) {
        (void)rhs;
        return *this;
    }
    ~PmergeMe() {}
    Container mergeInsertionSort(typename Container::iterator first,
                               typename Container::iterator last);

private:
    int binaryInsertion(Container& mainChain,
            const value_type& val,
            int right);
    void makePairsAndSwap(Container& elems,
                          Container& idx);
    void jacobsthalInsert(Container& mainChain, Container& mainIdx,
                                           const Container& remChain, const Container& remIdx,
                                           int N);
    void materializeChains(const_iterator first,
                            Container& mainIdx,
                            Container& remIdx,
                            Container& mainVals,
                            Container& remVals,
                            const Container& sortIdx);
    
};

#include "PmergeMe.tpp"

#endif