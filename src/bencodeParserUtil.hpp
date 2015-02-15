// bencdeParser.hpp                                                    -*-C++-*-
#ifndef INCLUDED_BENCODE_PARSER
#define INCLUDED_BENCODE_PARSER

//@PURPOSE: Provide a parser for bencode encoded data
//
//@CLASSES:
//  BencodeParserUtill: parser to convert bencode data into 'BencodeToken'  
//  BencodeToken: abstract base type for all bencode tokens
//  IntegerToken: type for integear values
//  ByteStringToken: type for all byte strings
//  BencodeList: a list of bencode tokens
//  BencodeDict: a dictionary with string keys and bencode token values

//@DESCRIPTION: This component defines a BencodeParser and all types for each
// unique token supported in Bencode.  For more information about bencode see
// the following description:
//..
// <https://en.wikipedia.org/wiki/Bencode>
//..

#include <string>
#include <list>
#include <vector>
#include <map>
#include <iosfwd>
#include <exception>
#include <functional>

namespace torrent {

class BencodeToken;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                            BencodeParserUtil     
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
class BencodeParserUtil {
  // A utility class for parsing bencode encoded data
  public:
    class ParseError : std::logic_error {
      // An exception to be throw in the case of parse error in bencoded values
      public:
        ParseError(const std::string &what);
          // Create a new 'ParseError' with the specified 'what' string.
    };
    
    BencodeParserUtil() = delete;
    // No instances of this class should be built. 
    
    static BencodeToken& parseStream(std::istream& in);
    // Parse the data in the specified 'in' strean and return a reference to
    // the first token. In the case of illformed data a 'ParseError' is thrown.
    // Note that only the first token is parsed, and 'in' may contain more than
    // more token. 
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                            Tokens   
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class BencodeToken {
  // Abstract base class for all bencode tokens.
  public:
    // CREATORS
    BencodeToken();
    // No one should construct a 'BencodeToken' instance.
    
    virtual ~BencodeToken() = 0;
    // Delete this object
};

class IntegerToken : public BencodeToken {
  public:
    IntegerToken();

    explicit IntegerToken(std::istream& in);

    int getValue() const;

    void setValue(int value);
  private:
    int m_value;
};
    
class ByteStringToken : public BencodeToken {
  
  // FRIENDS
  friend bool operator==(const ByteStringToken& lhs, const ByteStringToken& rhs);
    
  public:
    ByteStringToken();
    
    explicit ByteStringToken(const char *buffer, size_t length);
    
    template<size_t Size>
    ByteStringToken(const char(& array)[Size]);
    // Create a new object with the value of the specified 'array'. Note that this
    // method is defined implicit for the sake of syntax sugar and does not copy
    // the null-byte.
    
    explicit ByteStringToken(std::string& str);
        
    explicit ByteStringToken(std::istream& in);
    
    const std::vector<char>& getValue() const;
    
    std::string getString() const;
    
    void setValue(const std::vector<char>& value);
    
  private:
    std::vector<char> m_value;
};
    
class BencodeList : public BencodeToken {
  public:
    BencodeList();
        
    ~BencodeList();
        
    explicit BencodeList(std::istream& in);
        
    explicit BencodeList(const std::list<BencodeToken*>& toks);
        
    const std::list<BencodeToken *>& getTokens() const;
        
    void setTokens(const std::list<BencodeToken *>& toks);
  
  private:
    std::list<BencodeToken *> m_tokens;
};
    
class BencodeDict : public BencodeToken {
  public:
    typedef std::function<bool(const ByteStringToken&,
                               const ByteStringToken&)> BencodeDictComparator;
    
    typedef std::map<ByteStringToken, BencodeToken*, BencodeDictComparator>::const_iterator const_iterator;
    typedef std::map<ByteStringToken, BencodeToken*, BencodeDictComparator>::iterator       iterator;
    
    static BencodeDictComparator keyComparator;
    
    BencodeDict();
        
    ~BencodeDict();
            
    explicit BencodeDict(std::istream& in);
        
    explicit BencodeDict(const std::map<ByteStringToken, 
                                        BencodeToken *, 
                                        BencodeDictComparator>& dict);
    
    BencodeToken& operator[](const ByteStringToken& key);
    
    const_iterator find(const ByteStringToken& key) const;
    
    iterator find(const ByteStringToken& key);
    
    const std::map<ByteStringToken,
                   BencodeToken*,
                   BencodeDictComparator>& getValues() const;
    
    const_iterator begin() const;
    const_iterator end()   const;
    
    iterator begin();
    iterator end();
    
  private:
    std::map<ByteStringToken,
             BencodeToken*,
             BencodeDictComparator> m_dict;
};

//==============================================================================
//                       INLINE FUNCTION DEFINTIONS
//==============================================================================
inline BencodeToken::BencodeToken()
{
}

inline BencodeToken::~BencodeToken()
{
}

inline BencodeParserUtil::ParseError::ParseError(const std::string& what) 
: logic_error(what)
{
}

inline IntegerToken::IntegerToken()
{
}

inline void IntegerToken::setValue(int value)
{
    m_value = value;
}

inline int IntegerToken::getValue() const
{
    return m_value;
}

inline ByteStringToken::ByteStringToken()
{
}

inline const std::vector<char>& ByteStringToken::getValue() const
{
    return m_value;
}

inline void ByteStringToken::setValue(const std::vector<char> &value)
{
    m_value = value;
}

template<size_t Size>
ByteStringToken::ByteStringToken(const char(& array)[Size])
: m_value(array, array + Size - 1)
{
}

inline BencodeList::BencodeList()
{
}

inline BencodeList::~BencodeList()
{
}

inline BencodeList::BencodeList(const std::list<BencodeToken*>& toks) : m_tokens(toks)
{
}

inline BencodeDict::BencodeDict() :
  m_dict(keyComparator)
{
}

inline BencodeDict::~BencodeDict()
{
}

inline BencodeDict::BencodeDict(const std::map<ByteStringToken,
                                               BencodeToken*,
                                               BencodeDictComparator>& dict)
{
    m_dict = dict;
}

inline const std::list<BencodeToken*>& BencodeList::getTokens() const
{
    return m_tokens;
}

inline void BencodeList::setTokens(const std::list<BencodeToken *> &toks)
{
    m_tokens = toks;
}

inline BencodeToken& BencodeDict::operator[](const ByteStringToken& key)
{
    return *m_dict[key];
}
    
inline BencodeDict::const_iterator BencodeDict::find(const ByteStringToken& key) const
{
    return m_dict.find(key);
}

inline BencodeDict::iterator BencodeDict::find(const ByteStringToken& key)
{
    return m_dict.find(key);
}
    
inline BencodeDict::const_iterator BencodeDict::begin() const
{
    return m_dict.begin();
}
    
inline BencodeDict::const_iterator BencodeDict::end() const
{
    return m_dict.end();
}

inline BencodeDict::iterator BencodeDict::begin()
{
    return m_dict.begin();
}

inline BencodeDict::iterator BencodeDict::end()
{
    return m_dict.end();
}
    
inline
bool operator==(const ByteStringToken& lhs, const ByteStringToken& rhs)
{
    return lhs.m_value == rhs.m_value;
}

} // end of torrent namespace
#endif 