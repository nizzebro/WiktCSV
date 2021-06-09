#include <pch.h>
#include "Processor.h"

#define _CRT_SECURE_NO_WARNINGS

using namespace std;

inline bool hasGraphs(const char* s)
{
  while(*s && *s <= 0x20) { ++s;}
  return *s;
}

void printProgress (int nKb) {
  cout << "\r'  Processed, kb:\t" << nKb;
}
int main(int argc, char *argv[]) 
{
  char path[260];
  const char* p = "ruwiktionary.xml";
  cout << "\n  This program converts XML dump of Russsian wiktionary to .csv files.\n";
  
  if(argc <= 1)
  {
    cout << "\n  Path to the XML (ldefault: 'ruwiktionary.xml'):\n  " ;
    cin.getline (&path[0], 260);
    if(path[0]) p = path;
  }
  else 
  {
    p = argv[1];
    cout << "  " << p << "\n";
  }

  cout << "\n  Wait... \n\n"; 

  wiktcsv::Processor proc; 
  if(proc.process(path, printProgress)) cout << "\n\n  Done\n" ;
  else cout <<"\n\n  Error\n";
  return 0;
}
