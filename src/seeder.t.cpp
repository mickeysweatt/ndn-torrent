#include <ndn-cxx/contexts/consumer-context.hpp>

#include <ndn-cxx/interest.hpp>

#include <seeder.hpp>
#include <torrentClientProto.hpp>

#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>

using namespace torrent;
using std::bind;

static int numConsumed = 0;
static bool notCalled = true;
static bool success = true;

static const int N_PACKETS = 11;

class DummyTorrentClientProtocol : public TorrentClientProtocol
{
public:
	 void chunkDownloadSuccess(const Chunk& chunk)
   {
      // Suppress unused parameter warnings
      (void)chunk;
   }

	 void chunkDownloadFail(Error error, const ChunkInfo& chunkMetadata)
   {
      // Suppress unused parameter warnings
      (void)error, (void)chunkMetadata;
   }
};

void assertValidContent(uint8_t validator, ndn::Consumer& consumer, const uint8_t *buffer, size_t bufferSize)
{
   // Suppress unused parameter warnings
   (void)consumer;
   numConsumed++;
   for (size_t i = 0; i < bufferSize; i++)
   {
      if (validator != buffer[i])
      {
         success = false;
         return;
      }
   }
}

void assertNoCallback(ndn::Consumer& consumer, const uint8_t *buffer, size_t bufferSize)
{
  notCalled = false;
}

/*
void interestLeaveContext(ndn::Consumer& consumer, ndn::Interest &interest)
{
   // Suppress unused parameter warnings
   (void)consumer, (void)interest;
}

void dataEnterContext(ndn::Consumer& consumer, const ndn::Data &data)
{
   // Suppress unused parameter warnings
   (void)consumer, (void)data;
}
*/

int testSeeder1()
{
  DummyTorrentClientProtocol dummyProtocol;
  Seeder seeder(ndn::Name("/torrent/dummyFile"), dummyProtocol);
  std::list<Chunk> chunks;
  unsigned char dummyHash[20];
  for (size_t i = 0; i < N_PACKETS; i++)
  {
    ChunkInfo chunkInfo(i, dummyHash); // We'll ignore the hash
    std::vector<char> buffer;
    for (int j = 0; j < 256; j++)
       buffer.push_back(i + '0');
    chunks.emplace_back(chunkInfo, buffer);
  }

  seeder.upload(chunks);
  sleep(1);

  for (size_t i = 0; i < N_PACKETS; i++)
  {
    std::string chunkName = "/torrent/dummyFile/";
    chunkName += std::to_string(i);
    ndn::Consumer consumer(ndn::Name(chunkName.c_str()), SDR);
    consumer.setContextOption(MUST_BE_FRESH_S, true);
    consumer.setContextOption(CONTENT_RETRIEVED, static_cast<ndn::ConsumerContentCallback>(bind(assertValidContent, i + '0', _1, _2, _3)));
																	 
    consumer.consume(ndn::Name());
  }

  while (numConsumed < N_PACKETS)
     continue;

  if (success) {
    std::cout << "Seeder test 1 successful" << std::endl;
    return 0;
  }
  else {
    std::cout << "Seeder test 1 failure" << std::endl;
    return 1;
  }
}

int testSeeder2()
{
  DummyTorrentClientProtocol dummyProtocol;
  Seeder seeder(ndn::Name("/torrent/dummyFile2"), dummyProtocol);
  std::list<Chunk> chunks;
  unsigned char dummyHash[20];
  for (size_t i = 0; i < N_PACKETS; i++)
  {
    ChunkInfo chunkInfo(i, dummyHash); // We'll ignore the hash
    std::vector<char> buffer;
    for (int j = 0; j < 256; j++)
       buffer.push_back(i + '0');
    chunks.emplace_back(chunkInfo, buffer);
  }

  seeder.upload(chunks);
  sleep(1);
  seeder.stopUploading(chunks);
  sleep(1);

  for (size_t i = 0; i < N_PACKETS; i++)
  {
    std::string chunkName = "/torrent/dummyFile2";
    chunkName += std::to_string(i);
    ndn::Consumer consumer(ndn::Name(chunkName.c_str()), SDR);
    consumer.setContextOption(MUST_BE_FRESH_S, true);
    consumer.setContextOption(CONTENT_RETRIEVED, static_cast<ndn::ConsumerContentCallback>(bind(assertNoCallback,  _1, _2, _3)));
																	 
    consumer.consume(ndn::Name());
  }

  if (notCalled) {
    std::cout << "Seeder test 2 successful" << std::endl;
    return 0;
  }
  else {
    std::cout << "Seeder test 2 failure" << std::endl;
    return 1;
  }
}

int main()
{
  testSeeder1();
  // Currently fails, needs to be debugged
  testSeeder2();
}
