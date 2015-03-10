#include <chunk.hpp>
#include <chunkInfo.hpp>
#include <leecher.hpp>
#include <ndn-cxx/contexts/consumer-context.hpp>
#include <ndn-cxx/name.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <cassert>
#include <iostream>

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
       torrent::Leecher& m_leecher;
       
      public:
       ChunkCallback(torrent::Leecher& leecher)
        : m_leecher(leecher)
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
            auto it = m_leecher.getPendingChunks().find(suffix);
            // TODO, some processing with the name?
            std::cout << "IN PROCESS PAYLOAD " << prefix << std::endl;
            m_leecher.processDownloadedChunk(std::move(content), it->second, suffix);
        }
   };
}

namespace torrent {
    Leecher::Leecher(const ndn::Name& prefix, 
                     TorrentClientProtocol& clientProtocol)
   : m_prefix(prefix)
   , m_clientProtocol(clientProtocol)
   , m_consumer(prefix, RDR)
   {
       m_callback = new ChunkCallback(*this);
       m_consumer.setContextOption(MUST_BE_FRESH_S, true);
       m_consumer.setContextOption(CONTENT_RETRIEVED,
                          static_cast<ndn::ConsumerContentCallback>(
                            bind(&ChunkCallback::processPayload, 
                                 m_callback,
                                 _1, 
                                 _2, 
                                 _3)));
   }

   Leecher::~Leecher() 
   {
       m_consumer.stop();
       delete m_callback;
   }

   int Leecher::download(const ChunkInfo& chunkInfo, bool block)
   {
       // use API to request chunk with id in ChunkInfo
       std::ostringstream ostr;
       ostr << m_prefix << "/" << chunkInfo.getChunkId();
       std::cout << "CONSUMING: " << ostr.str() << std::endl;
       ndn::Name suffix = ndn::Name(ostr.str());
       m_consumer.asyncConsume(suffix);

       m_pendingChunks.insert(std::make_pair(suffix, &chunkInfo));
       // Returning immediately, means we must make sure that this object
       // remains in scope.
       if (block) {
           Consumer::consumeAll(); 
       }
       return 0;
   }

   int Leecher::download(const std::list<ChunkInfo>& chunkInfoList)
   {
        for (auto& it : chunkInfoList) {
            download(it, false);
        }
        Consumer::consumeAll(); 
        return 0;
   }
    
   int Leecher::stopDownload(const ChunkInfo& chunkInfo)
   {
//       auto it = m_pendingChunks.find(&chunkInfo);
//       if (m_pendingChunks.end() != it) {
//           it->second->stop();
//           m_pendingChunks.erase(it->first);
//           return 1;
//       }
       return 0;
   }


   int Leecher::stopDownload(const std::list<ChunkInfo>& chunkInfoList)
   {
       int rval = 0;
       for (auto chunk : chunkInfoList ) {
           rval += stopDownload(chunk);
       }
       return rval;
   }

    void Leecher::processDownloadedChunk(std::vector<char> content, 
                                         const ChunkInfo*  chunkInfo,
                                         const Name&       chunkName)
   {
       torrent::SHA1Hash sha1(reinterpret_cast<const unsigned char *>(
                                  content.data()),
                              static_cast<unsigned long>(content.size()));

    
       // if hashes match
        if (sha1 == chunkInfo->getChunkHash() ) {
            Chunk chunk(*chunkInfo, std::move(content));
            m_clientProtocol.chunkDownloadSuccess(chunk);
            m_pendingChunks.erase(chunkName);
        }
        else {
            assert(false && "hash did not match");
        }
   }
    
    Leecher::ChunkInfoMap Leecher::getPendingChunks() const
    {
        return m_pendingChunks;
    }
}
