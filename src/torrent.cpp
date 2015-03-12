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
         size_t index, length, accrued, endChunkId; // index is for place in chunk
         index = 0;
         accrued = 0;
         endChunkId = ceil(static_cast<double>(fileLength)/pieceLength) - 1;
         
         size_t currentChunkId = 0; // beginChunkId is always 0
         
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
             // Update offset in file for the current chunk
             if (currentChunkId == endChunkId)
                 length = fileLength - accrued;
             else
                 length = pieceLength - index;
             
             FilePiece newFilePiece(name, accrued, length,
                                    0, endChunkId, fileLength);
             ChunkInfo newChunkInfo(currentChunkId, hash);
             newChunkInfo.addFilePiece(newFilePiece);
             m_chunks.push_back(newChunkInfo);
             
             // Update variables
             currentChunkId++;
             accrued += length;
             index = accrued % pieceLength;
         }

         singleFileOffsetsTester(name, fileLength, pieceLength, *this);
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
        size_t index, length, accrued, beginChunkId, endChunkId; // index is for place in chunk
        index = 0;
        beginChunkId = endChunkId = 0;
        
        for (auto& tuple : fileTuples) {
            accrued = 0;
            endChunkId = ceil((tuple.second + index)/static_cast<double>(pieceLength))
                         + beginChunkId - 1;
            
            size_t currentChunkId = beginChunkId;
            
            // Update offset in file for the first chunk
            if (currentChunkId == endChunkId)
                length = tuple.second - accrued;
            else
                length = pieceLength - index;
            
            FilePiece newFilePiece(tuple.first, accrued, length,
                                   beginChunkId, endChunkId, tuple.second);

            if (!m_chunks.empty() && m_chunks.back().getChunkId() == beginChunkId)
            {
                m_chunks.back().addFilePiece(newFilePiece);
                currentChunkId++;
                accrued += length;
                index = (index + accrued) % pieceLength;
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
                // Update offset in file for the current chunk
                if (currentChunkId == endChunkId)
                    length = tuple.second - accrued;
                else
                    length = pieceLength - index;

                newFilePiece.setOffset(accrued, length);
                ChunkInfo newChunkInfo(currentChunkId, hash);
                newChunkInfo.addFilePiece(newFilePiece);
                m_chunks.push_back(newChunkInfo);
                
                // Update variables
                currentChunkId++;
                accrued += length;
                index = (index + length) % pieceLength;
            }
            beginChunkId = (index == 0)? endChunkId+1 : endChunkId;
        }

        multipleFileOffsetsTester(fileTuples, pieceLength, *this);
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
        size_t beginOffset, endOffset;
        size_t beginChunkId, endChunkId;
        beginOffset = 0;
        beginChunkId = endChunkId = 0;
        
        for (auto& tuple : fileTuples) {
            endChunkId = ceil((tuple.second + beginOffset)/static_cast<double>(pieceLength))
            + beginChunkId - 1;
            if (beginChunkId == endChunkId)
                endOffset = beginOffset + tuple.second - 1;
            else
                endOffset = pieceLength - 1;
            
            FilePiece newFilePiece(tuple.first, beginOffset, endOffset,
                                   beginChunkId, endChunkId, tuple.second);
            
            size_t currentChunkId = beginChunkId;
            size_t accrued = 0;
            if (!m_chunks.empty() && m_chunks.back().getChunkId() == beginChunkId)
            {
                m_chunks.back().addFilePiece(newFilePiece);
                currentChunkId++;
                accrued += endOffset - beginOffset + 1;
                beginOffset += accrued % pieceLength;
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
                // Update offsets for the current chunk
                if (currentChunkId == endChunkId)
                    endOffset = beginOffset + tuple.second - accrued - 1;
                else
                    endOffset = pieceLength - 1;
                
                // newFilePiece.setOffsets(beginOffset, endOffset);
                ChunkInfo newChunkInfo(currentChunkId, hash);
                newChunkInfo.addFilePiece(newFilePiece);
                m_chunks.push_back(newChunkInfo);
                
                // Update variables
                currentChunkId++;
                accrued += endOffset - beginOffset + 1;
                beginOffset = (endOffset + 1) % pieceLength;
            }
            beginOffset = (endOffset + 1) % pieceLength;
            beginChunkId = (beginOffset == 0)? endChunkId+1 : endChunkId;
        }
        
        multipleFileOffsetsTester(fileTuples, pieceLength, *this);
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
        }
        
        return s;
    }
    
    bool singleFileOffsetsTester(const std::string& name, const size_t fileLength,
                                        const size_t& pieceLength, const Torrent& t)
    {
        size_t accrued = 0;
        size_t prev_len = 0;
        size_t prev_offset = 0;
        for (auto chunkInfo : t.m_chunks)
        {
            size_t id = chunkInfo.getChunkId();
            for (auto filePiece : chunkInfo.getFilePieceList())
            {
                assert(filePiece.getFilePieceName() == name);
                std::pair<size_t, size_t> cr = filePiece.getFilePieceChunkRange();
                assert(cr.first <= id && cr.second >= id);
                accrued += filePiece.getFilePieceLen();
                if (prev_len != 0)
                {
                    assert( (filePiece.getFilePieceOffset() - prev_offset) == prev_len );
                }
                prev_len = filePiece.getFilePieceLen();
                prev_offset = filePiece.getFilePieceOffset();
            }
        }
        assert(fileLength == accrued);
        
        return fileLength == accrued;
    }
    
    bool multipleFileOffsetsTester(const std::list<Torrent::FileTuple>& fileTuples, const size_t&                            pieceLength, const Torrent& t)
    {
        std::map<std::string, size_t> fileLen;
        // std::cout << t.m_chunks.size() << std::endl;
        
        std::string prev_name;
        size_t prev_len = 0;
        size_t prev_offset = 0;
        for (auto chunkInfo : t.m_chunks)
        {
            size_t id = chunkInfo.getChunkId();
            for (auto filePiece : chunkInfo.getFilePieceList())
            {
                std::pair<size_t, size_t> cr = filePiece.getFilePieceChunkRange();
                assert(cr.first <= id && cr.second >= id);
                size_t length = filePiece.getFilePieceLen();
                auto it = fileLen.find(filePiece.getFilePieceName());
                if (it != fileLen.end())
                {
                    it->second += filePiece.getFilePieceLen();
                }
                else
                    fileLen[filePiece.getFilePieceName()] = length;
                if (prev_name == filePiece.getFilePieceName() && prev_len != 0)
                {
                    std::cout << "length: " << filePiece.getFilePieceOffset() - prev_offset << std::endl;
                    std::cout << "        " << prev_len << std::endl;
                    assert( (filePiece.getFilePieceOffset() - prev_offset) == prev_len );
                }
                prev_len = filePiece.getFilePieceLen();
                prev_offset = filePiece.getFilePieceOffset();
                prev_name = filePiece.getFilePieceName();
            }
            
        }
        
        for (auto tuple : fileTuples)
        {
            std::cout << tuple.first << ": " << tuple.second << std::endl
                      << "torrent: " << fileLen[tuple.first] << std::endl;
            assert(fileLen[tuple.first] == tuple.second);
        }
        
        return true;
    }
}
