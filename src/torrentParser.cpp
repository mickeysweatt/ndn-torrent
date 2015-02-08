#include "torrentParser.hpp"
#include "torrent.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <list>

namespace torrent {

    using std::string;
    
    static string getNextToken(std::ifstream& ifs);
    
    static string&& parseByteString(std::ifstream& ifs, size_t length)
    {
        char *buf = new char[length + 1];
        ifs.get(buf, length + 1);
        string rval = string(buf);
        delete [] buf;
        return std::move(rval);
    }
    
    static std::list<string>&& getNextTokenList(std::ifstream& ifs) {
        std::list<string> values;
        string value;
        char   type;
        type = ifs.get();
        char c;
        string buff;
        size_t length;
        
        switch(type) {
            case 'i': {
               value = "";
                while(c != 'e' && ifs.good()) {
                    c = ifs.get();
                    value += c;
                }
                values.push_back(value);
                //return std::move(buff);
            } break;
            case 'l': {
                while(ifs.peek() != 'e') {
                    value = getNextToken(ifs);
                    values.push_back(value);
                }
            } break;
            default: {
                // byte string
                if (isdigit(type)) {
                    value = type;
                    c = ifs.get();
                    while(c != ':' && ifs.good()) {
                        value += c;
                        ifs.get(c);
                    }
                    length = atoi(buff.c_str());
                    value = parseByteString(ifs, length);
                    values.push_back(value);
                }
                else {
                    std::cerr << "IMPLEMENT ME: " << type << std::endl;
                }
            }
        }
        return std::move(values);
    }
    
    static string getNextToken(std::ifstream& ifs) {
        string buff;
	 	char   type;
	 	size_t length;
	 	char   c;
	 	type = ifs.get();

	 	switch(type) {
	 		case 'i': {
	 			buff = "";
	 			while(c != 'e' && ifs.good()) {
	 				c = ifs.get();
	 				buff += c;		
	 			}
                //return std::move(buff);
	 		} break;
	 		default: {
                // byte string
                if (isdigit(type)) {
                    buff = type;
                    c = ifs.get();
                    while(c != ':' && ifs.good()) {
                        buff += c;
                        ifs.get(c);
                    }
                    length = atoi(buff.c_str());
                    buff = parseByteString(ifs, length);
                }
                else {
                    std::cerr << "IMPLEMENT ME: " << type << std::endl;
                }
	 		}
	 	}
        return buff;
	 }

	Torrent&& TorrentParserUtil::parseFile(const std::string& filename)
	{
         std::unordered_map<string, string> torrentDict;
         Torrent t;
         std::ifstream ifs;
		 std::string key;
		 std::string value;
         std::list<string> value_list;
         ifs.open(filename, std::ios::binary);
		 if (ifs.bad()) {
		 	throw new ParseError("unable to open: " + filename);
		 }

		 // A torrent file is a dict, so lets read the 'd' off the front
        
		 if ('d' != ifs.get()) {
		 	throw new ParseError("Malformed torrent file");
		 }
		 while(ifs.good()) {
		 	key   = getNextToken(ifs);
            if (key == "announce-list") {
                value_list = getNextTokenList(ifs);
            }
		 	value = getNextToken(ifs);
            //torrentDict[key] = value;
		 }
		 ifs.close();
        return std::move(t);
	}

}

int main() {
    torrent::TorrentParserUtil::parseFile("/Users/admin/Desktop/ubuntu-14.10-desktop-amd64.iso.torrent");
}

