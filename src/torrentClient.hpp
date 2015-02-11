#ifndef INCLUDED_TORRENT_CLIENT_HPP
#define INCLUDED_TORRENT_CLIENT_HPP

#include <torrentClientProto.hpp>
#include <string>

namespace torrent {

// Class that represents a request to upload/download the data associated
// with specific torrent file.  Handles file IO, choosing which chunks to 
// upload/download, etc.
class TorrentClient : public TorrentClientProtocol {
  public:
    //CREATORS
    explicit TorrentClient(const std::string& torrentFile,
            const std::string& downloadLocation);
  
    // Override TorrentClientProtocol method
    void chunkDownloadSuccess(const Chunk& chunk);
    // Override TorrentClientProtocol method
    void chunkDownloadFail(TorrentClientProtocol::Error error,
                           const ChunkInfo& chunkMetadata);
    
    // Given a path to a torrent file and a download location, download any
    // missing chunks, and upload all chunks we have.
    
    int start();
    // Stop downloads/uploads of a given torrent file.
    int stop();
  private:
    //DATA
    std::string torrentFile;
    std::string downloadLocation;
};

}

#endif
