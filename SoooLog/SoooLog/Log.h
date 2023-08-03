#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "MetaData.h"

#define CAT(X,Y) CAT2(X,Y)
#define CAT2(X,Y) X##Y
#define USABLE_LINE int(CAT(__LINE__,U)) 

#ifndef _MSC_VER
#ifndef __FUNCSIG__
//#ifndef __FUNCTION__
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
            return GenerateLog(level, format, funcsig, __FILE__, USABLE_LINE); \
        }\
    } anonymous_struct;\
    Log<decltype(anonymous_struct)>(__VA_ARGS__);\
}


size_t GetId() {
    static size_t i = 0;
    return ++i;
}

static constexpr size_t cSeed = 0xEA35D32C643E04EB;

constexpr size_t GetIdHash(std::string_view pStringView)
{  
  size_t d = (0xCBF29CE484222325 ^ cSeed) * static_cast<size_t>(0x100000001B3);
  for(auto& c : pStringView) {
    d = (d ^ static_cast<size_t>(c)) * static_cast<size_t>(0x01000193);
  }
  return d>>8;
}

static constexpr std::string_view Debug = "Debug";
static constexpr std::string_view Error = "Error";
static constexpr std::string_view Warning = "Warning";


struct LogMetaDataNode;
std::vector<LogMetaDataNode*> nodes{};

struct LogMetaDataNode {
    template<typename ...ARGS>
    LogMetaDataNode(MetaDataStatement const* pMetaData, ARGS&... args) {
        mMetaData = pMetaData;
        mId = GetIdHash(mMetaData->mMetaData.mFormat);        
        nodes.push_back(this);
    }
    MetaDataStatement const* mMetaData;
    size_t mId;
    std::vector<TypeDescriptor> mDescriptors;
};

constexpr MetaData GenerateLog(std::string_view level, std::string_view format,
    std::string_view funcSig,
    std::string_view filename,
    int32_t line) {
    return MetaData{ level, format, funcSig, filename, line, GetTranslation(format) };
}



template<typename F, typename ...ARGS>
inline LogMetaDataNode metaDataNode{ GetMetaData<F,ARGS...>() };

template<typename ...ARGS>
void OutputArguments(ARGS&&... args) {
    std::cout << " Arguments: [";
    ((std::cout << args << ", "), ...);
    std::cout << "]\n";
}

template<typename ...ARGS>
void Serialize(size_t pMetaDataId, ARGS&&... args) {
    //OutputMetaData(pMetaData);
    std::cout << "Logging Metadata: " << std::to_string(pMetaDataId) << " ";
    OutputArguments(args...);
    //std::cout << "\n";
}

template<typename F, typename ...ARGS>
void Log(ARGS const&... args) {
    auto metaDataId = metaDataNode<F, ARGS...>.mId;    
    Serialize(metaDataId, args...);
}

void GenerateTranslationFile(){
    std::ofstream outputFile{ "translation_new.h" };
    if (outputFile.is_open()) {
        outputFile << "#pragma once\n";
        outputFile << "#include <array>\n";
        outputFile << "#include <span>\n";
        outputFile << "#include <string>\n\n";

        outputFile << "static constexpr size_t numTranslatedLanguages = 2;\n";
        outputFile << "static constexpr size_t numMessages            = " << nodes.size() << ";\n\n";
        outputFile << "using LanguageTextPair   = std::pair<std::string_view, std::string_view>;\n";
        outputFile << "using LanguageTextList   = std::array<LanguageTextPair const, numTranslatedLanguages>;\n";
        outputFile << "using Translation        = std::pair<std::string_view, LanguageTextList>;\n";
        outputFile << "using Translations       = std::array<Translation const, numMessages>;\n";
        outputFile << "using MessageTranslation = std::span<LanguageTextPair const>;\n\n";

        outputFile << "static constexpr Translations sTranslations{\n";
        outputFile << "    {\n";
        for (auto& val : nodes) {
            auto format = val->mMetaData->mMetaData.mFormat;
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
}

void GenerateMetaDataFile() {
    std::ofstream outputFile{ "metadata.json" };
    if (outputFile.is_open()) {
        outputFile << "[\n";
        for (auto const& val : nodes) {
            auto metaData = val->mMetaData->mMetaData;
            auto descriptors = val->mMetaData->mDescriptors;

            std::string filename{ metaData.mFilename };
            std::replace(filename.begin(), filename.end(), '\\', '/');

            outputFile << "  {\n";
            outputFile << "    \"MetaDataId\"      : " << std::to_string(val->mId) << ",\n";
            outputFile << "    \"Level\"           : \"" << metaData.mLevel << "\",\n";
            outputFile << "    \"Function\"        : \"" << metaData.mFuncSig << "\",\n";
            outputFile << "    \"Filename\"        : \"" << filename << "\",\n";
            outputFile << "    \"Line\"            : " << std::to_string(metaData.mLine) << ",\n";
            outputFile << "    \"Message\"         : \"" << metaData.mFormat << "\",\n";
            outputFile << "    \"Translations\"    : [\n";
            for (auto const& translation : metaData.mTranslation) {
                outputFile <<
                    "                           { \"Language\": \"" << translation.first << "\", \"Message\": \"" << translation.second << "\" }";
                if (&translation != &metaData.mTranslation.back()) {
                    outputFile << ',';
                }
                outputFile << '\n';
            }
            outputFile << "                        ],\n";
            outputFile << "    \"PlaceholderTypes\": [ ";
            for (auto const& descriptor : descriptors) {
                auto index = descriptor.index();
                if (index == 1) {
                    auto type = std::get<1>(descriptor);
                    auto text = type.GetType();
                    outputFile << "\"" <<text <<"\"";
                }
                else if (index == 2) {
                    auto type = std::get<2>(descriptor);
                    auto text = type.GetType();
                    outputFile << "\"" << text << "\"";
                }
                if (&descriptor != &descriptors.back()) {
                    outputFile << ", ";
                }
            }
            outputFile << " ]\n";
            outputFile << "  }";
            if (&val != &nodes.back()) {
                outputFile << ",";
            }
            outputFile << '\n';
        }
        outputFile << "]\n";
    }
}

void GenerateMetaFiles() {
    std::cout << "--- Generating meta files ---\n";
    GenerateTranslationFile();
    GenerateMetaDataFile();
}