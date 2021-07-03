#pragma once
//#include <string>
//#include <algorithm>
#include <functional>
#include <type_traits>
#include <string>
#include <memory>
#include <cctype>

namespace simplified {
using  std::size_t;
template<typename T>
class basic_char_iterator {
  const T* _ptr;
  public:
    using predicate = std::function<bool(T)>;
    ///@{
    /** Basic unary predicates */ 
    static constexpr predicate is_eq(T ch) noexcept 
    { return [&](T c){return c == ch;};}
    static constexpr predicate is_lt(T ch) noexcept 
    { return [&](T c){return c < ch;};}
    static constexpr predicate is_gt(T ch) noexcept 
    { return [&](T c){return c > ch;};}   
    static constexpr predicate is_lt_eq(T ch) noexcept 
    { return [&](T c){return c <= ch;};} 
    static constexpr predicate is_gt_eq(T ch) noexcept
    { return [&](T c){return c >= ch;};}  
    static constexpr predicate is_space() noexcept 
    { return [](T c){return std::isspace(c);};}
    static constexpr predicate is_punct() noexcept 
    { return [](T c){return std::ispunct(c);};}
    static constexpr predicate is_blank() noexcept
    { return [](T c){return std::isblank(c);};}
    static constexpr predicate is_alnum() noexcept
    { return [](T c){return std::isalnum(c);};}
    static constexpr predicate is_print() noexcept 
    { return [](T c){return std::isprint(c);};}
    static constexpr predicate is_graph() noexcept
    { return [](T c){return std::isgraph(c);};}
    static constexpr predicate is_cntrl() noexcept 
    { return [](T c){return std::iscntrl(c);};}
    static constexpr predicate is_digit() noexcept 
    { return [](T c){return std::isdigit(c);};}

    static constexpr predicate is_not(predicate p) noexcept 
    { return [&](T c){return !p(c);};}
    static constexpr predicate is_of(predicate p1, predicate p2) noexcept 
    { return [&](T c){return p1(c) || p2(c);};}
    
    ///@}


    constexpr basic_char_iterator(const T* p = 0) noexcept: 
        _ptr(p) {};
    constexpr basic_char_iterator(std::nullptr_t)  noexcept: 
        _ptr(0) {};
    constexpr basic_char_iterator<T>& operator=(const T* p) noexcept
    {_ptr = p;   return *this; }
    constexpr basic_char_iterator<T>& operator=(std::nullptr_t) noexcept
    {_ptr = 0;   return *this; }

    ///@{
    /** Get pointer */ 

    constexpr const T* get() const noexcept { return _ptr; }
    constexpr explicit operator const T*() const noexcept { return get();}
    constexpr operator bool() const noexcept {return _ptr;}
    constexpr bool operator<(const T* p) const noexcept {return get() < p;}
    constexpr bool operator>(const T* p) const noexcept {return get() > p;}
    constexpr bool operator<=(const T* p) const noexcept {return get() <= p;}
    constexpr bool operator>=(const T* p) const noexcept {return get() >= p;}
    constexpr bool operator==(const T* p) const noexcept {return get() == p;}
    constexpr bool operator!=(const T* p) const noexcept {return get() != p;}

     constexpr friend bool operator<(T* p, const basic_char_iterator& it) noexcept
        {return it.operator<(p);}
    constexpr friend bool operator>(T* p, const basic_char_iterator& it) noexcept
        {return it.operator>(p);}
    constexpr friend bool operator<=(T* p, const basic_char_iterator& it) noexcept
        {return it.operator<=(p);}
    constexpr friend bool operator>=(T* p, const basic_char_iterator& it) noexcept
        {return it.operator>=(p);}
    constexpr friend bool operator==(T* p, const basic_char_iterator& it) noexcept
        {return it.operator==(p);}
    constexpr friend bool operator!=(T* p, const basic_char_iterator& it) noexcept
        {return it.operator!=(p);}

