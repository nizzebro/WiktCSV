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


	std::string title; // page title
	std::string rem;    // accumulator for special pages
	std::size_t idWord = 0;

	std::cout << "Processed pages/Mb:";
	std::cout << '\n';

    while(next()) // for each item
    {                
        if(isElement("page"))      // <page> start
        {
            rem = getText();                // accumulate anything until <ns> says which namespace this is
            auto i = getLevel();         
            while(next(i))                  // for each item in this page 
            {  
                rem.append(getText());

				if (!isText())  continue;

                auto name = getName();
                if(name == "title") title = getText();
                else if (name == "ns") 
                {
                    if(getText() != "0")     // special page (template, category, etc); append it to to rem.xml
                    {
						ofstreams[REM] << rem; // flush accum...
                        while(next(i))  // ... and the rest;
                        {
							ofstreams[REM] << getText();
							if (isElementEnd()) ofstreams[REM] << '\n';
                        }
                        break;
                    }
                }
                else if (name == "redirect") break;     // skip any redirecting stub pages
				else if (name == "text")
				{
					processText(idWord,  title); /// text always goes last; we have all vars collected
					if ((idWord & 0xFF) == 0) // each 256 pages
					{
						std::cout << "\r                       ";
						std::cout << '\r' << idWord << '\t' << (getFilePos() / 1024 / 1024);
					}
						
				}

            }
        }
    }
	int i = 0;
	for (auto & s : wordTypeNames)
	{
		ofstreams[WORDTYPENAMES] << i << '\t' << s << '\n';
		++i;
	}

    XmlParser::closeFile();
    XmlParser::FileWriter::closeFiles();
	std::cout << '\n' << "Processed total:" << '\n' <<
		idWord << " pages" << '\n' <<
		getFilePos() << " bytes" << '\n' <<
		"errorCode: " << (int)getErrorCode();
    return true;
}


