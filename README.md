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

### Boost

You need to download [Boost](https://www.boost.org/) and unpack the boost folder
in the archive in the `Source/Libraries` folder.

After unziping, `Source/Libraries/boost/container/small_vector.hpp` should
exist. The currently used version of Boost is 1.79.0.

Currently, Boost is used for its containers. I'm paranoid about allocating
memory, `boost::container::small_vector` and `boost::container::static_vector`
avoids allocating memory in most cases while providing the features of a
`std::vector`.


## About the code here

You can use the modifications done here on your own project. But if you copy
the code, you must cite me (SquonK) as the original author of the code somewhere
in your project code.

This repository does not provide a production ready version of a FlyFF server.

