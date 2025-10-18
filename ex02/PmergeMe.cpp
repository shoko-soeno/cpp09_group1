#include "PmergeMe.hpp"

PmergeMe::PmergeMe() {}
PmergeMe::PmergeMe(const PmergeMe& rhs) { *this = rhs; }
PmergeMe& PmergeMe::operator=(const PmergeMe& rhs) {
  (void)rhs;
  return *this;
}
PmergeMe::~PmergeMe() {}
