#include <torrentClient.hpp>
#include <torrentParserUtil.hpp>
#include <chunkInfo.hpp>
#include <chunk.hpp>
#include <torrent.hpp>
#include <fstream>
#include <vector>
#include <utility>
#include <sha1hash.hpp>
#include <boost/filesystem.hpp>

using namespace std;

namespace torrent {
// TODO: Chunks now know what their files are: change code to reflect this.
// REVIEW: Re-order to match order of declaration
    //Parse the torrent file, but don't do anything else.
    TorrentClient::TorrentClient(const string& torrentFile,
                                 const string& downloadLocation,
                                 std::function<void(const Torrent&)>& onFinishDownload)
    : m_downloadLocation(downloadLocation),
    m_seeder(nullptr),
    m_leecher(nullptr),
    m_uploading(false),
    m_onFinishDownload(onFinishDownload)
    {
        ifstream in(torrentFile);
        if (!in) {
            throw FileNotFound(torrentFile);
        }
        
        try {
            m_torrent = TorrentParserUtil::parseFile(in);
        } catch (TorrentParserUtil::ParseError& e) {
            throw BadTorrentFile(torrentFile + " has a bad format.");
        }
        in.close();
        
        // Add a trailing slash to the download location if it is missing.
        if (m_downloadLocation.back() != '/') {
            m_downloadLocation += "/";
        }
        // Now, add a directory to put the download in.
        m_downloadLocation += m_torrent.getName() + "/";
        // Check that this download location exists: if not, create it.
        if (!boost::filesystem::exists(m_downloadLocation)) {
            boost::filesystem::path path(m_downloadLocation);
            boost::filesystem::create_directories(
                boost::filesystem::absolute(path));
        }
        
        // Now that we have parsed the file, we can get the real name of
        // the torrent.
        cout << "Announcing/downloading from prefix\n"
             << "/ndn/edu/ucla/torrent/" << m_torrent.getName() << "/" << endl;

        m_seeder = new Seeder(ndn::Name("/ndn/edu/ucla/torrent/" + m_torrent.getName()), *this);
        m_leecher = new Leecher(ndn::Name("/ndn/edu/ucla/torrent/" + m_torrent.getName()), *this);
    }
    
    TorrentClient::~TorrentClient()
    {
        if (m_seeder != nullptr) {
            delete m_seeder;
        }
        if (m_leecher != nullptr) {
            delete m_leecher;
        }
    }
    
    // Assumption: the torrent file list is in chunk order.
            // This assumption can be removed by adding seeks.
    // Assumption: telling the seeder to upload something multiple times
            // or leecher to download something multiple times works properly
    int TorrentClient::start()
    {
        if (m_uploading) {
            return 0;
        }
        int errVal;
        m_downloadList.clear();
        m_uploadList.clear();
        string openFile;
        ifstream in;
        
        char* readBuffer = new char[m_torrent.getPieceLength()];
        
        //cout << "Max piece length: " << m_torrent.getPieceLength() << "\n";
        
        for (const ChunkInfo& chunk : m_torrent.getChunks())
        {
            bool skip_chunk = false;
            size_t chunk_offset = 0;
            
            // TODO:
            // We will repeatedly try to open the same file multiple times
            // if it does not exist.
            for (const FilePiece& file : chunk.getFilePieceList())
            {
                /* cout << "Chunk: " << chunk.getChunkId() << " File: "
                     << file.getFilePieceName() << " Offsets: "
                     << file.getFilePieceLen() << "("
                     << file.getFilePieceOffset() << ","
                     << (file.getFilePieceOffset() + file.getFilePieceLen())
                     << ")\n"; */
                // If we are now processing a different file, close current
                // file and open the next one.
                if (!in.is_open() || openFile != file.getFilePieceName()) {
                    if (in.is_open()) {
                        in.close();
                    }
                    in.open(m_downloadLocation + file.getFilePieceName());
                    if(!in) { // If file does not exist
                        // Skip this entire chunk.
                        skip_chunk = true;
                        break;
                    }
                    openFile = file.getFilePieceName();
                }
                
                // Read in the appropriate amount from this file.
                size_t read_amount =
                    min(m_torrent.getPieceLength() - chunk_offset,
                        file.getFilePieceLen());
                if (read_amount > m_torrent.getPieceLength()) {
                    cout << "Warning: chunk " << chunk.getChunkId()
                         << " exceeds maximum file length.\n"; 
                    read_amount = m_torrent.getPieceLength();
                }
                in.read(readBuffer + chunk_offset, read_amount);
                // If there is an error reading or incorrect number of bits
                // skip the chunk.
                if (!in || in.gcount() != read_amount) {
                    skip_chunk = true;
                    break;
                }
                
                chunk_offset += read_amount;
                //cout << "Read " << chunk_offset << " into chunk " << chunk.getChunkId() << "\n";
            }
            // If we did not successfully read the entire chunk,
            // then don't try to compute the checksums.
            
            // Otherwise, compare the checksum.
            if (!skip_chunk  && chunk.getChunkHash() == SHA1Hash(
                    reinterpret_cast<unsigned char*>(readBuffer), chunk_offset)) {
                m_uploadList.push_back(
                Chunk(chunk, vector<char>(readBuffer, readBuffer + chunk_offset)));
            }
            else {
                m_downloadList.push_back(std::move(chunk));
            }
        }
        
        if (in.is_open()) {
            in.close();
        }
        delete[] readBuffer;
        
        
        // We now have a list of chunks to download, and a list of chunks
        // to upload.  Tell the seeders/leechers to start downloading.
        
        if (m_uploadList.size() > 0) {
            if ((errVal = m_seeder->upload(m_uploadList)) != 0)
                return errVal;
            m_uploading = true;
        }
        if (m_downloadList.size() > 0) {
            if ((errVal = m_leecher->download(m_downloadList)) != 0)
                return errVal;
                // TODO: more reasonable behavior depending on return type.
        }
        else
        {
            // If everything already downloaded, notify the UI.
            m_onFinishDownload(m_torrent);
        }
        
        return 0;
    }
    
