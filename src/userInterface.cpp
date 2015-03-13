#include <torrentClient.hpp>
#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argn, char *argv[]) {
    //TODO: process command line arguments.
    string filename, downloadLocation;
    /*if (argn == 1) {
        cout << "Enter torrent file name: ";
        cin >> filename;
        cout << "Will use torrent file " << filename << endl;
        cout << "Enter download location: ";
        string downloadLocation;
        cin >> downloadLocation;
        cout << "Using " << downloadLocation << " as the download directory.\n";
    } else */if (argn == 3) {
        filename = string(argv[1]);
        downloadLocation = string(argv[2]);
        cout << "Will use torrent file " << filename << endl;
        cout << "Using " << downloadLocation << " as the download directory.\n";
    } else {
        filename = "/Users/admin/Dropbox/ndn-torrent/test/groks113_archive.torrent";
        downloadLocation = "/Users/admin/Desktop/";
        //cout << "Usage:\n" << argv[0] << " [torrentFile {downloadPath|uploadPath}]\n";
        //return 0;
    }
    torrent::TorrentClient c(filename, downloadLocation);
    c.start();
    // For now, loop forever 
    while(true) {
        sleep(1000);
    }
}
