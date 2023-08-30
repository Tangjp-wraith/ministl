#pragma once

#include <climits>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <new>

namespace ministl {

template <class T>
inline T* _allocate(ptrdiff_t size, T*) {
  std::set_new_handler(0);
  T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
  if (tmp == 0) {
    std::cerr << "out of memory!" << std::endl;
    std::exit(1);
  }
  return tmp;
}

template <class T>
inline void _deallocate(T* buffer) {
  ::operator delete(buffer);
}

// 这里的construct调用的是placement new, 在一个已经获得的内存里建立一个对象
template <class T1, class T2>
inline void _construct(T1* p, const T2& value) {
  new (p) T1(value);
}

template <class T>
inline void _destroy(T* ptr) {
  ptr->~T();
}

template <class T>
class allocator {
 public:
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;

  template <class U>
  struct rebind {
    typedef allocator<U> other;
  };

  pointer allocate(size_type n, const void* hint = 0) {
    return _allocate((difference_type)n, (pointer)0);
  }
  void deallocate(pointer ptr) { _deallocate(ptr); }
  void construct(pointer ptr, const_reference value) { _construct(ptr, value); }
  void destroy(pointer ptr) { _destroy(ptr); }
  pointer address(reference x) { return (pointer)&x; }
  const_pointer const_adress(const_reference x) { return (const_pointer)&x; }
  size_type max_size() const { return size_type(UINT_MAX / sizeof(T)); }
};

}  // namespace ministl