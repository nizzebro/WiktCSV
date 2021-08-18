#pragma once


#include "processor.h"

namespace wiktcsv {

	using namespace char_parsers;
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
// IdWord(counter), title(from xml), homograph, type, inflection, stress
// where homograph is level 1 header string, e.g. "{{заголовок|I}}" -
// or an empty field if there are no variations;
// type = enum Types, bit set
// pat = declension pattern
// stress = vowel stressed

// LINES
// IdWord, Text
// lines related to syntax, etimology, and meanings

// WORDMORPH
// IdWord, Type, Text
// Morphemes of words; this is a preliminary dump, the structure is to be 
// changed further by means of sql


// CATEGORY
// IdWord, Text
// categories from {{Категория}}

//	PHRASES, SAYINGS: 
//	Text
//  dumped from related sections

// WORDTYPENAMES (id, name) names of types

// Special pages from namespaces other than 0 (templates, categories etc) will be
// written as is to rem.xml.





class ProcessorRu: XmlParser, XmlParser::FileWriter
{
	enum {
		WORDS, LINES, PHRASES, SAYINGS, CATEGORY, WORDMORPH, WORDTYPENAMES, REM
	};

	static constexpr std::initializer_list<const char*> fileNames =
	{ "words.csv","lines.csv","phrases.csv","sayings.csv", 
		"category.csv","wordmorph.csv","wordtypenames.csv", "rem.xml" };
	enum
	{
		kTypeUnknown = 0,

		kTypeNoun,
		kTypeVerb,
		kTypeAdj,
		kTypeAdv,
		kTypeIntro,

		kTypePronoun,
		kTypeNumeral,
		kTypePred,
		kTypePossess,

		kTypePart,
		kTypeConj,
		kTypeInterj,
		kTypeSound,
		kTypePrep,

		kTypeRoot,
		kTypePrefix,
		kTypeSuffix,
		kTypeCircumfix,

		kTypePrefixoid,
		kTypeSuffixoid,
		kTypeInterfix,
		kTypeEnding,
		kTypePostfix,

		kTypeForm,
		kTypePhrase,
		kTypeProper,
		kTypeAbbrev
		
	};
	static constexpr std::initializer_list<const char*> wordTypeNames =
	{   
		u8"Существительное",u8"Глагол", u8"Прилагательное",u8"Наречие",u8"Вводное",
		u8"Местоимение",u8"Числительное",u8"Предикатив",u8"Поссессив",
		u8"Частица",u8"Союз",u8"Междометие",u8"Звукоподр",u8"Предлог",
		u8"Корень",u8"Префикс",u8"Суффикс",	u8"Циркумфикс",
		u8"Префиксоид",u8"Суффиксоид", u8"Интерфикс",u8"Окончание",u8"Постфикс",
		u8"Форма", u8"Фраза",u8"Имя собств", u8"Сокращение"
		
		};


	inline void processText(std::size_t& idWord, std::string_view title) noexcept;
	void appendInflection(int& tpe, u8charser s,  std::string & dst) noexcept;
	int getStressedSyllable(u8charser s) noexcept;
	XmlParser::FileWriter& ofstreams = static_cast<XmlParser::FileWriter&>(*this);

    public:

    bool process(const char* dir, const char* fileName) noexcept;

};

// supplementary class used for first-stage primary analysis of article headers 
// and syntax-line structure - to know if there are any non-standard headers 
// which also hold important data, to process them later in the main class.
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

