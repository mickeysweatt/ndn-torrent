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

#include <iostream>

namespace torrent {

Seeder::SeederCallback::SeederCallback(Seeder& seeder)
   : m_seeder(seeder)
{
}

void Seeder::SeederCallback::onCacheMiss(ndn::Producer& producer, const ndn::Interest& interest)
{
   const ndn::Name name = interest.getName();
   size_t chunkIdIndex = m_seeder.m_prefix.size();
   std::cout << "PRODUCING: " << interest.toUri() << std::endl;
   // Drop interest if malformed interest name
   if (chunkIdIndex >= name.size())
      return;

   size_t id = atoi(name.get(chunkIdIndex).toUri().c_str());
   // Drop interest if we don't have a suitable producer
   if (!m_seeder.m_chunks.count(id))
      return;

   const std::vector<char>& buffer = m_seeder.m_chunks[id].getBuffer();

   producer.produce(name.getSubName(chunkIdIndex, 1),
                    reinterpret_cast<const uint8_t*>(buffer.data()),
                    buffer.size());
}
    
void Seeder::SeederCallback::onCacheHit(ndn::Producer& producer, const ndn::Interest& interest)
{
    const ndn::Name name = interest.getName();
    size_t chunkIdIndex = m_seeder.m_prefix.size();
    std::cout << "PRODUCING: " << interest.toUri() << std::endl;
}

Seeder::Seeder(TorrentClientProtocol& clientProtocol)
   : m_prefix("/torrent/file"),
     m_clientProtocol(clientProtocol),
     m_producer(m_prefix),
     m_callback(new SeederCallback(*this))
{
   m_producer.setContextOption(CACHE_MISS,
                              static_cast<ndn::ProducerInterestCallback>(std::bind(&SeederCallback::onCacheMiss, m_callback, _1, _2)));
   m_producer.attach();
}

Seeder::Seeder(const ndn::Name& prefix,
                      TorrentClientProtocol& clientProtocol)
   : m_prefix(prefix),
     m_clientProtocol(clientProtocol),
     m_producer(prefix),
     m_callback(new SeederCallback(*this))
{
   m_producer.setContextOption(CACHE_MISS,
                              static_cast<ndn::ProducerInterestCallback>(std::bind(&SeederCallback::onCacheMiss, m_callback, _1, _2)));
   m_producer.attach();
}

/*
Seeder::Seeder(Seeder&& other)
   : m_prefix(std::move(other.m_prefix)),
     m_clientProtocol(other.m_clientProtocol),
     m_producer(std::move(other.m_producer)),
     m_chunks(std::move(other.m_chunks))
{
   m_callback = other.m_callback;
   other.m_callback = nullptr;
}
*/

/*
Seeder& Seeder::operator=(Seeder&& other)
{
   m_prefix = std::move(other.m_prefix);
   m_clientProtocol = other.m_clientProtocol;
   m_producer = std::move(other.m_producer);
   m_chunks = std::move(other.m_chunks);
   m_callback = other.m_callback;
   other.m_callback = nullptr;

   return *this;
}
*/

Seeder::~Seeder()
{
   if (m_callback != nullptr)
      delete m_callback;
}

Seeder::SeederError Seeder::upload(const std::list<Chunk>& chunkDataList)
{
   SeederError retErrorCode = NO_ERROR;
   SeederError errorCode;
   
    for (auto iter = chunkDataList.begin(); iter != chunkDataList.end(); ++iter) {
       std::cout << "UPLOADING " << iter->getMetadata().getChunkId() << std::endl;
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
      if ((errorCode = upload(std::move(*iter))) != NO_ERROR)
         retErrorCode = errorCode;

   return retErrorCode;
}

Seeder::SeederError Seeder::upload(const Chunk& chunk)
{
   size_t id = chunk.getMetadata().getChunkId();
   m_chunks.insert(std::make_pair(id, chunk));
   const std::vector<char>& buffer = chunk.getBuffer();
   std::ostringstream ostr;
   ostr << chunk.getMetadata().getChunkId();
    
   return NO_ERROR;
}

Seeder::SeederError Seeder::upload(Chunk&& chunk)
{
   size_t id = chunk.getMetadata().getChunkId();
   m_chunks.insert(std::make_pair(id, std::move(chunk)));
  
   return NO_ERROR;
}

Seeder::SeederError Seeder::stopUploading(const std::list<ChunkInfo>& chunkInfoList)
{
   SeederError retErrorCode = NO_ERROR;
   SeederError errorCode;

   for (auto iter = chunkInfoList.begin(); iter != chunkInfoList.end(); ++iter)
      if ((errorCode = stopUploading(*iter)) != NO_ERROR)
         retErrorCode = errorCode;

   return retErrorCode;
}

Seeder::SeederError Seeder::stopUploading(const std::list<Chunk>& chunkList)
{
   SeederError retErrorCode = NO_ERROR;
   SeederError errorCode;

   for (auto iter = chunkList.begin(); iter != chunkList.end(); ++iter)
      if ((errorCode = stopUploading(*iter)) != NO_ERROR)
         retErrorCode = errorCode;

   return retErrorCode;
}

Seeder::SeederError Seeder::stopUploading(const ChunkInfo& chunkInfo)
{
   size_t id = chunkInfo.getChunkId();

   if (!m_chunks.count(id))
      return CHUNK_NOT_FOUND;

   m_chunks.erase(id);
   return NO_ERROR;
}

Seeder::SeederError Seeder::stopUploading(const Chunk& chunk)
{
   return stopUploading(chunk.getMetadata());
}

}
