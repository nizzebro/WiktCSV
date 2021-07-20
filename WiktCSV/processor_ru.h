#pragma once


#include "processor.h"


namespace wiktcsv {


class ProcessorRu: XmlParser, XmlParser::FileWriter
{
    public:
    bool process(const char* dir, const char* fileName) noexcept;
    void processText(std::string_view title, std::string_view id) noexcept;
    protected:

};


};

