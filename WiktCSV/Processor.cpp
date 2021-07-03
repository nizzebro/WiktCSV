
#include "pch.h"
#include "Processor.h"

#define _CRT_SECURE_NO_WARNINGS


using namespace std;
using namespace wiktcsv;




//bool Processor::skip(int n)
//{
//  do {
//    n -= currentTextLength();
//    if(n > 0) {
//      cpos = endPos;
//      return true;
//    }
//  } while(readToBuffer());
//  return false;
//}
//
//
////  PER-FILE SEARCH SKIPPING DATA; advances endPos = cpos
//
//bool Processor::skipTo(const char* s, int n) noexcept {
//  do {
//    if(moveTo(s, n)) {
//      cpos = endPos;
//      return true;
//    }
//  } while(readToBuffer());
//  return false;
//}
//
//
//template<int N>
//bool Processor::skipTo(const char (&s)[N]) noexcept {
//  Processor::skipTo((const char*)s, N-1);
//}
//
//bool Processor::skipTo (char c) noexcept {
//  do {
//      if(moveTo(c)) {
//        cpos = endPos;
//        return true;
//      }
//    } while(readToBuffer());
//  return false;
//}
//
//bool Processor::skipToNonBlank() noexcept {
//   do {
//      if(moveToNonBlank()) {
//        cpos = endPos;
//        return true;
//      }
//    } while(readToBuffer());
//  return false;
//}
//
//bool Processor::skipToBlank() noexcept {
//  do {
//      if(moveToBlank()) {
//        cpos = endPos;
//        return true;
//      }
//    } while(readToBuffer());
//  return false;
//}
////
////
//
////  WRITE; doesn't affect pointers
//
//void Processor::write(size_t iFile, const char * s, int n) noexcept
//{
//  writeToBuffer(iFile, s, n);
//}
//
//void Processor::write(size_t iFile, const char * s) noexcept
//{
//  writeToBuffer(iFile, s, strlen(s));
//}
//
//
//template <int N>
//void Processor::write(size_t iFile, const char (&s)[N]) noexcept {
//  writeToBuffer(iFile, (const char*)s, N-1);
//}
//
//
////  PER-FILE SEARCH WRITING DATA; PPOS = CPOS 
//
//bool Processor::writeTo(size_t iFile, const char* s, int n) noexcept {
//  do {
//    bool b = moveTo(s, n);
//    Processor::write(iFile, cpos, (int) (endPos - cpos));
//    if(b) {
//      cpos = endPos;
//      return true;
//    }
//  } while(readToBuffer());
//  return false;
//}
//
//
//template <int N>
//bool Processor::writeTo(size_t iFile, const char (&s)[N]) noexcept {
//  Processor::writeTo(iFile, (const char*)s, N-1);
//}

//bool Processor::writeTo (size_t iFile, char c) noexcept {
//  do {
//      bool b = moveTo(c);
//      Processor::write(iFile, cpos, (int)(endPos - cpos));
//      if(b)  {
//        cpos = endPos;
//        return true;
//      }
//    } while(readToBuffer());
//  return false;
//}
//
//bool Processor::writeToNonBlank(size_t iFile) noexcept {
//  do {
//      bool b = moveToNonBlank();
//      Processor::write(iFile, cpos, (int)(endPos - cpos));
//      if(b)  {
//        cpos = endPos;
//        return true;
//      }
//    } while(readToBuffer());
//  return false;
//}
//
//bool Processor::writeToBlank(size_t iFile) noexcept {
//  do {
//      bool b = moveToBlank();
//      Processor::write(iFile, cpos, (int)(endPos - cpos));
//      if(b)  {
//        cpos = endPos;
//        return true;
//      }
//    } while(readToBuffer());
//  return false;
//}



//void Processor::openFiles(const char* path, int nOutputFiles, const char* const(&outputFileNames)[]) noexcept
//{
//  input = fopen( path, "rb" );
//  if(setvbuf(input,  nullptr, _IONBF, 0 ) == 0) 
//  {
//    string s (path);
//    int n = s.find_first_of("/\\") + 1; // if npos then 0
//    auto  p = outputFileNames;
//    outputs.reserve(nOutputFiles);
//    while(outputs.size() != nOutputFiles) 
//    {
//      s.replace(n,string::npos,*p, strlen(*p));
//      ++p;
//      FILE* f = fopen(&s[0], "wb");
//      if(setvbuf(f,  nullptr, _IONBF, 0 ) == 0) {
//        char* p = new char[write_buffer_size];
//        outputs.push_back({f, p, p, p + write_buffer_size});
//        continue;
//      }
//      err = true; break;
//    } 
//    return;
//  }
//  err = true;
//}

