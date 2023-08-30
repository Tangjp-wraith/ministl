#pragma once

#include <cstddef>
#include <cstdlib>
#include <iostream>

namespace ministl {

// 定义符合STL规格的配置器接口,
// 不管是一级配置器还是二级配置器都是使用这个接口进行分配的
template <class T, class Alloc>
class simple_alloc {
 public:
  static T* allocate(size_t n) {
    return (0 == n) ? 0 : (T*)Alloc::allocate(n * sizeof(T));
  }
  static T* allocate(void) { return (T*)Alloc::allocate(sizeof(T)); }
  static void deallocate(T* p, size_t n) {
    if (0 != n) {
      Alloc::deallocate(p, n * sizeof(T));
    }
  }
  static void deallocate(T* p) { Alloc::deallocate(p, sizeof(T)); }
};

// 一级配置器类 __malloc_alloc_template
template <int inst>
class __malloc_alloc_template {
 private:
  // 这里private里面的函数都是在内存不足的时候进行调用的
  static void* oom_malloc(size_t);          // 分配不足
  static void* oom_realloc(void*, size_t);  // 重新分配不足
  static void (*__malloc_alloc_oom_handler)();

 public:
  // 在分配和再次分配中, 都会检查内存不足,
  // 在不足的时候直接调用private中相应的函数
  static void* allocate(size_t n) {
    void* result = malloc(n);
    if (result == 0) {
      return oom_malloc(n);
    }
    return result;
  }
  static void deallocate(void* p, size_t) { free(p); }
  static void* reallocate(void* p, size_t, size_t new_sz) {
    void* result = realloc(p, new_sz);
    if (result == 0) {
      result = oom_realloc(p, new_sz);
    }
    return result;
  }
  // 这里是模仿c++的set_new_handler. 是由用户自己定义的处理函数, 没有设置默认为0
  static void (*set_malloc_handler(void (*f)()))() {
    void (*old)() = __malloc_alloc_oom_handler;
    __malloc_alloc_oom_handler = f;
    return (old);
  }
};
// 内存不足设置的处理例程, 默认设置的是0,
// 表示没有设置处理例程,这个处理例程是由用户手动设置的
template <int inst>
void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
inline void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
  void (*my_malloc_handler)();
  void* result;
  // 用户自定义处理例程, 就一直申请内存, 否则抛出异常
  for (;;) {
    my_malloc_handler = __malloc_alloc_oom_handler;
    if (my_malloc_handler == 0) {
      std::cerr << "out of memory" << std::endl;
      std::exit(1);
    }
    (*my_malloc_handler)();  // 调用处理例程，企图释放内存
    result = malloc(n);      // 再次尝试配置内存
    if (result) {
      return (result);
    }
  }
}

template <int inst>
void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
  void (*my_malloc_handler)();
  void* result;
  for (;;) {
    my_malloc_handler = __malloc_alloc_oom_handler;
    if (my_malloc_handler == 0) {
      std::cerr << "out of memory" << std::endl;
      std::exit(1);
    }
    (*my_malloc_handler)();
    result = realloc(p, n);
    if (result) {
      return (result);
    }
  }
}
// 默认将malloc_alloc设为0;
typedef __malloc_alloc_template<0> malloc_alloc;

enum { __ALIGN = 8 };  //设置对齐要求. 对齐为8字节, 没有8字节自动补齐
enum { __MAX_BYTES = 128 };  //二级分配器最大分配的内存大小
// 链表个数, 分别代表8, 16, 32....字节的链表
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };

//二级配置器类 __default_alloc_template
template <bool threads, int inst>
class __default_alloc_template {
 private:
  union obj {
    union obj* list_link;
    char data[1];
  };
  //向上取整(8进制)
  static size_t ROUND_UP(size_t bytes) {
    return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
  }
  // 16个free-lists
  static obj* volatile free_list[__NFREELISTS];
  //根据区块大小，选择free-list
  static size_t FREELIST_INDEX(size_t bytes) {
    return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
  }
  // 返回一个大小为n的对象，并可能加入大小为n的其他区块到free-list
  static void* refill(size_t n);
  // 配置一块空间，可以容纳nobjs个大小为size的区块
  // 如果配置nobjs个区块不便，nobjs可能会降低
  static char* chunk_alloc(size_t size, int& nobjs);

  static char* start_free;
  static char* end_free;
  static size_t heap_size;

