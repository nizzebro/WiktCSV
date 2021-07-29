#include "processor_ru.h"
#include <iostream>

#define _CRT_SECURE_NO_WARNINGS

using namespace std;
using namespace wiktcsv;


int main(int argc, char *argv[]) 
{

    ProcessorRu p;
    p.process("D:\\Projects\\Wiktionary\\", "sample.xml"); //"ruwiktionary-20210520-pages-articles-multistream.xml"

  return 0;
}

