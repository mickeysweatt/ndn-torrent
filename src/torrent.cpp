#include <torrent.hpp>
#include <chunkInfo.hpp>
#include <filePiece.hpp>
#include <cmath>
#include <cstring>
#include <iostream>
#include <cassert>
#include <utility>
#include <map>

namespace torrent {
    using std::endl;
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
     , m_completeHash(pieces)
     {
         size_t endOffset, endChunkId;
         endChunkId = ceil(static_cast<double>(fileLength)/pieceLength) - 1;
         endOffset = fileLength % pieceLength;
         endOffset = endOffset ? endOffset-1 : pieceLength-1;
         FilePiece newFilePiece(name, 0, endOffset,
                                0, endChunkId, fileLength);
         size_t currentChunkId = 0;
         while (currentChunkId <= endChunkId)
         {
             unsigned char hash[HASHLEN];
             if (HASHLEN*(currentChunkId+1) <= pieces.size())
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
             currentChunkId++;
         }
         std::cout << this;
     }
    
    // Multiple Files
    Torrent::Torrent(const std::unordered_set<std::string>& announceList,
                     std::string&                           name,
                     size_t                                 pieceLength,
                     const std::list<FileTuple>&            fileTuples,
                     const std::vector<char>&               pieces)
    : m_announceList(announceList)
    , m_name(name)
    , m_pieceLength(pieceLength)
    , m_completeHash(pieces)
    {
        // TODO
        size_t beginOffset, endOffset;
        size_t beginChunkId, endChunkId;
        beginOffset = 0;
        beginChunkId = endChunkId = 0;
        for (auto& tuple : fileTuples) {
            endChunkId = ceil((tuple.second + beginOffset)/static_cast<double>(pieceLength))
                         + beginChunkId - 1;
            endOffset = (tuple.second + beginOffset) % pieceLength;
            endOffset = endOffset ? endOffset-1 : pieceLength-1;
            FilePiece newFilePiece(tuple.first, beginOffset, endOffset,
                                   beginChunkId, endChunkId, tuple.second);
            
            size_t currentChunkId = beginChunkId;
            if (!m_chunks.empty() && m_chunks.back().getChunkId() == beginChunkId)
            {
                m_chunks.back().addFilePiece(newFilePiece);
                currentChunkId++;
            }
            while (currentChunkId <= endChunkId)
            {
                unsigned char hash[HASHLEN];
                if (HASHLEN*(currentChunkId+1) <= pieces.size())
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
                currentChunkId++;
            }
            beginOffset = (endOffset + 1) % pieceLength;
            beginChunkId = (beginOffset == 0)? endChunkId+1 : endChunkId;
            
        }
        // fileOffsetsTester(fileTuples, pieceLength, *this);
    }
    // MOVE
    Torrent::Torrent(std::unordered_set<std::string>&& announceList,
                     std::string&&                     name,
                     size_t                            pieceLength,
                     std::list<FileTuple>&&            fileTuples,
                     std::vector<char>&&               pieces)
    : m_announceList(announceList)
    , m_name(name)
    , m_pieceLength(pieceLength)
    , m_completeHash(pieces)
    {
        //TODO
        size_t beginOffset, endOffset;
        size_t beginChunkId, endChunkId;
        beginOffset = 0;
        beginChunkId = 0;
        for (auto& tuple : fileTuples) {
            endChunkId = ceil((tuple.second + beginOffset)/pieceLength) + beginChunkId - 1;
            endOffset = (tuple.second + beginOffset) % pieceLength;
            endOffset = endOffset ? endOffset-1 : pieceLength-1;
            FilePiece newFilePiece(tuple.first, beginOffset, endOffset,
                                   beginChunkId, endChunkId, tuple.second);
            
            size_t currentChunkId = beginChunkId;
            if (!m_chunks.empty() && m_chunks.back().getChunkId() == beginChunkId)
            {
                m_chunks.back().addFilePiece(newFilePiece);
                currentChunkId++;
            }
            while (currentChunkId <= endChunkId)
            {
                unsigned char hash[HASHLEN];
                if (HASHLEN*(currentChunkId+1) <= pieces.size())
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
                currentChunkId++;
            }
        }

    }
    
    Torrent::Torrent(Torrent&& other)
    : m_announceList(std::move(other.m_announceList)),
      m_name(std::move(other.m_name)),
      m_pieceLength(other.m_pieceLength),
      m_chunks(std::move(other.m_chunks)),
      m_completeHash(std::move(other.m_completeHash))
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

    Torrent& Torrent::operator=(Torrent&& rhs)
    {
        if (&rhs != this) {
            m_announceList = std::move(rhs.m_announceList);
            m_name = std::move(rhs.m_name);
            m_pieceLength = rhs.m_pieceLength;
            m_chunks = std::move(rhs.m_chunks);
            m_completeHash = std::move(rhs.m_completeHash);
        }
        return *this;
    }
    
    std::ostream& operator<<(std::ostream& s, const Torrent& t)
    {
        s << "Torrent name: " << t.m_name << endl
          << "Piece length: " << t.m_pieceLength << endl;
        auto it = t.m_completeHash.data();
        for (auto chunkInfo : t.m_chunks)
        {
            assert(0 == memcmp(chunkInfo.getChunkHash().getHash(), it, 20));
            it += 20;
//            size_t id = chunkInfo.getChunkId();
//            for (auto filePiece : chunkInfo.getFilePieceList())
//            {
//                std::pair<size_t, size_t> cr = filePiece.getFilePieceChunkRange();
//                std::pair<size_t, size_t> offset = filePiece.getFilePieceOffsets();
//            }
        }
        
        return s;
    }
    
    bool fileOffsetsTester(const std::list<Torrent::FileTuple>& fileTuples, const size_t&                            pieceLength, const Torrent& t)
    {
        std::map<std::string, size_t> fileLen;
        std::cout << t.m_chunks.size();
        for (auto chunkInfo : t.m_chunks)
        {
            size_t id = chunkInfo.getChunkId();
            for (auto filePiece : chunkInfo.getFilePieceList())
            {
                std::pair<size_t, size_t> cr = filePiece.getFilePieceChunkRange();
                assert(cr.first <= id && cr.second >= id);
                std::pair<size_t, size_t> offset = filePiece.getFilePieceOffsets();
                size_t length = offset.second - offset.first + 1;
                auto it = fileLen.find(filePiece.getFilePieceName());
                if (it != fileLen.end())
                {
                    it->second += length;
                }
                else
                    fileLen[filePiece.getFilePieceName()] = length;
            }
        }
        
        for (auto tuple : fileTuples)
        {
//            std::cout << 'torrent: ' << fileLen[tuple.first] == tuple.second
//            assert(fileLen[tuple.first] == tuple.second);
        }
        
        return false;
    }
}
