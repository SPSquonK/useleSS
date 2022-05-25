# useleSS

FlyFF v15 code with some refactor. This project does **NOT** currently aim to
add any major features.

The base code is from Blouflash V15
https://www.elitepvpers.com/forum/flyff-pserver-guides-releases/4244222-visual-studio-2017-source-files.html 


This source is developped on Visual Studio 2022 with the C++lastest option.
C++latest is used instead of C++20 because MSVC doesn't have range support
with the C++20 option for some reasons.


## Getting started

- Download Blouflash's version
- Unzip the Resource folder
- Clone this repository
- Apply the Database.sql script and create the ODBC links:
    - ODBC source `useless_account` = table `USELESS_ACCOUNT_DBF`
    - ODBC source `useless_character` = table `USELESS_CHARACTER_01_DBF`
    - ODBC source `useless_log` = table `USELESS_LOGGING_01_DBF`
    - You can create `useless_ranking` but IFAIK, it is never used.

## Dependencies

For now, the project does not use vcpkg. Instead, we rely on having the user
manually install the dependencies in the `Source/Librairies` folder.

We give here the list of dependencies with an example of a file that should exist after unzipping.


- [Boost v1.79.0](https://www.boost.org/) (`Source/Libraries/boost/container/small_vector.hpp`)
- [NamedTypes](https://github.com/joboccara/NamedType/tree/020be1e934f8916a02302f4e490c461671baaccc) (`Source/Libraries/NamedType/version.hpp`)
- [Rapidjson](https://github.com/Tencent/rapidjson/releases/tag/v1.1.0) (`Source/Libraries/rapidjson/writer.h`)


## About the code here

You can use the modifications done here on your own project. But if you copy
the code, you must cite me (SquonK) as the original author of the code somewhere
in your project code.

This repository does not provide a production ready version of a FlyFF server.

In particular, specific code that is explicitely commented with a reference to
the
[SquonK Hidden Boss License](http://squonk.fr/SquonK-Hidden-Boss-License.txt)
should keep their comments.


