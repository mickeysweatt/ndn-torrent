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
  	FilePiece(std::string name, size_t start, size_t end,
              size_t firstChunkId, size_t lastChunkId, size_t length);
    
    // Copy constructor
    FilePiece(const FilePiece &other) = default;

  	~FilePiece();

  	// ACCESSORS
  	std::string getFilePieceName() const;

  	size_t getFileLen() const;
    
    size_t getFilePieceLen() const;

  	std::pair<size_t, size_t> getFilePieceOffsets() const;
    
    std::pair<size_t, size_t> getFilePieceChunkRange() const;

    // MUTATOR
    void setOffsets(size_t start, size_t end);
    
  private:
  	std::string m_name;
  	size_t m_start;
  	size_t m_end;
    size_t m_firstChunkId;
    size_t m_lastChunkId;
  	size_t m_length;
};

    // INLINE ACCESSORS
    inline std::string FilePiece::getFilePieceName() const { return m_name; }
    
    inline size_t FilePiece::getFileLen() const { return m_length; }
    
    inline size_t FilePiece::getFilePieceLen() const { return m_end - m_start + 1; }
    
    inline std::pair<size_t, size_t> FilePiece::getFilePieceOffsets()  const
    { 
        return std::make_pair(m_start, m_end); 
    }
    inline std::pair<size_t, size_t> FilePiece::getFilePieceChunkRange() const
    {
        return std::make_pair(m_firstChunkId, m_lastChunkId);
    }
}
#endif // INCLUDED_FILEPIECE_HPP
