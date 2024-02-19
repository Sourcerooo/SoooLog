#pragma once
#include <cassert>
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
  size_t             mId;
};

struct MetaDataStatement {
  MetaData        mMetaData;
  TypeDescriptors mDescriptors;
};

template <typename T>
constexpr TypeDescriptor GetTypeDescriptor()
{
  assert((0 == 1) && "Template specialization of GetTypeDescriptor not implemented for this type");
  return Undefined{};
}

template <typename T>
  requires std::is_same_v<T, int>
constexpr TypeDescriptor GetTypeDescriptor()
{
  return Int{};
}

template <typename T>
  requires std::is_same_v<T, double>
constexpr TypeDescriptor GetTypeDescriptor()
{
  return Double{};
}

template <typename T>
  requires std::is_same_v<T, std::string>
constexpr TypeDescriptor GetTypeDescriptor()
{
  return String{};
}

template <typename T>
  requires std::is_same_v<T, const char*>
constexpr TypeDescriptor GetTypeDescriptor()
{
  return CharConst{};
}


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
  static constexpr std::array        tempDescriptors{ GetTypeDescriptor<T>(), GetTypeDescriptor<ARGS>()... };
  static constexpr std::span         descriptors{ tempDescriptors };
  static constexpr MetaDataStatement m{ F{}(), descriptors };
  return &m;
}
