#include <ndn-cxx/contexts/consumer-context.hpp>

#include <ndn-cxx/interest.hpp>

#include <seeder.hpp>
#include <torrentClientProto.hpp>

#include <iostream>
#include <mutex>
#include <string>
#include <vector>

#include <unistd.h>

using namespace torrent;
using std::bind;

std::mutex callbackMutex;

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

void assertValidContent(uint8_t validator, int *numConsumed, bool *success, ndn::Consumer& consumer, const uint8_t *buffer, size_t bufferSize)
{
   // Suppress unused parameter warnings
   (void)consumer;

   std::lock_guard<std::mutex> lock(callbackMutex);
   (*numConsumed)++;
   for (size_t i = 0; i < bufferSize; i++)
   {
      if (validator != buffer[i])
      {
         *success = false;
         return;
      }
   }
}

void assertNoCallback(bool *notCalled, ndn::Consumer& consumer, const uint8_t *buffer, size_t bufferSize)
{
   std::lock_guard<std::mutex> lock(callbackMutex);
   *notCalled = false;
}

void assertNack(bool *nackReceived, ndn::Consumer& consumer, const ndn::ApplicationNack& appNack)
{
   std::lock_guard<std::mutex> lock(callbackMutex);
   *nackReceived = true;
}

int testUpload()
{
   // Callback "return" values
   int numConsumed = 0;
   bool success = true;

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
      ndn::Consumer consumer(ndn::Name(chunkName.c_str()), RDR);
      consumer.setContextOption(MUST_BE_FRESH_S, true);
      consumer.setContextOption(CONTENT_RETRIEVED, static_cast<ndn::ConsumerContentCallback>(bind(assertValidContent, i + '0', &numConsumed, &success, _1, _2, _3)));
																	 
      consumer.consume(ndn::Name());
   }

   while (numConsumed < N_PACKETS)
   {
      sleep(1);
   }

   // Return value of 0 means success, return value of 1 means failure
   return !success;
}

int testStopUploading()
{
   // Callback "return" values
   bool notCalled = true;

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
      consumer.setContextOption(CONTENT_RETRIEVED, static_cast<ndn::ConsumerContentCallback>(bind(assertNoCallback, &notCalled, _1, _2, _3)));
																	 
      consumer.consume(ndn::Name());
   }

   return !notCalled;
}

int testNack()
{
   // Callback "return" values
   bool notCalled = true; 
   bool nackReceived = false;

   DummyTorrentClientProtocol dummyProtocol;
   Seeder seeder(ndn::Name("torrent/dummyFile3"), dummyProtocol);

   unsigned char dummyHash[20];
   ChunkInfo chunkInfo(0, dummyHash);

   std::vector<char> dummyBuffer;
   Chunk chunk(std::move(chunkInfo), std::move(dummyBuffer));

   seeder.upload(chunk);
   sleep(1);

   ndn::Consumer consumer(ndn::Name("torrent/dummyFile3"), RDR);
   consumer.setContextOption(MUST_BE_FRESH_S, true);
   consumer.setContextOption(CONTENT_RETRIEVED, static_cast<ndn::ConsumerContentCallback>(bind(assertNoCallback, &notCalled, _1, _2, _3)));
   consumer.setContextOption(NACK_ENTER_CNTX, static_cast<ndn::ConsumerNackCallback>(bind(assertNack, &nackReceived, _1, _2)));

   consumer.consume(ndn::Name("1"));

   return !nackReceived || !notCalled;
}

int main()
{
   int overallStatus = 0;
   int (*testFunctions[])() = {
      testUpload,
      testStopUploading,
      testNack,
      nullptr
   };

   for (int testNum = 0; testFunctions[testNum] != nullptr; testNum++)
   {
      int testStatus = testFunctions[testNum]();
      if (testStatus == 0)
         std::cout << "Seeder test " << testNum + 1 << " successful" << std::endl;
      else
      {
         overallStatus = 1;
         std::cout << "Seeder test " << testNum + 1 << " failed" << std::endl;
      }
   }

   return overallStatus;
}
