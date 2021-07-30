#pragma once


#include "processor.h"

namespace wiktcsv {


// Output data format.
// The main goal is to get morphology and syntax.
// Additionally, to extract etimology (except PIE bullshit), phrases and sayings -
// even though ther is a lot of garbage there, word meaning definitions and maybe
// english translations.
// Template links are to be written as is and processed further in table fields.

// Every other lang than "{{-ru-}}" will be thrown away.


// Field delimiters - \t; Record delimiters = \n; in text, these should be replaced
// by spaces.

// Tables:

// WORDS: homographs
// IdWord(counter), IdPage(from xml), Title(from xml), Header; 
// where Header is level 1 header string, e.g. "{{заголовок|I}}" -
// or an empty field if there are no variations;

// SYNTAX, ETIMOLOGY, PHRASES, SAYINGS, MEANINGS, CATEGORY: 
// IdWord, Text
// lines are lines only if there are formatting or entity that starts the line.

// Special pages from namespaces other than 0 (templates, categories etc) will be
// written as is to rem.xml.



class ProcessorRu: XmlParser, XmlParser::FileWriter
{
	enum {
		WORDS, SYNTAX, ETIMOLOGY, PHRASES, SAYINGS, MEANINGS, CATEGORY, REM
	};

	static constexpr std::initializer_list<const char*> fileNames =
	{ "words.csv","syntax.csv","etimology.csv","phrases.csv","sayings.csv", "meanings.csv",
		"category.csv","rem.xml" };

	inline void processText(std::size_t& idWord, std::string_view idPage, std::string_view title) noexcept;

	XmlParser::FileWriter& ofstreams = static_cast<XmlParser::FileWriter&>(*this);

    public:

    bool process(const char* dir, const char* fileName) noexcept;

};

// this class is for first-stage primary analysis of header and syntax-line structure;
// since wiki data is poorly organized and no way to know in advance which headers and templates
// might be are used in addition to common ones.
// It's code does two things:
// 1) extracts all unique sub-header names into a file;
// 2) extracts all unique template tags found within level1 and level2 headers

class HeaderAnalyserRu : XmlParser, XmlParser::FileWriter
{
	static constexpr std::initializer_list<const char*> fileNames =
	{ "unique_headers.csv","unique_upper_level_tags.csv" };
	XmlParser::FileWriter& ofstreams = static_cast<XmlParser::FileWriter&>(*this);
public:
	bool process(const char* dir, const char* fileName) ;
	
};








};

