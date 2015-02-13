#include <ndn-cxx/contexts/producer-context.hpp>
#include <ndn-cxx/contexts/context-options.hpp>
#include <ndn-cxx/name.hpp>

#include "chunk.hpp"
#include "chunkInfo.hpp"
#include "seeder.hpp"

#include <list>
#include <functional>
#include <memory>

namespace torrent {

int Seeder::upload(const std::list<Chunk>& chunkDataList)
{
	 for (auto iter = chunkDataList.begin(); iter != chunkDataList.end(); iter++)
	 {
			/// TODO: When ChunkInfo is defined, insert its ID as the key.
			int id = 0;
			/// TODO: When ChunkInfo is defined, generate a name.
			ndn::Name chunkName(std::string(TORRENT_NAMESPACE) + std::to_string(id));
			m_producers.insert(std::make_pair(id, std::move(std::unique_ptr<ndn::Producer>(new ndn::Producer(chunkName)))));
			std::unique_ptr<ndn::Producer>& chunkProducer = m_producers[id];

			chunkProducer->setContextOption(INTEREST_ENTER_CNTX, (ndn::InterestCallback)std::bind(&Seeder::onInterest, this, _1));
	 }
	 return 0;
}

int Seeder::stopUploading(const std::list<ChunkInfo>& chunkInfoList)
{
	 for (auto iter = chunkInfoList.begin(); iter != chunkInfoList.end(); iter++)
	 {
	 }
	 return 0;
}

void Seeder::onInterest(ndn::Interest& interest)
{
}

}
