
#include "pch.h"
#include "Processor.h"

using namespace wiktcsv;
using namespace std;

#define _CRT_SECURE_NO_WARNINGS

char Processor::buff[Processor::buffSize];

// returns pos of zero term.string in mem, or the end of mem if not found

inline char* Processor::findFirst(const char* s, const char* start, const char* end) noexcept
{
  assert (sizeof(s));
  const char* p = start; 
  while (p < end) {
    char c = *p; ++p;
    if(*s != c) continue;

    start = p;
    for(;;) {
      ++s; 
      if ((!*s) || (p == end)) return (char*)p;
      if(*s != *p) {
        s -= p - start;
        p = start;
        break;
      }
      ++p;
    } 
  }      
  return (char*) p;
}

// returns pos of char in mem, or the end of mem if not found

inline char*  Processor::findFirst(char c, const char* start, const char* end)noexcept
{
  const char* p = start;
  for (;(p < end) && (*p !=  c); ++p) {}
  return (char*) p;
}

inline char* Processor::findFirstNonSpace(const char* start, const char* end) noexcept
{
  for (; start <= end - 1; ++start) {if(*start > 0x20) return (char*)start;}
  return (char*) end;
}

char * Processor::findFirst(const char * s) noexcept
{
  return Processor::findFirst(s, pos, pChunkEnd);
}

// find first occurence of the given string and set pos.

char * Processor::findFirst(char c) noexcept
{
  return Processor::findFirst(c, pos, pChunkEnd);
}

char * wiktcsv::Processor::findFirstNonSpace() noexcept
{
  return findFirstNonSpace(pos, pChunkEnd);
}

/**
   The input file is read by chunks of fixed size (=chunkSize).
   The buffer has 256 bytes reserved located at the addr right before the chunk. 
   |------256 bytes----|-----------------------chunk-------------------------|
   This space is needed for the ability to move unprocessed final chars there,
   so that if we search for 'abc', and, only 'ab' occurs at the end, we move it
   to the front, load the next chunk and proceed further.

*/

// places n final chars before the chunk start and loads next chunk; sets pos 
// returns number of bytes read.

size_t Processor::loadNextChunk (int nCharsToSHift) noexcept
{
  for(; nCharsToSHift; --nCharsToSHift) {
    pChunk[-nCharsToSHift] = pChunkEnd[-nCharsToSHift];
  }
  if(xml.read(pChunk, chunkSize)) {
    nReadTotal +=chunkSize;
    return chunkSize;
  }
  if(!xml.eof()) bError = true;
  size_t n = (size_t)xml.gcount(); 
  nReadTotal += n; 
  // zero-fill the remaining part to erase past data, for any scans to be safe
  memset(pChunk + n, 0, chunkSize - n); 
  return n;
}



bool Processor::toFirst(const char* s) noexcept {
  do {
    pos = findFirst(s);
    if(pos < pChunkEnd) return true;
  } while(loadNextChunk(sizeof(s)-1));
  return false;
}

bool Processor::toFirst (char c) noexcept {
  do {
    pos = findFirst(c);
    if(pos < pChunkEnd) return true;
    } while(loadNextChunk());
  return false;
}

bool Processor::skipSpaces () noexcept {
  do {
    pos = findFirstNonSpace(pos, pChunkEnd);
    if(pos < pChunkEnd) return true;
    } while(loadNextChunk());
  return false;
}



bool Processor::processNextPage() noexcept {
  if(toFirst("<page>")) {
    
  }
  return false;
}


bool Processor::openFiles(const char* path) noexcept
{
  xml.open(path,ifstream::in);
  if(xml) 
  {
    string s (path);
    int n = s.find_first_of("/\\") + 1; // if npos then 0
    const char* fn [nOfstreams] = {"pages.csv","headers.csv","lines.csv","rem.xml"};
    const char** p = fn;
    for  (auto &fs : ofstreams) 
    {
      s.replace(n,string::npos,*p++);
      fs.open(s, ofstream::out| ofstream::trunc| ofstream::app);
      if(fs) continue;
      closeFiles();
      return false;
    } 
    return true;
  }
  return false;
}


bool Processor::process(const char* path, void (*callback) (int)) noexcept
{
  if(openFiles(path)) {
    while (processNextPage()) {
      callback(nReadTotal/1024);
    };

    Processor::closeFiles();
    return !bError;
  }
  return false;
}

void Processor::closeFiles() noexcept
{
  for (auto &fs : Processor::ofstreams)
  {
    if(fs.is_open()) fs.close(); 
    bError = bError || !fs;
  }
  if(xml.is_open()) xml.close();
  bError = (!xml) || bError;
}
