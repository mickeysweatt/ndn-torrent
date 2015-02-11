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

class IntegearToken : public BencodeToken {
  public:
    IntegearToken();

    explicit IntegearToken(std::istream& in);

    int getValue() const;

    void setValue(int value);
  private:
    int m_value;
};
    
class ByteStringToken : public BencodeToken {
  public:
    ByteStringToken();
        
    explicit ByteStringToken(std::istream& in);
    
    const std::vector<char>& getValue() const;
    
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
  private:
    typedef std::function<bool(const ByteStringToken&, 
                               const ByteStringToken&)> BencodeDictComparator;
    static BencodeDictComparator keyComparator;
 
  public:
    BencodeDict();
        
    ~BencodeDict();
            
    explicit BencodeDict(std::istream& in);
        
    explicit BencodeDict(const std::map<ByteStringToken, 
                                        BencodeToken *, 
                                        BencodeDictComparator>& dict);
    
    BencodeToken& operator[](const ByteStringToken key);
        
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

inline IntegearToken::IntegearToken()
{
}

inline void IntegearToken::setValue(int value)
{
    m_value = value;
}

inline int IntegearToken::getValue() const
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

inline BencodeToken& BencodeDict::operator[](const ByteStringToken key)
{
    return *m_dict[key];
}

}

#endif 