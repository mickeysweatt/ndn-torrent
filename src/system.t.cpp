#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

#include <sha1hash.hpp>
#include <torrentClient.hpp>

int testClient()
{
   torrent::TorrentClient testSeeder("/test/helloworld.torrent",
                                     "/test/seeder/");
   
   torrent::TorrentClient testLeecher("/test/helloworld.torrent",
                                      "/test/leecher/");

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

   /// TODO: Asymmetry in how leecher and seeder handle prefixes, should
   /// standardize
   std::string prefix = "/torrent/helloworld.torrent/";
   std::string prefix2 = "/torrent/helloworld.torrent";

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
   /// TODO: The asymmetry bothers me, we should change one of the constructor's
   /// parameter orders
    pid_t child;
   if (0 != (child = fork())) {
       torrent::Seeder seeder(testClientProtocol, prefix);
       seeder.upload(chunk);
       sleep(15);
       int rval;
       waitpid(child, &rval, 0);
       return rval;
   }
   else {
       torrent::Leecher leecher(prefix2, testClientProtocol);
       leecher.download(metadata);
       if (testClientProtocol.getStatus() == 1)
       {
           std::cout << "File transfer test succeeded" << std::endl;
           return 0;
       }
       else
       {
           std::cout << "File transfer test failure" << std::endl;
           return 1;
       }
   }
}

int main()
{
   return testFileTransfer();
}
