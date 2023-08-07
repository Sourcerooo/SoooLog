#pragma once
#include <array>
#include <span>
#include <string>

static constexpr size_t numTranslatedLanguages = 2;
static constexpr size_t numMessages = 5;

using LanguageTextPair = std::pair<std::string_view, std::string_view>;
using LanguageTextList = std::array<LanguageTextPair const, numTranslatedLanguages>;
using Translation = std::pair<size_t, LanguageTextList>;
using Translations = std::array<Translation const, numMessages>;
using MessageTranslation = std::span<LanguageTextPair const>;

static constexpr Translations sTranslations{
    {
         {1, LanguageTextList{ {} } },
         {2, LanguageTextList{ {{"de", "Protokolnachricht 1 {} {}"}, {"es", "Diaro aviso 1 {} {}"}} } },
         {3, LanguageTextList{ {{"de", "Protokolnachricht 2 {} {}"}, {"es", "Diaro aviso 2 {} {} {}"}} } },
         {4, LanguageTextList{ {} } },
         {5, LanguageTextList{ {} } }
    }
};

constexpr MessageTranslation GetTranslation(size_t pInput) {
  for (auto& [key, val] : sTranslations) {
    if (key == pInput) {
      return val;
    }
  }
  return MessageTranslation{};
}