//void Processor::writeToBuffer(size_t iFile, const char * s, int n) noexcept
//{
//  assert((n > 0) && (iFile < outputs._size()));
//  Output o = outputs[iFile];
//  do {
//    const char* end = min(o.bufferEnd, o.cpos + n);
//    while(o.cpos != end) {*o.cpos = *s;  ++o.cpos; ++s; --n;}
//    if(end <= o.bufferEnd) return;
//    o.cpos = o.buffer;
//  }
//  while(_fwrite_nolock((void*) o.buffer, 1, write_buffer_size, o.file) == write_buffer_size);
//  err = true;
//}
//
//
//void Writer::openFiles(const char* path, int nOutputFiles, const char* const(&outputFileNames)[]) noexcept
//{
//  input = fopen( path, "rb" );
//  if(setvbuf(input,  nullptr, _IONBF, 0 ) == 0) 
//  {
//    string s (path);
//    int n = s.find_first_of("/\\") + 1; // if npos then 0
//    auto  p = outputFileNames;
//    outputs.reserve(nOutputFiles);
//    while(outputs._size() != nOutputFiles) 
//    {
//      s.replace(n,string::npos,*p, strlen(*p));
//      ++p;
//      FILE* f = fopen(&s[0], "wb");
//      if(setvbuf(f,  nullptr, _IONBF, 0 ) == 0) {
//        char* p = new char[write_buffer_size];
//        outputs.push_back({f, p, p, p + write_buffer_size});
//        continue;
//      }
//      err = true; break;
//    } 
//    return;
//  }
//  err = true;
//}
//void Processor::closeFiles() noexcept
//{
//  for (auto &o : outputs)
//  {
//    if(o.file && (fclose(o.file) !=0)) err = true; 
//    delete[] o.buffer;
//  }
//  outputs.clear();
//  if(input && (fclose(input) !=0)) err = true; 
//  input = nullptr; 
//}

//bool Processor::append(std::string& s, size_t n) noexcept
//{
//    do {
//        size_t nMin = std::min((size_t)(bufferEnd - cpos), n);
//        s.append(cpos.get(), nMin);
//        n -= nMin; if(n == 0) break;
//    } while(read());
//
//    return false;
//}



//bool Processor::get(std::string& s, size_t n) noexcept
//{
//    s.clear(); 
//    return append(s, n);
//}



//
//char Processor::getChar() noexcept
//{
//    if(cpos != bufferEnd || read())
//    {
//        return *cpos++;
//    }
//    return 0;
//}
//
//char Processor::getChar(std::string & s) noexcept {
//
//    if(cpos != bufferEnd || read())
//    {
//        auto c = *cpos++;
//        s = c;
//        return c;
//    }
//    return 0;
//}

 
  

// reads a chunk from file to buffer, setting position pointer 
// to the beginning of the buffer, returns n bytes read; 0 = eof

size_t Processor::read() noexcept
{
  size_t nRead = _fread_nolock(buffer, 1, bufferEnd - buffer, input);
  nReadTotal += nRead; 
  if(nRead != (bufferEnd - buffer)) 
  {
      bufferEnd = buffer + nRead;
      if(ferror(input)) exitCode |= kErrRead;
      
  }
  cpos = buffer;
  return nRead;
}

// searches for char satisfying a condition;
// increments position pointer, stops on the char found or eof 
// returns the char found or 0: eof 
char Processor::parseSeekChar(char_iterator::predicate pred) noexcept 
{
  do { if(cpos.seek_if(pred, bufferEnd)) return *cpos; }  
  while(read());
  return 0;
}


// sets the position pointer to the next after "]]>"
// returns false on eof

bool Processor::parseSeekEndOfCData() noexcept
{
    while(parseSeekChar(char_iterator::is_eq(']')))
    {
        ++cpos; 
        if (cpos == bufferEnd) {if(!read()) break;}
        while(*cpos == ']') // skip potential "]]]]]...."
        {
            ++cpos; 
            if(cpos == bufferEnd) {if (!read()) break;}
            if(*cpos == '>') 
            {
                ++cpos; 
                return true;
            }
        }  
    }
    return false;
}




