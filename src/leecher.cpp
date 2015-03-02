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
    using ndn::Consumer;

   class ChunkCallback {
       // DATA
       const ChunkInfo& m_chunkInfo;
       const torrent::Leecher& m_leecher;
       
      public:
       ChunkCallback(const ChunkInfo& chunkInfo, const torrent::Leecher& leecher)
        : m_chunkInfo(chunkInfo)
        , m_leecher(leecher)
        {
        }

        void
        processPayload(Consumer& c, const uint8_t* buffer, size_t bufferSize)
        {
            auto begin = reinterpret_cast<const char *>(buffer);
            vector<char> content(begin, begin + bufferSize);
            Name prefix;
            c.getContextOption(PREFIX, prefix);
            Name suffix;
            c.getContextOption(SUFFIX, suffix);
            // TODO, some processing with the name?
            m_leecher.processDownloadedChunk(std::move(content), m_chunkInfo);
        }
   };
}

namespace torrent {
    Leecher::Leecher(const ndn::Name& prefix, TorrentClientProtocol& clientProtocol)
   : m_prefix(prefix)
   , m_clientProtocol(clientProtocol)
   {
   }

   int Leecher::download(const ChunkInfo& chunkInfo)
   {
        ChunkCallback cb(chunkInfo, *this);
        ndn::Consumer c(m_prefix, RDR);
       // use API to request chunk with id in ChunkInfo
       c.setContextOption(MUST_BE_FRESH_S, false);
       c.setContextOption(CONTENT_RETRIEVED,
                          static_cast<ndn::ConsumerContentCallback>(bind(&ChunkCallback::processPayload, &cb, _1, _2, _3)));
        std::ostringstream ostr; //output string stream
        ostr << m_prefix << "/" << chunkInfo.getChunkId();
        c.asyncConsume(ndn::Name(ostr.str()));
       return 0;
   }

   int Leecher::download(const std::list<ChunkInfo>& chunkInfoList)
   {
        for (auto& it : chunkInfoList) {
            download(it);
        }
        return 0;
   }

   int Leecher::stopDownload(const std::list<ChunkInfo>& chunkInfoList)
   {
    return 0;
   }

    void Leecher::processDownloadedChunk(std::vector<char> content, const ChunkInfo& chunkInfo) const
   {
       torrent::SHA1Hash sha1(reinterpret_cast<const unsigned char *>(content.data()),
                              static_cast<unsigned long>(content.size()));
        // if hashes match
        if ( sha1 == chunkInfo.getChunkHash() ) {
            Chunk chunk(chunkInfo, std::move(content));
            m_clientProtocol.chunkDownloadSuccess(chunk);
        }
        else {
            assert(false && "hash did not match");
        }
   }
}
