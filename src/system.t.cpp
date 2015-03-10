#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

#include <sha1hash.hpp>
#include <torrentClient.hpp>

int testClient()
{
   return 0;
}

int testFileTransfer()
{
   char message[] = "Hello, world!";
   // This is lazy, let's fix this later
   std::vector<char> buffer;
   for (int i = 0; message[i] != 0; i++)
      buffer.push_back(message[i]);

   torrent::SHA1Hash bufferHash(reinterpret_cast<const unsigned char*>(buffer.data()),
                                buffer.size());
   torrent::ChunkInfo metadata(0, bufferHash);
   torrent::Chunk chunk(metadata, buffer);

   ndn::Name prefix("/torrent/helloworld/");

   class TestClientProtocol : public torrent::TorrentClientProtocol
   {
   public:
      TestClientProtocol()
         : m_status(-1)
      {
      }

      virtual void chunkDownloadSuccess(const torrent::Chunk& chunk)
      {
         m_status = 1;
      }

      virtual void chunkDownloadFail(torrent::TorrentClientProtocol::Error error, 
                                     const torrent::ChunkInfo& chunkMetadata)
      {
         m_status = 0;
      }

      int getStatus()
      {
         while (m_status == -1)
            continue;

         return m_status;
      }
   private:
      int m_status;
   };

   TestClientProtocol testClientProtocol;

   pid_t child;
   if (0 != (child = fork())) {
      torrent::Seeder seeder(prefix, testClientProtocol);
      seeder.upload(chunk);
      int rval;
      waitpid(child, &rval, 0);
      return rval;
   }
   else {
      torrent::Leecher leecher(prefix, testClientProtocol);
      leecher.download(metadata);
      if (testClientProtocol.getStatus() == 1)
      {
         std::cout << "File transfer test succeeded" << std::endl;
         exit(0);
      }
      else
      {
         std::cout << "File transfer test failure" << std::endl;
         exit(1);
      }
   }
}

int main()
{
   return testFileTransfer();
}
