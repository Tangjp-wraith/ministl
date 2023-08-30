#pragma once

namespace ministl {

// 两个不包含任何成员的类，用于辅助实现type_traits模板
// 由于文件末尾还有名为 true_type，false_type的类别名，故而这里带一个下划线
struct _true_type {};
struct _false_type {};

template <typename type>
struct type_traits {
  typedef _false_type has_trivial_default_constructor;
  typedef _false_type has_trivial_copy_constructtor;
  typedef _false_type has_trivial_assignment_operator;
  typedef _false_type has_trivial_destructor;
  typedef _false_type is_POD_type;
};

// 针对各种算术整型的特化版本
template <>
struct type_traits<bool> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<char> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<unsigned char> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<signed char> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<wchar_t> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<short> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<unsigned short> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<int> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<unsigned int> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<long> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<unsigned long> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<long long> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<unsigned long long> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<float> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<double> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

template <>
struct type_traits<long double> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

// 针对指针的偏特化版本
template <typename type>
struct type_traits<type*> {
  typedef _true_type has_trivial_default_constructor;
  typedef _true_type has_trivial_copy_constructtor;
  typedef _true_type has_trivial_assignment_operator;
  typedef _true_type has_trivial_destructor;
  typedef _true_type is_POD_type;
};

// 一个辅助实现 true_type 和 false_type 的类
template <typename T, T v>
struct intergral_constant {
  typedef T value_type;
  typedef intergral_constant<T, v> type;

  static constexpr T value = v;
};

typedef intergral_constant<bool, true> true_type;
typedef intergral_constant<bool, false> false_type;

// 接受普通类型时继承 false_type ,此时其静态成员 value 为 false
template <typename T>
struct is_const : public false_type {};

// 偏特化使得接受 const 类型时继承 true_type，此时其静态成员 value 为 true
template <typename T>
struct is_const<const T> : public true_type {};
}  // namespace ministl