    constexpr bool operator<(const basic_char_iterator& it) const noexcept
        {return get() < it.get();}
    constexpr bool operator>(const basic_char_iterator& it) const noexcept
        {return get() > it.get();}
    constexpr bool operator<=(const basic_char_iterator& it) const noexcept
        {return get() <= it.get();}
    constexpr bool operator>=(const basic_char_iterator& it) const noexcept
        {return get() >= it.get();}
    constexpr bool operator==(const basic_char_iterator& it) const noexcept
        {return get() == it.get();}
    constexpr bool operator!=(const basic_char_iterator& it) const noexcept
        {return get() != it.get();}


    ///@}

    ///@{
    /** Get value */ 

    constexpr T operator*()  noexcept {return *get();}

    T* move(T* dst, size_t n) const noexcept
    {
        return std::char_traits<T>::move(dst, get(), n); 
    }

    T* copy(T* dst, size_t n) const  noexcept
    {
        return std::char_traits<T>::copy(dst, get(), n); 
    }

    T* c_str(T* dst, size_t n) const noexcept
    {   const T* src = get(); T c;
        for( ptrdiff_t i = n; ; ) 
        {c = *src++; if(--i <= 0) break; *dst++ = c;} 
        *dst = 0;
        return dst;
    }

    ///@}


    ///@{
    /** Advance and seek */

    constexpr basic_char_iterator& operator++() noexcept {++_ptr; return *this; }
    constexpr basic_char_iterator& operator--() noexcept {--_ptr; return *this; }
    constexpr basic_char_iterator operator++(int) noexcept 
    { return basic_char_iterator(_ptr++);}
    constexpr basic_char_iterator operator--(int) noexcept 
    { return basic_char_iterator(_ptr--);}
    constexpr basic_char_iterator& operator+=(std::size_t n) noexcept
    {_ptr += n; return *this;}
    constexpr basic_char_iterator& operator-=(std::size_t n) noexcept
    {_ptr -= n; return *this;}
    constexpr friend basic_char_iterator operator+(std::size_t d, basic_char_iterator it) noexcept
    {return it.operator+(d);}
    constexpr friend basic_char_iterator operator-(std::size_t d, basic_char_iterator it) noexcept
    {return it.operator-(d);}


    template<typename Predicate>
    constexpr std::size_t seek_if(Predicate q, const T* end) noexcept
    {
        auto p = _ptr;
        ptrdiff_t d =  end - p;
        if(d <= 0) return false;
        while (!q(*p++) && --d) {} 
        _ptr = p;
        return (std::size_t) d;
    }

    template<typename Predicate>
    constexpr std::size_t rseek_if(Predicate q, const T* last) noexcept
    {
        auto p = _ptr;
        ptrdiff_t d =  p - last;
        if(d <= 0) return false;
        while (!q(*--p) && --d) {} 
        _ptr = p;
        return (std::size_t) d;
    }

    constexpr std::size_t seek(T ch, const T* end) noexcept 
    { 
        return seek_if(is_eq(ch), end);
    }


    constexpr std::size_t rseek(T ch, const T* end) noexcept 
    { 
        return rseek_if(eq(ch), end);
    }


    constexpr std::size_t seek_of(const T* cstr, const T* end) noexcept
    {

        auto p = _ptr;  ptrdiff_t d =  end - p;
        if(d <= 0) return false;
        for(auto s = cstr; ;)
        {
            auto c = *s++;
            if (c != 0)
            {
                if (c == *p) break;
                continue;
            }
            ++p; if(--d == 0) break; 
        } 
        _ptr = p;  return (std::size_t)d;
    }

    constexpr std::size_t rseek_of(const T* cstr, const T* end) noexcept
    {
        auto p = _ptr;  ptrdiff_t d =  p - end; 
        if(d <= 0) return false;  --p;
        for(auto s = cstr; ;)
        {
            auto c = *s++;
            if (c != 0)
            {
                if (c == *p) break;
                continue;
            }
            --p; if(--d != 0) break; 
        }
        _ptr = p;    return (std::size_t)d;
    }

