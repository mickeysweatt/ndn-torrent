#include <ndn-cxx/contexts/producer-context.hpp>
#include <ndn-cxx/contexts/context-options.hpp>
#include <ndn-cxx/name.hpp>

#include <chunk.hpp>
#include <chunkInfo.hpp>
#include <seeder.hpp>

#include <list>
#include <functional>
#include <memory>
#include <string>

namespace torrent {

int Seeder::upload(const std::list<Chunk>& chunkDataList)
{
     int retErrorCode = NO_ERROR;
     int errorCode;
// REVIEW: ++iter, no reason to waste a copy
     for (auto iter = chunkDataList.begin(); iter != chunkDataList.end(); iter++)
     {
            if ((errorCode = upload(*iter)) != NO_ERROR)
                 retErrorCode = errorCode;
     }
     return retErrorCode;
}

int Seeder::upload(const Chunk& chunk)
{
     size_t id = chunk.getMetadata().getChunkId();
     ndn::Name chunkName(m_prefix + std::to_string(id));
     std::unique_ptr<ndn::Producer> chunkProducer(new ndn::Producer(chunkName));
// REVIEW: c-style casts are evil, use static_cast instead.
     chunkProducer->setContextOption(INTEREST_ENTER_CNTX,
                                     (ndn::ConstInterestCallback)std::bind(&Seeder::onInterest, this, _1));
     chunkProducer->setContextOption(CACHE_MISS,
                                     (ndn::ConstInterestCallback)std::bind(&Seeder::onCacheMiss, this, _1));
     chunkProducer->attach();

     m_producers.insert(std::make_pair(id, std::move(chunkProducer)));
     m_chunks.insert(std::make_pair(id, chunk));
     return NO_ERROR;
}

int Seeder::stopUploading(const std::list<ChunkInfo>& chunkInfoList)
{
     int retErrorCode = NO_ERROR;
     int errorCode;
// REVIEW: ++iter, no reason to waste a copy
     for (auto iter = chunkInfoList.begin(); iter != chunkInfoList.end(); iter++)
     {
            if ((errorCode = stopUploading(*iter)) != NO_ERROR)
                 retErrorCode = errorCode;
     }
     return retErrorCode;
}

int Seeder::stopUploading(const std::list<Chunk>& chunkList)
{
     int retErrorCode = NO_ERROR;
     int errorCode;
 // REVIEW: ++iter, no reason to waste a copy
     for (auto iter = chunkList.begin(); iter != chunkList.end(); iter++)
     {
            if ((errorCode = stopUploading(*iter)) != NO_ERROR)
                 retErrorCode = errorCode;
     }
     return retErrorCode;
}

int Seeder::stopUploading(const ChunkInfo& chunkInfo)
{
     size_t id = chunkInfo.getChunkId();
     if (!m_producers.count(id))
            return CHUNK_NOT_FOUND;
// REVIEW: We can ask about this if you are concerned?
     /// TODO: Doesn't seem like CP API actually detaches producer from network,
     /// may need a workaround if we want better performance
     m_producers.erase(id);
     m_chunks.erase(id);

     return NO_ERROR;
}

int Seeder::stopUploading(const Chunk& chunk)
{
     return stopUploading(chunk.getMetadata());
}

void Seeder::onInterest(const ndn::Interest& interest)
{
     // onCacheMiss seems to need this function to work properly, even if it does
     // nothing
     return;
}

void Seeder::onCacheMiss(const ndn::Interest& interest)
{
     const ndn::Name name = interest.getName();
     size_t lastComponentIndex = name.size() - 1;
     size_t id = atoi(name.get(lastComponentIndex).toUri().c_str());

     // Drop interest if we don't have a suitable producer
     if (!m_chunks.count(id))
            return;
            
     const std::vector<char>& buffer = m_chunks[id].getBuffer();
     std::unique_ptr<ndn::Producer>& producer = m_producers[id];
// REVIEW: c-style casts are evil, use static_cast instead.
     producer->produce(ndn::Name(), (uint8_t*)buffer.data(), buffer.size());
}

}
