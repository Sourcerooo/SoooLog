#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <source_location>
#include "MetaData.h"

#ifndef _MSC_VER
#ifndef __FUNCSIG__
    #define __FUNCSIG__ __PRETTY_FUNCTION__
#endif
#endif

#define CAT(X,Y) CAT2(X,Y)
#define CAT2(X,Y) X##Y
#define USABLE_LINE int(CAT(__LINE__,U))

#define LOG_DEBUG(format, ...) LOG(Debug, format, __VA_ARGS__)
#define LOG_ERROR(format, ...) LOG(Error, format, __VA_ARGS__)
#define LOG_WARNING(format, ...) LOG(Warning, format, __VA_ARGS__)

#define LOG(level, format,...) \
{                         \
    static constexpr auto funcsig = __FUNCSIG__;    \
    struct {                                    \
        constexpr MetaData operator()() {\
            return GenerateLog(level, format, funcsig, std::source_location::current().file_name(),  std::source_location::current().line()); \
        }\
    } anonymous_struct;\
    Log<decltype(anonymous_struct)>(__VA_ARGS__);\
}

static constexpr size_t gSeed = 0xEA35D32C643E04EB;
auto                    GetId() -> size_t;

constexpr auto GetIdHash(std::string_view const pStringView) -> size_t
{
  size_t d = (0xCBF29CE484222325 ^ gSeed) * static_cast<size_t>(0x100000001B3);
  for (auto& c : pStringView) {
    d = (d ^ static_cast<size_t>(c)) * static_cast<size_t>(0x01000193);
  }
  return d >> 8;
}

static constexpr std::string_view Debug   = "Debug";
static constexpr std::string_view Error   = "Error";
static constexpr std::string_view Warning = "Warning";


struct LogMetaDataNode;
inline std::vector<LogMetaDataNode*> gNodes{};

struct LogMetaDataNode {
  template <typename... ARGS>
  explicit LogMetaDataNode(MetaDataStatement const*  pMetaData,
                           [[maybe_unused]] ARGS&... pArgs) :
    mMetaData{ pMetaData }
  {
    mId = GetId();
    //mId = GetIdHash(mMetaData->mMetaData.mFormat);
    gNodes.push_back(this);
  }

  MetaDataStatement const* mMetaData;
  size_t                   mId;
};

constexpr MetaData GenerateLog(std::string_view level,
                               std::string_view format,
                               std::string_view funcSig,
                               std::string_view filename,
                               int32_t          line)
{
  return MetaData{ level, format, funcSig, filename, line, GetTranslation(GetIdHash(format)), GetIdHash(format) };
}


template <typename F, typename... ARGS>
inline LogMetaDataNode metaDataNode{ GetMetaData<F, ARGS...>() };

template <typename T>
  requires std::is_arithmetic_v<T>
std::string SerializeIO(T const& pValue)
{
  return std::to_string(pValue);
}

template <typename T>
std::string SerializeIO(T const& pValue)
{
  return pValue;
}

template <typename... ARGS>
void OutputArguments(ARGS&&... args)
{
  std::cout << " Arguments: [";
  ((std::cout << SerializeIO(args) << ", "), ...);
  std::cout << "]\n";
}


template <typename... ARGS>
void Serialize(size_t    pMetaDataId,
               ARGS&&... args)
{
  std::cout << "Logging Metadata: " << std::to_string(pMetaDataId) << " ";
  OutputArguments(args...);
}


template <typename F, typename... ARGS>
void Log(ARGS const&... args)
{
  static_assert(sizeof...(ARGS) > 0, "No ARGs provided to MetaDataStatement");
  auto metaDataId = metaDataNode<F, ARGS...>.mMetaData->mMetaData.mId;
  Serialize(metaDataId, args...);
}

template <typename F>
void Log()
{
  auto metaDataId = metaDataNode<F>.mMetaData->mMetaData.mId;
  Serialize(metaDataId);
}


void GenerateTranslationFile();
void GenerateMetaDataFile();
void GenerateMetaFiles();
