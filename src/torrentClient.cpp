#include <torrentClient.hpp>
#include <torrentParser.hpp>
#include <chunkInfo.hpp>
#include <chunk.hpp>
#include <torrent.hpp>
#include <fstream>
#include <vector>

using namespace std;

namespace torrent {

    //Parse the torrent file, but don't do anything else.
    TorrentClient::TorrentClient(const std::string& torrentFile,
                                 const std::string& downloadLocation)
    : m_downloadLocation(downloadLocation), m_seeder(*this),
      m_leecher(*this), m_uploading(false)
    {
        ifstream in(torrentFile);
        if (!in) {
            //TODO: throw an error: nonexistant file.
        }
        
        try {
            m_torrent = TorrentParserUtil::parseFile(in);
        } catch (TorrentParserUtil::ParseError e) {
            //TODO: throw an error: bad file format.
        }
        in.close();
    }
    
    // TODO: Currently only handles single file torrents.
    // Assumption: the torrent file list is in chunk order.
            // This assumption can be removed by adding seeks.
    // Assumption: telling the seeder to upload something multiple times
            // or leecher to download something multiple times works properly
    int TorrentClient::start()
    {
        // If already uploading, return.
        if (m_uploading)
            return 0;
        int errVal;
        m_downloadList.clear();
        m_uploadList.clear();
        string filename = m_torrent.getName();
        //To see how far we are in the download, open the download
        // file (if it exists), start examining checksums, and based
        // on this determine whether or not to download that chunk.
        ifstream in(m_downloadLocation + filename);
        if (!in) {
            //File does not exist: put all chunkInfo in download list.
            m_downloadList = list<ChunkInfo>(m_torrent.getChunks());
        }
        else {
            size_t size = m_torrent.getPieceLength();
            char buffer[size];
            for (ChunkInfo chunkInfo : m_torrent.getChunks()) {
                //try {
                    in.read(buffer, size);
                    size_t amount_read = size;
                    if (!in) {
                        // If we reached the end of the file, we read less.
                        amount_read = in.gcount();
                    }
                    
                    // Check the checksum of this part of the file.
                    // If the checksum matches, we have already downloaded
                    // this chunk: upload instead of downloading it.
                    /*if (chunkInfo.getChunkHash() == SHA1(buffer, amount_read)) {
                        m_uploadList.push_back(
                            Chunk(chunkInfo,
                                  vector<char>(buffer, buffer + amount_read)));
                    }
                    else {*/
                        m_downloadList.push_back(chunkInfo);
                    /*}*/
                    
                //}
                //catch () {
                    //Catch IO errors, handle
                //}
            }
            in.close();
        }
        
        // We now have a list of chunks to download, and a list of chunks
        // to upload.  Tell the seeders/leechers to start downloading.
        
        if (m_downloadList.size() > 0) {
            if ((errVal = m_leecher.download(m_downloadList)) != 0)
                return errVal;
                // TODO: more reasonable behavior depending on return type.
        }
        if (m_uploadList.size() > 0) {
            if ((errVal = m_seeder.upload(m_uploadList)) != 0)
                return errVal;
            m_uploading = true;
        }
        
        return 0;
    }
    
    int TorrentClient::stop()
    {
        if (!m_uploading)
            return 0;
        int errVal;
        if ((errVal = m_leecher.stopDownload(m_downloadList)) != 0)
            return errVal;
        list<ChunkInfo> uploadMetadata;
        for (Chunk c : m_uploadList) {
            uploadMetadata.push_back(c.getMetadata());
        }
        //if ((errVal = m_seeder.stopUploading(m_uploadList)) != 0)
        if ((errVal = m_seeder.stopUploading(uploadMetadata)) != 0)
            return errVal;
        
        m_uploading = false;
        return 0;
    }
    
    void TorrentClient::chunkDownloadFail(TorrentClientProtocol::Error error,
                                          const ChunkInfo& chunkMetadata)
    {
        //TODO: intellegent behavior based on the error.
        
        // Attempt to download the chunk again.
        m_leecher.download(list<ChunkInfo>(1, chunkMetadata));
    }
    void TorrentClient::chunkDownloadSuccess(const Chunk& chunk)
    {
        ChunkInfo metadata = chunk.getMetadata();
        //Remove this chunk from our download list.
        // TODO: is this step necessary? O(n), could be chagned to O(log(n))
        for (auto iter = m_downloadList.begin(); iter != m_downloadList.end(); iter++) {
            if (iter->getChunkId() == metadata.getChunkId()) {
                m_downloadList.erase(iter);
                break;
            }
        }
        // Save chunk to disk.
        // TODO: Assumes only one file, one download location.
        ofstream out(m_downloadLocation + m_torrent.getName());
        // TODO: handle write errors.
        out.seekp(metadata.getChunkId() * m_torrent.getPieceLength());
        out.write(chunk.getBuffer().data(), chunk.getBuffer().size());
        out.close();
        // Now add the chunk to our upload list, upload it.
        // TODO: this breaks the order of the list.  Do we care?
        m_uploadList.push_back(chunk);
        m_seeder.upload(list<Chunk>(1, chunk));
    }
}
