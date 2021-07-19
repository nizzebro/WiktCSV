#include "pch.h"
#include "processor_ru.h"
#include <cctype>
using namespace wiktcsv;


#include <iostream>


bool ProcessorRu::process(const char* fileName) noexcept
{
    std::string path;
    auto n = strlen(fileName);
    auto p = fileName + n - 1;  
    while (n && *p != '\\' && *p != '/') {--p; --n; }
    path.append(fileName, n);
    path += '\0';
    if(!writer.openFiles(path.data(), 
        {"pages.csv","headers.csv","lines.csv","rem.xml"})) return false;


    XmlParser::openFile(fileName);

      while(next())
      {
        if(isElement() && getName() == "page")
        {
          for(auto & a : getAttributes())
          { 
              std::cout << a.name << '=' << a.value << '\n';
          }

          auto i = level();
          while(next(i))
          {
              if(isText() && getName() == "text")
              {
                std::cout << "TEXT OF <PAGE>" << '\n' ;
              }
          }
          std::cout << " </page> is reached";
        }
      }

      XmlParser::closeFile();



    return true;
}
