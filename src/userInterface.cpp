#include <torrentClient.hpp>
#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argn, char *argv[]) {
    //TODO: process command line arguments.
//    cout << "Enter torrent file name: ";
    string filename = "/Users/admin/Dropbox/ndn-torrent/test/groks113_archive.torrent";
//    cin >> filename;
//    cout << "Will use torrent file " << filename << endl;
    cout << "Enter download location: ";
    string downloadLocation;
    cin >> downloadLocation;
    cout << "Using " << downloadLocation << " as the download directory.\n";
    torrent::TorrentClient c(filename, downloadLocation);
    c.start();
    // For now, loop forever 
    while(true) {
        sleep(1000);
    }
}
