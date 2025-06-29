#include <iostream>

#include "i18n/translations.h"

using ADS::Classes::Translations::Translations;
using namespace i18n::literals;

int main() {

    std::cout << "Start of Main" << std::endl;
    std::cout << "Using the C++ version: " << __cplusplus << std::endl;;

    Translations translations ("");

    i18n::translations translationSet{
      {"en_US", {{"moon", "Moon"}, {"sun", "Sun"}}},
      {"es_ES", {{"moon", "Luna"}, {"sun", "Sol"}}},
      {"ru_RU", {{"moon", "луна"}, {"sun", "солнце"}}},
    };
    translations.setTranslations(translationSet);
    std::cout << std::endl;

    std::cout << "English" << std::endl;
    translations.setLocale("en_US");
    std::cout << "Locale = " << translations.getLocale() << std::endl;
    std::cout << "Translations to English:" << std::endl;
    std::cout << "moon"_t << std::endl;
    std::cout << "sun"_t << std::endl;

    std::cout << std::endl;

    std::cout << "Spanish" << std::endl;
    translations.setLocale("es_ES");
    std::cout << "Translations to Spanish:" << std::endl;
    std::cout << "moon"_t << std::endl;
    std::cout << "sun"_t << std::endl;


    std::cout << std::endl;

    std::cout << "Russian" << std::endl;
    translations.setLocale("ru_RU");
    std::cout << "Translations to Russian:" << std::endl;
    std::cout << "moon"_t << std::endl;
    std::cout << "sun"_t << std::endl;

    std::cout << std::endl;

    std::cout << "Fake Russian" << std::endl;
    translations.setLocale("ru-RU");
    std::cout << "Translations to fake Russian:" << std::endl;
    std::cout << "moon"_t << std::endl;
    std::cout << "sun"_t << std::endl;

    std::cout << std::endl;
    std::cout << "End of Main" << std::endl;
    return 0;
}