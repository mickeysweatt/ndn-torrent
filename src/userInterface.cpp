#include <torrentClient.hpp>
#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argn, char *argv[]) {
    //TODO: process command line arguments.
    string filename, downloadLocation;
    if (argn == 1) {
        cout << "Enter torrent file name: ";
        cin >> filename;
        cout << "Will use torrent file " << filename << endl;
        cout << "Enter download location: ";
        string downloadLocation;
        cin >> downloadLocation;
        cout << "Using " << downloadLocation << " as the download directory.\n";
    } else if (argn == 3) {
        filename = string(argv[0]);
        downloadLocation = string(argv[1]);
        cout << "Will use torrent file " << filename << endl;
        cout << "Using " << downloadLocation << " as the download directory.\n";
    }
    torrent::TorrentClient c(filename, downloadLocation);
    c.start();
    // For now, loop forever 
    while(true) {
        sleep(1000);
    }
}
