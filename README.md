# useleSS

FlyFF v15 source code with some refactor.

- This repository **does not** provide a production ready of the source code. Many exploits are not (yet) fixed.
- **/!\\ Server developer /!\\: Read the How to use section of this document**

- **If you encounter a bug, please use the Github issue system, send a Discord PM (spsquonk) or a mail (git log to find it)**
  - Do not ask on some forums/places on the internet how to solve a bug related to this source. Instead, please, create an issue here, so I can actually see it and fix it
  - **Do not re-upload this source on these forums** without any modification:
    - It creates more bug reports on places I do not visit which is bad in short, medium and long term
    - It is pointless as it evolves quickly and is public
    - If you are scared about the source disappearing, just click the fork button on Github

## Getting started

- Download and install [Visual Studio 2022](https://visualstudio.microsoft.com/fr/vs/community/), [Direct X](https://www.microsoft.com/fr-fr/download/details.aspx?id=35), [SQL Server 2014](https://www.microsoft.com/fr-FR/download/details.aspx?id=42299) and [SQL Management Studio](https://docs.microsoft.com/en-us/sql/ssms/download-sql-server-management-studio-ssms?view=sql-server-ver16).
- Get the files
  - [Download Blouflash's version of FlyFF v15](https://www.elitepvpers.com/forum/flyff-pserver-guides-releases/4244222-visual-studio-2017-source-files.html)
  - Unzip the Resource folder
  - Clone this repository: `git clone --recurse-submodules https://github.com/SPSquonK/useleSS.git`
  - Paste Blouflash's Resources in useless resource folder. Do not overwrite anything.
  - Remove the now useless files: `rm Resource/layer.inc Resource/spevent.txt Resource/ticket.inc`
  - Install the dependencies. Either:
    - Follow the `Dependencies` section
    - [Download this archive](https://sflyff.fr/useless-libraries.zip) and unzip it the `Source/Librairies`
- Apply the Database.sql script and create the ODBC links:
    - ODBC source `useless_account` = table `USELESS_ACCOUNT_DBF`
    - ODBC source `useless_character` = table `USELESS_CHARACTER_01_DBF`
    - ODBC source `useless_log` = table `USELESS_LOGGING_01_DBF`
    - ODBC sources must be set to English language

## Details

The base code is from Blouflash V15
https://www.elitepvpers.com/forum/flyff-pserver-guides-releases/4244222-visual-studio-2017-source-files.html 

Some v22 resource compatibility have been added. See [Documentation/v15plus.md](Documentation/v15plus.md).

This source is developed on Visual Studio 2022 with the C++latest option.

## Dependencies

For now, the project does not use vcpkg. Instead, we rely on having the user
manually install the dependencies in the `Source/Librairies` folder.

Here is the list of dependencies with an example of a file that should exist after unzipping:
- [Boost v1.79.0](https://www.boost.org/) (`Source/Libraries/boost/container/small_vector.hpp`) (Do not overwrite this repository copy of the common files)
- [NamedTypes](https://github.com/joboccara/NamedType/tree/020be1e934f8916a02302f4e490c461671baaccc) (`Source/Libraries/NamedType/version.hpp`)
- [Rapidjson](https://github.com/Tencent/rapidjson/releases/tag/v1.1.0) (`Source/Libraries/rapidjson/writer.h`)


## About the code here

**This repository does not provide a production ready version of a FlyFF server.**

You can use the modifications done here on your own projects. But if you copy
the code, you must cite me (SquonK) as the original author of the code somewhere
in your project code (aka my modifications are under the Boost License)

The [Source/Libraries/boost/pfr/detail/core17_generated.hpp file](Source/Libraries/boost/pfr/detail/core17_generated.hpp) is licensed under the Boost License by its author. It is included in the project to break through the 100 limit.

## How to use this repository as a server developer?

As this repository does not fix bugs/exploits, change some parts of the architecture and does not implement new major features, using it as the start point for your project is probably counterproductive.

The best way to use this repository is probably to clone two versions of it, `git checkout original-code` on one folder, and use diff between the current HEAD and the initial commit (for example with a tool like WinMerge or Meld) to check for all the modifications. Then, integrate the modifications that you find useful in your own code.

Do not forget to put a message like `// Original code: SquonK, https://github.com/SPSquonK/useless` if you copy several lines of code. I am like graphic designers: I'm paid in visibility.

Examples of code that you might find useful include:
- `SendPacket` / `BroadcastPacket` API
- Neuz OnSnapshot error handling ([Gist](https://gist.github.com/SPSquonK/4b0ae08d924b55cc0ccd03b85071ead4))
- `CWndTListBox` class: a CWndListBox templated by the type of the objects to display and the way to display them
- New `CWndTradeGold` API: relies on callbacks instead of modifying its OnChildNotify method

Very common ideas in this repository include:
- Simplification of `CWndBase` derived classes by removing duplicated methods
- STL-ization of many classes: `CPtrArray`, … are considered obsolete

Note that some changes are very opinionated, for example:
- `UIVariant` API ([Design choices in French on my blog]( https://www.sà.fr/2022/06/04/uivariant/))
- Enforce the fact that `LPDIRECT3DDEVICE9` is a global variable. It is no more passed as an attribute / stored as a field everywhere ([See PR#95 related commits](https://github.com/SPSquonK/useleSS/pull/95))

## Contributing

Any contribution of any kind is welcomed. This includes some casual messages like "hey, check out this thing. They changed it in V21 / this private server source code release, and it is probably better than the V15 / what you've done here.", bug report, pull requests, ...
