#include "pch.h"
#include "processor_ru.h"
#include <cctype>
using namespace wiktcsv;
using namespace char_parsers;

#include <iostream>


bool ProcessorRu::process(const char* dir, const char* fileName) noexcept
{
    std::string s = dir;
    if(s.back() != '\\') s += ('\\');
    auto pos = s.size();
    s.append(fileName);
    s += ('\0');
    if (!XmlParser::openFile(s.data())) return false;
    s.erase(pos);
    if(!XmlParser::FileWriter::openFiles(std::move(s), fileNames)) return false;


	std::string title, idPage; // page title, id
	std::string rem;    // accumulator for special pages
	std::size_t idWord = 0;

	std::cout << "Processed pages/Mb:";
	std::cout << '\n';

    while(next()) // for each item
    {                
        if(isElement("page"))      // <page> start
        {
            rem = text;                // accumulate anything until <ns> says which namespace this is
            auto i = getLevel();         
            while(next(i))                  // for each item in this page 
            {  
                rem.append(text);  
                if(isText())            
                {
                    auto name = getName();
                    if(name == "title") title = text;
                    else if (name == "ns") 
                    {
                        if(text != "0")     // special page (template, category, etc); append it to to rem.xml
                        {
							ofstreams[REM] << rem; // flush accum...
                            while(next(i))  // ... and the rest;
                            {
								ofstreams[REM] << text;
								if (isElementEnd()) ofstreams[REM] << '\n';
                            }
                            break;
                        }
                    }
                    else if (name == "id") idPage = text;
                    else if (name == "redirect") break;     // skip any redirecting stub pages
					else if (name == "text")
					{
						processText(idWord, idPage, title); /// text always goes last; we have all vars collected
						if ((idWord & 0xFF) == 0) // each 256 pages
						{
							std::cout << "\r                       ";
							std::cout << '\r' << idWord << '\t' << (getFilePos() / 1024 / 1024);
						}
						
					}
                }
            }
        }
    }

    XmlParser::closeFile();
    XmlParser::FileWriter::closeFiles();
	std::cout << '\n' << "Processed total:" << '\n' <<
		idWord << " pages" << '\n' <<
		getFilePos() << " bytes" << '\n' <<
		"errorCode: " << (int)getErrorCode();
    return true;
}






inline void ProcessorRu::processText(std::size_t& idWord, std::string_view idPage, std::string_view title) noexcept
{
	charser it(text); // block iterator
	charser line;	 // line which the iterator gets each time

    bool ru = false;					// flag indicating a russian word block				
	std::string_view wordHeader = {};   // current homograph; empty if there's no level 2
	int subHeaderType = 0;				// file to write current line

	std::string outputLine;
	int braceCount = 0;					// counts '{' and '}' to detect multilines


    while(it)
    { 
		auto c = it.seek_span( { '\r','\n' }, true, line);
		if(c == '\r') it.skip('\n'); // set up the pointer for the next line, in advance 

		// parse the line; check for headers first
		bool isHeader = false;
		bool isSpec = false;

		if (line.startsWith('=')) isHeader = true;
		else if (line.startsWith(u8"&lt;!-- Служ")) isSpec = true;
			
		if (isHeader && line.startsWith("= ")) // level 1 header; language
		{
			if (ru) break; // a ru-section just ended; exit loop to write the word if needed; 
			ru = (line == ("= {{-ru-}} ="));
			continue;
		}

		if (!ru) continue; // skip non-ru 

		if (isHeader || isSpec)
		{	
			if (!isSpec)
			{
				if (line.skip("== ")) // level 2 header: homograph title
				{
					line.seek_span(" ==", false, line);
					if (!wordHeader.empty()) // flash previous
					{
						ofstreams[WORDS] << idWord << '\t' << idPage << '\t' << title << '\t' << wordHeader << '\n';
						++idWord;
					}

					wordHeader = line;
					continue;
				}

				line.seek(' ', true);

				if (line.startsWith(u8"Морф")) subHeaderType = SYNTAX;
				else if (line.startsWith(u8"Этим")) subHeaderType = ETIMOLOGY;
				else if (line.startsWith(u8"Фраз")) subHeaderType = PHRASES;
				else if (line.startsWith(u8"Посл")) subHeaderType = SAYINGS;
				else if (line.startsWith(u8"Знач")) subHeaderType = MEANINGS;
				else subHeaderType = 0;
			}
			else subHeaderType = CATEGORY;
			continue;
		}

		// text line

		if(!subHeaderType) continue; // skip data we do not need

		line.seek(gt(' ')); // trim leading blanks

		if (!line) continue; // skip empty ones

		if (line == "* " || line == "# ")  continue;

		if (subHeaderType == CATEGORY)
		{
			if (!line.startsWith(u8"{{Кат")) continue;
		}
		else
		{
			if (line == "{{table-bot}}") continue;
			if (line == "{{table-top}}") continue;
			if (line == u8"{{конец кол}}")  continue;
			if (line.startsWith(u8"{{кол|"))
			{
				line.seek("}}", true);
				if(!line)continue;
			}
		}
		if (!braceCount) outputLine.clear();

		// append chars to outputLine with clean up

		while (auto c = line.getc())
		{
			if (c == '{') ++braceCount;
			else if (c == '}') --braceCount;
			else if (c == '\t') c = ' ';
			outputLine += c;
		}

		if (braceCount)
		{
			outputLine += ' '; // otherwise words can glue together as we removed linebreaks
			continue; // until braces match in a next line
		}

		// write outputLine

		if (subHeaderType == CATEGORY) // split it into words and write separate records
		{
			charser categoryLine(outputLine);
			charser categoryDef;
			categoryLine.skip(u8"{{Категория");
			categoryLine.seek('|', true);		
			while (categoryLine)
			{
				categoryLine.seek(gt(' '));
				categoryLine.seek_span({ '|','}' }, true, categoryDef);
				if (!categoryDef) continue;
				if (categoryDef.startsWith ('}')) continue;
				if (categoryDef.startsWith(u8"язык") && categoryDef.contains('=')) continue;
				ofstreams[subHeaderType] << idWord << '\t' << categoryDef << '\n';
				categoryLine.seek(gt(' '));
			}
		
		}
		else
		{
			ofstreams[subHeaderType] << idWord << '\t' << outputLine << '\n';
		}

    }

	if (!ru) return; // no ru-section in this page

	// write the last homograph (or single one if no level 2 headers, in this case wordHeader is just empty)
	ofstreams[WORDS] << idWord << '\t' << idPage << '\t' << title << '\t' << wordHeader << '\n';
	++idWord;

}
