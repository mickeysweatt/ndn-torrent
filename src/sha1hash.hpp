#ifndef INCLUDED_SHA_HPP
#define INCLUDED_SHA_HPP

#include <cstring>
#include <openssl/sha.h>

namespace torrent{

class SHA1Hash
{
  public:
  	// CREATORS
  	SHA1Hash();

  	explicit SHA1Hash(const unsigned char *hash);
  	// Create a 'SHA1Hash' with the value of the specified
  	// 'hash' buffer.

  	SHA1Hash(const unsigned char *data, unsigned long length);
  	// Create a 'SHA1Hash' from data, use SHA1() to compute 
  	// the 'hash' and store it in m_hash.
    
  	// FRIENDS
  	friend bool operator==(const SHA1Hash& lhs, const SHA1Hash& rhs);
  private:
	unsigned char m_hash[20];
};

inline bool operator==(const torrent::SHA1Hash& lhs, const torrent::SHA1Hash& rhs)
{
    return 0 == memcmp(lhs.m_hash, rhs.m_hash, 20);
}

}

#endif // INCLUDED_SHA_HPP