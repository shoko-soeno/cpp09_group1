#ifndef PMERGEME_HPP
#define PMERGEME_HPP
#include <algorithm>    // for std::swap, std::distance
#include <cstddef>      // for std::size_t

#ifndef DEBUG
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
    Container mergeInsertionSort(const_iterator first,
                                 const_iterator last);

private:
    // elems と idx（長さ N）に対し、前半/後半をペアにして
    // 「大きい方の値が前半に来る」ように idx をスワップする
    void makePairsAndSwap(const Container& elems,
                          Container& idx);

    // 再帰で得た firstHalfIdx（0..N/2-1 の並び順）を使って
    // mainIdx（大きい側）と remIdx（小さい側）の “元配列インデックス” 列を構築
    void buildMainChainAndRemIndex(const Container& sortIdx,
                                 const Container& firstHalfIdx,
                                 Container& mainIdx,
                                 Container& remIdx);

    // mainIdx[0..rightInclusive] の値列に対し
    // 値 idxToInsert を二分探索で挿入する位置を返す
    // int binaryInsertion(const Container& elems,
    //                     const Container& mainIdx,
    //                     int rightInclusive, 
    //                     int idxToInsert);
    int binaryInsertion(const Container& mainVals,
                    const value_type& val,
                    int rightInclusive);

    // Jacobsthal 系列順に rem を main に二分挿入していく
    void jacobsthalInsert(Container& mainVals, Container& mainIdx,
                                           const Container& remVals, const Container& remIdx,
                                           int N);

    void materializeChains(const_iterator first,
                                            const Container& mainIdx,
                                            const Container& remIdx,
                                            Container& mainVals,
                                            Container& remVals);
    
};

#include "PmergeMe.tpp"

#endif