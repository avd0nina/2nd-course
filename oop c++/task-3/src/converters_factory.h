#pragma once

#include <string>
#include <map>
#include <utility>
#include <vector>
#include "converters/all_converters.h"

namespace Factory{
    using CommandsDescription = std::map<std::string_view, std::pair<std::string_view, std::vector<std::string_view>>>;
    // Объявление типа CommandsDescription, представляющего описание команд:
    // std::string_view: взгляд на строку(указатель + длина), без копирования. используется для: Ключа карты (std::map): хранение имени команды (строки). Описания команды и элементов векторе примеров.
    // std::pair: Хранит описание команды и примеры ее использования.

    class ConvertersFactory{ // отвечает за создание объектов-конвертеров (Converters::Converter) и предоставление описания доступных команд (CommandsDescription). Он использует PImpl (Pointer to Implementation) идиому, скрывающую детали реализации.
    public:
        ConvertersFactory(); // Конструктор класса.

        [[nodiscard]] const Converters::Converter& create_converter(std::string_view command_name) const;

        [[nodiscard]] const CommandsDescription& get_commands_description() const;

        ~ConvertersFactory();
    private:
        struct Impl;
        Impl* _pimpl;
    };
}
