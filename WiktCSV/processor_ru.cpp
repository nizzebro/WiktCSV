#include "pch.h"
#include "processor_ru.h"
#include <cctype>
using namespace wiktcsv;
using namespace char_parsers;

#include <iostream>

enum {
    PAGES, HEADERS, LINES, REM
};

bool ProcessorRu::process(const char* dir, const char* fileName) noexcept
{
    std::string s = dir;
    if(s.back() != '\\') s += ('\\');
    auto pos = s.size();
    s.append(fileName);
    s += ('\0');
    if (!XmlParser::openFile(s.data())) return false;
    s.erase(pos);
    if(!XmlParser::FileWriter::openFiles(std::move(s), {"pages.csv","headers.csv","lines.csv",
        "rem.xml"})) return false;

    std::string title, id; // page title, id
    std::string    rem;    // accumulator for pages of special namespaces (templates, categories etc)

    while(next()) // for each item
    {
        auto name = getName();                 
        if(isElement() && name == "page")      // page start
        {
            rem = text;                // assign the first tag to non-vocab pages accumulator
            auto i = getLevel();         
            while(next(i))                  // for each item in this page 
            {  
                rem.append(text);           // append each item until <ns> says which namespace this is
                if(isText())            
                {
                    name = getName();
                    if(name == "title") title = text;
                    else if (name == "ns") 
                    {
                        if(text != "0") 
                        {
                            while(next(i)) rem.append(text);
                            write(rem, REM);
                            break;
                        }
                    }
                    else if (name == "id") id = name;
                    else if (name == "text") processText(title, id);
                }
            }
        }
    }

    XmlParser::closeFile();
    XmlParser::FileWriter::closeFiles();
    return true;
}

void ProcessorRu::processText(std::string_view title, std::string_view id) noexcept
{
    charser it(text);
    while (it.seek("\n= {{"), true)
    {
        int i = 0;

    }
}
