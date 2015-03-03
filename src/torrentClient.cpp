#include <torrentClient.hpp>
#include <torrentParserUtil.hpp>
#include <chunkInfo.hpp>
#include <chunk.hpp>
#include <torrent.hpp>
#include <fstream>
#include <vector>
#include <utility>
#include <sha1hash.hpp>

using namespace std;

typedef pair<string, size_t> FileTuple;

namespace torrent {
// REVIEW: Re-order to match order of declaration
    //Parse the torrent file, but don't do anything else.
    TorrentClient::TorrentClient(const string& torrentFile,
                                 const string& downloadLocation)
    : m_downloadLocation(downloadLocation), m_seeder(*this),
    m_leecher(ndn::Name("FIXME"), *this), m_uploading(false)
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
    }
    
    // Assumption: the torrent file list is in chunk order.
            // This assumption can be removed by adding seeks.
    // Assumption: telling the seeder to upload something multiple times
            // or leecher to download something multiple times works properly
    // Assumption: None of the files to be downloaded/uploaded are empty.
    int TorrentClient::start()
    {
        // If already uploading, return.
        if (m_uploading)
            return 0;
        int errVal;
        m_downloadList.clear();
        m_uploadList.clear();
        //string filename = m_torrent.getName();
        vector<FileTuple> filenames
                = m_torrent.getFileTuples();
        vector<FileTuple>::iterator current
                = filenames.begin();
        vector<FileTuple>::iterator end
                = filenames.end();
        size_t total_read = 0; // Total amount of the torrent file we read.
        // Amount to read before we must switch to the next file.
        size_t file_limit = current->second;
        // Amount we have read in this file.
        size_t file_offset = 0;
        // Amount we have read of the current chunk
            // (only used for chunks that span multiple files
        size_t chunk_offset = 0;
        size_t size = m_torrent.getPieceLength();
        char *buffer = new char[size];
        list<ChunkInfo> chunks = m_torrent.getChunks();
        
        //To see how far we are in the download, open the download
        // file (if it exists), start examining checksums, and based
        // on this determine whether or not to download that chunk.
        ifstream in(m_downloadLocation + current->first);
        
// REVIEW: You do not seem to mutate this object, make this a const reference
        for (list<ChunkInfo>::iterator curChunk = chunks.begin();
                curChunk != chunks.end(); curChunk++) {
            // This loop deals with all cases where we must skip/
            // move on to the next file.`
            // If file does not exist or we went past the end of the file:
            while (!in || total_read >= file_limit) {
                in.close();
                //Only need to skip chunks if the file does not exist.
                //Otherwise, keep chunk_offset, etc same.
                if (total_read < file_limit) {
                    // Skip all chunks, bytes that were from this file.
                    // Calculate index of next byte to read.
                    size_t curChunkIndex = total_read / size;
                    size_t nextChunkIndex = (file_limit - 1) / size + 1;
                    size_t nextByte = nextChunkIndex * size;
                    for (auto j = curChunkIndex; j < nextChunkIndex; j++) {
                        // These chunks need to be downloaded.
                        m_downloadList.push_back(*curChunk);
                        curChunk++;
                    }
                    // file_offset can be nonzero if the skipped chunk
                    // included some bytes of this file.
                    file_offset = nextByte - file_limit;
                    chunk_offset = 0;
                    total_read = nextByte;
                }
                else {
                    // If we are in the middle of a chunk, and opened a new
                    // file to read the rest of this chunk, file_offset
                    // should be zero.
                    file_offset = 0;
                    if (chunk_offset > 0) {
                        // We already incremented curChunk due to the for loop,
                        // but we are not through processing it.
                        curChunk--;
                    }
                }
                // Skip to the next file.
                current++;
                if (current == end) {
                    break; //Exit while loop, then break out of for loop.
                }
                
                file_limit += current->second;
                in.open(m_downloadLocation + current->first);
                // The while loop will check if the condition is properly met.
            }
            if (current == end) {
                // If there are no more files to read, exit for loop.
                break;
            }
            //try {
                in.read(buffer + chunk_offset, size - chunk_offset);
                size_t amount_read = size;
                if (!in) {
                    // If we reached the end of the file, we read less.
                    amount_read = in.gcount();
                }
                
                chunk_offset += amount_read;
                file_offset += amount_read;
                total_read += amount_read;
                // Only compare checksums if we have the whole chunk:
                // This is either if we have read size bytes or reach the
                // end of the last file.
                if (chunk_offset == size || (current + 1) == end) {
                    // Check the checksum of this part of the file.
                    // If the checksum matches, we have already downloaded
                    // this chunk: upload instead of downloading it.
// REVIEW: C-style casts are the devil, static_cast instead
                    if (curChunk->getChunkHash()
                            == SHA1Hash(reinterpret_cast<unsigned char*>(buffer), amount_read)) {
                        m_uploadList.push_back(
                            Chunk(*curChunk, vector<char>(buffer, buffer + amount_read)));
                    }
                    else {
                        m_downloadList.push_back(*curChunk);
                    }
                    chunk_offset = 0;
                }
            //}
            //catch () {
                //Catch IO errors, handle them
                //Close input stream, delete buffer if returning/throwing.
            //}
        }
        if (in.is_open()) {
            in.close();
        }
        delete[] buffer;
        
        // We now have a list of chunks to download, and a list of chunks
        // to upload.  Tell the seeders/leechers to start downloading.
        
        if (m_uploadList.size() > 0) {
            if ((errVal = m_seeder.upload(m_uploadList)) != 0)
                return errVal;
            m_uploading = true;
        }
        if (m_downloadList.size() > 0) {
            if ((errVal = m_leecher.download(m_downloadList)) != 0)
                return errVal;
                // TODO: more reasonable behavior depending on return type.
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
        for (Chunk& c : m_uploadList) {
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
// REVIEW: Since this is so common, we should add API support for single chunk
        m_leecher.download(list<ChunkInfo>(1, chunkMetadata));
    }
    void TorrentClient::chunkDownloadSuccess(const Chunk& chunk)
    {
        const ChunkInfo& metadata = chunk.getMetadata();
        //Remove this chunk from our download list.
        // TODO: is this step necessary? O(n), could be chagned to O(log(n))
        for (auto iter = m_downloadList.begin(); iter != m_downloadList.end(); iter++) {
            if (iter->getChunkId() == metadata.getChunkId()) {
                m_downloadList.erase(iter);
                break;
            }
        }
        // Save chunk to disk.
        // First, figure out what bytes these are in our torrent.
        size_t start = metadata.getChunkId() * m_torrent.getPieceLength();
        size_t end = start + chunk.getBuffer().size();
        size_t current_byte = 0;
        size_t chunk_offset = 0;
        vector<FileTuple> files = m_torrent.getFileTuples();
        ofstream out;
        for (unsigned int i = 0; i < files.size(); i++) {
            // Skip files not containing our chunk.
            if (current_byte + files[i].second <= start) {
                current_byte += files[i].second;
                continue;
            }
            size_t write_amount = min(files[i].second, end - start - chunk_offset);
            out.open(m_downloadLocation + files[i].first);
            // TODO: handle write errors.
            out.seekp(start - current_byte);
            out.write(chunk.getBuffer().data() + chunk_offset, write_amount);
            out.close();
            chunk_offset += write_amount;
            if (start + chunk_offset >= end)
                break;
        }
        // Now add the chunk to our upload list, upload it.
        // TODO: this breaks the order of the list.  Do we care?
        m_uploadList.push_back(chunk);
// REVIEW: HERE TOO
        m_seeder.upload(list<Chunk>(1, chunk));
    }
}



// REVIEW
// As a side note your lack of curly braces makes me sad, but that is purely
// style and thus does not matter since you are more than capable of knowing 
// when you need them.
