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

int Seeder::upload(const std::list<Chunk>& chunkDataList)
{
	 for (auto iter = chunkDataList.begin(); iter != chunkDataList.end(); iter++)
	 {
			size_t id = iter->getMetadata().getChunkId();
			ndn::Name chunkName(m_prefix + std::to_string(id));
			std::unique_ptr<ndn::Producer> chunkProducer(new ndn::Producer(chunkName));
			chunkProducer->setContextOption(INTEREST_ENTER_CNTX,
																			(ndn::ConstInterestCallback)std::bind(&Seeder::onInterest, this, _1));
			chunkProducer->setContextOption(CACHE_MISS,
																			(ndn::ConstInterestCallback)std::bind(&Seeder::onCacheMiss, this, _1));
			chunkProducer->attach();

			m_producers.insert(std::make_pair(id, std::move(chunkProducer)));
			m_chunks.insert(std::make_pair(id, *iter));
	 }
	 return 0;
}

int Seeder::stopUploading(const std::list<ChunkInfo>& chunkInfoList)
{
	 int retVal = 0;
	 for (auto iter = chunkInfoList.begin(); iter != chunkInfoList.end(); iter++)
	 {
			size_t id = iter->getChunkId();
			if (!m_producers.count(id))
			{
				 retVal = 1; // Chunk ID not found
				 continue;
			}

			/// TODO: Doesn't seem like CP API actually detaches producer from network
			m_producers.erase(id);
			m_chunks.erase(id);
	 }
	 return retVal;
}

void Seeder::onInterest(const ndn::Interest& interest)
{
}

void Seeder::onCacheMiss(const ndn::Interest& interest)
{
	 const ndn::Name name = interest.getName();
	 size_t lastComponentIndex = name.size() - 1;
	 size_t id = atoi(name.get(lastComponentIndex).toUri().c_str());

	 // Drop interest if we don't have a suitable producer
	 if (!m_chunks.count(id))
	 {
			std::cout << "Producer not found" << std::endl;
			return;
	 }
			
	 const std::vector<char>& buffer = m_chunks[id].getBuffer();
	 std::unique_ptr<ndn::Producer>& producer = m_producers[id];
	 producer->produce(ndn::Name(), (uint8_t*)buffer.data(), buffer.size());
}

}