 public:
  // n一定要大于0
  static void* allocate(size_t n) {
    obj* volatile* my_free_list;
    obj* result;
    //大于128b就第一级空间配置器
    if (n > (size_t)__MAX_BYTES) {
      return (malloc_alloc::allocate(n));
    }
    my_free_list = free_list + FREELIST_INDEX(n);
    result = *my_free_list;
    if (result == 0) {
      //没找到可用的free list，重新填充free-list
      void* r = refill(ROUND_UP(n));
      return r;
    }
    // 调整free-list
    *my_free_list = result->list_link;
    return (result);
  }
  // p指针不能为0
  static void deallocate(void* p, size_t n) {
    obj* q = (obj*)p;
    obj* volatile* my_free_list;
    if (n > (size_t)__MAX_BYTES) {
      malloc_alloc::deallocate(p, n);
      return;
    }
    my_free_list = free_list + FREELIST_INDEX(n);
    q->list_link = *my_free_list;
    *my_free_list = q;
  }
  static void* reallocate(void* p, size_t old_sz, size_t new_sz);
};
// 设置初始值
template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::start_free = 0;
template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::end_free = 0;
template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;
template <bool threads, int inst>
typename __default_alloc_template<threads, inst>::
    obj* volatile __default_alloc_template<threads,
                                           inst>::free_list[__NFREELISTS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t n) {
  int nobjs = 20;
  // 调用chunk_alloc，尝试取得nobjs个区块作为free-list的新节点
  char* chunk = chunk_alloc(n, nobjs);
  obj* volatile* my_free_list;
  obj* result;
  obj *current_obj, *next_obj;
  // 如果只得到一个区块，这个区块就分配给调用者用 free-list无新节点
  if (nobjs == 1) {
    return (chunk);
  }
  // 否则调用free-list，纳入新节点
  my_free_list = free_list + FREELIST_INDEX(n);
  // 以下在chunk空间内建立free-list
  result = (obj*)chunk;  // 这一块准备返回客端
  // 以下引导free-list指向新的配置空间（取自内存池）
  *my_free_list = next_obj = (obj*)(chunk + n);
  // 以下将free-list的各节点串起来，第0块返回给客端，从1开始
  for (int i = 1;; ++i) {
    current_obj = next_obj;
    next_obj = (obj*)((char*)next_obj + n);
    if (nobjs - 1 == i) {
      current_obj->list_link = 0;
      break;
    } else {
      current_obj->list_link = next_obj;
    }
  }
  return (result);
}

template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size,
                                                           int& nobjs) {
  char* result;
  size_t total_bytes = size * nobjs;
  size_t bytes_left = end_free - start_free;
  if (bytes_left >= total_bytes) {
    //内存完全满足剩余空间需求量
    result = start_free;
    start_free += total_bytes;
    return (result);
  } else if (bytes_left >= size) {
    //内存不能完全满足需求量，但足够供应(含)一个以上的区块
    nobjs = bytes_left / size;
    total_bytes = size * nobjs;
    result = start_free;
    start_free += total_bytes;
    return (result);
  } else {
    //内存池剩余空间连一块大小都无法提供。 heapsize清0操作
    size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
    if (bytes_left > 0) {
      //尝试内存池剩余的大小还能不能利用
      //先寻找free list
      obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
      //调整free list 利用剩余空间
      ((obj*)start_free)->list_link = *my_free_list;
      *my_free_list = (obj*)start_free;
    }
    //配置堆空间
    start_free = (char*)malloc(bytes_to_get);
    if (0 == start_free) {
      //堆空间不足，分配失败
      int i;
      obj *volatile *my_free_list, *p;

      for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
        my_free_list = free_list + FREELIST_INDEX(i);
        p = *my_free_list;
        if (0 != p) {
          *my_free_list = p->list_link;
          start_free = (char*)p;
          end_free = start_free + i;
          return (chunk_alloc(size, nobjs));
        }
      }
      end_free = 0;  //如果都没内存了
      start_free = (char*)malloc_alloc::allocate(bytes_to_get);
    }
    heap_size += bytes_to_get;
    end_free = start_free + bytes_to_get;
    return (chunk_alloc(size, nobjs));
  }
  return nullptr;
}
// 默认用第二级空间配置器
typedef __default_alloc_template<false, 0> alloc;

}  // namespace ministl