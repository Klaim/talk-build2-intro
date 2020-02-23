


0. Quick Start
    - clone a project
    - make a modification
    - test on several configs
    - install
    - run
    - commit
    - push change
    - request CI
1. presentation
    - who am I (?)
    - why this talk
    - clarifications (?)
    - questions
2. what is build2
 - website definition
 - toolset organisation
3. buildsystem demo
 - b
 - targets
 - options/properties
4. slides: concepts
 - configuration
 - project/package
5. demo project management
 - project files (manual)
 - bdep new (automatic)
 - bpkg ?
 - manage dependencies (+cppget.org)
 - use a modified/cloned dependency
6. project life tools (demo?)
 - testing (`b test`, testscript)
 - dist, install
 - CI
 - release
 - publish
7. build2 in perspective
 - project status
 - why not use it
 - why use it
8. DONE

--------------------------------------

`build2` from scratch
=====================

## Hello World ?

```
bdep new kikoo
cd kikoo
b test
code . #
b install config.install.root=../install

```

## Presentation
### Who am I
  Joël Lamotte aka "Klaim", I use CMake at work and home,
  I also use `build2` to experiment for future projects,
  and at work in some prototypes.
  My job is currently in robotics but I have a background in other domains
  mostly videogames and embedded software.

### Why this talk?
  Someone asked a few months ago to CPPFrugs
  that I present `build2` as an alternative to `CMake`.
  A lot of people are frustrated with `CMake` and are looking
  for new tools but lack the time for a proper introduction.
  As I am interested in `build2` (but not an expert) and experiment a lot
  with it for future projects, I was asked to give an intro of how it works.

  I will explain how `build2` works with the goal that you can start using
  it immediately after the talk after having installed it.
  You still need to read the official documentation, but this should be a good
  starting point to at least play with it.

### Clarifications
  - `build2` have nothing to do with `boost.build`'s `b2` provided and used by Boost.
  - I will not directly compare `build2` to CMake, that would be too long
  - We'll use `build2` `v0.12.0`
  - I'll use Windows to demo but it's working the same or easier on unix-like platforms.
  - This will be interactive, raise your hand if you have questions.

### Questions

1. Who is familiar with CMake?
2. Who is familiar with Make?
3. Who is not familiar with how C++ code is built? (transformed into executable binaries)
4. Who is familiar with C++20 Modules?
5. Who wants to make a talk about Meson?
6. Who wants to make a talk about vcpkg?
7. Who is only here to eat for free?

## What Is `build2`

https://build2.org

> build2 is an open source (MIT), cross-platform build toolchain for developing and packaging C and C++ code. It is a hierarchy of tools that includes the build system, package dependency manager (for package consumption), and project dependency manager (for project development).
>
> Key features:
> - Next-generation, Cargo-like integrated build toolchain for C and C++.
> - Covers entire project lifecycle: creation, development, testing, and delivery.
> - Uniform and consistent interface across all platforms and compilers.
> - Fast, multi-threaded build system with parallel building and testing.
> - Archive and version control-based package repositories.
> - Dependency-free, all you need is a C++ compiler.

### Toolset's Core

1. Build-system: `b`
    - same position as: make, msbuild, Ninja, Bazel, ...
    - not a meta-buildsystem like: CMake, Meson, PreMake...
      - CMake, Meson and Premake could have generators for `build2`
    - supports C and C++ by default
    - extensible through build-system modules (languages, build-system features)
    - provides testing features

2. Package Manager: `bpkg`
    - same position as: Conan, vcpkg, hunter, pip, gem, npm, ...
    - handles build configurations (more on that later)
    - relies on `b` to build packages
    - supports `build2` packages, `pkg-config` packages (and probably more in the future)
    - supports remote package repositories as:
        - archives-based repositories (simple `http` server or `brep`)
        - `git` repositories

3. Project Dependency Manager: `bdep`
    - drives `bpkg` and `b` for larger operations
    - helps creating new projects and sub-projects

4. Most of the time when developing a project we use either `bdep` or `bpkg`.
    Direct usage of `bpkg` comes when doing more advanced manipulations.

### Facilities

1. Build-system modules for testing, versionning, etc.
2. Continuous Integration
3. Online Repository

