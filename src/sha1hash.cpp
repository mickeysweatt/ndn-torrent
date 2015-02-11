#include <sha1hash.hpp>
#include <openssl/sha.h>

namespace torrent {
	
	SHA1Hash::SHA1Hash(const unsigned char *data, unsigned long length)
	{
		SHA1(data, length, m_hash);
	}
}