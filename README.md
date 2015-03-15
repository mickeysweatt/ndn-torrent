# ndn-torrent
An implementation of a torrent-like peer-to-peer sharing system running over NDN.

### Building ndn-torrent

ndn-torrent supports Mac OSX and Ubuntu, and has two external dependencies that must be installed before compiling:

* [Consumer-Producer API for NDN](https://github.com/iliamo/Consumer-Producer-API)
* [OpenSSL Crypto Library](https://www.openssl.org/source/)

Run the following command in the main directory to configure the build system and verify that the external dependencies have been satisfied, and then compile the code:

```
./waf configure build
```

### How to run

The NDN Forwarding Daemon (NFD) must be installed and running when using this application:

* [NDN Forwarding Daemon](http://named-data.net/doc/NFD/current/INSTALL.html)

By default, our application will seed and leech files from the /ndn/ucla/edu namespace. Therefore, before running our application you must register an appropriate TCP tunnel with NFD. We recommend that you set this up using the following commands:

```
nfd-start
nfdc register /ndn tcp://131.179.196.46
```

After compiling the source code, the application can be found in the `build` directory as the executable file `userInterface`. The executable must be supplied with the following arguments:

```
build/userInterface path/to/torrent path/to/download/location
```

The application will then automatically begin downloading the files specified in the torrent file into a directory in the download location.

