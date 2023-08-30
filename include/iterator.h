#pragma once

#include <cstddef>
namespace ministl {
  
/**
分别表示5种迭代器 category 的 struct
Input Iterator               read only
Output Iterator              write only
Forward Iterator             允许"写入型"算法在其指向区间进行操作
Bidirectional Iterator       提供双向访问能力
Random Access Iterator       支持原生指针具有的全部能力
*/
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// 迭代器模板
template <typename Category, typename T, typename Distance = ptrdiff_t,
          typename Pointer = T*, typename Reference = T&>
struct iterator {
  typedef Category iterator_category;
  typedef T value_type;
  typedef Distance difference_type;
  typedef Pointer pointer;
  typedef Reference reference;
};

// 迭代器的 traits
template <class Iterator>
struct iterator_traits {
  typedef typename Iterator::iterator_category iterator_category;
  typedef typename Iterator::value_type value_type;
  typedef typename Iterator::difference_type difference_type;
  typedef typename Iterator::pointer pointer;
  typedef typename Iterator::reference reference;
};
// 针对原生指针 T* 生成的 traits 偏特化
template <class T>
struct iterator_traits<T*> {
  typedef random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef ptrdiff_t difference_type;
  typedef T* pointer;
  typedef T& reference;
};
// 针对原生指针 const T* 生成的 traits 偏特化
template <class T>
struct iterator_traits<const T*> {
  typedef random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef ptrdiff_t difference_type;
  typedef const T* pointer;
  typedef const T& reference;
};

template <typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category iterator_category(
    const Iterator&) {
  typedef typename iterator_traits<Iterator>::iterator_category category;
  return category();
}

template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type* difference_type(
    const Iterator&) {
  return static_cast<typename iterator_traits<Iterator>::difference*>(0);
}

template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(
    const Iterator&) {
  return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type distance_aux(
    InputIterator first, InputIterator last, input_iterator_tag) {
  typename iterator_traits<InputIterator>::difference_type n = 0;
  for (; first != last; ++first, ++n)
    ;
  return n;
}

template <typename RandomAccessIterator>
inline typename iterator_traits<RandomAccessIterator>::difference_type
distance_aux(RandomAccessIterator first, RandomAccessIterator last,
             random_access_iterator_tag) {
  return last - first;
}

template <typename InputIterator>
inline typename iterator_traits<InputIterator>::difference_type distance(
    InputIterator first, InputIterator last) {
  return distance_aux(first, last, iterator_category(first));
}

template <typename InputIterator, typename Distance>
inline void distance_aux(InputIterator first, InputIterator last, Distance& n,
                         input_iterator_tag) {
  for (; first != last; ++first, ++n)
    ;
}

template <typename RandomAccessIterator, typename Distance>
inline void distance_aux(RandomAccessIterator first, RandomAccessIterator last,
                         Distance& n, random_access_iterator_tag) {
  n += last - first;
}

template <typename InputIterator, typename Distance>
inline void distance(InputIterator first, InputIterator last, Distance& n) {
  distance_aux(first, last, n, iterator_category(first));
}

template <typename InputIterator, typename Distance>
inline void advance_aux(InputIterator iter, const Distance& n,
                        input_iterator_tag) {
  for (; n != 0; --n, ++iter)
    ;
}

template <typename BidirectionIterator, typename Distance>
inline void advance_aux(BidirectionIterator iter, const Distance& n,
                        bidirectional_iterator_tag) {
  if (n >= 0)
    for (; n != 0; --n, ++iter)
      ;
  else
    for (; n != 0; ++n, --iter)
      ;
}

template <typename RandomAccessIterator, typename Distance>
inline void advance_aux(RandomAccessIterator iter, const Distance& n,
                        random_access_iterator_tag) {
  iter += n;
}

template <typename InputIterator, typename Distance>
inline void advance(InputIterator iter, const Distance& n) {
  advance_aux(iter, n, iterator_category(iter));
}

// 反向迭代器的实现
template <typename RandomAccessIterator, typename T, typename Reference = T&,
          typename Distance = ptrdiff_t>
class reverse_iterator {
 protected:
  typedef reverse_iterator<RandomAccessIterator, T, Reference, Distance> self;
  RandomAccessIterator current;

 public:
  typedef random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef Distance difference_type;
  typedef T* pointer;
  typedef Reference reference;
  reverse_iterator() {}
  explicit reverse_iterator(RandomAccessIterator iter) : current(iter) {}
  RandomAccessIterator base() const { return current; }
  // 各种操作符重载
  Reference operator*() const { return *(current - 1); }
  pointer operator->() { return &(operator*()); }
  self& operator++() {
    --current;
    return *this;
  }
  self operator++(int) {
    self tmp = *this;
    --current;
    return tmp;
  }
  self& operator--() {
    ++current;
    return *this;
  }
  self operator--(int) {
    self tmp = *this;
    ++current;
    return *this;
  }
  self operator+(Distance n) { return self(current - n); }
  self& operator+=(Distance n) {
    current -= n;
    return *this;
  }
  self operator-(Distance n) { return self(current + n); }
  self& operator-=(Distance n) {
    current += n;
    return *this;
  }
  Reference operator[](Distance n) const { return *(*this); }
};  // end reverse_iterator

template <typename RandomAccessIterator, typename T, typename Reference,
          typename Distance>
inline bool operator==(
    reverse_iterator<RandomAccessIterator, T, Reference, Distance>& lhs,
    reverse_iterator<RandomAccessIterator, T, Reference, Distance>& rhs) {
  return lhs.base() == rhs.base();
}

template <typename RandomAccessIterator, typename T, typename Reference,
          typename Distance>
inline bool operator<(
    reverse_iterator<RandomAccessIterator, T, Reference, Distance>& lhs,
    reverse_iterator<RandomAccessIterator, T, Reference, Distance>& rhs) {
  return lhs.base() < rhs.base();
}

template <typename RandomAccessIterator, typename T, typename Reference,
          typename Distance>
inline Distance operator-(
    reverse_iterator<RandomAccessIterator, T, Reference, Distance>& lhs,
    reverse_iterator<RandomAccessIterator, T, Reference, Distance>& rhs) {
  return lhs.base() - rhs.base();
}

template <typename RandomAccessIterator, typename T, typename Reference,
          typename Distance>
inline reverse_iterator<RandomAccessIterator, T, Reference, Distance> operator+(
    Distance n,
    reverse_iterator<RandomAccessIterator, T, Reference, Distance>& iter) {
  return reverse_iterator<RandomAccessIterator, T, Reference, Distance>(
      iter.base() - n);
}
}  // namespace ministl