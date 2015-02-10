#include "torrentParser.hpp"
#include "torrent.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <list>
#include <functional>
#include <algorithm>

using std::string;
using std::list;

namespace {
  class BencodeToken {
    public:
        // CREATORS
        BencodeToken();
      
      virtual ~BencodeToken() {}
  };
    
  class BencodeSimpleToken : public BencodeToken {
    public:
        BencodeSimpleToken();
      
        virtual ~BencodeSimpleToken();
      
        explicit BencodeSimpleToken(const string& tok);
      
        const string& getValue() const;
      
        void setValue(const string& tok);
    
    private:
      string m_value;
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

        explicit BencodeList(const list<BencodeToken*>& toks);
        
        const list<BencodeToken *>& getTokens() const;
        
        void setTokens(const list<BencodeToken *>& toks);
        
      private:
        list<BencodeToken *> m_tokens;
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

namespace torrent {
    static BencodeToken& getNextToken(std::istream& ifs);
}

namespace {
    
    BencodeToken::BencodeToken()
    {
    }
    
    BencodeSimpleToken::BencodeSimpleToken()
    {
    }
    
    BencodeSimpleToken::BencodeSimpleToken(const string& tok) : m_value(tok)
    {
    }
    
    const string& BencodeSimpleToken::getValue() const
    {
        return m_value;
    }
    
    BencodeSimpleToken::~BencodeSimpleToken()
    {
        
    }
    
    void BencodeSimpleToken::setValue(const string &tok){
        m_value = tok;
    }
    
    IntegearToken::IntegearToken()
    {
    }
    
    IntegearToken::IntegearToken(std::istream& in)
    {
        string int_buff;
        char c;
        if ('i' != in.peek()) {
            throw new torrent::TorrentParserUtil::ParseError("Attempting to construct integear token from non-integear");
        }
        // consume the i
        in.get();
        while('e' != (c = in.get())) {
            if (!isdigit(c)) {
                throw new torrent::TorrentParserUtil::ParseError("Attempting to construct integear token from non-integear");
            }
            int_buff += c;
        }
        setValue(int_buff);
    }
    
    ByteStringToken::ByteStringToken()
    {
    }
    
    ByteStringToken::ByteStringToken(std::istream& in){
        string length_str;
        char *buffer;
        int length;
        char c;
        
        if (!isdigit(in.peek())) {
            throw torrent::TorrentParserUtil::ParseError("Attemptying to construct a byte string from an illformed string");
        }
        while(':' != (c = in.get())) {
            if (!isdigit(c)) {
                throw new torrent::TorrentParserUtil::ParseError("Attempting to construct integear token from illformed string");
            }
            length_str += c;
        }
        length = atoi(length_str.c_str());
        buffer = new char[length + 1];
        in.get(buffer, length + 1);
        setValue(string(buffer));
    }
    
    BencodeList::BencodeList()
    {
    }
    
    BencodeList::~BencodeList()
    {
    }
    BencodeList::BencodeList(std::istream& in)
    {
        BencodeToken* curr_tok;
        if ('l' != in.peek())
        {
            throw new torrent::TorrentParserUtil::ParseError("invalid list");
        }
        in.get();
        while ('e' != in.peek()) {
            curr_tok = &torrent::getNextToken(in);
            m_tokens.push_back(curr_tok);
        }
        in.get();
    }

    BencodeList::BencodeList(const list<BencodeToken*>& toks) : m_tokens(toks)
    {
    }
    
    const list<BencodeToken*>& BencodeList::getTokens() const
    {
        return m_tokens;
    }
    
    void BencodeList::setTokens(const list<BencodeToken *> &toks)
    {
        m_tokens = toks;
    }
    
    BencodeDict::BencodeDictComparator BencodeDict::keyComparator =
    [] (const ByteStringToken& lhs, const ByteStringToken& rhs)
    {
        return lhs.getValue().compare(rhs.getValue());
    };
    
    BencodeDict::BencodeDict() :
        m_dict(keyComparator)
    {
    }
    
    BencodeDict::~BencodeDict()
    {
    }
    
    BencodeDict::BencodeDict(const std::map<ByteStringToken, BencodeToken*, BencodeDictComparator>& dict)
    {
        m_dict = dict;
    }
    
    BencodeDict::BencodeDict(std::istream& in) : m_dict(keyComparator)
    {
        BencodeToken *value;
        ByteStringToken *key;
        if ('d' != in.peek()) {
            throw new torrent::TorrentParserUtil::ParseError("Bad dict");
        }
        in.get();
        while ('e' != in.peek()) {
            key = dynamic_cast<ByteStringToken *>(&torrent::getNextToken(in));
            if (nullptr == key) {
                throw new torrent::TorrentParserUtil::ParseError("Bad key");
            }
            else {
                std::cout << key->getValue() << std::endl;
            }
            value = &torrent::getNextToken(in);
            m_dict[*key] = value;
        }
        in.get();
    }
    
    BencodeToken& BencodeDict::operator[](const ByteStringToken key)
    {
        return *m_dict[key];
    }
    
}

namespace torrent {
    static BencodeToken& getNextToken(std::istream& ifs) {
        BencodeToken *curr_tok_p = nullptr;
        char   type;
        type = ifs.peek();

        switch(type) {
            case 'i': {
              curr_tok_p = new IntegearToken(ifs);
            } break;
            case 'l' : {
              curr_tok_p = new BencodeList(ifs);
            } break;
            case 'd': {
                curr_tok_p = new BencodeDict(ifs);
            } break;
            default: {
                // byte string
                if (isdigit(type)) {
                    curr_tok_p = new ByteStringToken(ifs);
                }
                else {
                    std::cerr << "IMPLEMENT ME: " << type << std::endl;
                }
            }
        }
        return *curr_tok_p;
     }

    Torrent&& TorrentParserUtil::parseFile(std::istream& in)
    {
        Torrent t;
        BencodeToken *ast;
        BencodeSimpleToken* ast_s;
        BencodeList *ast_l;
        
        if (in.bad()) {
            throw new ParseError("Bad stream");
        }
        ast = &getNextToken(in);
        ast_s = dynamic_cast<BencodeSimpleToken*>(ast);
        ast_l = dynamic_cast<BencodeList *>(ast);
        //std::string s = ast_s->getValue();
        return std::move(t);
     }
}

