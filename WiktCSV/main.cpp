#include "processor_ru.h"
#include <iostream>

#define _CRT_SECURE_NO_WARNINGS

using namespace std;
using namespace wiktcsv;


int main(int argc, char *argv[]) 
{
	HeaderAnalyserRu p;
	p.process("D:\\Projects\\Wiktionary\\", "ruwiktionary-20210520-pages-articles-multistream.xml");

    //ProcessorRu p;
    //p.process("D:\\Projects\\Wiktionary\\", "ruwiktionary-20210520-pages-articles-multistream.xml"); //

  return 0;
}

