#pragma once


#include "processor.h"


namespace wiktcsv {



class ProcessorRu: public XmlParser
{


void ProcessorRu::onPrefix(const Element & elem, bool isSelfClosing);
void ProcessorRu::onContent(const Element & elem);
void ProcessorRu::onSuffix(const Element & elem);

virtual void onDTD(const std::string& text){}
virtual void onPI(const std::string& text, const std::vector<STag>& path) {}
virtual void onComment(const std::string& text, const std::vector<STag>& path) {}


    public:
    using XmlParser::process;

};


};