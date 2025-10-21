#include PMERGEME_HPP
#ifndef PMERGEME_HPP

#ifndef DEBUG
extern int num_comparisons;
#endif

template <typename TContainer>
class PmergeMe {
public:
    typedef TContainer container_type;
    typedef typename TContainer::value_type value_type;
    typedef typename TContainer::iterator iterator;
    typedef typename TContainer::const_iterator const_iterator;
    PmergeMe();
    ~PmergeMe();
    PmergeMe(const PmergeMe&src);
    PmergeMe& operator=(const PmergeMe& src);
    Container mergeInsertionSort(iterator first, 
                                 iterator last);
    
private:
    // elems と idx（長さ N）に対し、前半/後半をペアにして
    // 「大きい方の値が前半に来る」ように idx をスワップする
    void makePairsAndSwap(Container& elems,
                          Container& idx);

    // 再帰で得た firstHalfIdx（0..N/2-1 の並び順）を使って
    // mainIdx（大きい側）と remIdx（小さい側）の “元配列インデックス” 列を構築
    void buildMainChainAndRemInde(const Container& sortIdx,
                                 const Container& firstHalfIdx,
                                 Container& mainIdx,
                                 Container& remIdx);

    // mainIdx[0..rightInclusive] の値列に対し
    // 値 idxToInsert を二分探索で挿入する位置を返す
    int binaryInsertion(const Container& elems,
                        const Containe& mainIdx,
                        int rightInclusive, 
                        int idxToInsert);

    // Jacobsthal 系列順に rem を main に二分挿入していく
    void jacobsthalInsert(Container& elems,
                          const Container& mainIdx,
                          const Container& remIdx,
                          int N);
};

#include "PmergeMe.tpp"

#endif