#pragma once

#include "Processor.h"

namespace wiktcsv {

const char* const a [4] = {"pages.csv","omographs.csv","meanings.csv","rem.input"};

class ProcessorRu: private Processor
{
public:
  //  std::ofstream&  csvPages = ofstreams[0];
  //std::ofstream&  csvHeaders = ofstreams[1];
  //std:: ofstream&  csvLines = ofstreams[2];
  //std::ofstream&  xmlRem = ofstreams[3];
  //virtual void process(const StringView&);
};


};