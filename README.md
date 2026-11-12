# Introduction

This program is designed to retime a circuit for minimal clock period.

## Circuit Format

Circuit should be in the following format. All circuits to be retimed should
follow this example.

```txt
TotalNodes=4;
NodeDelays=1,1,2,2;
Edge1,3=1;
Edge1,4=2;
Edge2,1=1;
Edge3,2=0;
Edge4,2=0;
MaxClockCycle=3;
```

## Building

### Prerequisites

To build this program you must have cmake and C++ build tools installed on your 
system. You can do this on Ubuntu with:

```txt
$ sudo apt update
$ sudo apt install -y build-essential cmake
```

### Build

Once you have the prerequisites installed run the following in the project root
directory:

```txt
$ cmake -S . -B build
$ cmake --build build
```

The binary will be in build directory, `build/retimeCircuit`

## Install

To install the program, if you are on Ubuntu, copy it to your `/usr/local/bin`
or `~/.local/bin/` directory

## Running

The most basic way to run the program is to copy your circuit named `inputCircuit.txt`
to the build directory and run `./retimeCircuit`. The program will search for `inputCircuit.txt`
in the pwd and output files to the pwd.

### Inputs

The program expects a file `inputCircuit.txt` to be in your pwd. Otherwise you
can specify the input file name.

### Outputs

The program outputs two files, a `.graph` file and `.txt` file. The `.graph`
file shows the retimed circuit in the form of an adjacency matrix with the sources
"u" on the left hand side and the destinations "v" on the top. The `.txt` file
is the retimed circuit in the input format as shown above. By default the
output files will be named `retimedCircuit.graph` and `retimedCircuit.txt`
respectively.

### Command Line Options

There are a few basic command line options:

* `-v` enables verbosity. By default the program executes silently to avoid
spamming your terminal. To see the intermediate steps, enable verbosity.
* `-i` allows you to specify an input file. Example: `./retimeCircuit -i myCircuit.txt`.
Default input file is `inputCircuit.txt`.
* `-o` allows you to specify an output file. Example: `./retimeCircuit -o myCircuitOutput`.
Default output files are named `retimedCircuit.graph` and `retimedCircuit.txt`.
* `-version` shows the version info.
* `-h` prints the command line help info

## AI Usage Disclosure

Chat GPT 6 Thinking model was used in the development of this program. The model
assisted in using correct syntax for regex in the parser, C++ and CMAKE language syntax.
It assisted in the development of the binary search, Floyd-Warshall and
Bellman-Ford algorithms, and general debugging and assorted optimizations.

## Copyright and License

Copyright (C) Matthew Hardenburgh 2026. All rights reserved

This version of the program is licensed to all under the GPLv3 licensing agreement.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
