#ifndef INCLUDED_FILEPIECE_HPP
#define INCLUDED_FILEPIECE_HPP

#include <string>
#include <utility>

namespace torrent{

class FilePiece
{
  public:
  	// CREATORS
  	FilePiece() = default;

    // Normal constructor
  	FilePiece(std::string name, size_t offset, size_t length,
              size_t firstChunkId, size_t lastChunkId, size_t fileLen);
    
    // Copy constructor
    FilePiece(const FilePiece &other) = default;

  	~FilePiece();

  	// ACCESSORS
  	std::string getFilePieceName() const;

  	size_t getFileLen() const;
    
    size_t getFilePieceLen() const;

  	size_t getFilePieceOffset() const;
    
    std::pair<size_t, size_t> getFilePieceChunkRange() const;

    // MUTATOR
    void setOffset(size_t offset, size_t length);
    
  private:
  	std::string m_name;
  	size_t m_offset;
  	size_t m_length;
    size_t m_firstChunkId;
    size_t m_lastChunkId;
  	size_t m_fileLen;
};

    // INLINE ACCESSORS
    inline std::string FilePiece::getFilePieceName() const { return m_name; }
    
    inline size_t FilePiece::getFileLen() const { return m_fileLen; }
    
    inline size_t FilePiece::getFilePieceLen() const { return m_length; }
    
    inline size_t FilePiece::getFilePieceOffset()  const { return m_offset; }
    
    inline std::pair<size_t, size_t> FilePiece::getFilePieceChunkRange() const
    {
        return std::make_pair(m_firstChunkId, m_lastChunkId);
    }
}
#endif // INCLUDED_FILEPIECE_HPP
