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
	std::size_t idWord = 1;

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
	int i = 1;
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
	int wordStress = 0;

	std::string pat;

	u8charser t(title);
	auto c = t.getc();
	if (of_range(U'А', U'Я')(c))
	{
		c = t.getc();
		if (!(of_range(U'А', U'Я')(c))) wordType = kTypeProper;
		else wordType = kTypeAbbrev;
	}
	else if(t.contains(U' ')) wordType = kTypePhrase;

    while(text)
    { 
		text.seek_span( {'\n', '\r' }, true, line);
		text.trimLeading('\n'); // set up the pointer for the next line, in skip 
		line.trim(lt_eq(' '));
		
		if (!line || line == "*" || line == "#")  continue;

		// parse the line; check for headers first

		if (!braceCount) // not a prolongation of a previous multiline tag?
		{
			if (line.skip_if('='))
			{
				if (!line.skip_if('=')) // level 1 header; language
				{
					if (ru) break; // a ru-section just ended; exit loop to write the word if needed; 
					line.trimLeading(lt_eq(' '));
					ru = (line.startsWith("{{-ru-}}"));	
					continue;
				}

				if (!ru) continue; // skip non-ru headers

				if (!line.skip_if('=')) // level 2 header: homograph title
				{
					line.seek_span("==", false, line);
					line.trim(lt_eq(' '));

					// flash previous
					if (!homograph.empty())
					{
						ofstreams[WORDS] << idWord << '\t' << title << '\t' << homograph;
						ofstreams[WORDS] << '\t' << wordType << '\t' << pat << '\t' << wordStress << '\n';
						pat.clear();
						++idWord;
						subHeaderType = 0;
						if(wordType < kTypePhrase) wordType = kTypeUnknown; // do not reset if abbr, proper name, or phrase
						wordStress = 0;
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

		bool brace = false; // to track spaces after '{' which prevent parsing

		UChar c;
		while (c = line.getc())
		{
			if (c == '{')
			{
				++braceCount;
				brace = true;
			}
			else 
			{	
				if (brace)
				{
					if (c <= ' ') continue;
					brace = false;
				}
				if (c == '}') --braceCount;
				else if (c == '\t') c = ' ';
				else if (c == '&')
				{
					if (line.skip_if("quot;")) c = '\"';
					else if (line.skip_if("lt;")) c = '<';
					else if (line.skip_if("gt;")) c = '>';
					else if (line.skip_if("apos;")) c = '\'';
				}
			}
			outputLine += c;
		}

		if (braceCount) // token is unfinished yet?
		{
			if(!brace) outputLine += ' '; // otherwise words might glue together as we removed linebreaks
			continue;
		}

		// the string is complete, parse and write

		u8charser item(outputLine);
		
		if (item.startsWith(u8"{{илл|")) continue;
		if (item.startsWith(u8"{{table-")) continue;
		if (item.startsWith(u8"{{кол|")) continue;
		if (item.startsWith(u8"{{конец")) continue;
		if (item.startsWith(u8"{{списки ")) continue;
		if (item.startsWith(u8"{{перев-блок|")) continue;
		if (item.startsWith(u8"{{длина ")) continue;
		if (item == u8"#{{пример|}}") continue;
		if (item.skip_if(u8"{{Категория"))
		{	
			item.seek('|', true);
			u8charser def;
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

		if ((subHeaderType == PHRASES) || (subHeaderType == SAYINGS))
		{	
			ofstreams[subHeaderType] << outputLine << '\n';
			continue;
		}

		if (subHeaderType == LINES)
		{
			// skip empty ones
			if (item == u8"От {{этимология:|да}}")  continue;
			if (item == u8"Происходит от {{этимология:|да}}")  continue;
			if (item == u8"{{этимология:|да}}")  continue;
		}

		if (!wordType) // not phrase, proper or abbrev
		{


			// handle {{морфо-ru}}
			if (item.startsWith(u8"{{морфо"))
			{
				item.seek('|', true);
				u8charser def;
				while (item)
				{
					item.seek_span({ '|','}' }, true, def);
					def.trim(' ');
					if (!def) continue;
					if (def.startsWith(u8"и=")) continue;
					int type = 0;
					if (def.skip_if('-'))
					{
						if (!def.endsWith('-'))
						{
							type |= kTypeSuffix;
							if (def.skip_if('-')) type = kTypeSuffixoid;
						}
						else
						{
							def.setEnd(def.end() - 1);
							type |= kTypeInterfix;
						}
					}
					else
					{
						if (def.skip_if('+')) type = kTypeEnding;
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
				continue;
			}

			// {{morpho-ru}}
			if (item.startsWith(u8"{{morph"))
			{
				item.seek('|', true);
				u8charser def;
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
		
			// first, handle parts of speech which have inflection pattern

			if (item.startsWith(u8"{{сущ "))  wordType = kTypeNoun;
			else if (item.startsWith(u8"{{гл "))  wordType = kTypeVerb;
			else if (item.startsWith(u8"{{прил "))  wordType = kTypeAdj;
			else if (item.startsWith(u8"{{мест "))  wordType = kTypePronoun;
			else if (item.startsWith(u8"{{Мс-"))  wordType = kTypePossess;
			else if (item.startsWith(u8"{{числ ")) wordType = kTypeNumeral;
			if (wordType)
			{
				item.seek("ru", true);
				appendInflection(wordType, item, pat);
			}
			else
			{
				if (item.startsWith(u8"{{сущ-")) wordType = kTypeNoun;
				else if (item.startsWith(u8"{{прил-")) wordType = kTypeAdj;
					
				if (wordType)
				{
					item.seek('|', true);
					item.seek('|', true);
					item.skip_if(u8"индекс=");
					appendInflection(wordType, item, pat);
				}
				else if (item.startsWith(u8"{{числ-"))
				{
					wordType = kTypeNumeral;
					item.seek('-', true);
					appendInflection(wordType, item, pat);
				}
				else if (item.startsWith(u8"{{adv ")) wordType = kTypeAdv;
				else if (item.startsWith(u8"{{conj ")) wordType = kTypeConj;
				else if (item.startsWith(u8"{{прич ") || 
					item.startsWith(u8"{{деепр ") ||
					item.startsWith(u8"{{Форм") || 
					item.startsWith(u8"{{словоформа")) wordType = kTypeForm;
				else if (item.startsWith(u8"{{part ")) wordType = kTypePart;
				else if (item.startsWith(u8"{{prep")) wordType = kTypePrep;
				else if (item.startsWith(u8"{{interj")) wordType = kTypeInterj;
				else if (item.startsWith(u8"{{onomatop")) wordType = kTypeSound;
				else if (item.startsWith(u8"{{pred")) wordType = kTypePred;
				if (item.startsWith(u8"{{suffix")) wordType = kTypeSuffix;
				else if (item.startsWith(u8"{{intro")) wordType = kTypeIntro;

			} // end if (!wordType)

		} // end if (!wordType)

		else if (wordType == kTypeProper) // handle inflections for proper names
		{
			if (item.startsWith(u8"{{сущ ") || item.startsWith(u8"{{прил "))
			{
				item.seek("ru", true);
				appendInflection(wordType, item, pat);
			}
			else if (item.startsWith(u8"{{сущ-") || item.startsWith(u8"{{прил-"))
			{
				item.seek('|', true);
				item.seek('|', true);
				item.skip_if(u8"индекс=");
				appendInflection(wordType, item, pat);
			}
		} // end if (wordType)...else if(wordType == kTypeProper)

		
		if (!wordStress)
		{
			wordStress = getStressedSyllable(item);

		}
			

		if (!subHeaderType) continue;

		//  LINES left only...
		ofstreams[LINES] << idWord << '\t' << outputLine << '\n';
    }

	if (!ru) return; // no ru-section in this page


	// write the last homograph (or single one if no level 2 headers, in this case wordHeader is just empty)

	
	ofstreams[WORDS] << idWord << '\t' << title << '\t' << homograph;
	ofstreams[WORDS] << '\t' << wordType << '\t' << pat << '\t' << wordStress << '\n';
	++idWord;
	
}



void wiktcsv::ProcessorRu::appendInflection(int& tpe, u8charser s, std::string& dst) noexcept
{
	s.seek_span('|', false, s);
	if (tpe == kTypeNoun)
	{
		bool compound = false;
		if (s.contains('+')) 
		{ 
			s.seek_span(',', false, s);
			compound = true;
		}
		if (s.skip_if(u8"м ")) { dst += "m ina "; }
		else if (s.skip_if(u8"м,"))	{ dst += "m ina,"; }
		else if (s.skip_if(u8"мо ")) { dst += "m a "; }
		else if (s.skip_if(u8"мо,")) { dst += "m a,"; }
		else if (s.skip_if(u8"ж ")) { dst += "f ina "; }
		else if (s.skip_if(u8"ж,")) { dst += "f ina,"; }
		else if (s.skip_if(u8"жо ")) { dst += "f a "; }
		else if (s.skip_if(u8"жо, ")) { dst += "f a,"; }
		else if (s.skip_if(u8"с ")) { dst += "n ina "; }
		else if (s.skip_if(u8"с,")) { dst += "n ina,"; }
		else if (s.skip_if(u8"со ")) { dst += "n a"; }
		else if (s.skip_if(u8"со,")) { dst += "n a,"; }
		else if (s.skip_if(u8"мо-жо")) { dst += "mf "; }

	}
	else if (tpe == kTypeAdj)
	{
		if (s == u8"сравн")
		{
			tpe = kTypeForm;
			return;
		}
	}

	dst.append(s);
	// replace -- with -
}



// {{по-слогам|}}
 int wiktcsv::ProcessorRu::getStressedSyllable(u8charser s) noexcept
{
	s.trim(' ');
	
	if (s.seek(u8"{{по", true))
	{
		if (!s.skip_if({ U'-',U' ' })) return 0;
		if (!s.seek(u8"слогам|", true)) return 0;
	}
	else if (!s.seek(u8"{{слоги|", true)) return 0;

	 any_of<UChar> pred({U'а',U'е',U'и',U'о',U'у',U'ы',U'э',U'ю',U'я'});
	int i = 0;
	while(UChar c = s.getc())
	{
		if (c == U'}')
		{
			break;
		}

		if (pred(c) || pred(c - 0x20))
		{
			++i;
		}
 		else if (c == 0x301) // acute
		{
			return i;
		}
		else if (c == U'ё' || c == U'Ё')
		{
			return i + 1;
		}	
	}

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
				if (c == '\r') it.skip('\n'); // set up the pointer for the next line, in skip 

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
					if (line.skip_if("{{"))
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
