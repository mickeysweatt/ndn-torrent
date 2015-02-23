// bencdeParserUtil.cpp                                                -*-C++-*-
#include <bencodeParserUtil.hpp>

#include <exception>
#include <functional>
#include <memory>

#include <string>
#include <istream>
#include <cassert>

using std::list;
using std::string;
using std::istream;
using std::shared_ptr;

namespace torrent {
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                       BencodeParserUtil
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    shared_ptr<BencodeToken> BencodeParserUtil::parseStream(istream& ifs) {
        shared_ptr<BencodeToken> curr_tok_p = nullptr;
        char   type;
        type = ifs.peek();
        
        switch(type) {
            case 'i': {
                curr_tok_p = std::make_shared<BencodeIntegerToken>(ifs);
            } break;
            case 'l' : {
                curr_tok_p = std::make_shared<BencodeList>(ifs);
            } break;
            case 'd': {
                curr_tok_p = std::make_shared<BencodeDict>(ifs);
            } break;
            default: {
                // byte string
                if (isdigit(type)) {
                    curr_tok_p =  std::make_shared<BencodeByteStringToken>(ifs);
                }
                else {
                    throw new ParseError("Unrecognized type.");
                }
            }
        }
        return curr_tok_p;
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
        shared_ptr<BencodeToken> curr_tok_p = nullptr;
        assert('l' == in.peek());
        // consume the l
        in.get(); 
        while ('e' != in.peek()) {
            curr_tok_p = BencodeParserUtil::parseStream(in);
            m_tokens.push_back(std::move(curr_tok_p));
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
        std::shared_ptr<BencodeToken>           value;
        std::shared_ptr<BencodeByteStringToken> key; 
        assert('d' == in.peek());
        // consume the 'd'
        in.get();
        while ('e' != in.peek()) {
            key = std::dynamic_pointer_cast<BencodeByteStringToken>(
                                            BencodeParserUtil::parseStream(in));
            assert(nullptr != key);
            value = BencodeParserUtil::parseStream(in);
            m_dict[std::move(std::move(*key))] = value;
        }
        in.get();
    }
    
    const std::map<BencodeByteStringToken,
                   std::shared_ptr<BencodeToken>,
                   BencodeDict::BencodeDictComparator>& 
    BencodeDict::getValues() const
    {
        return m_dict;
    }
}
