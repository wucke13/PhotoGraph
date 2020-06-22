# PhotoGraph
The second-generation implementation of my graph-based image editor, this time with tiled output and smarter caching!
There are some interesting components here that I will describe in a paper that I am in the process of preparing.

# License
The library is licensed under the GPL version 3, a copy of which can be found under [gpl-3.0.txt](gpl-3.0.txt).

# Dependencies
* [ImageGraph](https://github.com/Fingolfin1196/ImageGraph) (licensed under the LGPL version 3)
* [QNodeGraph](https://github.com/Fingolfin1196/QNodeGraph) (licensed under the LGPL version 3)
* [Abseil](https://abseil.io/) (licensed under the Apache version 2.0 license)

# Building
Instructions for building my two libraries ImageGraph and QNodeGraph are contained in their respective repositories.

Abseil is linked as a static library into the executable created by this project.
Creating a static library is only possible for LTS releases and did not work for me without disabling GMock and tests.
