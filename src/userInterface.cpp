#include<torrentClient.hpp>
#include <iostream>

using namespace std;

int main(int argn, char *argv[]) {
    //TODO: process command line arguments.
    cout << "Enter torrent file name: ";
    string filename;
    cin >> filename;
    cout << "Will use torrent file " << filename << endl;
    cout << "Enter download location: ";
    string downloadLocation;
    cin >> downloadLocation;
    cout << "Using " << downloadLocation << " as the download directory.\n";
    torrent::TorrentClient c(filename, downloadLocation);
    return c.start();
}
