#include "processor_ru.h"
#include <iostream>

#define _CRT_SECURE_NO_WARNINGS

using namespace std;
using namespace wiktcsv;


//void printProgress (int nKb) {
//  std::cout << "\r'  Processed, kb:\t" << nKb/1024;
//}

int main(int argc, char *argv[]) 
{

    ProcessorRu p;
    p.process("D:\\Projects\\Wiktionary\\", "ruwiktionary-20210520-pages-articles-multistream.xml"); //

  return 0;
}



  //Processor<LangCode::ru> proc; 
  //if(proc.process(path, printProgress)) cout << "\n\n  Done\n" ;
  //else cout <<"\n\n  Error\n";



 /* char path[260];
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

  cout << "\n  Wait... \n\n"; */