#include "PmergeMe.hpp"

int main(int argc, char** argv) {
  std::vector<int> v; v.reserve(argc-1);
  for (int i=1;i<argc;++i) v.push_back(std::atoi(argv[i]));

  std::deque<int> d(v.begin(), v.end());

  clock_t t1 = clock();
  std::vector<int> vs = mergeInsertionSort(v);
  clock_t t2 = clock();
  std::deque<int>  ds = mergeInsertionSort(d);
  clock_t t3 = clock();

  // 表示
  std::cout << "After (vector): ";
  for (size_t i=0;i<vs.size();++i) std::cout << vs[i] << ' ';
  std::cout << "\nAfter (deque):  ";
  for (size_t i=0;i<ds.size();++i) std::cout << ds[i] << ' ';
  std::cout << "\nTime vector: " << (double)(t2-t1)/CLOCKS_PER_SEC*1000 << " ms";
  std::cout << "\nTime deque : " << (double)(t3-t2)/CLOCKS_PER_SEC*1000 << " ms\n";
}
