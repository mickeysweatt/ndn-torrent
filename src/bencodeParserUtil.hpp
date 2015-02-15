// bencdeParser.hpp                                                    -*-C++-*-
#ifndef INCLUDED_BENCODE_PARSER
#define INCLUDED_BENCODE_PARSER

//@PURPOSE: Provide a parser for bencode encoded data
//
//@CLASSES:
//  BencodeParserUtill: parser to convert bencode data into 'BencodeToken'  
//  BencodeToken: abstract base type for all bencode tokens
//  BencodeIntegerToken: type for integear values
//  BencodeByteStringToken: type for all byte strings
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

//                            =================
//                            BencodeParserUtil
//                            =================
    
class BencodeParserUtil {
  // A utility class for parsing bencode encoded data
  public:
    // PUBLIC TYPES
    class ParseError : std::logic_error {
      // An exception to be throw in the case of parse error in bencoded values.
      public:
        ParseError(const std::string &what);
          // Create a new 'ParseError' with the specified 'what' string.
    };
    
    BencodeParserUtil() = delete;
        // No instances of this class should be built.
    
    static BencodeToken& parseStream(std::istream& in);
        // Parse the data in the specified 'in' strean and return a reference 
        // to the first token. In the case of illformed data a 'ParseError' is 
        // thrown.  Note that only the first token is parsed, and 'in' may 
        // contain more than more token. 
};

//                            ======
//                            Tokens   
//                            ======

class BencodeToken {
  // Abstract base class for all bencode tokens.
  public:
    // CREATORS
    BencodeToken();
        // No one should construct a 'BencodeToken' instance.
    
    virtual ~BencodeToken() = 0;
        // Delete this object.
};

class BencodeIntegerToken : public BencodeToken {
    // Unconstrained value-semantic type to represent Bencode integer tokens.

  // DATA
  int m_value;
  
  public:
    // CREATORS
    BencodeIntegerToken();
        // Create an empty object.

    virtual ~BencodeIntegerToken();
        // Destroy this object.

    explicit BencodeIntegerToken(std::istream& in);
        // Create a 'BencodeIntegerToken' from the first token in the specified
        // 'in' stream. The behavior is undefined unless  the begining of 'in'
        // represents a valid 'BencodeIntegerToken'.

    // ACCESSORS
    int getValue() const;
        // Return the value held within this token.
};
    
class BencodeByteStringToken : public BencodeToken {
    // Unconstrainted value-semantic type to represent Bencode Byte string
    // tokens. 
  
  // FRIENDS
  friend bool operator==(const BencodeByteStringToken& lhs,
                         const BencodeByteStringToken& rhs);
  
  friend bool operator!=(const BencodeByteStringToken& lhs,
                         const BencodeByteStringToken& rhs);
  // DATA
  std::vector<char> m_value;

  public:
    // CREATORS
    BencodeByteStringToken();
        // Create an empty object.

    virtual ~BencodeByteStringToken();
        // Destroy this object.
    
    explicit BencodeByteStringToken(const char *buffer, size_t length);
        // Create a object with value of the first specified 'length' bytes of
        // the specified 'buffer'.
    
    template<size_t Size>
    BencodeByteStringToken(const char(& array)[Size]);
        // Create a new object with the value of the specified 'array'.  Note 
        // that this method is declared implicit for the sake of syntax sugar
        // and does not copy the null-byte.
    
    explicit 
    BencodeByteStringToken(std::string& str);
        // Create an object with the value of the specified 'str' string. 
        
    explicit 
    BencodeByteStringToken(std::istream& in);
        // Create an object with the value of the 'ByteStringToken' at the
        // start of the specified 'in' strem.

    // ACCESSORS
    const std::vector<char>& getValue() const;
        // Return an unmodifiable reference to the internal buffer.
    
    std::string getString() const;
        // Return a string with the content of this object. The behavior is
        // undefined unless the contents of this object are representable by
        // a 'std::string'. 
};
    
class BencodeList : public BencodeToken {
    // A value-semantic type for Bencode list tokens.
  
    // DATA
    std::list<BencodeToken *> m_tokens;
  
  public:
    // PUBLIC TYPES
    typedef std::list<BencodeToken *>::const_iterator const_iterator;
    
    // CREATORS
    BencodeList();
        // Create an empty object.

    ~BencodeList();
        // Destory this object.

    explicit 
    BencodeList(std::istream& in);
        // Create a object with the value of the Bencode list token at the 
        // start of the specified 'in' stream. The behavior is undefined unless
        // in begins with a valid Benocde list.

    explicit 
    BencodeList(const std::list<BencodeToken*>& toks);
        // Construct a object with the value of the specified 'toks' list.
        
    // ACCESSORS
    const std::list<BencodeToken *>& getTokens() const;
        // Return an unmodifiable reference to a list containing all the tokens
        // in this object.

    const_iterator begin() const;
        // Return a bi-directional iterator providing unmodifiable accesss to
        // the first element in this list. The iterator is only valid if this
        // is a non-empty list.

    const_iterator end() const;
        // Return a bi-directional iterator referencing one element past the 
        // last element of this list. Note that this iterator cannot be 
        // deferenced.
};
    
