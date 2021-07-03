#pragma once

#include "pch.h"

namespace wiktcsv {

using namespace simplified;

class Processor {

    public:
    /** The result of processing. Bits are combined. */
    enum ExitCode 
    {
        kOk = 0,            /**< no errors */
        kErrOpen = 1,       /**< can't open file */
        kErrRead = 2,       /**< error while reading from file */
        kErrClose = 4,      /**< error while closing file. */
        kErrData = 8        /**< tag unmatch etc. */
    };

    struct  Attribute  {
        std::string_view name; 
        std::string_view value;
    };

    // element tags are kept as is, with the name and attribs extracted when needed
    struct STag: std::string
    { 
        public:
        using std::string::string;
        std::string_view getName() const noexcept;  // extract tag name
        std::vector<Attribute> getAttributes() const noexcept; // extract attributes names / values
    };

    // context of the current element 
    class Element
    { 
        std::string_view getName() const noexcept {return path.back().getName();}  
        std::vector<Attribute> getAttributes() const noexcept {return path.back().getAttributes();};  
        // nesting level; 0 =  doc level, 1 = first element etc
        size_t level() noexcept {return path.size();} 
        const std::vector<STag>& getPath() const noexcept {return path;}
        const std::string& getStartTagString() const noexcept {return path.back();} 
        const std::string& getEndTagString()  noexcept; // see endTag variable

       protected:
        friend class Processor;
        // end-tag string, lazy-initialized by getter in case if the actual end-tag is not reaced yet
        std::string endTag;  
         // stack of tags, the last one is the current one 
        std::vector<STag> path;   
        
        // settings TODO
        bool ignoreCurrentNested = false; // skip sub-elements of the current one 
        bool ignoreNestedComments = false; // skip comments inside any tags
        bool ignoreNestedPIs = false;   // skip PI-s inside any tags
        bool keepCDataTags = false;     // do not erase "<![CDATA"
        bool keepCharReferences = false; // do not replace "&#xxx" with actual values
        // TODO: the same for token references; for now, nothing related is implemented

        // parsed tag type; set by data getters when they reach the next tag and parse it
        enum TagType{ kNone, kSTag, kSCTag, kETag, kCData, kComment, kPI, kDTD};
        TagType nextTagType = TagType::kNone;  

        // set only when passing through contents; prevents access to buffer from other callbacks
        bool contentAvailable = false;         
        // set while passing through cData; to take its terminator into account
        bool insideCData = false;   
        bool cDataBracket = false; //  ']' flag for getChar() in cdata

    };
    


    private:

    static const size_t buffer_gran = 0x10000;  // read buffer alignment
    static const size_t default_buffer_size = buffer_gran * 4; 
    static const size_t max_tag_length = 0x10000;  // tags that are longer produce error

    char* buffer;           
    const char* bufferEnd;
    char_iterator cpos;      // read position
    std::string tagBuffer;     // for tag parsing

    FILE* input;
    size_t exitCode;
    std::size_t nReadTotal;
    Element xml;       

    size_t read() noexcept;
    char parseSeekChar(char_iterator::predicate pred) noexcept;
    bool parseSeekEndOfCData() noexcept;
    char parsePickChar() noexcept;
    char parseAppendChar() noexcept;
    char parseAppendLineWith(char_iterator::predicate pred, size_t lim = max_tag_length) noexcept;
    size_t parseAppendWhile(const char* cstr) noexcept;
    bool parseAppendRestOfPI() noexcept;
    bool parseAppendRestOfComment() noexcept;
    Element::TagType parseTag() noexcept;
    Element::TagType processContent() noexcept;
    bool processTag(Element::TagType t) noexcept;
    bool processNextElement() noexcept;
    void processSelfClosingElement()  noexcept;
    bool processNextTopLevelEnitity() noexcept;


    protected:


    /** Set global options (can be changed during processing */

    /** Skip comments inside content, to prevent extra onContents() calls? (default = false) */
    void setIgnoreNestedComments(bool ignore) {xml.ignoreNestedComments = ignore;}
    /** Skip PI inside content, to prevent extra onContents() calls? (default = false) */
    void setIgnoreNestedPIs(bool ignore) {xml.ignoreNestedPIs = ignore;}
    /** Should "<![CDATA[", "]]>" be kept (default: false, remove them) */
    void setKeepCDataTags(bool keep) {xml.keepCDataTags = keep;}
    /** Should "&#xxx>" be kept (default: false, replace them with actual values) */
    void setKeepCharReferences(bool keep) {xml.keepCharReferences = keep;}

    /** Starts processing, calls virtual handlers in a loop and returns ExitCode. */
    int process(const char* path, std::size_t bufferSize = default_buffer_size) noexcept;

    /** Get the result of processing. */
    int getExitCode() const noexcept {return exitCode;} // after processing

    /**  Handlers */ 

    /**  Called on start-tag or self-closing tag; no contents is available here */ 
    virtual void onPrefix(const Element& elem, bool isSelfClosing)= 0;
    /**  Called on text contents, a number of times if there are interleaved ones */ 
    virtual void onContent(const Element& elem)= 0;
    /**  Called on end-tag  */ 
    virtual void onSuffix(const Element& elem)= 0;
    /**  Called on a <!DTD...> */
    virtual void onDTD(const std::string& text)= 0;
    /**  Called on a <?PI..?> */
    virtual void onPI(const std::string& text, const std::vector<STag>& path) = 0;
    /**  Called on a <!-- comment --> */
    virtual void onComment(const std::string& text, const std::vector<STag>& path) = 0;

    /** Get current number of bytes read from file. */
    size_t getNumBytesRead() const noexcept {return nReadTotal;}

   
    /** Get contents. Return false or '\0' if current block is finished */
   
    char getChar() noexcept;
    char getChar(std::string& s) noexcept;
    char appendChar(std::string& s) noexcept;
    bool getChars(std::string& s, size_t n = SIZE_MAX) noexcept; 
    bool appendChars(std::string& s, size_t n = SIZE_MAX) noexcept;
    bool getLine(std::string& s, char delim = '\n') noexcept; 
    bool appendLine(std::string& s, char delim = '\n') noexcept; 
};

class Writer {
  //void writeToBuffer(size_t iFile , const char * p, int n) noexcept;
  //void write(int iFile) noexcept;
  //void write(int iFile, int pos, int n) noexcept;
  //void write(int iFile, const char * s, int n) noexcept;
  //template<int N>
  //void write(int iFile, const char(&)[N]) noexcept;

    // write

  /*static const int write_buffer_size = 1024UL * 4;*/

  //struct Output {
  //  FILE* file;
  //  char* buffer; 
  //  char* cpos = buffer;    // points at the free space to write into
  //  char* bufferEnd = buffer; // smth needed anyway for alignment; but bufferEnd will reduce calculations
  //};

 
  //std::vector<Output> outputs;

 /* void closeFiles() noexcept;*/

  //void write(size_t iFile, const char * s, int n) noexcept;

  //void write(size_t iFile, const char*) noexcept;
  //template<int N>
  //void write(size_t iFile, const char(&)[N]) noexcept;

  //bool writeTo(size_t iFile, const char*, int) noexcept;
  //template<int N>
  //bool writeTo(size_t iFile, const char(&)[N]) noexcept;

  //bool writeTo(size_t iFile, char) noexcept;

  //bool writeToNonBlank(size_t iFile) noexcept;
  //bool writeToBlank(size_t iFile) noexcept;

};


}; // end namespace