// safely gets current char or 0 on eof; no increment
char Processor::parsePickChar() noexcept
{
    if(cpos != bufferEnd || read())
    {
        return *cpos;
    }
    return 0;
}

// appends one char to tagBuffer and increments position;
// returns the appended char or 0 on eof
char Processor::parseAppendChar() noexcept {

    if(cpos != bufferEnd || read())
    {
        auto c = *cpos++;
        tagBuffer. append(1, c);
        return c;
    }
    return 0;
}

// appends chars to tagBuffer ending with a char that satisfies a condition
// (appends this char either, and increments position)
// returns that last char - or 0 when either eof or size limit is reached

char Processor::parseAppendLineWith(char_iterator::predicate pred, size_t lim) noexcept
{   
    do {
        auto n = std::min((size_t)(bufferEnd - cpos), lim);
        if (cpos.seek_append_if(pred, tagBuffer, cpos.get() + n)) 
        {
            auto c = *cpos++;
            return c;
        }
        lim -= n; if(lim == 0) break;
    } while (read());

    return 0;
}

// appends chars to tagBuffer while equal to a c string 
// returns the number of chars copied
size_t Processor::parseAppendWhile(const char* cstr) noexcept
{   
    assert (*cstr);
    size_t n = 0;
    while (parsePickChar() == *cstr)
    {
        tagBuffer += *cstr;
        ++cstr; ++cpos; ++n;
    }   
    return n;
}

// appends chars to tagBuffer ending with "-->"
// the curr. position should be next after "<--"
// returns false on eof

bool Processor::parseAppendRestOfComment() noexcept 
{   
    while(auto c = parseAppendLineWith(char_iterator::is_eq('-'))) 
    {
        c = parseAppendChar();
        while (c == '-') // skip potential "-----..."
        {
            c = parseAppendChar();
        }
        if (c == '>') return true;   
        if(!c) break;
    } 
    return false;    
}

// appends chars to tagBuffer ending with "?>"
// the curr. position should be next after "<?"
// returns false on eof

bool Processor::parseAppendRestOfPI() noexcept 
{   
    // heap  = "(...<?)"
    // this routine is called either from processTag()->processPI(), to handle a separate PI, 
    // or, from processDecl(), to deal with nested PI's inside a DTD.
    // search for "?>"
    while (auto c = parseAppendLineWith(char_iterator::is_eq('?')))
    {
        c = parseAppendChar(); 
        if (c == '>') return true;
        if(!c) break;
    }
    return false;    
}

// reads tag to tagBuffer, advancing position to the next after it
// the curr. position should be next on "<"
// returns tag code (on error, Element::TagType::kNone = 0)

Processor::Element::TagType Processor::parseTag() noexcept 
{

   tagBuffer = '<'; 
   ++cpos;
   auto c = parseAppendChar();
   if(c == '/') // End-tag: "</" (any chars except '>') '>' 
   {
       if(parseAppendLineWith(char_iterator::is_eq('>')))
           return Element::TagType::kETag;
   }
   else if(c == '?') // PI (Processor Instruction): "<?" (any chars except "?>") "?>" 
   {
       if(parseAppendRestOfPI()) return Element::TagType::kPI;  
       
   }
   else if(c == '!') // comment or DTD or CData: "<!" (any chars, comments or PI's ) '>' 
   {
       // comments are: "<!--" (any chars except "-->") "-->"
       // cData's are: "<[CDATA[" (any chars except "]]>") "]]>"
       // DTD's are: "<!" (not only any chars except '>', but also nested DTD's, PI's and comments) ">"

       // check if it is a comment, CData or DTD

        auto c = parsePickChar(); // lookup to avoid removing next '<' from queque (can be DTD's nested elements)

        if (c == '-') // "<!-" check for a comment
        {
            ++cpos;
            c = parsePickChar(); // lookup again
            if (c == '-') // "<!--" yup, comment
            {
                ++cpos;
                if(parseAppendRestOfComment()) 
                    return Element::TagType::kComment;
            }
        } 
        else if(c == '[' && xml.level()) // CDATA should not appear outside an element
        {
            ++cpos;
            if(parseAppendWhile("CDATA[") == sizeof("CDATA["))
                return  Element::TagType::kCData; 
        }
        else
        {
            int iNested = 1;        // count matching '< >'
            while (c = parseAppendLineWith (char_iterator::is_of(char_iterator::is_eq('<'),
                char_iterator::is_eq('>'))))
            {
                if (c == '<')  // "...<"
                {   
                    c = parseAppendChar(); 

                    if(c == '!') // "...<!"
                    {
                        auto c = parsePickChar();
                        if(c == '-') // "...<!-" nested comment?
                        {
                            ++cpos;
                            c = parsePickChar(); 
                            if (c == '-') // "...<!--" yup, nested comment
                            {
                                ++cpos;
                                if(!parseAppendRestOfComment()) break;   
                            }
                        } 
                    }

                    else if(c == '?') // "...<?"
                    {
                        if(!parseAppendRestOfPI()) break;
                    }

                    else if(!c) break;

                    else ++iNested;          
                }
                else // '...>'
                {  
                    if((--iNested) == 0) return Element::TagType::kDTD;

                } // end if '<' or '>'

            } // end while is_of('<', '>')
        } // end else (dispatch the char next after "<!")
   } 
   else if(c)
   {
        if(parseAppendLineWith(char_iterator::is_eq('>')))
        {
            return tagBuffer[tagBuffer.size()-1] == '/'? 
                Element::TagType::kSCTag : Element::TagType::kSTag;
        }
   }

    return  Element::TagType::kNone;   

}