    template<typename U>
    constexpr std::size_t seek_append_if(U q, std::basic_string<T>& s, const T* end) noexcept
    {
        auto d = seek_if(q, end);
        s.append(end - d, end - d - get()); return d;
    }
    constexpr std::size_t seek_append(T c, std::basic_string<T>& s, const T* end) noexcept
    {
        return seek_append_if(eq(c), end);
    }

    constexpr std::size_t seek_append_of(const T* cstr, std::basic_string<T>& s, const T* end) noexcept
    {
        auto d = seek_of(cstr, end);
        s.append(end - d, end - d - get()); return d;
    }
    template<typename U>
    constexpr std::size_t rseek_append_if(U q, std::basic_string<T>& s, const T* end) noexcept
    {
        auto d = rseek_if(q, end);
        s.append(get() + d, d); return d;
    }
    constexpr std::size_t rseek_append(T c, std::basic_string<T>& s, const T* end) noexcept
    {
        return rseek_append_if(eq(c), end);
    }

    constexpr std::size_t rseek_append_of(const T* cstr, std::basic_string<T>& s, const T* end) noexcept
    {
        auto d = rseek_of(cstr, end);
        s.append(get() + d, d); return d;
    }


};

using char_iterator  = basic_char_iterator<char>;
using u16char_iterator = basic_char_iterator<char16_t>;
using u32char_iterator = basic_char_iterator<char32_t>;
//using wchar_iterator   = basic_char_iterator<wchar_t>;


  

/** Non-resizable heap-allocated string.*/
 
/**Acts similar to unique_ptr..*/


template<typename T>
class basic_cstring: public std::unique_ptr<T[]> {
  public:
  using std::unique_ptr<T[]>::get;


  constexpr basic_cstring(const T* s, size_t n) noexcept:
    std::unique_ptr<T[]> (new T[n + 1])
  {
    auto d = get();
    for ( ;n; ++d, ++s, --n) {*d = *s;} 
    *d = 0;
  };

  constexpr explicit basic_cstring(const T* s) noexcept: 
    basic_cstring(s, char_traits<T>::length(s)){
  }
   

  template<typename Iterator> 
  constexpr basic_cstring(const Iterator& first, const Iterator& last) noexcept: 
    std::unique_ptr<T[]>(new T[last - first + 1])
  {
    std::copy<Iterator, T*>(first, last, get());
    get()[last - first] = 0;
  };

  template<typename Container> 
  constexpr basic_cstring(const Container & ctr) noexcept: 
    basic_cstring(ctr.begin(),ctr.end())
  { 
  };

  constexpr basic_cstring<T>& operator=(const T* s) noexcept
  {
    return operator=(std::move(basic_cstring<T> {s}));
  };

  template<typename Container> 
  constexpr basic_cstring<T>& operator=(const Container & ctr) noexcept
  {
    return operator=(std::move(basic_cstring<T> {ctr}));
  };

  ///@{
  /** Compare strings */ 

  constexpr bool operator==(const T* s) const noexcept
  {   
    for(auto p = get(); *p == *s && *s; ++p, ++s){}
    return !*s;
  }

  constexpr bool operator!=(const T* s) const noexcept {return !operator==(s);}
  friend constexpr bool operator==(const T* x, const basic_cstring& y) noexcept {return y.operator==(x);}
  friend constexpr bool operator!=(const T* x, const basic_cstring& y) noexcept {return y.operator!=(x);}

  constexpr bool operator==(const basic_cstring& s) const noexcept {return operator==(s.get());}
  constexpr bool operator!=(const basic_cstring& s) const noexcept {return operator!=(s.get());}

  size_t size() const noexcept {return char_traits<T>::length(get());}

  ///@}
};

using cstring    = basic_cstring<char>;
using u16cstring = basic_cstring<char16_t>;
using u32cstring = basic_cstring<char32_t>;
using wcstring   = basic_cstring<wchar_t>;

}; // end namespace simplified











  
	