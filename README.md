libpsirngclient
===============

A library to interact with a [psirng](https://github.com/nullspook/psirng) gRPC server.

Installation
------------

```bash
# Install build-essential and cmake

sudo apt update
sudo apt -y install build-essential cmake

# Install libpsirngclient

mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

License
-------

    Copyright (C) 2025 NullSpook

    libpsirngclient is free software: you can redistribute it and/or modify it
    under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    libpsirngclient is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
    License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with libpsirngclient.  If not, see <https://www.gnu.org/licenses/>.