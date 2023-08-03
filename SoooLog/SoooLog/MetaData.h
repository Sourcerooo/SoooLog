#pragma once
#include <string>
#include <variant>
#include <span>
#include "Translation.h"

struct Int {
  constexpr std::string_view GetType() { return "Integer"; }
};

struct Double {
  constexpr std::string_view GetType() { return "Double"; }
};

struct Undefined {
  constexpr std::string_view GetType() { return "Undefined"; }
};

using TypeDescriptor = std::variant<Undefined, Int, Double>;
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

template <typename F, typename... ARGS>
MetaDataStatement const* GetMetaData()
{
  static constexpr std::array tempDescriptors{ GetTypeDescriptor<ARGS>::cValue... };
  static constexpr std::span descriptors{ tempDescriptors };
  static constexpr MetaDataStatement m{ F{}(), descriptors };
  return &m;
}
