
#include "Processor.h"
#include "pch.h"

//  pos of zero term. string in mem; if not found returns the end 

inline char* find(const char* str, const char* start, const char* end)
{
  for (; start <= end - sizeof(str); ++start) 
  {
    for (int i = 0;;++i) {
      if (str[i]==0)  return (char*) start;  
      if (str[i] != start[i]) break;
    }      
  }
  return (char*) end;
}


inline char* find(char c, const char* start, const char* end)
{
  for (; start <= end - 1; ++start) {if(*start ==  c) return (char*)start;}
  return (char*) end;
}

inline char* skipSpaces(const char* start, const char* end)
{
  for (; start <= end - 1; ++start) {if(*start > 0x20) return (char*)start;}
  return (char*) end;
}

/**
   The input file is read by chunks of fixed size (=chunkSize).
   The buffer has 256 bytes reserved located at the addr right before the chunk. 
   |------256 bytes----|-----------------------chunk-------------------------|
   This space is needed for the ability to move unprocessed final chars there,
   so that if we search for 'abc', and, only 'ab' occurs at the end, we move it
   to the front, load the next chunk and proceed further.

*/

// places n final chars before the chunk start and loads next chunk; returns true on success

bool Processor::loadNextChunk (int nCharsToSHift) 
{
  if(xml) 
  {
    strncpy(pChunk-nCharsToSHift, pBuffEnd - nCharsToSHift, nCharsToSHift); 
    xml.read(pChunk, chunkSize);
    if(!xml.eof()) 
    {
      nReadTotal +=chunkSize;
    }
    else {
      // zero-fill the remining part for any scans to be safe
      memset(pChunk + xml.gcount(), 0, chunkSize - xml.gcount()); 
      nReadTotal += xml.gcount();      
    }
    callback(nReadTotal/1024); // tell them how many kilobytes read 
    return true;
  }
  return false;
}

// find first occurence of the given string and set pos.

bool Processor::toFirst(char* s) {
  if(!loadNextChunk(xml,0)) return false;
  pos = find("<page>");
  while (pos >= pChunkEnd) {
    loadNextChunk(ifs, sizeof("<page>")-1));
    if(!n) return 0;

    pos = find("<page>", pChunk-(sizeof("<page>")-1));
  }
  return true;
}

bool Processor::toFirstPage () {
  if(!loadNextChunk(xml,0)) return false;
  const char* pChunkEnd = pChunk + n;
  const char* p = find(pChunk, pChunkEnd,"<page>");
  while (p >= pChunkEnd) {
    n = loadNextChunk(ifs, sizeof("<page>")-1));
    if(!n) return 0;

    p = find(pChunk-(sizeof("<page>")-1), pChunk + n,"<page>");
  }
  return p ;
}


char* Processor::toNextPage(char* p){
  pos = find("<page>", pos, pChunkEnd);
  while (pos >= pChunkEnd) {
    int n = loadNextChunk(ifs, sizeof("<page>")-1));
    if(!n) return 0;
    p = find(pChunk-(sizeof("<page>")-1), pChunk + n,"<page>");
  }
}


bool Processor::closeFiles() // true: success
{
  bool b = true;
  for (auto f in ofstreams)
  {
    if(f.is_open()) f.close(); 
    b = b || f;
  }
  if(xml.is_open()) xml.close();
  return(xml) || b;
}

bool Processor::openFiles(const char* path) // true: success
{
  xml.open(path,ifstream::in);
  if(xml) 
  {
    int n = path.find_last_of("/\\") + 1;
    string s (path, 0, n);
    const char*[nOfstreams] fn = {"pages.csv","headers.csv","lines.csv","rem.xml"};
    const char* p = fn;
    for (auto fs in ofstreams) 
    {
      s.replace(n,string::npos,p++);
      fs.open(s, ofstream::out, ofstream::trunc, ofstream::app);
      if(fs) continue;
      closeFiles();
      return false;
    } 
    return true;
  }
  return false;
}

bool Processor::process(const char* path, NumReadCallback cb) 
{
  if(openFiles(path) {
    callback = cb;
  
  
    return closeFiles();
  }
  return false;
}







