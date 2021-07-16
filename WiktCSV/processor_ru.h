#pragma once


#include "processor.h"


namespace wiktcsv {


class ProcessorRu: protected XmlParser
{
    public:
    void process(const char* path) noexcept;
    protected:
    virtual void process();
};


};

