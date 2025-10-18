#include PMERGEME_HPP
#ifndef PMERGEME_HPP

#ifndef DEBUG
extern int num_comparisons;
#endif

template <typename TContainer>
class PmergeMe {
private:
    PmergeMe(const PmergeMe&src) {(void)src;};
    PmergeMe& operator=(const PmergeMe& src) {(void)src; return *this;};

    int binaryInsertion(Container& mainChain, typename Container::value_type value, int right);

public:
    PmergeMe(){};
    ~PmergeMe();

    TContainer mergeInsertionSort(typename TContainer::iterator first, 
                                typename TContainer::iterator last);

    // ベンチ用：vector と deque の両方で走らせて計測
    std::vector<int> runVector(const std::vector<int>& xs);
    std::deque<int>  runDeque (const std::deque<int>&  xs);
}

// ペア分割 大きい方を前半、小さい方を後半に
void pairPartition(TContainer& vals, std::vector<int>& idx) {
  const int N = (int)vals.size();
  for (int i = 0, j = N/2; i < N/2; ++i, ++j) {
    if (vals[i] < vals[j]) { std::swap(vals[i], vals[j]); std::swap(idx[i], idx[j]); }
  }
}

// 前半だけ再帰ソート → 相対インデックスを返す
std::vector<int> sortFirstHalfOrder(const TContainer& vals) {
  TContainer firstHalf(vals.begin(), vals.begin() + vals.size()/2);
  return fordJohnsonIndices(firstHalf);  // 0..N/2-1 の並び順
}

// chain構築
void buildChains(const TContainer& orig, const std::vector<int>& pairIdx,
                 const std::vector<int>& firstHalfOrder,
                 TContainer& mainVals, std::vector<int>& mainIdx,
                 TContainer& remVals,  std::vector<int>& remIdx) {
  const int N = (int)orig.size();
  mainIdx.assign(N, -1);
  mainVals.clear(); remVals.clear(); remIdx.clear();

  for (int i = 0; i < N/2; ++i) {
    int a = mainIdx[i] = pairIdx[firstHalfOrder[i]];
    mainVals.push_back(orig[a]);
    int b = (a >= N/2) ? a - N/2 : a + N/2;
    remIdx.push_back(b);
    remVals.push_back(orig[b]);
  }
  if (N % 2) { remIdx.push_back(N-1); remVals.push_back(orig[N-1]); }
}

void insertFirstRemAtFront(TContainer& mainVals, std::vector<int>& mainIdx,
                           const TContainer& remVals, const std::vector<int>& remIdx) {
  mainVals.insert(mainVals.begin(), remVals.front());
  mainIdx .insert(mainIdx .begin(), remIdx .front());
}

// // 二分挿入（探索上限つき lower_bound 相当）
// size_t boundedLowerBound(const TContainer& a, const T& x, size_t hiExclusive) {
//   size_t lo = 0, hi = hiExclusive;
//   while (lo < hi) {
//     size_t mid = lo + (hi - lo)/2;
//     if (a[mid] < x) lo = mid + 1; else hi = mid;
//   }
//   return lo;
// }

// // ヤコブスタール由来の「段の境界」を作る（簡易）
// inline std::vector<size_t> makeJacobsthalBoundaries(size_t m) {
//   // J0=0, J1=1, Jn = J(n-1) + 2*J(n-2)  → 0,1,1,3,5,11,21,...
//   std::vector<size_t> J; J.push_back(0); J.push_back(1);
//   while (J.back() < m) J.push_back(J.back() + 2*J[J.size()-2]);
//   return J; // 区間 (J[k-1], J[k]] を「降順」で挿入
// }

// // 1段分の挿入（降順で、値とidxを同期して insert）
// void insertBatchDesc(TContainer& mainVals, std::vector<int>& mainIdx,
//                      const TContainer& remVals, const std::vector<int>& remIdx,
//                      size_t L_excl, size_t R_incl, size_t& upperBound /* 2^k-1 */) {
//   for (long i = (long)R_incl; i > (long)L_excl; --i) { // 符号付きで後ろ→前
//     size_t pos = boundedLowerBound(mainVals, remVals[i], upperBound + 1);
//     mainVals.insert(mainVals.begin() + pos, remVals[i]);
//     mainIdx .insert(mainIdx .begin() + pos, remIdx [i]);
//   }
// }

// std::vector<int> fordJohnsonIndices(const TContainer& input) {
//   const int N = (int)input.size();
//   std::vector<int> id(N); for (int i=0;i<N;++i) id[i]=i;
//   if (N <= 1) return id;

//   TContainer vals(input.begin(), input.end());
//   std::vector<int> idx = id;

//   pairPartition(vals, idx);
//   if (N == 2) { std::swap(id[0], id[1]); return id; }

//   std::vector<int> firstHalfOrder = sortFirstHalfOrder(vals);

//   TContainer mainVals, remVals;
//   std::vector<int> mainIdx, remIdx;
//   buildChains(input, idx, firstHalfOrder, mainVals, mainIdx, remVals, remIdx);

//   insertFirstRemAtFront(mainVals, mainIdx, remVals, remIdx);

//   std::vector<size_t> J = makeJacobsthalBoundaries(remVals.size());

//   // 探索上限（主鎖の上限インデックス）：3→7→15→…（=2^k-1）
//   size_t upper = (mainVals.size() < 4) ? (mainVals.size()-1) : 3;

//   for (size_t k = 2; k < J.size(); ++k) {
//     size_t L = J[k-1];
//     size_t R = std::min(J[k], (size_t)remVals.size());
//     if (R > L) insertBatchDesc(mainVals, mainIdx, remVals, remIdx, L, R-1, upper);
//     upper = std::min(2*upper + 1, mainVals.size() ? mainVals.size()-1 : 0);
//   }
//   return mainIdx; // 並び替え後の元インデックス順
// }

// template <typename TContainer>
// TContainer mergeInsertionSort(const TContainer& in) {
//   std::vector<int> order = fordJohnsonIndices(in);
//   TContainer out; out.resize(order.size());
//   for (size_t i=0;i<order.size();++i) out[i] = in[order[i]];
//   return out;
// }

#endif