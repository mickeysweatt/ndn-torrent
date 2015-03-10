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

Seeder::SeederError Seeder::upload(const std::list<Chunk>& chunkDataList)
{
   SeederError retErrorCode = NO_ERROR;
   SeederError errorCode;

   for (auto iter = chunkDataList.begin(); iter != chunkDataList.end(); ++iter)
   {
      if ((errorCode = upload(*iter)) != NO_ERROR)
         retErrorCode = errorCode;
   }
   return retErrorCode;
}

Seeder::SeederError Seeder::upload(std::list<Chunk>&& chunkDataList)
{
   SeederError retErrorCode = NO_ERROR;
   SeederError errorCode;

   for (auto iter = chunkDataList.begin(); iter != chunkDataList.end(); ++iter)
   {
      if ((errorCode = upload(std::move(*iter))) != NO_ERROR)
         retErrorCode = errorCode;
   }
   return retErrorCode;
}

Seeder::SeederError Seeder::upload(const Chunk& chunk)
{
   size_t id = chunk.getMetadata().getChunkId();
   ndn::Name chunkName(m_prefix + std::to_string(id));
   std::unique_ptr<ndn::Producer> chunkProducer(new ndn::Producer(chunkName));
   chunkProducer->setContextOption(INTEREST_ENTER_CNTX,
                                   static_cast<ndn::ProducerInterestCallback>(std::bind(&Seeder::onInterest, this, _1, _2)));
   chunkProducer->setContextOption(CACHE_MISS,
                                   static_cast<ndn::ProducerInterestCallback>(std::bind(&Seeder::onCacheMiss, this, _1, _2)));
   chunkProducer->attach();

   m_producers.insert(std::make_pair(id, std::move(chunkProducer)));
   m_chunks.insert(std::make_pair(id, chunk));
   return NO_ERROR;
}

Seeder::SeederError Seeder::upload(Chunk&& chunk)
{
   size_t id = chunk.getMetadata().getChunkId();
   ndn::Name chunkName(m_prefix + std::to_string(id));
   std::unique_ptr<ndn::Producer> chunkProducer(new ndn::Producer(chunkName));
   chunkProducer->setContextOption(INTEREST_ENTER_CNTX,
                                   static_cast<ndn::ProducerInterestCallback>(std::bind(&Seeder::onInterest, this, _1, _2)));
   chunkProducer->setContextOption(CACHE_MISS,
                                   static_cast<ndn::ProducerInterestCallback>(std::bind(&Seeder::onCacheMiss, this, _1, _2)));
   chunkProducer->attach();

   m_producers.insert(std::make_pair(id, std::move(chunkProducer)));
   m_chunks.insert(std::make_pair(id, std::move(chunk)));
   return NO_ERROR;
}

Seeder::SeederError Seeder::stopUploading(const std::list<ChunkInfo>& chunkInfoList)
{
   SeederError retErrorCode = NO_ERROR;
   SeederError errorCode;

   for (auto iter = chunkInfoList.begin(); iter != chunkInfoList.end(); ++iter)
   {
      if ((errorCode = stopUploading(*iter)) != NO_ERROR)
         retErrorCode = errorCode;
   }
   return retErrorCode;
}

Seeder::SeederError Seeder::stopUploading(const std::list<Chunk>& chunkList)
{
   SeederError retErrorCode = NO_ERROR;
   SeederError errorCode;

   for (auto iter = chunkList.begin(); iter != chunkList.end(); ++iter)
   {
      if ((errorCode = stopUploading(*iter)) != NO_ERROR)
         retErrorCode = errorCode;
   }
   return retErrorCode;
}

   Seeder::SeederError Seeder::stopUploading(const ChunkInfo& chunkInfo)
   {
      size_t id = chunkInfo.getChunkId();
      if (!m_producers.count(id))
         return CHUNK_NOT_FOUND;

      /// TODO: Doesn't seem like CP API actually detaches producer from network
      /// upon deletion of a producer.
      /// Not an issue for now, we can worry about this later.
      m_producers.erase(id);
      m_chunks.erase(id);

      return NO_ERROR;
   }

Seeder::SeederError Seeder::stopUploading(const Chunk& chunk)
{
   return stopUploading(chunk.getMetadata());
}

void Seeder::onInterest(ndn::Producer& producer, const ndn::Interest& interest)
{
   // onCacheMiss seems to need this function to work properly, even if it does
   // nothing
    std::cout << "IN ON INTEREST:" << interest.toUri() << std::endl;
   return;
}

void Seeder::onCacheMiss(ndn::Producer& producer, const ndn::Interest& interest)
{
   const ndn::Name name = interest.getName();
   size_t lastComponentIndex = name.size() - 1;
   size_t id = atoi(name.get(lastComponentIndex).toUri().c_str());
   std::cout << "IN ON CACHE MISS" << interest.toUri() << std::endl;
   // Drop interest if we don't have a suitable producer
   if (!m_chunks.count(id))
      return;
            
   const std::vector<char>& buffer = m_chunks[id].getBuffer();
   // std::unique_ptr<ndn::Producer>& producer = m_producers[id];

   producer.produce(ndn::Name(), reinterpret_cast<const uint8_t*>(buffer.data()), buffer.size());
}

}
