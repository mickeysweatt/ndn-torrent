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

  	FilePiece(std::string name, size_t start, size_t end,
              size_t firstChunkId, size_t lastChunkId, size_t length);

  	~FilePiece();

  	// ACCESSORS
  	std::string getFilePieceName();

  	size_t getFilePieceLen();

  	std::pair<size_t, size_t> getFilePieceOffsets();
    
    std::pair<size_t, size_t> getFilePieceChunkRange();

  private:
  	std::string m_name;
  	size_t m_start;
  	size_t m_end;
    size_t m_firstChunkId;
    size_t m_lastChunkId;
  	size_t m_length;
};

    // INLINE ACCESSORS
    inline std::string FilePiece::getFilePieceName() { return m_name; }
    inline size_t FilePiece::getFilePieceLen() { return m_length; }
    inline std::pair<size_t, size_t> FilePiece::getFilePieceOffsets() 
    { 
        return std::make_pair(m_start, m_end); 
    }
    inline std::pair<size_t, size_t> FilePiece::getFilePieceChunkRange()
    {
        return std::make_pair(m_firstChunkId, m_lastChunkId);
    }
}
#endif // INCLUDED_FILEPIECE_HPP