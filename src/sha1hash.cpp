#include <cstring>
#include <sha1hash.hpp>
#include <openssl/evp.h>
#include <cstring>
#include <cryptopp/sha.h>

namespace torrent {

	SHA1Hash::SHA1Hash()
	{
	}
	
	SHA1Hash::SHA1Hash(const unsigned char *data, unsigned long length)
	{
        CryptoPP::SHA1().CalculateDigest(m_hash, data, length);
	}

	SHA1Hash::SHA1Hash(const unsigned char *hash)
	{
		memcpy(m_hash, hash, 20);
	}
}