inline void ProcessorRu::processText(std::size_t& idWord, std::string_view title) noexcept
{
	charser text(getText()); // block iterator
	charser line;	 // line which the iterator gets each time

    bool ru = false;					// inside a russian word block				
	std::string_view homograph = {};    // title of homograph (level 2); empty if none
	int subHeaderType = 0;				 // header type; value from the file enum

	std::string outputLine = {};		// buffer to process lines before writing 
	int braceCount = 0;					// counts '{' and '}' to detect multilines
	
	int wordType = 0;
	int stress = 0;

	std::string pat;

	if (title[0] >= (char)0xD0 && title[1] <= (char)0xAF)
	{
		wordType = kTypeProper;
	}
	else
	{
		charser it(title);
		if (it.contains(' ')) wordType = kTypePhrase;
	}

    while(text)
    { 
		text.seek_span( {'\n', '\r' }, true, line);
		text.trimLeading('\n'); // set up the pointer for the next line, in advance 
		line.trim(lt_eq(' '));
		
		if (!line || line == "*" || line == "#")  continue;

		// parse the line; check for headers first

		if (!braceCount) // not a prolongation of a previous multiline tag?
		{
			if (line.skip('='))
			{
				if (!line.skip('=')) // level 1 header; language
				{
					if (ru) break; // a ru-section just ended; exit loop to write the word if needed; 
					line.trimLeading(lt_eq(' '));
					ru = (line.startsWith("{{-ru-}}"));	
					continue;
				}

				if (!ru) continue; // skip non-ru headers

				if (!line.skip('=')) // level 2 header: homograph title
				{
					line.seek_span("==", false, line);
					line.trim(lt_eq(' '));

					// flash previous
					if (!homograph.empty())
					{
						ofstreams[WORDS] << idWord << '\t'  << title << '\t' << homograph <<
							'\t' << wordType << '\t' << pat << '\t' << stress << '\n';
						pat.clear();
						++idWord;
						subHeaderType = 0;
						wordType = kTypeUnknown;
						stress = 0;
					}

					homograph = line;
					continue;
				}
				// level >= 3 header

				line.trimLeading('=');
				line.trimLeading(' ');

				if (line.startsWith(u8"Морф")) subHeaderType = LINES;
				else if (line.startsWith(u8"Тип и")) subHeaderType = LINES;
				else if (line.startsWith(u8"Омофор")) subHeaderType = LINES;
				else if (line.startsWith(u8"Этим")) subHeaderType = LINES;
				else if (line.startsWith(u8"Фраз")) subHeaderType = PHRASES;
				else if (line.startsWith(u8"Типич")) subHeaderType = PHRASES;
				else if (line.startsWith(u8"Посл")) subHeaderType = SAYINGS;
				else if (line.startsWith(u8"Знач")) subHeaderType = LINES;
				else if (line.startsWith(u8"Сема")) subHeaderType = LINES;
				else subHeaderType = -1;

				continue;
			}

			if(!ru) continue; // skip non-ru lines

			if (line.startsWith(u8"&lt;!-- Служ"))
			{
				subHeaderType = CATEGORY;
				continue;
			}

			if (subHeaderType == -1) continue; // skip sub-headers we do not need


			outputLine.clear(); // we will append

		} // end if (!braceCount)

		// text line
		
		// append line to outputLine, counting braces and replacing tabs with spaces


		while (auto c = line.getc())
		{
			if (c == '{')++braceCount;
			else if (c == '}') --braceCount;
			else if (c == '\t') c = ' ';
			outputLine += c;
		}

		if (braceCount) // token is unfinished yet?
		{
			outputLine += ' '; // otherwise words can glue together as we removed linebreaks
			continue;
		}

		// the string is complete, parse and write

		charser item(outputLine);

		if (item.startsWith(u8"{{илл|")) continue;
		if (item.startsWith(u8"{{table-")) continue;
		if (item.startsWith(u8"{{кол|")) continue;
		if (item.startsWith(u8"{{конец")) continue;
		if (subHeaderType == CATEGORY || item.startsWith(u8"{{Категория"))
		{	
			if (!item.skip(u8"{{Категория")) continue;	
			item.seek('|', true);
			charser def;
			// break into separate category tokens and write them to table
			while (item)
			{
				if(item.seek_span({ '|','}', '{'}, true, def) == '{') break;
				def.trim(' ');
				if (!def) continue;
				if (def.startsWith(u8"язык") && def.contains('=')) continue;
				ofstreams[CATEGORY] << idWord << '\t' << def << '\n';
			}
			continue;
		}

		if (subHeaderType)
		{
			if (subHeaderType == LINES)
			{
				// skip empty ones
				if (item == u8"От {{этимология:|да}}")  continue;
				if (item == u8"Происходит от {{этимология:|да}}")  continue;
			}
			else
			{
				ofstreams[subHeaderType] << outputLine << '\n';
				continue;
			}
		}

		// handle {morpho-ru}
		if (item.startsWith(u8"{{морфо"))
		{
			item.seek('|', true);
			charser def;
			while (item)
			{
				item.seek_span({ '|','}' }, true, def);
				def.trim(' ');
				if (!def) continue;
				if (def.startsWith(u8"и=")) continue;
				int type = 0;
				if (def.skip('-'))
				{
					if (!def.endsWith('-'))
					{
						type |= kTypeSuffix;
						if (def.skip('-')) type = kTypeSuffixoid;
					}
					else
					{
						def.setEnd(def.end() - 1);
						type |= kTypeInterfix;
					}
				}
				else
				{
					if (def.skip('+')) type = kTypeEnding;
					else
					{
						if (!def.endsWith('-')) type = kTypeRoot;
						else
						{
							def.setEnd(def.end() - 1);
							if (!def.endsWith('-')) type = kTypePrefix;
							else
							{
								def.setEnd(def.end() - 1);
								type = kTypePrefixoid;
							}
						}
					}
				}
				ofstreams[WORDMORPH] << idWord << '\t' << type << '\t' << def << '\n';
			}
		}
		else if (!wordType)
		{
			
			if (item.startsWith(u8"{{morph"))
			{
				item.seek('|', true);
				charser def;
				while (item)
				{
					item.seek_span({ '|','}' }, true, def);
					def.trim(' ');
					if (!def) continue;
					if (def.startsWith("p=") || def.startsWith("pr"))
					{
						wordType = kTypePrefix;
					}
					else if (def.startsWith("po")) wordType = kTypePostfix;
					else if (def.startsWith("s")) wordType = kTypeSuffix;
					else if (def.startsWith("i")) wordType = kTypeInterfix;
					else if (def.startsWith("t")) wordType = kTypeInterfix;
					else if (def.startsWith("c")) wordType = kTypeCircumfix;
					else if (def.startsWith("r")) wordType = kTypeRoot;
					else if (def.startsWith("e")) wordType = kTypeEnding;
					continue;
				}
			}
			else if (item.startsWith(u8"{{suffix")) wordType = kTypeSuffix;
			else if (item.startsWith(u8"{{собств")) wordType = kTypeProper;
			else if (item.startsWith(u8"{{phrase")) wordType = kTypePhrase;
			else if (item.startsWith(u8"{{Форм") || item.startsWith(u8"{{словоформа")) wordType = kTypeForm;
			else
			{
				int t = 0;

				if (item.startsWith(u8"{{числ ")) t = kTypeNumeral;
				else if (item.startsWith(u8"{{сущ "))  t = kTypeNoun;
				else if (item.startsWith(u8"{{прил "))  t = kTypeAdj;
				else if (item.startsWith(u8"{{мест "))  t = kTypePronoun;
				else if (item.startsWith(u8"{{Мс-"))  t = kTypePossess;
				else if (item.startsWith(u8"{{гл "))  t = kTypeVerb;
				else if (item.startsWith(u8"{{прич "))  t = kTypeParticiple;

				if (t)
				{
					item.seek("ru", true);
					item.seek_append('|', false, pat);
				}
				else
				{
					if (item.startsWith(u8"{{сущ-")) t = kTypeNoun;
					else if (item.startsWith(u8"{{прил-")) t = kTypeAdj;
			
					if (t)
					{
						item.seek('|', true);
						item.seek('|', true);
						item.skip(u8"индекс=");
						item.seek_append('|', false, pat);
					}
					else if (item.startsWith(u8"{{adv ")) t = kTypeAdv;
					else if (item.startsWith(u8"{{conj ")) t = kTypeConj;
					else if (item.startsWith(u8"{{деепр ")) t = kTypeAdvParticiple;
					else if (item.startsWith(u8"{{part ")) t = kTypePart;
					else if (item.startsWith(u8"{{prep ")) t = kTypePrep;
					else if (item.startsWith(u8"{{interj ")) t = kTypeInterj;
					else if (item.startsWith(u8"{{pred ")) t = kTypePred;
				}
			}

		}

		if (!stress) stress = getStressedSyllable(item);

		if (!subHeaderType) continue;

		//  LINES left only...
		ofstreams[LINES] << idWord << '\t' << outputLine << '\n';
    }

	if (!ru) return; // no ru-section in this page


	// write the last homograph (or single one if no level 2 headers, in this case wordHeader is just empty)
	ofstreams[WORDS] << idWord << '\t' <<  title << '\t' << homograph 
		<< '\t' << wordType << '\t' << pat << '\t' << stress << '\n';
	++idWord;
	
}


