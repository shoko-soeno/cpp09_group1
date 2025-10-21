#ifndef PMERGE_ME_TPP
#define PMERGE_ME_TPP

#ifndef DEBUG
extern int num_comparisons;
#endif

// ========= Ctors / Dtor =========
template <typename Container>
PmergeMe<Container>::PmergeMe() {}

template <typename Container>
PmergeMe<Container>::~PmergeMe() {}

// ========= makePairsAndSwap =========
// idx は 0..N-1 の “元配列インデックス”。
// 前半 i と 後半 j=i+N/2 をペアにし、elems[idx[i]] と elems[idx[j]] を比較して
// 大きい方のインデックスが前半側(idx[i])に来るようにスワップ。
template <typename Container>
void PmergeMe<Container>::makePairsAndSwap(const Container& elems, Container& idx) {
  const int N    = static_cast<int>(idx.size());
  const int half = N / 2;
  for (int i = 0, j = half; i < half; ++i, ++j) {
#ifdef DEBUG
    ++num_comparisons;
#endif
    if (elems[idx[i]] < elems[idx[j]]) {
      std::swap(idx[i], idx[j]);
    }
  }
}

// ===== buildMainAndRemIndex =====
// sortIdx: 0..N-1（元配列インデックス）の並び。
// firstHalfIdx: [0..N/2) の「並び順インデックス」（再帰の戻り値）。
// mainIdx: 「大きい側」(N/2 個) の元配列インデックスが入る（残りは -1で埋める）。
// remIdx : 「小さい側」(N/2 個, 奇数なら +1) の元配列インデックスが入る。
template <typename Container>
void PmergeMe<Container>::buildMainAndRemIndex(const Container& sortIdx,
                                               const Container& firstHalfIdx,
                                               Container& mainIdx,
                                               Container& remIdx) {
  const int N    = static_cast<int>(sortIdx.size());
  const int half = N / 2;

  mainIdx.assign(N, -1);
  for (int i = 0; i < half; ++i) {
    mainIdx[i] = sortIdx[firstHalfIdx[i]];
  }

  remIdx.clear();
  remIdx.reserve(half + (N % 2 ? 1 : 0));
  for (int i = 0; i < half; ++i) {
    const int idx    = mainIdx[i];
    const int paired = (idx >= half) ? (idx - half) : (idx + half);
    remIdx.push_back(paired);
  }
  if (N % 2) remIdx.push_back(N - 1);
}

// ========= buildMainAndRemIndex =========
// 再帰で得た firstHalfIdx（前半の並び替え順）を使って
// mainIdx（ペアの大きい側＝前半）と remIdx（ペアの小さい側＝後半）を作る。
// 奇数要素があれば remIdx の末尾に N-1 を追加。
template <typename Container>
void PmergeMe<Container>::buildMainAndRemIndex(const Container& sortIdx,
                                               const Container& firstHalfIdx,
                                               Container& mainIdx,
                                               Container& remIdx) {
  const int N = static_cast<int>(sortIdx.size());
  const int half = N / 2;

  mainIdx.assign(N, -1);
  for (int i = 0; i < half; ++i) {
    mainIdx[i] = sortIdx[firstHalfIdx[i]];
  }
  remIdx.clear();
  remIdx.reserve(half + (N % 2 ? 1 : 0));
  for (int i = 0; i < half; ++i) {
    const int idx = mainIdx[i];
    const int paired = (idx >= half) ? (idx - half) : (idx + half);
    remIdx.push_back(paired);
  }
  if (N % 2) remIdx.push_back(N - 1);
}


// ========= materializeChains =========
// index 配列に従って、元配列 first から値列を構築する
template <typename Container>
void PmergeMe<Container>::materializeChains(const iterator first,
                                            const Container& mainIdx,
                                            const Container& remIdx,
                                            Container& mainVals,
                                            Container& remVals) {
  const int half = static_cast<int>(remIdx.size()) - (remIdx.empty() ? 0 : (remIdx.back() == -1 ? 1 : 0));
  (void)half; // 使わないけど保険で残しておくなら消してOK

  mainVals.clear();
  remVals.clear();

  // mainIdx の有効領域は [0 .. remIdx.size()-1] まで（N/2 個、奇数時は main と rem の合計で N 個）
  for (std::size_t i = 0; i < remIdx.size(); ++i) {
    if (i < mainIdx.size() && mainIdx[i] >= 0)
      mainVals.push_back(*(first + mainIdx[i]));
    remVals.push_back(*(first + remIdx[i]));
  }
}


