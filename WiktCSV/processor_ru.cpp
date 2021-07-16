#include "pch.h"
#include "processor_ru.h"
#include <cctype>
using namespace wiktcsv;

//const char* const ProcessorBase::outputFileNames[nOfstreams] = {"pages.csv","headers.csv","lines.csv","rem.input"};



#include <iostream>
void wiktcsv::ProcessorRu::process()
{
    int i = 0;
    while(next())
    {
        if(isElement())     std::cout << getName() << '\n';
        ++i;
        if(i == 590)
        {
            auto c = 0;
        }
    }
}
