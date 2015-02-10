//
//  BencodeParser.cpp
//  Torrent
//
//  Created by admin on 2/10/15.
//  Copyright (c) 2015 admin. All rights reserved.
//

#include <bencodeParser.hpp>
#include <string>
#include <istream>

using std::list;
using std::string;

namespace torrent {
    BencodeToken& BencodeParser::parseStream(std::istream& ifs) {
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
                    throw new BencodeToken::ParseError("Unrecognized type.");
                }
            }
        }
        return *curr_tok_p;
    }
    

    
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
    
    BencodeToken::ParseError::ParseError(const std::string& what) : logic_error(what)
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
            throw new BencodeToken::ParseError("Attempting to construct integear token from non-integear");
        }
        // consume the i
        in.get();
        while('e' != (c = in.get())) {
            if (!isdigit(c)) {
                throw new BencodeToken::ParseError("Attempting to construct integear token from non-integear");
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
            throw BencodeToken::ParseError("Attemptying to construct a byte string from an illformed string");
        }
        while(':' != (c = in.get())) {
            if (!isdigit(c)) {
                throw new BencodeToken::ParseError("Attempting to construct integear token from illformed string");
            }
            length_str += c;
        }
        length = atoi(length_str.c_str());
        buffer = new char[length + 1];
        in.read(buffer, length);
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
            throw new BencodeToken::ParseError("invalid list");
        }
        in.get();
        while ('e' != in.peek()) {
            curr_tok = &BencodeParser::parseStream(in);
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
            throw new BencodeToken::ParseError("Bad dict");
        }
        in.get();
        while ('e' != in.peek()) {
            key = dynamic_cast<ByteStringToken *>(&BencodeParser::parseStream(in));
            if (nullptr == key) {
                throw new BencodeToken::ParseError("Bad key");
            }
            value = &BencodeParser::parseStream(in);
            m_dict[*key] = value;
        }
        in.get();
    }
    
    BencodeToken& BencodeDict::operator[](const ByteStringToken key)
    {
        return *m_dict[key];
    }

}