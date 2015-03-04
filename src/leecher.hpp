#ifndef INCLUDED_LEECHER_HPP
#define INCLUDED_LEECHER_HPP

#include <torrentClientProto.hpp>
#include <ndn-cxx/contexts/consumer-context.hpp>
#include <list>
#include <unordered_map>
#include <ndn-cxx/name.hpp>

namespace torrent {

class ChunkInfo;

class Leecher {
   // A class that represents the leeching, i.e. downloading, portion of the
   // client. It will send interest packets for chunks and communicate with a
   // TorrentClientProtocol whenever it finishes downloading a chunk.   
public:
   // CREATORS
    explicit Leecher(const ndn::Name& prefix, TorrentClientProtocol& clientProtocol);
   // Creates a Leecher that communicates with clientProtocol.

   ~Leecher() = default;
   // Destroy this object.

   int download(const ChunkInfo& chunkInfo, bool block = true);
   // Begins the download for the chunk described by ChunkInfo, if not 
   // already in the process of downloading.

   int download(const std::list<ChunkInfo>& chunkInfoList);
   // Begins the download for chunks in chunkInfoList, if not already in the
   // process of downloading.

   int stopDownload(const std::list<ChunkInfo>& chunkInfoList);
   // Terminates the download for chunks in chunkInfoList if possible.
    
    int stopDownload(const ChunkInfo& chunkInfo);
    // Terminates the download for chunks in chunkInfoList if possible.

   void processDownloadedChunk(std::vector<char> content, 
                               const ChunkInfo& chunk);

private:
   // DATA
   ndn::Name                                              m_prefix;
   TorrentClientProtocol&                                 m_clientProtocol;
   std::unordered_map<const ChunkInfo *, ndn::Consumer *> m_pendingChunks;
};

} // namespace torrent

#endif // INCLUDED_LEECHER_HPP
