#pragma once

#include <memory>
#include <vector>
#include <string>
#include <any>
#include "../streams.h"
#include "../config_parser.h"

namespace Converters{ // Открытие пространства имен Converters, чтобы логически группировать функции, классы и другие элементы, относящиеся к конвертерам, и избежать конфликтов имен с другими частями программы.
    class RawAbstractConverter;

    using Converter = std::unique_ptr<RawAbstractConverter>;
    using ConverterParams = std::vector<std::any>;

    class RawAbstractConverter{
    public:
        virtual void convert(std::string_view command,
                             Streams::OutputStream& output_stream,
                             Streams::InputStream& input_stream,
                             const ConverterParams& params) const = 0;

        virtual ~RawAbstractConverter() = default;
    };
}
