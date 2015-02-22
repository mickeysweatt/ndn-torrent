// bencdeParserUtil.cpp                                                -*-C++-*-
#include <bencodeParserUtil.hpp>
#include <string>
#include <istream>
#include <cassert>

using std::list;
using std::string;

namespace torrent {
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                       BencodeParserUtil
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
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
// REVIEW: seems a little odd to return a reference variable to allocated memory
// that if the callee is responsible for deleting it
        return *curr_tok_p;
    }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                       BencodeIntegerToken
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    BencodeIntegerToken::BencodeIntegerToken(std::istream& in)
    {
        string int_buff;
        char c;
        assert('i' == in.peek());
        // consume the i
        in.get();
        while('e' != (c = in.get())) {
            assert(isdigit(c));
            int_buff += c;
        }
        m_value = atoi(int_buff.c_str());
    }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                       BencodeByteStringToken
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
    BencodeByteStringToken::BencodeByteStringToken(std::istream& in){
        string length_str;
        int length;
        char c;
        
        assert(isdigit(in.peek()));
        while(':' != (c = in.get())) {
            assert(isdigit(c));
            length_str += c;
        }
        length = atoi(length_str.c_str());
        m_value.resize(length);
        in.read(m_value.data(), length);
    }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                               BencodeList
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    BencodeList::BencodeList(std::istream& in)
    {
// REVIEW: consistency in pointer declaration; elsewhere you put the * next to
// the variable name
        BencodeToken* curr_tok;
        assert('l' == in.peek());
        // consume the l
        in.get(); 
        while ('e' != in.peek()) {
            curr_tok = &BencodeParserUtil::parseStream(in);
// REVIEW: possible memory leak; do the tokens ever get deleted?
            m_tokens.push_back(curr_tok);
        }
        in.get();
    }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                              BencodeDict
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
        // consume the 'd'
        in.get();
        while ('e' != in.peek()) {
            key = dynamic_cast<BencodeByteStringToken *>(
                                           &BencodeParserUtil::parseStream(in));
            assert(nullptr != key);
            value = &BencodeParserUtil::parseStream(in);
// REVIEW: possibly memory leak; are the tokens ever deleted?
            m_dict[std::move(*key)] = value;
        }
        in.get();
    }
    
    const std::map<BencodeByteStringToken,
                   BencodeToken*,
                   BencodeDict::BencodeDictComparator>& 
    BencodeDict::getValues() const
    {
        return m_dict;
    }
}