// {{по-слогам|}}
inline int wiktcsv::ProcessorRu::getStressedSyllable(u8charser s) noexcept
{
	s.trim(' ');
	
	if (!s.seek(u8"{{по-слогам|", true)) return 0;
	u8charser def;
	int i = 0;
	UChar c;
	do 
	{
		c = s.seek_span('|', true, def);
		if (!def) return 0;
		++i;
		if (def.contains(std::initializer_list<UChar>{0x301, U'ё' })) return i; // acute or ё
	} while (c);

	return i == 1? i : 0;
}


/////////////////////////////////////////

bool wiktcsv::HeaderAnalyserRu::process(const char * dir, const char * fileName) 
{
		std::string s = dir;
		if (s.back() != '\\') s += ('\\');
		auto pos = s.size();
		s.append(fileName);
		s += ('\0');
		if (!XmlParser::openFile(s.data())) return false;
		s.erase(pos);
		if (!XmlParser::FileWriter::openFiles(std::move(s), fileNames)) return false;

		struct Item
		{
			std::string name;
			std::size_t count;
		};

		std::vector<Item> hdrNames;
		std::vector<Item> tmplNames;

		auto addHeaderName = [&](std::string_view s)
		{
			for (auto & h : hdrNames)
			{
				if (h.name == s)
				{
					h.count++;
					return;
				}
			}
			hdrNames.push_back({ std::string(s), 1});
		};
		auto addTmplName = [&](std::string_view s)
		{
			for (auto & h : tmplNames)
			{
				if (h.name == s)
				{
					h.count++;
					return;
				}
			}
			tmplNames.push_back({ std::string(s), 1 });
		};


		bool isWord = false;


		while (next()) // for each item
		{
			if (!isText()) continue;

			auto name = getName();

			if (name == "ns")
			{
				isWord = (getText() == "0");
				continue;
			}
			if (name == "redirect") isWord = false;
			if (name != "text") continue;
		
			charser it(getText()); // block iterator
			charser line;	 // line which the iterator gets each time

			bool ru = false;			// inside russian word block				
			bool subHeader = false;     // inside header of level >= 2
			while (it)
			{
				auto c = it.seek_span({ '\r','\n' }, true, line);
				if (c == '\r') it.skip('\n'); // set up the pointer for the next line, in advance 

				line.trim(lt_eq(' '));
				if (!line) continue; // skip empty ones

				if (line.skip('='))
				{
					if (!line.skip('=')) // level 1 header; language
					{
						if (ru) break; // a ru-section just ended; exit loop to write the word if needed; 
						line.trimLeading(lt_eq(' '));
						ru = (line.startsWith("{{-ru-}}"));
						continue;
					}

					if (!ru) continue; // skip non-ru 

					if (!line.skip('=')) // level 2 header: homograph title
					{
						subHeader = false;
						continue; // level 2 header: homograph title
					}

					subHeader = true;

					line.trimLeading('='); //skip the rest

					line.seek_span('=', false, line); // load span into itself 

					line.trim(lt_eq(' '));
					if (!line)
					{
						continue;
					}
					
					addHeaderName(line);

				}

				if (!ru) continue; // skip non-ru 

				if (line.startsWith(u8"&lt;!-- Служ")) subHeader = true;

				if (!subHeader)
				{
					if (line.skip("{{"))
					{
						line.seek_span({ '|','}' }, false, line);
						addTmplName(line);
					}
				}
			}
		}

		
		for (auto & h : hdrNames)
		{
			ofstreams[0] << h.name << '\t' <<  h.count << '\n';
		}
		for (auto & h : tmplNames)
		{
			ofstreams[1] << h.name << '\t' << h.count << '\n';
		}

		



		XmlParser::closeFile();
		XmlParser::FileWriter::closeFiles();
		return true;


}
