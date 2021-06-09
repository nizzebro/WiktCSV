#pragma once

#include "pch.h"
namespace wiktcsv {

class Processor {

public:
  
  // takes path to ru-wikt xml dump file, and generates linked csv tables.
  // callback is called with when next chunk is processed to show progress
  // returns true on success; false on any kind of error.

  bool process(const char* path, void (*callback) (int) = [] (int){}) noexcept;


protected:

  static const int chunkSize = 4096*20;         
  static const int buffSize = 256 + chunkSize;  

  static char buff[Processor::buffSize];
       
  static constexpr char*  pChunk  = Processor::buff + 256;    
  static constexpr char*  pChunkEnd = Processor::buff + Processor::buffSize;

  std::size_t nReadTotal = 0;
  bool bError = false;

  std::ifstream xml;
  static const int nOfstreams = 4;
  std::ofstream ofstreams[Processor::nOfstreams];
  std::ofstream&  csvPages = ofstreams[0];
  std::ofstream&  csvHeaders = ofstreams[1];
  std:: ofstream&  csvLines = ofstreams[2];
  std::ofstream&  xmlRem = ofstreams[3];
  
  char* pos = Processor::pChunkEnd;

  bool openFiles(const char* path) noexcept;
  void closeFiles() noexcept;

  size_t loadNextChunk(int nCharsToSHift = 0) noexcept;

  static char*  findFirst(const char* s, const char* start, const char* end ) noexcept;
  static char*  findFirst(char, const char* start, const char* end ) noexcept;
  static char * findFirstNonSpace(const char * start, const char * end) noexcept;
  char* findFirst(const char*) noexcept;
  char* findFirst(char) noexcept;
  char* findFirstNonSpace() noexcept;
  bool toFirst(const char*) noexcept;
  bool toFirst(char c) noexcept;
  bool skipSpaces() noexcept;
  bool processNextPage() noexcept;
  const char* finalMsg = "Success.";
};

}; // end namespace

