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
    if(writer.openFiles(path.data(), {"pages.csv","headers.csv","lines.csv","rem.input"}))
    {
        bool b = XmlParser::process(path);
        writer.closeFiles();
        return b;
    }
    return false;
}


void ProcessorRu::process()
{

    while(next())
    {

    }


}
