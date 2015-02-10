#ifndef INCLUDED_TORRENT_CLIENT_HPP
#define INCLUDED_TORRENT_CLIENT_HPP

#include <torrentClientProto.hpp>
#include <string>

namespace torrent {

// Class that represents a request to upload/download the data associated
// with a torrent file.  Handles file IO, choosing which chunks to upload/
// download, etc.
class TorrentClient : public TorrentClientProtocol {
  public:
    // Override TorrentClientProtocol method
    void chunkDownloadSuccess(const Chunk& chunk);
    // Override TorrentClientProtocol method
    void chunkDownloadFail(error_t error, const ChunkInfo& chunkMetadata);
    
    // Given a path to a torrent file and a download location, download any
    // missing chunks, and upload all chunks we have.
    void add(std::string torrentFile, std::string downloadLocation);
    // Stop downloads/uploads of a given torrent file.
    void stop(std::string torrentFile);
};

}

#endif
