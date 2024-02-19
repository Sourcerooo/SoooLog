#include "Log.h"

#include <algorithm>
#include <fstream>

auto GetId() -> size_t
{
  static size_t i = 0;
  return ++i;
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
    auto format = val->mMetaData->mMetaData.mFormat;
    auto id     = val->mId;
    //auto id = val->mMetaData->mMetaData.mId;
    auto translations = GetTranslation(id);
    outputFile << "         {" << id << ", LanguageTextList{ {";
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
    auto const id          = val->mId;
    auto       metaData    = val->mMetaData->mMetaData;
    auto       descriptors = val->mMetaData->mDescriptors;

    std::string filename{ metaData.mFilename };
    std::ranges::replace(filename, '\\', '/');


    outputFile << R"(  {)" << ",\n";
    outputFile << R"(    "Id"              : )" << std::to_string(id) << ",\n";
    //outputFile << R"(    "Id"              : )" << std::to_string(metaData.mId) << ",\n";
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
        auto&          type = std::get<0>(descriptor);
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
        auto&          type = std::get<3>(descriptor);
        auto constexpr text = type.GetType();
        outputFile << "\"" << text << "\"";
      }
      else if (index == 4) {
        auto&          type = std::get<4>(descriptor);
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
