#ifndef PMERGE_ME_TPP
#define PMERGE_ME_TPP
#include "PmergeMe.hpp"

#ifdef DEBUG
extern int num_comparisons;
#endif

template <typename Container>
int PmergeMe<Container>::binaryInsertion(Container& mainChain,
                                       const typename Container::value_type& val,
                                       int right) {
    int left = 0;
    if (mainChain.size() <= static_cast<size_t>(right)) {
      right = mainChain.size() - 1;
    }
    while (left <= right) {
#ifdef DEBUG
      num_comparisons++;
#endif  // DEBUG
        int mid = left + (right - left) / 2;
        if (mainChain[mid] == val) {
            left = mid;
            break;
        } else if (mainChain[mid] < val) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    mainChain.insert(mainChain.begin() + left, val);
    return left;
}

template <typename Container>
void PmergeMe<Container>::makePairsAndSwap(Container& elems, Container& sortIdx) {
  const int N    = static_cast<int>(sortIdx.size());
  const int half = N / 2;
  for (int i = 0, j = half; i < half; ++i, ++j) {
#ifdef DEBUG
      num_comparisons++;
#endif  // DEBUG
    if (elems[i] < elems[j]) {
      std::swap(elems[i], elems[j]);
      std::swap(sortIdx[i], sortIdx[j]);
    }
  }
}

template <typename Container>
void PmergeMe<Container>::materializeChains(const_iterator first,
                                            Container& mainIdx,
                                            Container& remIdx,
                                            Container& mainChain,
                                            Container& remChain,
                                            const Container& sortIdx) {
  const int N = static_cast<int>(mainIdx.size());
  for (int i = 0; i < N / 2; i++) {
    mainChain.push_back(first[mainIdx[i]]);
    remChain.push_back(first[remIdx[i]]);
  }
  if (N % 2 == 1) {
    remChain.push_back(first[N - 1]);
    // remIdx.push_back(N - 1);
    remIdx.push_back(sortIdx[N - 1]);
  }
  mainChain.insert(mainChain.begin(), remChain.front());
  for (int i = N / 2 - 1; i >= 0; --i) {
    mainIdx[i + 1] = mainIdx[i];
  }
  mainIdx.front() = remIdx.front();
}

// ========= jacobsthalInsert =========
// remVals を Jacobsthal 順に mainVals へ二分挿入していき、対応するインデックス列 mainIdx も更新する
template <typename Container>
void PmergeMe<Container>::jacobsthalInsert(Container& mainChain, Container& mainIdx,
                                           const Container& remChain, const Container& remIdx,
                                           int N) {
  // Jacobsthal: J0=0, J1=2, Jn=J(n-1)+2*J(n-2)
  std::size_t numInserted  = 1; // remChain[0] is already inserted
  std::size_t prevGroup    = 0;
  std::size_t curGroup     = 2;
  std::size_t upperBoundIx = (N - 1 < 4 ? N - 1 : 3);

  while (numInserted + curGroup < remChain.size()) {
    std::size_t end   = numInserted + curGroup;
    std::size_t i = end - 1;
    while (i >= numInserted) {
      int pos = binaryInsertion(mainChain, remChain[i],
                                upperBoundIx - 1);
      // mainIdx を pos 以降で右にずらして remIdx[index] を pos に入れる
      for (int j = N - 1; j > pos; --j) mainIdx[j] = mainIdx[j - 1];
      mainIdx[pos] = remIdx[i];
      if (i == numInserted) break; // size_t の underflow 防止
      --i;
    }
    // update Jacobsthal
    numInserted += curGroup;
    std::size_t nextGroup = curGroup + 2 * prevGroup;
    prevGroup    = curGroup;
    curGroup     = nextGroup;
    // 2 * (2^k - 1) + 1
    // = 2^(k+1) - 2 + 1
    // = 2^(k+1) - 1
    upperBoundIx = 2 * upperBoundIx + 1;
  }

  // 残り（最後の不完全グループ）を逆順で挿入
  if (numInserted < remChain.size()) {
    std::size_t i = remChain.size() - 1;
    while (i >= numInserted) {
      int pos = binaryInsertion(mainChain, remChain[i],
                                upperBoundIx - 1);
      for (int j = N - 1; j > pos; --j) mainIdx[j] = mainIdx[j - 1];
      mainIdx[pos] = remIdx[i];
      if (i == numInserted) break;
      --i;
    }
  }
}

// ========= mergeInsertionSort 本体 =========
template <typename Container>
Container PmergeMe<Container>::mergeInsertionSort(typename Container::iterator first,
                               typename Container::iterator last) {
  const int N = static_cast<int>(std::distance(first, last));
  Container elems(first, last);
  Container sortIdx(N);
  for (int i = 0; i < N; ++i) sortIdx[i] = i;
  if (N <= 1) return sortIdx;
  makePairsAndSwap(elems, sortIdx);
  if (N == 2) { // 2要素の場合は昇順にして返す
    std::swap(sortIdx[0], sortIdx[1]);
    return sortIdx;
  }
  Container firstHalfIdx =
      mergeInsertionSort(elems.begin(), elems.begin() + (N / 2));
  Container mainIdx(N, -1);
  for (int i = 0; i < N / 2; ++i) {
    mainIdx[i] = sortIdx[firstHalfIdx[i]];
  }
  Container remIdx;
  // for (int i = 0; i < N / 2; ++i) {
  //   int idx = mainIdx[i];
  //   int pairedIdx = idx >= N / 2 ? idx - N / 2 : idx + N / 2;
  //   remIdx.push_back(pairedIdx);
  // }
  const int half = N / 2;
  for (int k = 0; k < half; ++k) {
    int pos = firstHalfIdx[k];             // 0..half-1
    mainIdx[k] = sortIdx[pos];             // main（大きい方の元インデックス）
    remIdx.push_back(sortIdx[pos + half]); // pend（小さい方の元インデックス）
  }
  Container mainChain, remChain;
  materializeChains(first, mainIdx, remIdx, mainChain, remChain, sortIdx);
  jacobsthalInsert(mainChain, mainIdx, remChain, remIdx, N);
  return mainIdx;
}

#endif // PMERGE_ME_TPP
