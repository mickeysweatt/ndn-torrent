//
//  BencodeParser.cpp
//  Torrent
//
//  Created by admin on 2/10/15.
//  Copyright (c) 2015 admin. All rights reserved.
//

#include <bencodeParserUtil.hpp>
#include <string>
#include <istream>

using std::list;
using std::string;

namespace torrent {
    BencodeToken& BencodeParserUtil::parseStream(std::istream& ifs) {
        BencodeToken *curr_tok_p = nullptr;
        char   type;
        type = ifs.peek();
        
        switch(type) {
            case 'i': {
                curr_tok_p = new BencodeIntegerToken(ifs);
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
                    curr_tok_p = new BencodeByteStringToken(ifs);
                }
                else {
                    throw new ParseError("Unrecognized type.");
                }
            }
        }
        return *curr_tok_p;
    }
    
    BencodeIntegerToken::BencodeIntegerToken(std::istream& in)
    {
        string int_buff;
        char c;
        if ('i' != in.peek()) {
            throw new BencodeParserUtil::ParseError("Attempting to construct "
                                                    "integer token from "
                                                    "non-integer ");
        }
        // consume the i
        in.get();
        while('e' != (c = in.get())) {
            if (!isdigit(c)) {
                throw new BencodeParserUtil::ParseError("Attempting to "
                                                        "construct integer "
                                                        "token from "
                                                        "non-integer");
            }
            int_buff += c;
        }
        m_value = atoi(int_buff.c_str());
    }
    
    BencodeByteStringToken::BencodeByteStringToken(const char *buffer, size_t length)
    : m_value(buffer, buffer + length)
    {
    }
    
    BencodeByteStringToken::BencodeByteStringToken(std::string& str)
    : m_value(str.begin(), str.end())
    {
    }
    
    BencodeByteStringToken::BencodeByteStringToken(std::istream& in){
        string length_str;
        int length;
        char c;
        char *buffer;
        
        if (!isdigit(in.peek())) {
            throw BencodeParserUtil::ParseError("Attemptying to construct a "
                                                "byte string from an illformed "
                                                "string");
        }
        while(':' != (c = in.get())) {
            if (!isdigit(c)) {
                throw new BencodeParserUtil::ParseError("Attempting to construct integer "
                                                        "token from illformed string");
            }
            length_str += c;
        }
        length = atoi(length_str.c_str());
        m_value.reserve(length);
        buffer = new char[length];
        in.read(buffer, length);
        std::copy(buffer, buffer + length, std::back_inserter(m_value));
        m_value.size();
        delete [] buffer;
    }
    
    string BencodeByteStringToken::getString() const {
        return std::move(string(m_value.begin(), m_value.end()));
    }
    
    BencodeList::BencodeList(std::istream& in)
    {
        BencodeToken* curr_tok;
        if ('l' != in.peek())
        {
            throw new BencodeParserUtil::ParseError("invalid list");
        }
        in.get();
        while ('e' != in.peek()) {
            curr_tok = &BencodeParserUtil::parseStream(in);
            m_tokens.push_back(curr_tok);
        }
        in.get();
    }
    
    BencodeDict::BencodeDictComparator BencodeDict::keyComparator =
    [] (const BencodeByteStringToken& lhs, const BencodeByteStringToken& rhs)
    {
        return lhs.getValue() < rhs.getValue();
    };
    
    BencodeDict::BencodeDict(std::istream& in) : m_dict(keyComparator)
    {
        BencodeToken *value;
        BencodeByteStringToken *key;
        if ('d' != in.peek()) {
            throw new BencodeParserUtil::ParseError("Bad dict");
        }
        in.get();
        while ('e' != in.peek()) {
            key = dynamic_cast<BencodeByteStringToken *>(
                                           &BencodeParserUtil::parseStream(in));
            if (nullptr == key) {
                throw new BencodeParserUtil::ParseError("Bad key");
            }
            value = &BencodeParserUtil::parseStream(in);
            m_dict[*key] = value;
        }
        in.get();
    }
    
    const std::map<BencodeByteStringToken,
                   BencodeToken*,
                   BencodeDict::BencodeDictComparator>& BencodeDict::getValues() const
    {
        return m_dict;
    }
}