// calls contents handler; checks if the next tag was parsed from within handler,
// if not - skips to the next tag and parses it; returns the parsed tag type.

Processor::Element::TagType Processor::processContent() noexcept 
{
    xml.contentAvailable = true;
    onContent(xml);

    // test if next tag is already parsed by getters 

    if(!xml.contentAvailable) return xml.nextTagType;

    // not parsed: skip to the next tag and parse it

    xml.contentAvailable = false; // reset; this prevents data access from callbacks

    if(xml.insideCData)
    {
        if(!parseSeekEndOfCData()) return Element::TagType::kNone;
        xml.insideCData = false;
    }

    if (!parseSeekChar(char_iterator::is_eq('<'))) return Element::TagType::kNone;


    // parse tag and skip all possible cdata's to prevent recursion in processTag()

    Element::TagType t;

    while((t = parseTag()) == Element::TagType::kCData) 
    {
        if(!parseSeekEndOfCData()) return Element::TagType::kNone;
        if (!parseSeekChar(char_iterator::is_eq('<'))) return Element::TagType::kNone;
    }

    return  t;

}

bool Processor::processTag(Processor::Element::TagType t) noexcept 
{
    switch (t)
    {
        case Element::TagType::kETag:
            xml.endTag = std::move(tagBuffer);
            onSuffix(xml);
            xml.path.pop_back();
            return true;
        case Element::TagType::kSTag: 
            return processNextElement();
        case Element::TagType::kSCTag: 
            processSelfClosingElement(); 
            return true; 
        case Element::TagType::kCData:
            xml.insideCData = true;
            return processTag(processContent()); 
        case Element::TagType::kPI: 
            onPI(tagBuffer, xml.path); 
            return true; 
        case Element::TagType::kComment: 
            onComment(tagBuffer, xml.path); 
            return true; 
        case Element::TagType::kDTD: // shuld not appear here but anyway
            onDTD(tagBuffer);
            return true; 
    }

    return false;
}


bool  Processor::processNextElement() noexcept 
{
    xml.path.push_back(STag(std::move(tagBuffer)));
    onPrefix(xml, false);

    while(auto c = parseSeekChar(char_iterator::is_gt(' ')))
    {
        return processTag((c == '<'? parseTag() : processContent()));
    } 

    return false;
}


void Processor::processSelfClosingElement() noexcept
{
    xml.path.push_back(STag(std::move(tagBuffer)));
    onPrefix(xml, true);
    xml.path.pop_back();
}

bool Processor::processNextTopLevelEnitity() noexcept
{
    auto c =parseSeekChar(char_iterator::is_eq('<'));

    if(c) 
    {
        switch (Element::TagType t = parseTag()) 
        {    
            case Element::TagType::kDTD:
                onDTD(tagBuffer); return true;   
            case Element::TagType::kPI: 
                onPI(tagBuffer, xml.path);  return true;  
            case Element::TagType::kSTag: 
                return processNextElement();
            case Element::TagType::kSCTag: 
                processSelfClosingElement(); return true;  
            case Element::TagType::kComment: 
                onComment(tagBuffer, xml.path);  return true;  
        }
        exitCode |= kErrData;
    }

    return false;
}


