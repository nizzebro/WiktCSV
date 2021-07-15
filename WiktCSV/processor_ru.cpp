#include "pch.h"
#include "processor_ru.h"
#include <cctype>
using namespace wiktcsv;

//const char* const ProcessorBase::outputFileNames[nOfstreams] = {"pages.csv","headers.csv","lines.csv","rem.input"};


#include <iostream>/*
void ProcessorRu::onPrefix(const Element & elem, bool isSelfClosing)
{
    if(selfClosing) return;
    auto s = elem.getName();
    if (s == "page") 
    {
        insidePage = true;
        return;
    }
    
    if(s == "title")
    {
        charser it(s);
        if (it.seek(':')) return;
        title = s;
        return;
    }
    if(s == "ns")
    {
        title = s;
        return;
    }

    std::cout. write(s.data(), s.size());
    std::cout << "\n";
}

void ProcessorRu::onContent(const Element & elem)
{
    if (!insidePage) return;




}

void ProcessorRu::onSuffix(const Element & elem)
{
    auto s = elem.getName();
    if (s == "page") 
    {
        insidePage = false;
        return;
    }
}*/


//void wiktcsv::ProcessorRu::process(const StringView& s)
//{
  //std::string title;
  //std::string id;
  //while(getTag("page")) {
  //  while(getTag()){
  //    if (s.tagName() = "title") getWord(title); continue; 
  //    if (s.tagName() = "ns") if(getWord() != "0")
  //    {
  //      // write to extras
  //      break;
  //    }; 
  //    if(s.tagName() = "id") 
  //    {
  //      getWord(id); continue; 
  //    }
  //    if(s.tagName() = "text") 
  //    {
  //      if(empty(title)) || empty(id)) break;
  //      int nEntries = 0;
  //      while(getLine()) {
  //        if(s != "= {{-ru-}} =") continue;
  //        while(getLine()) && !s.beginsWith("= ")) {
  //          
  //        }
  //      }
  //      if(n > 0) {
  //          //create new record in pages.csv;
  //      }
  //    }
  //  }
  //}
//}

void wiktcsv::ProcessorRu::process()
{
}
