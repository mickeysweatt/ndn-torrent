#include<torrentClient.hpp>
#include <iostream>

using namespace std;

int main2(int argn, char *argv[]) {
    //TODO: process command line arguments.
    cout << "Enter filename:\t";
    string filename;
    cin >> filename;
    cout << "Enter download location:\t";
    string downloadLocation;
    cin >> downloadLocation;
    torrent::TorrentClient c(filename, downloadLocation);
    return c.start();
}
