#include <torrent.hpp>
#include <chunkInfo.hpp>
#include <filePiece.hpp>
#include <cmath>
#include <cstring>
#include <iostream>

namespace torrent {
    const int HASHLEN = 20;
    
	// Single File
    Torrent::Torrent(const std::unordered_set<std::string>& announceList,
                     std::string&                     name,
                     size_t                           pieceLength,
                     size_t                           fileLength,
                     const std::vector<char>&         pieces)
     : m_announceList(announceList)
     , m_name(name)
     , m_pieceLength(pieceLength)
     {
         size_t endOffset, endChunkId;
         endChunkId = ceil(fileLength/pieceLength) - 1;
         endOffset = fileLength % pieceLength;
         endOffset = endOffset ? endOffset-1 : pieceLength-1;
         FilePiece newFilePiece(name, 0, endOffset,
                                0, endChunkId, fileLength);
         
         size_t currentChunkId = 0;
         while (currentChunkId <= endChunkId)
         {
             unsigned char hash[HASHLEN];
             if (HASHLEN*(currentChunkId+1) <= sizeof(pieces))
             {
                 memcpy(hash, &(pieces[HASHLEN*currentChunkId]), HASHLEN*sizeof(char));
             }
             else
             {
                 std::cerr << "Hash length error!" << std::endl;
                 return;
             }
             ChunkInfo newChunkInfo(currentChunkId, hash);
             newChunkInfo.addFilePiece(newFilePiece);
             m_chunks.push_back(newChunkInfo);
         }
     }
    
    // Multiple Files
    Torrent::Torrent(const std::unordered_set<std::string>& announceList,
                     std::string&                           name,
                     size_t                                 pieceLength,
                     const std::list<FileTuple>&          fileTuples,
                     const std::vector<char>&               pieces)
    : m_announceList(announceList)
    , m_name(name)
    , m_pieceLength(pieceLength)
    {
        // TODO
        size_t beginOffset, endOffset;
        size_t beginChunkId, endChunkId;
        beginOffset = 0;
        beginChunkId = 0;
        for (auto tuple : fileTuples) {
            endChunkId = ceil((tuple->second() + beginOffset)/pieceLength) + beginChunkId - 1;
            endOffset = (tuple->second() + beginOffset) % pieceLength;
            endOffset = endOffset ? endOffset-1 : pieceLength-1;
            FilePiece newFilePiece(tuple->first(), beginOffset, endOffset,
                                   beginChunkId, endChunkId, tuple->first());
            
            size_t currentChunkId = beginChunkId;
            if (!m_chunks.empty() && m_chunks.back().getChunkId() == beginChunkId)
            {
                m_chunks.back().addFilePiece(newFilePiece);
                currentChunkId++;
            }
            while (currentChunkId <= endChunkId)
            {
                unsigned char hash[HASHLEN];
                if (HASHLEN*(currentChunkId+1) <= sizeof(pieces))
                {
                    memcpy(hash, &(pieces[HASHLEN*currentChunkId]), HASHLEN*sizeof(char));
                }
                else
                {
                    std::cerr << "Hash length error!" << std::endl;
                    return;
                }
                ChunkInfo newChunkInfo(currentChunkId, hash);
                newChunkInfo.addFilePiece(newFilePiece);
                m_chunks.push_back(newChunkInfo);
            }
        }
    }
    
    // MOVE
    Torrent::Torrent(std::unordered_set<std::string>&& announceList,
                     std::string&&                     name,
                     size_t                            pieceLength,
                     std::list<FileTuple>&&          files,
                     std::vector<char>&&               pieces)
    : m_announceList(announceList)
    , m_name(name)
    , m_pieceLength(pieceLength)
    {
        // TODO
    }
    
    Torrent::Torrent(Torrent&& other)
    : m_announceList(std::move(other.m_announceList)),
      m_name(std::move(other.m_name)),
      m_pieceLength(other.m_pieceLength),
      m_chunks(std::move(other.m_chunks))
    {
    }

    const std::unordered_set<std::string>& Torrent::getAnnounceURLList() const
    {
        return m_announceList;
    }
    
    const std::string& Torrent::getName() const
    {
        return m_name;
    }
    
    size_t Torrent::getPieceLength() const
    {
        return m_pieceLength;
    }
    
    const std::list<ChunkInfo>& Torrent::getChunks() const
    {
        return m_chunks;
    }
    
    // TODO: if the vector sticks around, change to const vector<...>&
    std::vector<Torrent::FileTuple> Torrent::getFileTuples() const
    {
        //TODO: a real implementation.
        return std::vector<Torrent::FileTuple>();
    }

    Torrent& Torrent::operator=(Torrent&& rhs)
    {
        if (&rhs != this) {
            m_announceList = std::move(rhs.m_announceList);
            m_name = std::move(rhs.m_name);
            m_pieceLength = rhs.m_pieceLength;
            m_chunks = std::move(rhs.m_chunks);
        }
        return *this;
    }
}