4. Self-hosted package repository: `brep` (not provided by default)
    - base of the central repository: https://cppget.org
    - you can setup your own instance (didn't try yet)



-----------------------------------------------------------------------------



## Build-System
### Demo 1 : just build some code

1. open `hello.cpp`

2. create a `buildfile` and open it.
  - `buildfile` is equivalent to a `makefile`
  - It describes the targets to build if we invoke the build system for this directory

3. Use `C++`:
    ```
    using cxx
    ```
   - In `build2`, C++ is just a build-system module.
   - That's what is used to build `build2` itself.
   - There are other buils-system modules to add features and languages (`c`, `testing` etc.)
   - There is an experiment for a `rust` build-system module.
   - Allow users to extend the build system for their projects or add new languages.


4. Add a target:
    ```
    exe{hello} : cxx{hello}
    ```
   - `build2` follows a inherit the target system from make:
   ```
   target-name : requirement-1 requirement-2
   ```
   - However `build2` adds **typing** to the targets, instead of **recipes**
   ```
   typeA{target-name} : typeB{requirement-1} typeC{requirement-2}
   ```
   - `cxx` targets are C++ compilation unit, `hxx` are C++ headers etc.
   -

5. Change the default extension to `cpp` and `hpp`:
    ```
    cxx{*}: extension = cpp
    hxx{*}: extension = hpp
    ```
   - By default the extension searched is `cxx`, `hxx`, etc. but we can change it to whatever.
   - Targets and target types have variables to specify their properties.

6. Build & run:
    ```
    b
    ./hello Man
    ```
    - `b` is the build system
    - `b` invoked alone means `b update` which means **"update this directory's buildfile output"**
    - The compiler used by default depends on which compiler was used to build/install `build2`
    - We can specify the compiler and other variables for a specific invocation:
    ```
    b config.cxx=clang++
    ```
    - On Windows, the default `msvc` compiler is the highest version, including previews.
    - We can pecify a compiler path (the rest is guessed from the compiler name):
    ```
    b "config.cxx='...\VC\Tools\MSVC\14.23.28105\bin\Hostx64\x86\cl'"
    ```
    - We can specify `C++` version that way too, otherwise it's `latest`:
    ```
    b config.cxx.std=17     # -std=c++17 (if the compiler allows it)
    b config.cxx.std=latest # highest available C++ version
    ```

7. Cleanup:
    ```
    b clean
    ```
    - We can chain commands: `b clean update`
    - Commands are provided and implemented by build-system modules.

8. Info:
    ```
    b info
    ```
    - To see all the available commands of a project: `b info`
    - (describe each field)
    - note that this is not considered as a true "project" yet


### Demo 2 : just build all the files

1. Separate the printing into files: `print.hpp` `print.cpp`

2. To build these files too:
    ```
    exe{hello} : cxx{hello print} hxx{print}
    ```
    - Requirements of the same types can be grouped in the same expression.
    ```
    exe{hello} : {hxx cxx}{hello print}
    ```
    - Search files of the right type/extension with these names.

3. Glob all the cpp and hpp files:
    ```
    exe{hello} : cxx{*} hxx{*}
    ```
    - We just get all the files of the extension/type specified as requirements.
    ```
    exe{hello} : cxx{**} hxx{**}
    ```
    - Recursively glob.
    ```
    exe{hello} : {cxx hxx}{**}
    ```
    - Shorter.
    - It's recommended to use globing with `build2`, there is no issue with it:
        - no need to touch the buildfiles if you just add code
        - makes things easier with massive refactoring

### Demo 3 : just a library

1. Move "print" files into a library.

2. Create `print` library target:
    ```
    exe{hello} : {cxx}{hello} lib{print}

    lib{print} : {cxx hxx}{print}
    ```
    - order of target declaration is not important
    - by default the first target (and it's requirement) is built
    - requiring the directory to build targtets by default:
    ```
    ./ : exe{hello} lib{print}
    ```
    - or by chaining requirements:
    ```
    ./ : exe{hello} : {cxx}{hello} lib{print}
    ./ : lib{print} : {cxx hxx}{print}
    ```

3. Force usage of the static version of the `print` library (`liba` target type):
    ```
    exe{hello} : {cxx}{hello} liba{print}
    ```
    - Libraries can be static-only, dynamic/shared-only or both (the default).
    - User can specify which version they want.
    - By default, use the dynamic/shared version.
    - Use the usual symbol export/import macros in libraries that need to be shared/dynamic.

### Demo 4 : just a bunch of libraries

1. Grouped target sources in directories.
    ```
    exe{hello} : hello/{hxx cxx}{*} liba{print}
    lib{print} : print/{cxx hxx}{*}
    ```
    - `lib{print}` headers are not found by `exe{hello}` (yet).

2. Make add the root directories to include directories:
    ```
    cxx.poptions =+ "-I$src_root"
    ```
    - Also need to use the directory in the include: `#include <print/print.hpp>`
    - `poptions` means "preprocessor options"
    - There are other options: `coptions` (compiler), `loptions` (link), etc.
    - We use the gcc/clang syntax which will be translated for msvc (if possible).
    - This is ok for simple cases but we didn't make consumers of our library inherit
      information to consume the library.

3. Specify properties that library consumers should have:
    ```
    lib{print} : cxx.export.poptions =+ "-I$src_root"
    ```
    - As before, targets have specific properties.
    - All the `*.export.*` properties are for the consuming side.

### Demo 5 : did you say "modules" ?

1. We want to experiment with incoming C++ features:
    ```
    cxx.std = experimental # or = 20 if C++20 is officially available in your compiler
    ```
    - `cxx.std = latest` is better most of the time.

2. Modules "primary module interface" needs to have a different target-type/extension
    ```
    mxx{*}: extension = mpp
    ```
    - By default it's `mxx`, you don't need this line if you follow the default names.

3. No need for headers anymore, grab the module interfaces:
    ```
    lib{print} : print/{cxx mxx}{*}
    ```

4. Change the code to import instead of include. (open source files)


### Demo 6 : did you say "modules" as dynamic libaries?

1. Extension provided by `build2`: `symexport`
    ```
    cxx.features.symexport = true
    ```
    - Requires modules to be available.

2. Force usage of the shared-library:
    ```
    exe{hello} : hello/{hxx cxx}{*} libs{print}
    ```

3. Use `__symexport` in code where you would use symbol import/export macros.


### Demo 7 : conditions, loops, switches

We now `lib{print}` use `<thread>`, it needs to depend on `pthread`.

1. Conditionally depend on pthread:
    ```
    if ($cxx.target.class != 'windows')
    {
        cxx.libs += -lpthread
    }
    ```
    - Language is declarative but does allow loops and conditions

2. Pattern matching with `switch`:

2. Generate executable per cpp with `for`:
    ```
    for test_cpp : cxx{tests/print/**}
    {
        test_name = $name($test_cpp)
        ./ : exe{$test_name} : $test_cpp liba{print}
    }
    ```
    - Repeat rules depending on context
    - No need to update the buildfile to add tests

### Demo 8 : Project

- setup code as project, with a name
- add more bs modules
- show b info

### Demo 9 : Tests

- make group of executables as test
- show testscript (good case, error case)

### Demo 10 : Configuration

- change the compiler to clang
- store the configuration
- edit the configuration
- make separate configurations for clang and msvc

### Demo 11 : declare Dependencies

- add an import
- introduce the notion of package
- add a manifest
- add module "version"

### Demo 12 : Handle dependencies in configurations

- introduce the notion of configuration directory
- create a configuration
- build a dependency using their git repo
- check config and list of dependencies directires
- add a repository
- build a package from that repo

- build the project using that configuration (manually)

### Demo 13 : New project

bdep new (exe)
modifie le projet
build & test
add dependencies & repositories
bdep init with old configuration
bdep init with new configuration
build test with all configurations

bdep new lib
make the exe project depend on the lib
bdep init in configurations
build test all

### Demo 14 : juggle with dependencies

- in libkikoo project:
- add dependencies from main repo
- bdep status -r
- in kikoo project:
- change version of one dependency (no manifest change)
-

### Demo 15 : release versions

- make a release of libkikoo
- modify libkikoo
- make another release of libkikoo



### Demo 16 :


















