#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "MetaData.h"
#include <source_location>

#define CAT(X,Y) CAT2(X,Y)
#define CAT2(X,Y) X##Y
#define USABLE_LINE int(CAT(__LINE__,U))

#ifndef _MSC_VER
#ifndef __FUNCSIG__
#define __FUNCSIG__ __PRETTY_FUNCTION__
#endif
#endif

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


size_t GetId()
{
  static size_t i = 0;
  return ++i;
}

static constexpr size_t gSeed = 0xEA35D32C643E04EB;

constexpr size_t GetIdHash(std::string_view const pStringView)
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
    mId = GetIdHash(mMetaData->mMetaData.mFormat);
    gNodes.push_back(this);
  }

  MetaDataStatement const*    mMetaData;
  size_t                      mId;
  std::vector<TypeDescriptor> mDescriptors;
};

constexpr MetaData GenerateLog(std::string_view level,
                               std::string_view format,
                               std::string_view funcSig,
                               std::string_view filename,
                               int32_t          line)
{
  return MetaData{ level, format, funcSig, filename, line, GetTranslation(format) };
}


template <typename F, typename... ARGS>
inline LogMetaDataNode metaDataNode{ GetMetaData<F, ARGS...>() };


template <typename... ARGS>
void OutputArguments(ARGS&&... args)
{
  std::cout << " Arguments: [";
  ((std::cout << args << ", "), ...);
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
  auto metaDataId = metaDataNode<F, ARGS...>.mId;  
  Serialize(metaDataId, args...);
}

template <typename F>
void Log()
{
  auto metaDataId = metaDataNode<F>.mId;  
  Serialize(metaDataId);
}


void GenerateTranslationFile()
{
  std::ofstream outputFile{ "translation_new.h" };
  if (!outputFile.is_open()) {
    return;
  }
  outputFile << "#pragma once\n";
  outputFile << "#include <array>\n";
  outputFile << "#include <span>\n";
  outputFile << "#include <string>\n\n";

  outputFile << "static constexpr size_t numTranslatedLanguages = 2;\n";
  outputFile << "static constexpr size_t numMessages            = " << gNodes.size() << ";\n\n";
  outputFile << "using LanguageTextPair   = std::pair<std::string_view, std::string_view>;\n";
  outputFile << "using LanguageTextList   = std::array<LanguageTextPair const, numTranslatedLanguages>;\n";
  outputFile << "using Translation        = std::pair<std::string_view, LanguageTextList>;\n";
  outputFile << "using Translations       = std::array<Translation const, numMessages>;\n";
  outputFile << "using MessageTranslation = std::span<LanguageTextPair const>;\n\n";

  outputFile << "static constexpr Translations sTranslations{\n";
  outputFile << "    {\n";
  for (auto& val : gNodes) {
    auto format       = val->mMetaData->mMetaData.mFormat;
    auto translations = GetTranslation(format);
    outputFile << "         {\"" << format << "\", LanguageTextList{ {";
    for (auto const& translationPair : translations) {
      outputFile << "{\"" << translationPair.first << "\", \"" << translationPair.second << "\"}";
      if (&translationPair != &translations.back()) {
        outputFile << ", ";
      }
    }
    outputFile << "} } }\n";
  }
  outputFile << "    }\n";
  outputFile << "};\n\n";

  outputFile << "constexpr MessageTranslation GetTranslation(std::string_view pInput) {\n";
  outputFile << "    for (auto& [key, val] : sTranslations) {\n";
  outputFile << "        if (key == pInput) {\n";
  outputFile << "            return val;\n";
  outputFile << "        }\n";
  outputFile << "     }\n";
  outputFile << "    return MessageTranslation{};\n";
  outputFile << "}\n";

  outputFile.close();
}

void GenerateMetaDataFile()
{
  std::ofstream outputFile{ "metadata.json" };
  if (!outputFile.is_open()) {
    return;
  }
  outputFile << "[\n";
  for (auto const& val : gNodes) {
    auto metaData    = val->mMetaData->mMetaData;
    auto descriptors = val->mMetaData->mDescriptors;

    std::string filename{ metaData.mFilename };
    std::ranges::replace(filename, '\\', '/');

    
    outputFile << R"(  {
    "MetaDataId"      : )" << std::to_string(val->mId) << ",\n";
    outputFile << R"(    "Level"           : ")" << metaData.mLevel << "\",\n";
    outputFile << R"(    "Function"        : ")" << metaData.mFuncSig << "\",\n";
    outputFile << R"(    "Filename"        : ")" << filename << "\",\n";
    outputFile << R"(    "Line"            : )" << std::to_string(metaData.mLine) << ",\n";
    outputFile << R"(    "Message"         : ")" << metaData.mFormat << "\",\n";
    outputFile << R"(    "Translations"    : [
)";
    for (auto const& translation : metaData.mTranslation) {
      outputFile <<
        R"(                           { "Language": ")" << translation.first << R"(", "Message": ")" << translation.second << "\" }";
      if (&translation != &metaData.mTranslation.back()) {
        outputFile << ',';
      }
      outputFile << '\n';
    }
    outputFile << "                        ],\n";
    outputFile << "    \"PlaceholderTypes\": [ ";
    for (auto const& descriptor : descriptors) {
      if (auto const index = descriptor.index(); index == 0) {
        auto& type = std::get<0>(descriptor);
        auto constexpr text = type.GetType();
        outputFile << "\"" << text << "\"";
      }
      if (auto const index = descriptor.index(); index == 1) {
        auto&          type = std::get<1>(descriptor);
        auto constexpr text = type.GetType();
        outputFile << "\"" << text << "\"";
      }
      else if (index == 2) {
        auto&          type = std::get<2>(descriptor);
        auto constexpr text = type.GetType();
        outputFile << "\"" << text << "\"";
      }
      else if (index == 3) {
        auto& type = std::get<3>(descriptor);
        auto constexpr text = type.GetType();
        outputFile << "\"" << text << "\"";
      }
      else if (index == 4) {
        auto& type = std::get<4>(descriptor);
        auto constexpr text = type.GetType();
        outputFile << "\"" << text << "\"";
      }
      if (&descriptor != &descriptors.back()) {
        outputFile << ", ";
      }
    }
    outputFile << " ]\n";
    outputFile << "  }";
    if (&val != &gNodes.back()) {
      outputFile << ",";
    }
    outputFile << '\n';
  }
  outputFile << "]\n";
}

void GenerateMetaFiles()
{
  std::cout << "--- Generating meta files ---\n";
  GenerateTranslationFile();
  GenerateMetaDataFile();
}
