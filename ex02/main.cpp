#include "PmergeMe.hpp"

#include <iostream>     // for std::cerr
#include <vector>       // for std::vector
#include <deque>        // for std::deque
#include <sstream>      // for std::stringstream
#include <iomanip>  // for std::setw
#include <ctime>    // for clock_t, clock(), CLOCKS_PER_SEC
#ifdef DEBUG
int num_comparisons = 0;
#endif

bool parseArgs(int argc, char** argv,
               std::vector<int>& vectorInput,
               std::deque<int>& dequeInput) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <list of integers>\n";
    return false;
  }
  for (int i = 1; i < argc; i++) {
    int num;
    std::stringstream ss(argv[i]);
    if (ss >> num && num > 0) {
      vectorInput.push_back(num);
      dequeInput.push_back(num);
    } else {
      std::cerr << "Error: Invalid input '" << argv[i] << "'. Please provide positive integers only.\n";
      return false;
    }
  }
  return true;
}

template <typename Container>
void printContainer(const Container& container) {
  size_t count = 0;
  const size_t maxToShow = 10; // 最初の10個だけ表示
  for (typename Container::const_iterator it = container.begin();
       it != container.end() && count < maxToShow; ++it, ++count) {
    std::cout << *it << " ";
  }
  if (container.size() > maxToShow) {
    std::cout << "[...]";
  }
  std::cout << std::endl;
}

template <typename Container>
double benchSortAndPrint(Container& input, const std::string& label, int countShown) {
  PmergeMe<Container> sorter;
  clock_t start = clock();
  Container indices = sorter.mergeInsertionSort(input.begin(), input.end());
  clock_t end = clock();
  // ソート結果の検証
  Container sortedResult;
  for (size_t i = 0; i < indices.size(); ++i) {
    sortedResult.push_back(input[indices[i]]);
  }
  // 1. サイズチェック
  if (sortedResult.size() != input.size()) {
    throw std::runtime_error("Sort error: Result size mismatch");
  }
  // 2. ソート順序チェック（昇順になっているか）
  for (size_t i = 1; i < sortedResult.size(); ++i) {
    if (sortedResult[i] < sortedResult[i-1]) {
      throw std::runtime_error("Sort error: Result is not in ascending order");
    }
  }
  // 3. 要素の一致チェック（元の配列と同じ要素が含まれているか）
  Container originalCopy = input;
  Container resultCopy = sortedResult;
  std::sort(originalCopy.begin(), originalCopy.end());
  std::sort(resultCopy.begin(), resultCopy.end());
  if (originalCopy.size() != resultCopy.size()) {
    throw std::runtime_error("Sort error: Element count mismatch");
  }
  for (size_t i = 0; i < originalCopy.size(); ++i) {
    if (originalCopy[i] != resultCopy[i]) {
      throw std::runtime_error("Sort error: Elements don't match original input");
    }
  }

  #ifdef DEBUG
  std::cout << label << ": number of comparisons: " << num_comparisons << "\n";
  num_comparisons = 0; // Reset for next measurement
  #endif
  std::cout << "After (" << label << "); ";
  printContainer(sortedResult);
  // for (size_t i = 0; i < indices.size(); ++i) {
  //   std::cout << input[indices[i]] << (i == indices.size() - 1 ? "\n" : " ");
  // }
  double us = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1e6;
  std::cout << "Time to process a range of " << std::setw(4) << countShown
            << " elements with std::" << label << " : " << us << " us" << std::endl;
  return us;
}

int main(int argc, char** argv) {
  try {
    std::vector<int> vectorInput;
    std::deque<int> dequeInput;
    if (!parseArgs(argc, argv, vectorInput, dequeInput)) return 1;

    std::cout << "Before: ";
    printContainer(vectorInput);
    benchSortAndPrint(vectorInput, "vector", vectorInput.size());
    benchSortAndPrint(dequeInput, "deque", dequeInput.size());

  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }
}

// 要素の順番で管理し、要素の挿入・削除ができるコンテナ
// std::vector
// ランダムアクセスが高速（要素へのアクセスが O(1）なので、比較やマージが速い）
// メモリ管理がシンプル（データが1つの領域にまとまっているため、管理が容易）
// 小規模なデータでは特に高速（メモリ再確保が少なく、データアクセスが最適化されやすい
// std::deque　※両端キュー（Double-ended queue）
// 先頭・末尾の挿入・削除が高速（途中での分割や再配置が発生しにくい）
// メモリの再確保が少ない（内部的に分割して格納されるため、リサイズの影響が小さい）
// 大規模データでも安定して動作（vector のように一括メモリ確保の影響を受けにくい）
// つまり、ソート数が少なければ std::vector 、多ければ std::deque の方が処理時間が短くなる