class BencodeDict : public BencodeToken {
    // A value-sematantic type to represent dictionary bencdoe tokens. 
  public:
    // PUBLIC TYPES
    typedef std::function<bool(
        const BencodeByteStringToken&,
        const BencodeByteStringToken&)>
    BencodeDictComparator;
    
    typedef std::map<BencodeByteStringToken,
                     BencodeToken*,
                     BencodeDictComparator>::const_iterator
    const_iterator;
    
  private:
    // DATA
    std::map<BencodeByteStringToken,
             BencodeToken*,
             BencodeDictComparator> m_dict;

    // PRIVATE MANIPULATORS
    BencodeToken& operator[](const BencodeByteStringToken& key);

  public:

    // PUBLIC CLASS MEMBERS
    static BencodeDictComparator keyComparator;
    
    // CREATORS
    BencodeDict();
        // Create an empty dictionary.

    ~BencodeDict();
        // Destroy this object.

    explicit 
    BencodeDict(std::istream& in);
        // Create a dictionary with value of the bencode dictionary token that
        // the front of the specified 'in' stream. The behavior is unspecified
        // unless the front of the 'in' stream represents a valid Bencode
        // dictionary.

    explicit 
    BencodeDict(const std::map<BencodeByteStringToken, 
                                BencodeToken *, 
                                BencodeDictComparator>& dict);
        // Create a dictionary with the value of the specified 'dict'.
    
    // ACCESSORS
    const_iterator find(const BencodeByteStringToken& key) const;
        // Return a iterator providing unmodifiable access to the element in
        // this dictionary for the specified 'key'. Return 'end()'' if key not 
        // in dictionary.
    
    const std::map<BencodeByteStringToken,
                   BencodeToken*,
                   BencodeDictComparator>& getValues() const;
        // Return a map with the contents of this dictionary.
    
    const_iterator begin() const;
        // Return a bi-directional iterator providing unmodifiable accesss to
        // the first element in this dictionary. The iterator is only valid if 
        // this is a non-empty dictionary.
    
    const_iterator end() const;
        // Return a bi-directional iterator referencing one element past the 
        // last element of this dictionary. Note that this iterator cannot be 
        // deferenced.
};

//==============================================================================
//                       INLINE FUNCTION DEFINTIONS
//==============================================================================

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                       BencodeParserUtil::ParseError
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
inline 
BencodeParserUtil::ParseError::ParseError(const std::string& what) 
: logic_error(what)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                              BencodeToken
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
inline 
BencodeToken::BencodeToken()
{
}

inline 
BencodeToken::~BencodeToken()
{
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                            BencodeIntegerToken
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline 
BencodeIntegerToken::BencodeIntegerToken()
{
}

inline 
BencodeIntegerToken::~BencodeIntegerToken()
{
}


inline
int BencodeIntegerToken::getValue() const
{
    return m_value;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                            BencodeByteStringToken
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline 
BencodeByteStringToken::BencodeByteStringToken()
{
}

inline 
BencodeByteStringToken::~BencodeByteStringToken()
{
}

inline 
const std::vector<char>& BencodeByteStringToken::getValue() const
{
    return m_value;
}

template<size_t Size>
BencodeByteStringToken::BencodeByteStringToken(const char(& array)[Size])
: m_value(array, array + Size - 1)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                            BencodeList
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline 
BencodeList::BencodeList()
{
}

inline 
BencodeList::~BencodeList()
{
}

inline 
BencodeList::BencodeList(const std::list<BencodeToken*>& toks) 
: m_tokens(toks)
{
}

inline 
const std::list<BencodeToken*>& BencodeList::getTokens() const
{
    return m_tokens;
}
    
inline 
BencodeList::const_iterator BencodeList::begin() const
{
    return m_tokens.begin();
}

inline
BencodeList::const_iterator BencodeList::end() const
{
    return m_tokens.end();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                            BencodeDict
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
inline 
BencodeDict::BencodeDict() : m_dict(keyComparator)
{
}

inline 
BencodeDict::~BencodeDict()
{
}

inline 
BencodeDict::BencodeDict(const std::map<BencodeByteStringToken,
                                        BencodeToken*,
                                        BencodeDictComparator>& dict)
{
    m_dict = dict;
}

inline 
BencodeToken& BencodeDict::operator[](const BencodeByteStringToken& key)
{
    return *m_dict[key];
}
    
inline
BencodeDict::const_iterator 
BencodeDict::find(const BencodeByteStringToken& key) const
{
    return m_dict.find(key);
}
 
inline 
BencodeDict::const_iterator BencodeDict::begin() const
{
    return m_dict.begin();
}
    
inline 
BencodeDict::const_iterator BencodeDict::end() const
{
    return m_dict.end();
}

//==============================================================================
//                          FRIENDS
//==============================================================================
inline
bool operator==(const BencodeByteStringToken& lhs, 
                const BencodeByteStringToken& rhs)
{
    return lhs.m_value == rhs.m_value;
}

inline
bool operator!=(const BencodeByteStringToken& lhs, 
                const BencodeByteStringToken& rhs)
{
    return lhs.m_value != rhs.m_value;
}

} // end of torrent namespace
#endif 