int Processor::process(const char* path, size_t bufferSize) noexcept
{
    nReadTotal = 0;
    input = fopen(path, "rb");
    exitCode = setvbuf(input,  nullptr, _IONBF, 0 ) == 0 ? kOk : kErrOpen;
    if(exitCode) 
    {
        bufferSize = (bufferSize + (buffer_gran - 1)) & (~(buffer_gran - 1));
        buffer = new char [bufferSize];
        bufferEnd = buffer + bufferSize;
        cpos = buffer;
        while(processNextTopLevelEnitity()) {}      
        delete [] buffer;
        xml = {};
    }
    if(input && (fclose(input) != 0)) { exitCode |= kErrClose; }
    return exitCode;
}


std::string_view Processor::STag::getName() const noexcept
{
    const char* p = data() + 1;
    char_iterator it(p);
    it.seek(' ', data() + size());
    return std::string_view(p, it.get() - p);
}

std::vector<Processor::Attribute> Processor::STag::getAttributes() const noexcept
{
    const char* p = data() + 1;
    char_iterator it(p);
    std::vector<Attribute> v;
    const char* end = data() + size();
    if(it.seek(' ', end)) 
    {
        ++it; 
        for(;;)
        {
            p = it.get();
            if(!it.seek('=', end)) break;
            std::string_view name(p, it.get() - p); 
            ++it; if(it == end) break;
            if(*it != '"') break;
            ++it; 
            if(!it.seek('"', end)) break;
            std::string_view value(p, it.get() - p); 
            ++it; 
            v.push_back(Attribute{name, value});
        }
    }
    return std::move(v);
}

const std::string & Processor::Element::getEndTagString() noexcept
{
    if(endTag.empty())
    {
       endTag += '<';
       endTag += '/';
       endTag.append(getName());
       endTag  += '>';
    }
    return endTag;
}

char wiktcsv::Processor::getChar() noexcept
{
    if(xml.contentAvailable)
    {
        
        if(!xml.insideCData)
        {
            auto c = parsePickChar();

            if(c != '<') 
            {
                ++cpos;
                return c;
            }
            auto t = parseTag();
            if(t == Element::TagType::kCData)
            {
                xml.insideCData = true;
                return getChar(); // try once again
            }
                
            xml.contentAvailable = false; // end of block
            xml.nextTagType = t;    // save 

            return 0;
        }
        else
        {
            if(xml.cDataBracket)
            {
                xml.cDataBracket = false;
                return ']';
            }

            auto c = parsePickChar();
            ++cpos;
            
            if(c != ']') return c;
            
            c = parsePickChar(); // next after "]"
            
            if(c != ']') return ']'; 

            ++cpos;  
            c = parsePickChar(); // next after "]]"
            if(c != '>') 
            {
                xml.cDataBracket = true;
                return ']';
            }
            ++cpos;   // to next after "]]>"
           
            
            auto t = parseTag();
            if(t != Element::TagType::kCData)
            {
                xml.insideCData = false;
                xml.contentAvailable = false; // end of block
                xml.nextTagType = t;    // save 
                return 0;          
            }
                
            return getChar(); // one more cdata: try once again  
        }
    }
    return 0;
}

char wiktcsv::Processor::getChar(std::string & s) noexcept
{
    auto c = getChar();
    if(c) s = c;
    return c;
}

char wiktcsv::Processor::appendChar(std::string & s) noexcept
{
    auto c = getChar();
    if(c) s += c;
    return c;
}

bool wiktcsv::Processor::getChars(std::string & s, size_t n) noexcept
{
    s.clear();
    return appendChars(s, n);
}

bool wiktcsv::Processor::appendChars(std::string & s, size_t n) noexcept
{
    char c;
    while(n && (c = getChar())) {s += c;  --n;}
    return !n;
}

bool wiktcsv::Processor::getLine(std::string & s, char delim) noexcept
{
    s.clear();
    return appendLine(s, delim);
}

bool wiktcsv::Processor::appendLine(std::string & s, char delim) noexcept
{
    while(auto c = getChar())
    {
        if (c != delim)
        {
             s += c;
             continue;
        }
       return true;
    };
    return false;
}
