#ifndef INCLUDED_TORRENT_CLIENT_PROTOCOL_HPP
#define INCLUDED_TORRENT_CLIENT_PROTOCOL_HPP

namespace torrent {

class Chunk;
class ChunkInfo;

// An abstract class used to represent the functions that a
// leecher/seeder call to notify the TorrentClient.  Currently,
// all functionality is for use by the leecher.
class TorrentClientProtocol {
  public:
    //Errors:
    //More errors should probably be added.
    enum Error {
        NONEXISTANT_NAME,
    };
  
    // Callback function to be called when a chunk downloads.
    virtual void chunkDownloadSuccess(const Chunk& chunk) = 0;
    // Callback function to be called if there is an error downloading.
    virtual void chunkDownloadFail(Error error,
                    const ChunkInfo& chunkMetadata) = 0;
    
    virtual ~TorrentClientProtocol() {}
};

}

#endif
