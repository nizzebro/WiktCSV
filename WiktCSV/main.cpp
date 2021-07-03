#include <pch.h>
#include "Processor.h"
#include <iostream>

#define _CRT_SECURE_NO_WARNINGS

using namespace std;
using namespace wiktcsv;


void printProgress (int nKb) {
  std::cout << "\r'  Processed, kb:\t" << nKb/1024;
}

int main(int argc, char *argv[]) 
{
  char path[260];
  const char* p = "ruwiktionary.input";
  cout << "\n  This program converts XML dump of Russsian wiktionary to .csv files.\n";
  
  if(argc <= 1)
  {
    cout << "\n  Path to the XML (ldefault: 'ruwiktionary.input'):\n  " ;
    cin.getline (&path[0], 260);
    if(path[0]) p = path;
  }
  else 
  {
    p = argv[1];
    cout << "  " << p << "\n";
  }

  cout << "\n  Wait... \n\n"; 

  //Processor<LangCode::ru> proc; 
  //if(proc.process(path, printProgress)) cout << "\n\n  Done\n" ;
  //else cout <<"\n\n  Error\n";
  return 0;
}