// ========= jacobsthalInsert =========
// remVals を Jacobsthal 順に mainVals へ二分挿入していき、対応するインデックス列 mainIdx も更新する
template <typename Container>
void PmergeMe<Container>::jacobsthalInsert(Container& mainVals, Container& mainIdx,
                                           const Container& remVals, const Container& remIdx,
                                           int N) {
  // まず remVals[0] を main の先頭に入れる（仕様に合わせる）
  mainVals.insert(mainVals.begin(), remVals.front());
  // mainIdx をシフトして先頭に remIdx[0] を入れる
  for (int j = N - 1; j >= 1; --j) mainIdx[j] = mainIdx[j - 1];
  mainIdx[0] = remIdx.front();

  // Jacobsthal: J0=0, J1=2, Jn=J(n-1)+2*J(n-2) をグループサイズとして使う
  std::size_t numInserted  = 1;
  std::size_t prevGroup    = 0;
  std::size_t curGroup     = 2;
  std::size_t upperBoundIx = (N - 1 < 4 ? static_cast<std::size_t>(N - 1) : 3);

  // 中間グループを逆順で挿入
  while (numInserted + curGroup < remVals.size()) {
    std::size_t end   = numInserted + curGroup;   // [numInserted, end)
    std::size_t index = end - 1;
    while (index >= numInserted) {
      int pos = binaryInsertion(mainVals, remVals[index],
                                static_cast<int>(upperBoundIx - 1));
      // mainIdx も回転
      for (int j = N - 1; j > pos; --j) mainIdx[j] = mainIdx[j - 1];
      mainIdx[pos] = remIdx[index];

      if (index == numInserted) break; // size_t の underflow 防止
      --index;
    }
    // 次の Jacobsthal グループへ
    numInserted += curGroup;
    std::size_t nextGroup = curGroup + 2 * prevGroup;
    prevGroup    = curGroup;
    curGroup     = nextGroup;
    upperBoundIx = 2 * upperBoundIx + 1;
  }

  // 残り（最後の不完全グループ）を逆順で挿入
  if (numInserted < remVals.size()) {
    std::size_t index = remVals.size() - 1;
    while (index >= numInserted) {
      int pos = binaryInsertion(mainVals, remVals[index],
                                static_cast<int>(upperBoundIx - 1));
      for (int j = N - 1; j > pos; --j) mainIdx[j] = mainIdx[j - 1];
      mainIdx[pos] = remIdx[index];

      if (index == numInserted) break;
      --index;
    }
  }
}


// ========= mergeInsertionSort 本体 =========
template <typename Container>
Container PmergeMe<Container>::mergeInsertionSort(iterator first, iterator last) {
  const int N = static_cast<int>(std::distance(first, last));
  Container elems(first, last);

  Container sortIdx;
  sortIdx.resize(N);
  for (int i = 0; i < N; ++i) sortIdx[i] = i;

  if (N <= 1) return sortIdx;

  // ペアを作って大きい方を前半にスワップ
  makePairsAndSwap(elems, sortIdx);

  if (N == 2) {
    // 2 要素なら前半(=大)が先になるはずなので、元のインデックス並びを 0,1 から 1,0 に直す
    std::swap(sortIdx[0], sortIdx[1]);
    return sortIdx;
  }

  // 前半を再帰ソート（ここで返るのは [0..N/2) の「並び順インデックス」）
  Container firstHalfIdx =
      mergeInsertionSort(elems.begin(), elems.begin() + (N / 2));

  // main/rem のインデックス列を作る
  Container mainIdx, remIdx;
  mainIdx.assign(N, -1);
  buildMainAndRemIndex(sortIdx, firstHalfIdx, mainIdx, remIdx);

  // 実体の値列を作る
  Container mainVals, remVals;
  materializeChains(first, mainIdx, remIdx, mainVals, remVals);

  // Jacobsthal 順で rem を main に二分挿入していく（mainIdx も同様に更新）
  jacobsthalInsert(mainVals, mainIdx, remVals, remIdx, N);

  // mainIdx が「元配列の並び替え結果（インデックス列）」
  return mainIdx;
}

#endif // PMERGE_ME_TPP
