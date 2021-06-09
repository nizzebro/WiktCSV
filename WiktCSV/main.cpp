

#include <pch.h>
#define _CRT_SECURE_NO_WARNINGS

using namespace std;
//
//
//
//
//
//// find next <page> block and load the content between tags into the given string
//bool getNextPageContents(ifstream& ifs, string& s){ 
//  const char* pStart = tagsearch(pPageEnd, pChunkEnd,"<page>");
//  while (pStart >= pChunkEnd) {
//    if(!ifs) return false;
//    // smth like '<pag' may still be at the end so move the reminder to front;
//    // initial 32 bytes are for better alignment
//    strncpy(pChunk-(sizeof("<page>")-1), pChunkEnd - (sizeof("<page>")-1), sizeof("<page>")-1); 
//    ifs.read(pChunk, chunkSize);
//    pStart = tagsearch(pChunk-(sizeof("<page>")-1), pChunk + ifs.gcount(),"<page>");
//  }
//  pStart += sizeof("<page>");
//  const char* pEnd = tagsearch(pStart, pChunkEnd, "</page>");
//  if (pEnd >= pChunkEnd) {  // contents is larger than buffer?
//    s.append(pStart, (int) (pChunkEnd - (sizeof("</page>")-1) - pStart));
//    for(;;) {
//      if(!ifs) return false;
//      strncpy(pChunk-(sizeof("</page>")-1), pChunkEnd - (sizeof("</page>")-1), sizeof("</page>")-1); 
//      ifs.read(pChunk, chunkSize); 
//      pEnd = tagsearch(pChunk-(sizeof("</page>")-1), pChunk + ifs.gcount(),"</page>");
//      if(pEnd < pChunkEnd) {
//        s.append(   pChunk-(sizeof("</page>")-1), pEnd - (pChunk-(sizeof("</page>")-1))    );
//        pPageEnd = pEnd + sizeof("</page>");
//        return true;
//      }
//      s.append((const char*)(pChunk-(sizeof("</page>")-1)), chunkSize);
//    }
//  }
//  s.append(pStart, (int) (pEnd - pStart));
//  pPageEnd = pEnd + sizeof("</page>");
//  return true;
//}
//
//char* replaceDelimiters (const char* src, char* dst) {
//  char prev = 0;
//  for (;;) {
//    char c = *src;  if(!c) break;
//    ++src;
//    if(isspace(c) {
//      
//    }
//
//    if(!(isspace(*src ) && prev == *src)) continue;
//    prev = *src;
//    *dst = c;
//    ++dst;
//  }
//  s = tmp;
//}
//
//
//
//bool processPageContent(ifstream& ifs, ofstream& ofs, char* p) {
//  char* p = toFirstPageContent(ifs);
//  if(!p) return false;
//  char* p = processPage(ifs, ofs, p);
//  if(!p) return false;
//}
//
//bool process(ifstream& ifs, ofstream& ofs) {
//  char* p = toFirstPageContent(ifs);
//  if(!p) return false;
//  p = processPage(ifs, ofs, p);
//  if(!p) return ifs.eof();
//}

inline bool hasGraphs(const char* s)
{
  while(*s && *s <= 0x20) { ++s;}
  return *s;
}

void printNumberOfRead(size_t n) {
  cout << '\r' << "Read, kb: " << n;
}

int main(int argc, char *argv[]) 
{
  char path[260];
  cout << "This program converts XML dump of Russsian wiktionary to .csv files." << '\n';
  cout << "XML file to process:" << '\n';
  if(argc > 1) {
    path = argv[1];
    cout << path << '\n';
  }
  else {
    do {
      getline(path, sizeof(path));
    } while (!hasGraphs(path));
  }
  cout << "Processing... Wait. " << '\n'; 

  Processor proc;
  if  (proc.process(path)) cout << '\n' << proc.process(path, &printNumberOfRead)? "Success." : "Error.";
  return 0;
}
