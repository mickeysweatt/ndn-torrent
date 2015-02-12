#include "chunk.hpp"
#include "chunkInfo.hpp"
#include "seeder.hpp"

namespace torrent {

int Seeder::upload(const std::list<Chunk>& chunkDataList)
{
	 for (auto iter = chunkDataList.begin(); iter != chunkDataList.end(); iter++)
	 {
	 }
    return -1;
}

int Seeder::stopUploading(const std::list<ChunkInfo>& chunkInfoList)
{
	 for (auto iter = chunkInfoList.begin(); iter != chunkInfoList.end(); iter++)
	 {
	 }
    return -1;
}

}
