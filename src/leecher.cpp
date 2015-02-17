#include "chunk.hpp"
#include "chunkInfo.hpp"
#include <leecher.hpp>

namespace torrent{
   Leecher::Leecher(TorrentClientProtocol& clientProtocol)
   : m_clientProtocol(clientProtocol)
   {
   }
   Leecher::~Leecher() {}
   int Leecher::download(const std::list<ChunkInfo>& chunkInfoList)
   {

   	return 0;
   }

   int Leecher::stopDownload(const std::list<ChunkInfo>& chunkInfoList)
   {

   	return 0;
   }
}
