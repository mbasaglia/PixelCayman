PixelCayman
===========

PixelCayman is an upcoming Open Source, customizable pixel art editor
and animation tool.

License
-------

GPLv3+ see COPYING

Getting the latest sources
--------------------------

A web front-end to the repository is available at
https://github.com/mbasaglia/PixelCayman

The git URL is https://github.com/mbasaglia/PixelCayman.git

Since this project uses git submodules,
you need to make sure you clone them as well the main repository.

Eg:

    git clone --recursive https://github.com/mbasaglia/PixelCayman.git

Or

    git clone https://github.com/mbasaglia/PixelCayman.git
    cd PixelCayman
    git submodule init
    git submodule update

Dependencies
------------

* CMake
* Any C++11 compliant compiler
* Qt 5.4

On Ubuntu and similar compilation requires the following packages:
    cmake build-essential qtbase5-dev

Compiling
---------

This project uses the standard CMake build conventions:

    mkdir build && cd build && cmake .. && make

Contributing
------------

Contributions of any kind are very welcome,
see Contributing.md for some info and guidelines.

Contacts
--------

IRC: #PixelCayman on FreeNode

Mattia Basaglia <mattia.basaglia@gmail.com>
