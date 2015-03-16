#include <torrentClient.hpp>
#include <iostream>
#include <unistd.h>
#include <functional>
#include <boost/filesystem.hpp>
#include <chunkInfo.hpp>
#include <filePiece.hpp>
#include <cstdlib>

using namespace std;

int main(int argn, char *argv[]) {
    string filename, downloadLocation;
    
    std::function<void(const torrent::Torrent&)> callback
        = [](const torrent::Torrent& torrent) -> void {
            cout << torrent.getName() << " has finished downloading." << endl;
    };
    const char* const hidden_flag_name = "--hidden";
    
    if (argn == 1 || (argn == 2 && strcmp(argv[1], hidden_flag_name) == 0)) {
        cout << "Enter torrent file name: ";
        cin >> filename;
        cout << "Will use torrent file " << filename << endl;
        cout << "Enter download location: ";
        string downloadLocation;
        cin >> downloadLocation;
        cout << "Using " << downloadLocation << " as the download directory.\n";
    } else if (argn == 3 || (argn == 4 && strcmp(argv[1], hidden_flag_name) == 0)) {
        int argumentPosition = (argn == 3 ? 1 : 2);
        filename = string(argv[argumentPosition]);
        downloadLocation = string(argv[1 + argumentPosition]);
        cout << "Will use torrent file " << filename << endl;
        cout << "Using " << downloadLocation << " as the download directory.\n";
    } else {
        cout << "Usage:\n" << argv[0] << " [" << hidden_flag_name
             << "] [torrentFile {downloadPath|uploadPath}]\n";
        return 0;
    }
    if ((argn == 2 || argn == 4) && strcmp(argv[1], "--hidden") == 0) {
        // To support hard linking, actually download to/from hidden folder,
        // and then link to the actual location.
        string finalDownloadLocation = downloadLocation;
        if (finalDownloadLocation.back() != '/')
            finalDownloadLocation += "/";
        downloadLocation = string(getenv("HOME")) + "/.ndn-torrent/";
        // Call back function creates hard links when download completes.
        callback = [=](const torrent::Torrent& torrent) -> void {
            string hardLinkLocation = finalDownloadLocation + torrent.getName() + "/";
            string actualLocation = downloadLocation + torrent.getName() + "/";
            cout << "Creating hard links\n";
            if (!boost::filesystem::exists(hardLinkLocation)) {
                boost::filesystem::path path(hardLinkLocation);
                boost::filesystem::create_directories(
                    boost::filesystem::absolute(path));
            }
            // TODO: allow getting a list of files from the torrent directly.
            string fileName = "";
            for (const torrent::ChunkInfo& chunk : torrent.getChunks()) {
                for (const torrent::FilePiece& file : chunk.getFilePieceList())
                {
                    if (fileName == file.getFilePieceName()
                      || boost::filesystem::exists(hardLinkLocation + file.getFilePieceName()))
                    {
                        continue;
                    }
                    fileName = file.getFilePieceName();
                    
                    cout << "Creating hard link from " << 
                        (actualLocation + fileName)
                        << " to "
                        << (hardLinkLocation + fileName) << "\n";
                    boost::filesystem::path newPath(hardLinkLocation
                        + file.getFilePieceName());
                    boost::filesystem::path oldPath(actualLocation
                        + file.getFilePieceName());
                    
                    boost::filesystem::create_hard_link(oldPath, newPath);
                }
            }
            cout << torrent.getName() << " has finished downloading." << endl;
        };
    }
    
    torrent::TorrentClient c(filename, downloadLocation, callback);
    c.start();
    // For now, loop forever 
    while(true) {
        sleep(1000);
    }
}
