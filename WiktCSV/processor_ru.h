#pragma once


#include "processor.h"


namespace wiktcsv {



class ProcessorRu: public XmlParser
{
    bool insidePage = false;
    bool isArticle = false;
    std::string id;
    std::string title;

    virtual void process();

    public:
    //using XmlParser::openFile;

};


};