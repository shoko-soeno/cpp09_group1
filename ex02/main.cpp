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
  for (typename Container::const_iterator it = container.begin(); it != container.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;
}

template <typename Container>
double benchSortAndPrint(Container& input, const std::string& label, int countShown) {
  PmergeMe<Container> sorter;
  clock_t start = clock();
  Container indices = sorter.mergeInsertionSort(input.begin(), input.end());
  clock_t end = clock();

  #ifdef DEBUG
  std::cout << label << ": number of comparisons: " << num_comparisons << "\n";
  num_comparisons = 0; // Reset for next measurement
  #endif
  std::cout << "After (" << label << "); ";
  for (size_t i = 0; i < indices.size(); ++i) {
    std::cout << input[indices[i]] << (i == indices.size() - 1 ? "\n" : " ");
  }
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
