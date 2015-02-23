#ifndef INCLUDED_TORRENT_CLIENT_HPP
#define INCLUDED_TORRENT_CLIENT_HPP

#include <leecher.hpp>
#include <seeder.hpp>
#include <torrent.hpp>
#include <torrentClientProto.hpp>
#include <string>
#include <list>

namespace torrent {

// Class that represents a request to upload/download the data associated
// with specific torrent file.  Handles file IO, choosing which chunks to 
// upload/download, etc.
class TorrentClient : public TorrentClientProtocol {
  public:
    //CREATORS
    TorrentClient(const std::string& torrentFile,
            const std::string& downloadLocation);
    
    //METHODS
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
    
    //PUBLIC TYPES
    class FileNotFound : public std::logic_error {
      public:
        explicit FileNotFound(const std::string& filename)
        : logic_error("File " + filename + " not found."), m_filename(filename)
        {
        }
      private:
        std::string m_filename;
    };
    
    class BadTorrentFile : public std::logic_error {
      public:
        BadTorrentFile(const std::string& what)
        : logic_error(what)
        {
        }
    };
  private:
    //DATA
    std::string m_downloadLocation;
    Torrent m_torrent;
    std::list<Chunk> m_uploadList;
    std::list<ChunkInfo> m_downloadList;
    Seeder m_seeder;
    Leecher m_leecher;
    bool m_uploading;
};

}

#endif
