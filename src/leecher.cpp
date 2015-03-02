#include <chunk.hpp>
#include <chunkInfo.hpp>
#include <leecher.hpp>
#include <ndn-cxx/contexts/consumer-context.hpp>
#include <ndn-cxx/name.hpp>
#include <string>
#include <vector>
#include <sstream>

#include <cassert>

using std::vector;
using std::string;
using std::bind;
using ndn::Name;

namespace {
    using torrent::ChunkInfo;
    using torrent::Chunk;
    using torrent::TorrentClientProtocol;

   class ChunkCallback {
       // DATA
       const ChunkInfo& m_chunkInfo;
       TorrentClientProtocol& m_clientProtocol;
      public:
        ChunkCallback(const ChunkInfo& chunkInfo, TorrentClientProtocol& clientProtocol) 
        : m_chunkInfo(chunkInfo), m_clientProtocol(clientProtocol)
        {
        }

        void
        processPayload(const uint8_t* buffer, size_t bufferSize)
        {
            auto begin = reinterpret_cast<const char *>(buffer);
            vector<char> content(begin, begin + bufferSize);
            Chunk newChunk(m_chunkInfo, std::move(content));
            
            // compute hash and compare
            torrent::SHA1Hash sha1(reinterpret_cast<const unsigned char *>(buffer),
                          static_cast<unsigned long>(bufferSize));
            // if hashes match
            if ( sha1 == m_chunkInfo.getChunkHash() )
                m_clientProtocol.chunkDownloadSuccess(newChunk);
            else { // hash doesn't match
                // FIXME
                assert(false && "hash did not match");
            }
                //TODO
        }
   };
}

namespace torrent{
   Leecher::Leecher(TorrentClientProtocol& clientProtocol)
   : m_clientProtocol(clientProtocol)
   {
   }
   Leecher::~Leecher() 
   {
   }

   int Leecher::download(const ChunkInfo& chunkInfo)
   {
        ndn::Name chunkName("come up with a name with namespace info");
      
        ChunkCallback cb(chunkInfo, m_clientProtocol);

        ndn::Consumer c(chunkName, RDR);
        // use API to request chunk with id in ChunkInfo
        c.setContextOption(CONTENT_RETRIEVED, 
                    (ndn::ContentCallback)std::bind(&ChunkCallback::processPayload, &cb, _1, _2));

        std::ostringstream ostr; //output string stream
        ostr << chunkInfo.getChunkId();
        c.consume(ndn::Name(ostr.str())); // double check
       return 0;
   }

   int Leecher::download(const std::list<ChunkInfo>& chunkInfoList)
   {
        for (auto it : chunkInfoList) {

        }
        return 0;
   }

   int Leecher::stopDownload(const std::list<ChunkInfo>& chunkInfoList)
   {

    return 0;
   }
}
