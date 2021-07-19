#pragma once


#include "processor.h"


namespace wiktcsv {


class ProcessorRu: XmlParser 
{
    public:
    bool process(const char* fileName) noexcept;
    protected:
    private:
    XmlParser::FileWriter writer;
};


};

