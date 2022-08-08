# useleSS

FlyFF v15 code with some refactor.

The base code is from Blouflash V15
https://www.elitepvpers.com/forum/flyff-pserver-guides-releases/4244222-visual-studio-2017-source-files.html 

Some v22 resource compatiblity have been added. See [Documentation/v15plus.md](Documentation/v15plus.md).

This source is developped on Visual Studio 2022 with the C++lastest option.
C++latest is used instead of C++20 because MSVC doesn't have range support
with the C++20 option for some reasons. A recent version of C++ is used because I want to improve in modern
C++, and it is dumb to not use it in a project that does not have any restriction that goes against it.


## Getting started

- Download Blouflash's version
- Unzip the Resource folder
- Clone this repository
- Apply the Database.sql script and create the ODBC links:
    - ODBC source `useless_account` = table `USELESS_ACCOUNT_DBF`
    - ODBC source `useless_character` = table `USELESS_CHARACTER_01_DBF`
    - ODBC source `useless_log` = table `USELESS_LOGGING_01_DBF`
    - You can create `useless_ranking` but IFAIK, it is never used.
    - Database.sql script might be glitchy. See [this issue](https://github.com/SPSquonK/useleSS/issues/39)

## Dependencies

For now, the project does not use vcpkg. Instead, we rely on having the user
manually install the dependencies in the `Source/Librairies` folder.

We give here the list of dependencies with an example of a file that should exist after unzipping.


- [Boost v1.79.0](https://www.boost.org/) (`Source/Libraries/boost/container/small_vector.hpp`) (Do not overwrite this repository copy of the common files)
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



The [Source/Libraries/boost/pfr/detail/core17_generated.hpp file](Source/Libraries/boost/pfr/detail/core17_generated.hpp) is licensied under the Boost License by its author. It is included in the project to break through the 100 limit.
