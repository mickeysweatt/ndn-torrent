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

### Demonstration Plan

Here are the things that we intend to demonstrate, and instructions on how to replicate them. Make sure to have NFD set up before performing any of these tests.

* Download a single file torrent

We provide the `Groks050102.mp3.torrent` file and the corresponding file `Groks050102.mp3` in the `test/Groks050102.mp3` directory. Run the following commands in the main directory in one terminal:

```
build/userInterface test/Groks050102.mp3.torrent test
```

Then, in another terminal (either on the same computer or on a different computer):

```
build/userInterface test/Groks050102.mp3.torrent test/downloads
```

There should be some output indicating that interest packets and data packets are being sent and received. Eventually the download will finish, and you should verify that the file `test/downloads/Groks050102.mp3/Groks050102.mp3` is the same as the seeder's copy.

* Download a multi-file torrent

We provide the `groks113_archive.torrent` file and the corresponding files in the `test/groks113` directory. Run the following commands in the main directory in one terminal:

```
build/userInterface test/groks113_archive.torrent test
```

Then, in another terminal (either on the same computer or on a different computer):

```
build/userInterface test/groks113_archive.torrent test/downloads
```

Eventually the download will finish, and you should verify that the files in `test/downloads/groks113` are the same as the seeder's copy.

* Leecher becomes a Seeder

Perform the above demonstration. Kill the original seeder with an interrupt signal, and then, with the original leecher still running, run the command:

```
build/userInterface test/groks113_archive.torrent test/downloads2
```

You should notice that the original leecher is now receiving and serving interests for the new leecher.

* Resuming partial downloads

In one terminal, run the command for the seeder:

```
build/userInterface test/groks113_archive.torrent test
```

In another, run the command for the leecher:

```
build/userInterface test/groks113_archive.torrent test/downloads3
```

Before the download complete, send an interrupt signal to kill the leecher. Resume it with the same command; if you read the output statements, you should see that only the chunk IDs of chunks that have yet to be downloaded are being downloaded.

* Seeding files that have been moved

In one terminal, run the command for the seeder:

```
build/userInterface test/groks113_archive.torrent test
```

In another, run the command for the leecher:

```
build/userInterface --hidden test/groks113_archive.torrent test/downloads4
```

Notice the use of the `--hidden` flag. After the download completes, delete all of the files in `test/downloads4/groks113`, and then re-run the leecher command. You should notice that no download occurs; this is because there exists a link to the files in a hidden directory `.ndn-torrent` in the user's home directory, and with the `--hidden` flag our application will consult the `.ndn-torrent` directory for files.