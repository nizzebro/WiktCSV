#pragma once


#include "processor.h"


namespace wiktcsv {



class ProcessorRu: protected XmlParser
{
    public:
    using XmlParser::process;
    protected:
    virtual void process();

    public:

};


};