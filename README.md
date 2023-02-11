# useleSS

FlyFF v15 code with some refactor.

The base code is from Blouflash V15
https://www.elitepvpers.com/forum/flyff-pserver-guides-releases/4244222-visual-studio-2017-source-files.html 

Some v22 resource compatiblity have been added. See [Documentation/v15plus.md](Documentation/v15plus.md).

This source is developped on Visual Studio 2022 with the C++lastest option.

- This repository **does not** provide a production ready of the source code. Many exploits are not (yet) fixed.
- If you encounter a bug, please use the Github issue system, or in the worst case send a mail.


## Getting started

- Download and install [Visual Studio 2022](https://visualstudio.microsoft.com/fr/vs/community/), [Direct X](https://www.microsoft.com/fr-fr/download/details.aspx?id=35), [SQL Server 2014](https://www.microsoft.com/fr-FR/download/details.aspx?id=42299) and [SQL Management Studio](https://docs.microsoft.com/en-us/sql/ssms/download-sql-server-management-studio-ssms?view=sql-server-ver16).
- [Download Blouflash's version of FlyFF v15](https://www.elitepvpers.com/forum/flyff-pserver-guides-releases/4244222-visual-studio-2017-source-files.html)
- Unzip the Resource folder
- Clone this repository
- Apply the Database.sql script and create the ODBC links:
    - ODBC source `useless_account` = table `USELESS_ACCOUNT_DBF`
    - ODBC source `useless_character` = table `USELESS_CHARACTER_01_DBF`
    - ODBC source `useless_log` = table `USELESS_LOGGING_01_DBF`
- Use [Server Starter](https://github.com/SPSquonK/ServerStarter): `git clone https://github.com/SPSquonK/ServerStarter.git`

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

**This repository does not provide a production ready version of a FlyFF server.**

In particular, specific code that is explicitely commented with a reference to
the
[SquonK Hidden Boss License](http://squonk.fr/SquonK-Hidden-Boss-License.txt)
should keep their comments.



The [Source/Libraries/boost/pfr/detail/core17_generated.hpp file](Source/Libraries/boost/pfr/detail/core17_generated.hpp) is licensied under the Boost License by its author. It is included in the project to break through the 100 limit.
