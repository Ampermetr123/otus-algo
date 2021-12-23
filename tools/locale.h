#pragma once

#include <iostream>
#include <locale>

struct separate_thousands : std::numpunct<char> {
  char_type do_thousands_sep() const override { return '\''; }  // separate with commas
    string_type do_grouping() const override { return "\3"; } // groups of 3 digit
};

/**
 * @brief Настройка потока на вывод больших чисел с разделителем по 3 цифры
 */
inline void setup_locale_thouthands(std::ostream& stream) {
  auto thousands = std::make_unique<separate_thousands>();
  stream.imbue(std::locale(std::cout.getloc(), thousands.release()));
}

