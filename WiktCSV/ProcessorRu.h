#pragma once

#include "Processor.h"

namespace wiktcsv {

const char* const a [4] = {"pages.csv","omographs.csv","meanings.csv","rem.input"};

class ProcessorRu: private Processor
{
public:
using Processor::process;
  
    
  //  std::ofstream&  csvPages = ofstreams[0];
  //std::ofstream&  csvHeaders = ofstreams[1];
  //std:: ofstream&  csvLines = ofstreams[2];
  //std::ofstream&  xmlRem = ofstreams[3];
  //virtual void process(const StringView&);


    /**  Called on start-tag or self-closing tag; no content is available at this moment */ 
    virtual void onPrefix(const Element& elem, bool isSelfClosing);
    /**  Called after onPrefix on text contents - potentially a number of times if there are
    text blocks, interleaved with nested tags */ 
    virtual void onContent(const Element& elem);
    /**  Called after onContent on end-tag  */ 
    virtual void onSuffix(const Element& elem);
    /**  Called on a <!DTD...> */
    virtual void onDTD(const std::string& text) {}
    /**  Called on a <?PI..?> - either in the prolog (then, path is empty) or a nested one. */
    virtual void onPI(const std::string& text, const std::vector<STag>& path)  {}
    /**  Called on a <!-- comment --> - either in the prolog (then, path is empty) or a nested one. */
    virtual void onComment(const std::string& text, const std::vector<STag>& path) {}


};


};