#ifndef INCLUDED_LEECHER_HPP
#define INCLUDED_LEECHER_HPP

#include <torrentClientProto.hpp>
#include <list>

namespace torrent {

class ChunkInfo;

class Leecher {
   // A class that represents the leeching, i.e. downloading, portion of the
   // client. It will send interest packets for chunks and communicate with a
   // TorrentClientProtocol whenever it finishes downloading a chunk.   
public:
   // CREATORS
   explicit Leecher(TorrentClientProtocol& clientProtocol);
   // Creates a Leecher that communicates with clientProtocol.

   ~Leecher();
   // Destroy this object.

   int download(const std::list<ChunkInfo>& chunkInfoList);
   // Begins the download for chunks in chunkInfoList, if not already in the
   // process of downloading.

   int stopDownload(const std::list<ChunkInfo>& chunkInfoList);
   // Terminates the download for chunks in chunkInfoList if possible.

private:
   // DATA
   TorrentClientProtocol& m_clientProtocol;
};

} // namespace torrent

#endif // INCLUDED_LEECHER_HPP
