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
            std::cout << "IN PROCESS PAYLOAD " << prefix <<  "/" << suffix << std::endl;
            assert(it != m_leecher.getPendingChunks().end());
            m_leecher.processDownloadedChunk(std::move(content), it->second, suffix);
        }
        void
        processLeavingInterest(Consumer& c, ndn::Interest& interest)
        {
            ndn::shared_ptr<ndn::Face> f;
            c.getContextOption(FACE, f);
           std::cout << "LEAVES  " << interest.toUri() << std::endl;
//            std::cout << "FACE pending interests: " << f->getNPendingInterests() << std::endl;
        }
       void
       processExpiredInterest(Consumer& c, ndn::Interest& interest)
       {
           ndn::shared_ptr<ndn::Face> f;
           c.getContextOption(FACE, f);
//           std::cout << "EXPIRED  " << interest.toUri() << std::endl;
//           std::cout << "FACE pending interests: " << f->getNPendingInterests() << std::endl;
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
       m_consumer.setContextOption(INTEREST_LIFETIME, 1000);
       m_consumer.setContextOption(CONTENT_RETRIEVED,
                          static_cast<ndn::ConsumerContentCallback>(
                            bind(&ChunkCallback::processPayload, 
                                 m_callback,
                                 _1, 
                                 _2, 
                                 _3)));
       m_consumer.setContextOption(INTEREST_LEAVE_CNTX,
                           static_cast<ndn::ConsumerInterestCallback>(
                            bind(&ChunkCallback::processLeavingInterest,
                                 m_callback,
                                 _1,
                                 _2)));
       m_consumer.setContextOption(INTEREST_EXPIRED,
                                   static_cast<ndn::ConsumerInterestCallback>(
                                      bind(&ChunkCallback::processExpiredInterest,
                                           m_callback,
                                           _1,
                                           _2)));
   }

   Leecher::~Leecher() 
   {
       m_consumer.stop();
       delete m_callback;
   }

   int Leecher::download(const ChunkInfo& chunkInfo, bool block)
   {
       // use API to request chunk with id in ChunkInfo
       std::cout << "CONSUMING: " << m_prefix << "/" << chunkInfo.getChunkId() << std::endl;

       // ndn::Name suffix = ndn::Name(ostr.str());
       std::ostringstream suffixOstr;
       suffixOstr << chunkInfo.getChunkId();
       ndn::Name suffix = ndn::Name(suffixOstr.str());
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
        for (auto& chunkInfo : chunkInfoList) {
            download(chunkInfo, false);
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
            //assert(false && "hash did not match");
        }
   }
    
    const Leecher::ChunkInfoMap& Leecher::getPendingChunks() const
    {
        return m_pendingChunks;
    }
}
