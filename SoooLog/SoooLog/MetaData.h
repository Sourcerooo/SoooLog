#pragma once
#include <string>
#include <variant>
#include <span>
#include "Translation.h"

struct Int {
  static constexpr std::string_view GetType() { return "Integer"; }
};

struct Double {
  static constexpr std::string_view GetType() { return "Double"; }
};

struct CharConst {
  static constexpr std::string_view GetType() { return "CharConst*"; }
};

struct String {
  static constexpr std::string_view GetType() { return "String"; }
};

struct Undefined {
  static constexpr std::string_view GetType() { return "Undefined"; }
};

using TypeDescriptor = std::variant<Undefined, Int, Double, CharConst, String>;
using TypeDescriptors = std::span<TypeDescriptor const>;

struct MetaData {
  std::string_view   mLevel;
  std::string_view   mFormat;
  std::string_view   mFuncSig;
  std::string_view   mFilename;
  int32_t            mLine;
  MessageTranslation mTranslation;
};

struct MetaDataStatement {
  MetaData        mMetaData;
  TypeDescriptors mDescriptors;
};

template <typename T>
constexpr TypeDescriptor GetTypeDescriptor2()
{
  return Undefined{};
}

template<typename T>
  requires std::is_same_v<T, int>
constexpr TypeDescriptor GetTypeDescriptor2(){
  return Int{};
}

template<typename T>
  requires std::is_same_v<T, double>
constexpr TypeDescriptor GetTypeDescriptor2(){
  return Double{};
}

template<typename T>
  requires std::is_same_v<T, std::string>
constexpr TypeDescriptor GetTypeDescriptor2(){
  return String{};
}

template<typename T>
  requires std::is_same_v<T, const char*>
constexpr TypeDescriptor GetTypeDescriptor2(){
  return CharConst{};
}


template <typename T>
struct GetTypeDescriptor {
  static constexpr TypeDescriptor cValue{ Undefined{} };
};

template <>
struct GetTypeDescriptor<int> {
  static constexpr TypeDescriptor cValue{ Int{} };
};

template <>
struct GetTypeDescriptor<double> {
  static constexpr TypeDescriptor cValue{ Double{} };
};

template <>
struct GetTypeDescriptor<char const*> {
  static constexpr TypeDescriptor cValue{ CharConst{} };
};

template <>
struct GetTypeDescriptor<std::string> {
  static constexpr TypeDescriptor cValue{ String{} };
};

//Specialization for no parameter passed
template <typename F>
MetaDataStatement const* GetMetaData()
{
  static constexpr std::array<TypeDescriptor, 0> tempDescriptors{};
  static constexpr std::span                     descriptors{ tempDescriptors };
  static constexpr MetaDataStatement             m{ F{}(), descriptors };
  return &m;
}

//Generalization for at least one parameter passed
template <typename F, typename T, typename... ARGS>
MetaDataStatement const* GetMetaData()
{  
  static constexpr std::array        tempDescriptors{ GetTypeDescriptor2<T>(), GetTypeDescriptor2<ARGS>()...};
  static constexpr std::span         descriptors{ tempDescriptors };
  static constexpr MetaDataStatement m{ F{}(), descriptors };
  return &m;
}