    int TorrentClient::stop()
    {
        if (!m_uploading)
            return 0;
        int errVal;
        if ((errVal = m_leecher->stopDownload(m_downloadList)) != 0)
            return errVal;
        list<ChunkInfo> uploadMetadata;
        for (Chunk& c : m_uploadList) {
            uploadMetadata.push_back(c.getMetadata());
        }
        if ((errVal = m_seeder->stopUploading(uploadMetadata)) != 0)
            return errVal;
        
        m_uploading = false;
        return 0;
    }
    
    void TorrentClient::chunkDownloadFail(TorrentClientProtocol::Error error,
                                          const ChunkInfo& chunkMetadata)
    {
        //TODO: intellegent behavior based on the error.
        
        // Attempt to download the chunk again.
        cout << "Failure to download of chunk " << chunkMetadata.getChunkId() << "\n";

        m_leecher->download(chunkMetadata);

    }
    void TorrentClient::chunkDownloadSuccess(const Chunk& chunk)
    {
        const ChunkInfo& metadata = chunk.getMetadata();
        //Remove this chunk from our download list.
        // TODO: is this step necessary? O(n), could be chagned to O(log(n))
        cout << "Successful download of chunk " << chunk.getMetadata().getChunkId() << "\n";
        for (auto iter = m_downloadList.begin(); iter != m_downloadList.end(); iter++) {
            if (iter->getChunkId() == metadata.getChunkId()) {
                m_downloadList.erase(iter);
                break;
            }
        }
        // Save chunk to disk.
        size_t chunk_offset = 0;
        size_t chunk_size = chunk.getBuffer().size();
        list<FilePiece> files = metadata.getFilePieceList();
        ofstream out;
        for (const FilePiece& file : files) {
            size_t write_amount = min(file.getFilePieceLen(),
                                      chunk_size - chunk_offset);
            out.open(m_downloadLocation + file.getFilePieceName(), ios_base::app);
            //TODO: handle write errors
            out.seekp(file.getFilePieceOffset());
            out.write(chunk.getBuffer().data() + chunk_offset, write_amount);
            out.close();
            chunk_offset += write_amount;
        }
        // TODO: this breaks the order of the list.  Do we care?
        m_uploadList.push_back(chunk);
        m_seeder->upload(chunk);
        
        if (m_downloadList.empty())
        {
            // The download has completed: let the UI know.
            m_onFinishDownload(m_torrent);
        }
    }
}



// REVIEW
// As a side note your lack of curly braces makes me sad, but that is purely
// style and thus does not matter since you are more than capable of knowing 
// when you need them.
