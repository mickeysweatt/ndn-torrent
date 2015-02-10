//
//  BencodeParser.h
//  Torrent
//
//  Created by admin on 2/10/15.
//  Copyright (c) 2015 admin. All rights reserved.
//

#ifndef INCLUDED_BENCODE_PARSER
#define INCLUDED_BENCODE_PARSER

#include <string>
#include <list>
#include <map>
#include <iosfwd>
#include <exception>

namespace torrent {
    class BencodeToken;
    
    class BencodeParser {
      public:
        static BencodeToken& parseStream(std::istream& in);
    };


class BencodeToken {
  public:
    class ParseError : std::logic_error {
      public:
        ParseError(const std::string &what);
    };
    // CREATORS
    BencodeToken();
    
    virtual ~BencodeToken() {}
};

class BencodeSimpleToken : public BencodeToken {
  public:
    BencodeSimpleToken();
        
    virtual ~BencodeSimpleToken();
        
    explicit BencodeSimpleToken(const std::string& tok);
        
    const std::string& getValue() const;
        
    void setValue(const std::string& tok);
        
    private:
        std::string m_value;
};
    
    
class IntegearToken : public BencodeSimpleToken {
  public:
    IntegearToken();
        
    explicit IntegearToken(std::istream& in);
};
    
class ByteStringToken : public BencodeSimpleToken {
  public:
    ByteStringToken();
        
    explicit ByteStringToken(std::istream& in);
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
    typedef std::function<bool(const ByteStringToken &, const ByteStringToken&)> BencodeDictComparator;
    static BencodeDictComparator keyComparator;
  public:
    BencodeDict();
        
    ~BencodeDict();
            
    explicit BencodeDict(std::istream& in);
        
    explicit BencodeDict(const std::map<ByteStringToken, BencodeToken*, BencodeDictComparator>& dict);
    
    BencodeToken& operator[](const ByteStringToken key);
        
  private:
    std::map<ByteStringToken,
             BencodeToken*,
             std::function<bool(const ByteStringToken &, const ByteStringToken &)> > m_dict;
};

}

#endif 