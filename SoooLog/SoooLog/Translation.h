#pragma once
#include <array>
#include <span>
#include <string>

static constexpr size_t numTranslatedLanguages = 2;
static constexpr size_t numMessages            = 3;

using LanguageTextPair   = std::pair<std::string_view, std::string_view>;
using LanguageTextList   = std::array<LanguageTextPair const, numTranslatedLanguages>;
using Translation        = std::pair<std::string_view, LanguageTextList>;
using Translations       = std::array<Translation const, numMessages>;
using MessageTranslation = std::span<LanguageTextPair const>;

static constexpr Translations sTranslations{
    {
        {"LogMessage 1 {} {}"   , LanguageTextList{{ {"de", "Protokolnachricht 1 {} {}"}   ,{"es", "Diaro aviso 1 {} {}"}}}},
        {"LogMessage 2 {} {} {}", LanguageTextList{{ {"de", "Protokolnachricht 2 {} {}"}   ,{"es", "Diaro aviso 2 {} {} {}"}}}},
        {"LogMessage 3 {} {} {}", LanguageTextList{{ {"de", "Protokolnachricht 3 {} {} {}"},{"es", "Diaro aviso 3 {} {} {}"}}}}
    }
};

constexpr MessageTranslation GetTranslation(std::string_view pInput) {
    for (auto& [key, val] : sTranslations) {
        if (key == pInput) {
            return val;
        }
    }
    return MessageTranslation{};
}
