#pragma once


class Processor {

public:

  typedef void (*NumReadCallback)(size_t);

  // takes path to ru-wikt xml dump file, and generates linked csv tables.
  // callback is called with when next chunk is processed to show progress
  // returns true on success; false on any kind of error.

  bool process(const char* path, NumReadCallback callback = [](size_t){});

protected:

  const int chunkSize = 4096*20;         
  const int buffSize = 256 + chunkSize;  

  static char buff[buffSize]={};        

  char* const pChunk = buff + 256;    
  char* const pChunkEnd = buff + buffSize;

  int nReadTotal = 0;
  NumReadCallback callback;

  ifstream xml;
  const int nOfstreams = 4;
  ofstream ofstreams[nOfstreams];
  ofstream& const csvPages = ofstreams[0];
  ofstream& const csvHeaders = ofstreams[1];
  ofstream& const csvLines = ofstreams[2];
  ofstream& const xmlRem = ofstreams[3];
  
  char* pos = pChunkEnd;

 

  bool closeFiles();
  bool openFiles(const char* path);

  bool loadNextChunk(int nCharsToSHift = 0);
  bool toFirst(char * s);
  bool toFirstPage();
};

