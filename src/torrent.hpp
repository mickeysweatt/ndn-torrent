#ifndef INCLUDED_TORRENT_HPP
#define INCLUDED_TORRENT_HPP

#include <list>
#include <string>
#include <map>

namespace torrent {

class Torrent {
    // A value-semantic attribute class for torrents the
  public:
    // CREATORS
    Torrent();
    // Create an empty 'torrent' with all attributes set to 'nullptr'.

  private:
    std::string            m_announceURL;
    std::list<std::string> m_announceList;
};

}
#endif