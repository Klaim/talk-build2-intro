`build2` from scratch
=====================

These are the live-coding notes.

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
   - (There is an experiment for a `rust` build-system module.)?
   - Allow users to extend the build system for their projects or add new languages.


4. Add a target:
    ```
    exe{hello} : cxx{hello}
    ```
    - `build2` follows a inherit the target system from make
    - (show slides on target syntax)
    - `cxx` targets are C++ compilation unit, `hxx` are C++ headers etc.
    - By default `cxx` is associated to extension `.cxx`. We can change that:
    ```
    cxx{*} : extension = cpp
    ```

5. Build & run:
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
    - We can pecify a compiler path, the toolchain will be deduced.
    - Almost all the variables can be overwritten through cli.


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
    - We could add them one by one but it's tedious and prevent easy refactoring.
    - Specify that our headers type `hxx` will have extension `hpp`:
    ```
    hxx{*} : extension = hpp
    ```

2. Glob all the cpp and hpp files:
    ```
    exe{hello} : cxx{*} hxx{*}
    ```
    - We just get all the files of the extension/type specified as requirements.
    ```
    exe{hello} : cxx{**} hxx{**}
    ```
    - Recursively glob everything with the types.
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
    - order of target declaration is not important most of the time
    - by default the first target (and it's requirement) is built (there are ways to change that)

3. Force usage of the static version of the `print` library (`liba` target type):
    ```
    exe{hello} : {cxx}{hello} liba{print}
    ```
    - `lib` target type generate `liba` static, `libs` dynamic/shared.
    - By default, use the dynamic/shared version.
    - User can specify which kind they want.
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

We want this to be a real testable project.

0. New files for projects:
    - switch to `build2file` mode

1. Setup code as project, with a name :
    - Add `build2/bootstrap.build2`
    ```
    projet = kikoo
    ```
    - `b info` we can see the name of the project

3. Add more bs modules in `build2/bootstrap.build2`:
    ```
    using config
    using test
    using install
    using dist
    ```
    - `b info` we can see more operations available

4. Add info common to all buildfiles in `build2/root.build2`:
    ```
    cxx.std = 17

    using cxx

    cxx{*}: extension = cpp
    hxx{*}: extension = hpp
    ```
    - Any `build2file` in the project will be prefixed by the content of this file.

### Demo 9 : Tests

0. `b test` shows that there is nothing to test.

1. Mark executable tests as being tests (in `build2file`):
    ```
    exe{$test_name} : test = true
    ```
    - `b test` will run the executable and expect `0` for success.
    - (Show failing test and fix it)
    - Tests are launched in parallel by default (overlapping output).
    - We can use `-s` to do operations in a "serial" way.

2. `testscript` is a tool for testing output and interactive programs:
    - Add `hello/testscript`
    ```
    : basics
    :
    $* 'World' > '> Hello, World!'

    : missing-name
    :
    $* 2>>EOE != 0
    > I need a (1) name.
    EOE
    ```
    - Make `hello` being tested by adding to `build2file`:
    ```
    exe{hello} : hello/testscript
    ```
    - `b test` shows failing tests, fix them
    -

### Demo 10 : Simple Configuration

SKIP SKIP SKIP SKIP SKIP SKIP

We want to always use clang instead of the default compiler (msvc):

1. Change the compiler to clang and build and test:
    ```
    b config.cxx=clang++ test
    ```
    - All configuration information are variables.
    ```
    b config.cxx=clang++ clean
    ```
    - Repeating the config in cli is problematic.

2. Store the configuration:
    ```
    b configure: config.cxx=clang++
    ```
    - `b -v` Shows that we work with clang now.

3. Edit the configuration:
    - (show `build2/config.build2`)
    - This project will always use this config file.
    - Variables are the same than in cli.
    - Replace:
    ```
    config.cxx = cl
    ```

4. We can create separate configuration directories.
    Handling multiple configurations manually with `b` is possible.
    However, it is far simpler if we make the project be a package first.

### Demo 11 : Complete Project Package

0. Add in `build2/bootstrap.build2'
    ```
    using version
    ```
    - We need this to make version information available to other operations.

1. Add a `manifest` file.
    ```
    : 1
    name: kikoo
    version: 0.1.0-a.0.z
    summary: kikoo toolset
    license: proprietary
    description-file: README.md
    url: https://example.org/mylib
    email: mjklaim@gmail.com

    depends: * build2 >= 0.12.0
    depends: * bpkg >= 0.12.0
    ```
   - (Introduces the notion of package.)
   - `b info` Now shows the info from the manifest.
   - The version number follows SEMVER pluss a few rules to make `build2` automatically manage it.
   - Here the `z` is whatever identifies this non-released version and `a` means "alpha".

2. SKIP SKIP SKIP: Install:
    ```
    b install
    ```
    - Fails until we specify `config.install.root=../some/dir`
    ```
    b install config.install.root=../install
    ```
    - Builds and install, follows rules from the builfiles and target types defaults.
    - Missing `.lib` files because no symbol is exported, not a problem for now (we'll see later how to fix this).


### Demo 12 : Importing libraries

0. Importing Dependencies

1. Depend on `fmt` package:
    - Add in `manifest`:
    ```
    depends: fmt ^6.0.0
    ```
    - The package is still not found because it have not be made available.
    - Time to setup a configuration with our packages using `bpkg`.
    - The version expression defines a range of allowed versions.

2. Depend on `lib{fmt}`:
    - Add in `build2file`:
    ```
    import dependencies = fmt%lib{fmt}
    lib{print} : $dependencies
    ```
    - We get an error because the package is not found.


### Demo 13 : Configurations And Dependencies

(introduce the notion of configuration)

1. Create a configuration
    ```
    bpkg create -d ./build-msvc cc
    ```
    - We could specify the properties on cli too:
    ```
    bpkg create -d ./build-clang cc config.cxx=clang++ config.c=clang
    ```

2. Build a dependency using their git repo:
    ```
    cd build-msvc
    bpkg build https://github.com/build2-packaging/sol2.git
    ```
    - Add repository, fetch, find last released package, take a source snapshot, configure, build.
    - We can also build a specific version:
    ```
    bpkg build https://github.com/build2-packaging/sol2.git#develop
    ```
    - We can also `test`, `install` etc.

3. (Look into the config directory)
    - We can edit `build/config.build2` manually.

4. Add a repository (cppget):
    ```
    bpkg add https://pkg.cppget.org/1/stable
    ```
    - (show cppget.org/fmt and the links to the stable repos)

5. Build `fmt` from that repo:
    ```
    bpkg build fmt
    ```
    - As before, last version is fetched

6. Build the project using that configuration (manually):
    ```
    bpkg build ../kikoo/
    ```
    - (show the content of the configuration)

### Demo 14 : New project

Usually we don't manipulate projects that way because there is too many operations and things to know.
Instead we use `bdep` to handle our projects.

1. Create a new project:
    ```
    bdep new kikoo
    ```
    - (show directory created)
    - Almost similar project except:
    - Git files
    - Readme
    - `repositories.manifest` : where we can specify some repositories to add to configs automatically.
        - Useful for specific in-dev dependencies.
    - Default naming is `cxx`

2. Initialize project:
    ```
    cd kikoo
    bdep init -C ../build-msvc cc
    ```
    - First creates a new configuration (`-C`) using `bpkg`
    - Second associate that configuration to this project as a default configuration.
    ```
    b test
    ```
    - Everything works fine.
    - Symbolic links were added for executables.
    - Make a first commit:
    ```
    git add .
    git commit -m "bdep init"
    ```

3. Modify the project:

    - (change the output to be "Kikoo, XXX!")
    - (update the testscripts too)
    - `b test`
    - `git add . && git commit -m "Outputs Kikoo instead of Hello"`

4. Create a new library project:
    ```
    cd ..
    bdep new print -t lib,alt-naming -l c++,cpp
    ```
    - Creates a library that import/export symbols when shared (see `print/export.hpp`)
    - Tests are in a sub-project (show how the lib is imported).

5. Initialize the library project with the same configuration:
    ```
    cd print
    bdep init -A ../build-msvc
    ```
    - Then test the library: `b test`

6. Make `kikoo` depends on `print`:
    - Modify kikoo's `manifest`:
    ```
    depends: print
    ```
    - Modify `kikoo/buildfile`:
    ```
    import libs += print%lib{print}
    ```
    - Modify `kikoo/kikoo.cxx`:
    ```
    #include <print/print.hpp>
    ```
    - Ready to test: `b test`

7. Modify dependency, build end app:
    - In `print/print.cpp` add:
    ```
    void print(std::string text)
    {
        std::cout << text << std::endl;
    }
    ```
    - In `print/print.hpp` add:
    ```
    PRINT_SYMEXPORT
    void print(std::string text);
    ```
    - Try to build `kikoo`:
    ```
    cd ../kikoo
    b test
    ```
    - Fails because changes tried to build `print` first but there is a header missing in `print/print.cpp`:
    ```
    #include <iostream>
    ```
    - `b test` in works as expected.

8. Create a new configuration using `clang`:
    ```
    cd ../print
    bdep init -C ../build-clang @clang --options-file ../options/clang.config
    ```
    - Note `@clang` is an alias to that configuration.
    ```
    cd ../kikoo
    bdep init -A ../build-clang @clang
    ```

9. Test all configurations:
    ```
    bdep test -a
    ```
    - We can also test the dependencies recursively
    ```
    bdep test -a -r
    ```

10. Add a dependencies from the central repo (in `print/`):
    - (Uncomment cppget.org in `repositories.manifest`)
    - Add somme packages in `manifest`:
    ```
    depends: fmt ^6.0.0
    depends: sol2 ^3.2.0
    ```
    - Try to test `kikoo`:
    ```
    cd ../kikoo
    b test
    ```
    - `build2` detects changes of all dependencies and changes in repository lists
    - It does not detects if the content of repositories have changed, in that case we would need to fetch the new dependencies with `bdep fetch`

### Demo 15 : Juggle with dependencies

Let's see how to handle dependencies on the fly.

0. New project with configuration
    ```
    bdep new myapp
    cd myapp
    ```

1. Add `plf-colony` as dependency:
    - (Uncomment cppget in `repositories.manifest`)
    - Add in `manifest` : `depends: plf-colony ~5.11.0` (any patch version)
    - `bdep init -C ../build-msvc`
    - Everything is fine.

2. Add `libmachin` as dependency:
    - Add in `repositories.manifest`:
    ```
    :
    role: prerequisite
    location: https://github.com/Klaim/talk-build2-libmachin.git
    ```
    - Add in `manifest` : `depends: libmachin ^1.0.0`
    - `b test`

3. Upgrade dependency `libmachin`:
    - Change `manifest`: `depends: libmachin ^2.0.0`
    - Conflict because `libmachin` v2.x depends on `plf-colony v5.20`

4. Fix conflict:
    - Change `manifest`: `depends: plf-colony ^5.0.0` (any minor+patch version)
    - `b test`

5. Force a specific version that is compatible with the manifest:
    - Try `libmachin v2.0.0`:
    ```
    bdep sync libmachin/2.0.0
    ```

6. See dependency tree:
    ```
    bdep status -r
    ```

### Demo 16 : CI

1. Clone and initialize a bigger project:
    ```
    git clone https://github.com/klaim/talk-build2-bigdemo.git bigdemo
    cd bigdemo
    bdep init -C ../build-msvc
    ```
    - Everything is dowloaded and configured to the right versions.
    - Look at the dependency tree:
    ```
    bdep status -r
    ```
    - That project contains 2 packages.
    -

### Demo 17 : release versions

- make a release of libkikoo
- modify libkikoo
- make another release of libkikoo


### Demo 18 : Publish






















