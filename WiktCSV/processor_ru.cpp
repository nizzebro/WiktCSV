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
    return true;
}






void ProcessorRu::processText(std::size_t& idWord, std::string_view idPage, std::string_view title) noexcept
{
	charser it(text); // block iterator
	charser line;	// line it gets each time

    bool ru = false;					// flag: ru-block				
	std::string_view wordHeader = {};   // curr homograph; empty if there is no level 2
	int subHeaderType = 0;				// sub-header id; values from file enum
	int braceCount = 0;					// trace "{{...}}"

    while(auto c = it.getspan(line, {'\r','\n' }, true))
    { 
		if(c == '\r') it.skip_if('\n'); // set up the pointer for the next line, in advance 

		// parse the line; check for headers first

		if (line.startsWith("= ")) // level 1 header; language
		{
			if (ru) break; // a ru-section just ended; exit loop to write the word if needed; 
			ru = (line == ("= {{-ru-}} ="));
			continue;
		}

		if (!ru) continue; // skip all non-ru lines - until a next level 1 header, if any

		// ru section

		if (!line) continue; // skip empty ones


		if (line.startsWith("==")) // some header
		{
			if (braceCount) // unclosed multiline template link - should not happen, but...
			{
				ofstreams[subHeaderType] << '\n';
				braceCount = 0;
			}

			subHeaderType = 0;

			if (line.startsWith("== ")) // level 2 header: homograph title
			{
				line.skip(3);
				line.getspan(line, " ==", false);
				if (!wordHeader.empty()) // flash previous
				{
					ofstreams[WORDS] << idWord << '\t' << idPage << '\t' << title << '\t' << wordHeader << '\n';
					++idWord;
				}
				
				wordHeader = line;
				continue;
			}

			line.seek(' ', true); 

			if (line.startsWith(u8"Морф"))
			{
				subHeaderType = SYNTAX;
				continue;
			}
			if (line.startsWith(u8"Этим"))
			{
				subHeaderType = ETIMOLOGY;
				continue;
			}
			if (line.startsWith(u8"Фраз"))
			{
				subHeaderType = PHRASES;
			    continue;
			}
			if (line.startsWith(u8"Посл"))
			{
				subHeaderType = SAYINGS;
				continue;
			}
			if (line.startsWith(u8"Знач"))
			{
				subHeaderType = MEANINGS;
				continue;
			}	
			continue;
		}

		if (line.startsWith(u8"&lt;!-- Служ"))
		{
			if (braceCount) // unclosed multiline template link - should not happen, but...
			{
				ofstreams[subHeaderType] << '\n';
				braceCount = 0;
			}
			subHeaderType = CATEGORY;
			continue;
		}

		// text line

		if(!subHeaderType) continue;

		if (!braceCount)
		{
			if (line == "* " || line == "# ")  continue;

			if (subHeaderType == CATEGORY && !line.startsWith(u8"{{Категория")) continue;
			if (subHeaderType == CATEGORY && line == u8"{{Категория|язык = ru|||}}") continue;

		} 


		auto n = 0; // local brace count

		charser tmp(line);

		while (auto c = tmp.getc()) // count braces and replace tabs
		{
			if(c == '{') ++n;
			else if (c == '}') --n;
			else if (c == '\t')
			{
				char* p = (char*)tmp.get() - 1;
				*p = ' ';
			}
		}

		
		if (!braceCount) ofstreams[subHeaderType] << idWord << '\t';
		ofstreams[subHeaderType] << line;
		braceCount += n;
		if (!braceCount) ofstreams[subHeaderType] << '\n';
    }

	if (!ru) return; // no ru-section in this page
	// write the last homograph (or single one if no level 2 headers, in this case wordHeader is just empty)
	ofstreams[WORDS] << idWord << '\t' << idPage << '\t' << title << '\t' << wordHeader << '\n';
	++idWord;
	if (braceCount) // unclosed multiline template link - should not happen, but...
	{
		ofstreams[subHeaderType] << '\n';
		braceCount = 0;
	}

}
