#include "pch.h"
#include "processor_ru.h"
#include <cctype>
using namespace wiktcsv;

//const char* const ProcessorBase::outputFileNames[nOfstreams] = {"pages.csv","headers.csv","lines.csv","rem.input"};



#include <iostream>

void wiktcsv::ProcessorRu::process(const char * path) noexcept
{
    const char* const outputFileNames[] = {"pages.csv","headers.csv","lines.csv","rem.input"};

}

void wiktcsv::ProcessorRu::process()
{
    int i = 0;
    while(next())
    {
        if(isText()) {} //   std::cout << getText() << '\n'<< '\n'<< '\n'<< '\n'<< '\n';

    }
}




//void Processor::writeToBuffer(size_t iFile, const char * s, int n) noexcept
//{
//  assert((n > 0) && (iFile < outputs._size()));
//  Output o = outputs[iFile];
//  do {
//    const char* end = min(o.bufferEnd, o.cpos + n);
//    while(o.cpos != end) {*o.cpos = *s;  ++o.cpos; ++s; --n;}
//    if(end <= o.bufferEnd) return;
//    o.cpos = o.buffer;
//  }
//  while(_fwrite_nolock((void*) o.buffer, 1, write_buffer_size, o.file) == write_buffer_size);
//  err = true;
//}
//

//void Processor::closeFiles() noexcept
//{
//  for (auto &o : outputs)
//  {
//    if(o.file && (fclose(o.file) !=0)) err = true; 
//    delete[] o.buffer;
//  }
//  outputs.clear();
//  if(input && (fclose(input) !=0)) err = true; 
//  input = nullptr; 
//}