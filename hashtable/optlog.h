/**
 * @file log.h
 * @brief Логгер, управляемый флагом компиляции
 */
#pragma once

#include <iomanip>
#include <iostream>
#include <string>

// Default value, if it wasn't setup in build system
#ifndef VERBOSE_LEVEL
#define VERBOSE_LEVEL 0
#endif

namespace optlog {

/**
 * @brief Логгер.
 *
 * Логгер конфигрируюется а этапе компиляции.
 * Логгер игнорирует все запросы, если verbLevel меньше MsgLevel.
 *
 * @tparam stream указатель на объект потока, в который выводятся сообщения, если их уровень меньше
 * уровня протоколироввания
 * @tparam verbLevel уровень протоколирования
 * @tparam MsgLevel уровень сообщения
 */
template <std::ostream* stream, int verbLevel, int MsgLevel>
struct ConstOptLog {
    /**
     * @brief Оператор записи в поток.
     *
     * @tparam ValType тип аргумента
     * @param val занчение для вывода в поток
     * @return const ConstOptLog& ссылка на себя
     */
    template <typename ValType>
    const ConstOptLog& operator<<(const ValType& val) const {
        if constexpr (verbLevel >= MsgLevel) {
            (*stream) << val;
        }
        return *this;
    }


    /**
     * @brief Применение к потоку маниплятора (std::hex, std::endl и т.п.)
     *
     * @param manip манипулятор потока
     * @return const ConstOptLog& ссылка на себя
     */
    const ConstOptLog& operator<<(std::basic_ostream<char>& (*manip)(std::basic_ostream<char>&)) const {
        return operator<<<>(manip);
    }
};


}  // namespace optlog


// /**
//  * @brief Вывод в поток/логгер элементов контйенера в шестнадцатиричном виде
//  * Вывод в формате: Префикс [ x1, x2, x3] 3 элемента
//  *
//  * @tparam TCont тип контейнера;
//  * @tparam TLogger тип логгера;
//  * @param logger логгер или поток;
//  * @param prefix префикс;
//  * @param container контейнер;
//  * @param sep символ-разделитель между элементами контейнера
//  * @param print_size флаг вывода в поток количества элементов в контейнере
//  */
// template <class TCont, class TLogger>
// void log_container(TLogger& logger, std::string prefix, const TCont& container, std::string sep = ", ",
//                    bool print_size = true) {
//     std::string seporator;
//     logger << prefix << " [";
//     for (const auto& s : container) {
//         logger << seporator << std::hex << std::setw(2) << std::setfill('0') << int(s);
//         seporator = sep;
//     }
//     logger << "]" << std::dec;
//     if (print_size) {
//         logger << container.size() << " элемента" << std::endl;
//     };
// }


// /**
//  * @brief Вывод в логгер/поток объека через функцию print объекта
//  *
//  * @tparam TStream тип потока или логгера
//  * @tparam T объект, которые имеет функцию-член \code std::ostream& print(std::ostream&os); \endcode
//  * @param os поток или логеер
//  * @param t объект
//  * @return os
//  */
// template <class TStream, class T>
// auto operator<<(TStream& os, T const& t) -> decltype(t.print(os), os) {
//     t.print(os);
//     return os;
// }


/// Объект логгера для вывода сообщений уровня 0.
[[maybe_unused]] static optlog::ConstOptLog<&std::cout, VERBOSE_LEVEL, 0> lg0;


/// Объект логгера для вывода сообщений уровня 1.
[[maybe_unused]] static optlog::ConstOptLog<&std::cout, VERBOSE_LEVEL, 1> lg1;


/// Объект логгера для вывода сообщений уровня 2.
[[maybe_unused]] static optlog::ConstOptLog<&std::cout, VERBOSE_LEVEL, 2> lg2;
