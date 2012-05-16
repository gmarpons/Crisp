Crisp
=====

A coding rule validation add-on for **LLVM/clang**, GPL v3
licensed.

**Coding Rules** constrain admissible constructs of a language to help
produce better code. Crisp can be used to define and enforce rules for
C and C++, like those in [High Integrity C++] [HICPP], and
[CERT's Secure Coding Standards] [CERT-SCS].

Crisp rules are written in Prolog. A high-level declarative DSL to
easily write new rules is under development. It will be called CRISP,
an acronym for *Coding Rules in Sugared Prolog*.

  [HICPP]:    http://www.codingstandard.com/
  [CERT-SCS]: http://www.cert.org/secure-coding/

Installation
============

Although Crisp should be quite portable, it has been tested on
GNU/Linux only so far.

Prerequisites
-------------

1. [Install and build **LLVM/clang sources**]
   [CLANG-GET-STARTED]. Note that:
   - (Steps 2 and 3) There are also Git repositories available for
     both LLVM (`http://llvm.org/git/llvm.git`) and clang
     (`http://llvm.org/git/clang.git`) that you can use as an
     alternative to `svn`.
   - (Step 4) It is not necessary.
   - (Step 5) By default, LLVM/clang is build in *Debug+Asserts*
     mode. There are many [other build combinations]
     [LLVM-COMPILE]. E.g., you can set environment variables
     `ENABLE_OPTIMIZED=1` and `DISABLE_ASSERTIONS=1` when running
     `make` to build in *Release* mode.

2. Install **SWI-Prolog**. Version 5.10 or greater is recommended. It
   is most likely provided as a pre-compiled package for your
   GNU/Linux distribution. On Debian/Ubuntu (and derivatives) you
   simply need to type `sudo apt-get install swi-prolog`. On other
   systems, you need to be sure that a dynamic library (called
   `libswipl.so` on POSIX systems) is available. That means that if
   you install from [sources] [SWIPL-DOWNLOAD], you must pass option
   `--enable-shared` to the `configure` script.

3. Install **Boost C++ Libraries**, version 1.46.1 or greater. Crisp
   relies on header-only libraries. On Debian/Ubuntu (and derivatives)
   you only need to type `sudo apt-get install libboost-dev`. For
   other systems (or more recent versions of Boost) follow the
   [general installation instructions for Unix/Linux]
   [BOOST-UNIX].

  [CLANG-GET-STARTED]: http://clang.llvm.org/get_started.html
  [LLVM-COMPILE]:      http://www.llvm.org/docs/GettingStarted.html#compile
  [SWIPL-DOWNLOAD]:    http://www.swi-prolog.org/Download.html
  [BOOST-UNIX]:        http://www.boost.org/doc/libs/1_49_0/more/getting_started/unix-variants.html


Download Crisp source code
--------------------------

Source code is available at GitHub:

    git clone git://github.com/gmarpons/Crisp.git crisp

Build Crisp
-----------

Let `LLVM_SRC_ROOT` (resp. `LLVM_OBJ_ROOT`) the absolute
root path of your LLVM/clang source (resp. build) tree, and
`CRISP_SRC_ROOT` the absolute root path of your Crisp source
tree. Then do the following:

    mkdir $LLVM_OBJ_ROOT/projects/crisp
    cd $LLVM_OBJ_ROOT/projects/crisp
    $CRISP_SRC_ROOT/configure \
      --with-llvmsrc=$LLVM_SRC_ROOT \
      --with-llvmobj=$LLVM_OBJ_ROOT

(In fact, it is possible to build Crisp in a different directory, but
`$LLVM_OBJ_ROOT/projects` is meant to be used that way).

You will probably need also to set the header directory for
SWI-Prolog, with the `configure`'s extra argument
`--with-swipl-includes`. E.g., in Debian/Ubuntu (and derivatives) you
need to use
`--with-swipl-includes=/usr/lib/swi-prolog/include/`. Documentation
for other `configure` command line options (such as `--prefix`, or
`--with-swipl-libs`) can be obtained typing

    $CRISP_SRC_ROOT/configure --help

Compilation is possible with a recent version of LLVM/clang (it does
not work with GCC). In order to build and install Crisp, and assuming
that commands `clang` and `clang++` are accessible from the `PATH`
variable, type:

    make
    make install

As usual, you can specify a particular building compiler with
environment variables `CC` and `CXX`. E.g., write

    make CC=$LLVM_OBJ_ROOT/$BUILD_MODE/bin/clang \
      CXX=$LLVM_OBJ_ROOT/$BUILD_MODE/bin/clang++

to compile Crisp using the same compiler you are building an add-on
for, where `BUILD_MODE` can be `Release`, `Debug+Asserts`, or another
combination describing the debugging/optimizing/profiling options you
have used to build LLVM/clang (see Prerequisite 1 above).

If build/install works correctly, it should drop two shared libraries
in a `lib` sub-directory of your installation place: `crispclang.so`
and `crispllvm.so`. They are a `clang` plugin and a loadable analysis
pass for `opt` command, respectively. They are meant to work together
to detect and report rule violations in your C/C++ code.


Basic Usage
===========

Current code is still a proof of concept with very limited
functionality. The very small number of coding rules already
implemented can be found in the `prolog/Rules` directory of the source
distribution. You can test them with your own C/C++ code, or using the
example code in `docs/examples`.

For example, you can enable coding rule validation for testing file
`hicpp_3_3_11.cpp` with

    cd $CRISP_SRC_ROOT/docs/examples
    $LLVM_OBJ_ROOT/$BUILD_MODE/bin/clang++ -cc1                       \
      -load $CRISP_INSTALL_ROOT/lib/crispclang.so                     \
      -add-plugin crisp-clang -plugin-arg-crisp-clang SomeHICPPrules  \
      -emit-llvm hicpp_3_3_13.cpp

where the meaning of all the options and variables used is the
following:

- `-cc1`: run the clang compiler, not the driver that invokes the
  different LLVM tools.
- `-load $CRISP_INSTALL_ROOT/lib/crispclang.so`: dynamically load a
  plugin that emits a warning message when some coding rule is
  violated. `$CRISP_INSTALL_ROOT` is the base directory where you have
  asked to install Crisp during configuration (see section "Build
  Crisp" above). If you have compiled Crisp with option
  `ENABLE_DATA_OBJ_ROOT=1`, all the necessary data to run the plugin
  can be found in the build tree (assume that its root is
  `$CRISP_OBJ_ROOT`), and you can load the plugin found in
  `$CRISP_OBJ_ROOT/$BUILD_MODE/lib/crispclang.so` (`$BUILD_MODE` is
  one of *Release*, *Debug+Asserts*, etc.)
- `-add-plugin crisp-clang`: run the plugin.
- `-plugin-arg-crisp-clang SomeHICPPrules`: an argument to the plugin
  to choose a file with rule definitions (so far codified in
  Prolog). A `.pl` extension is optional. Rule files are first
  searched in the working directory, and then in a specific directory
  of the distribution/installation.
- `-emit-llvm`: compile to LLVM IR.
- `hicpp_3_3_13.cpp`: input file with C++ code to compile/analyze.

Some rules need a second step (those that rely on alias analysis
information):

    $LLVM_OBJ_ROOT/$BUILD_MODE/bin/clang++ -cc1                       \
      -load $CRISP_INSTALL_ROOT/lib/crispclang.so                     \
      -add-plugin crisp-clang -plugin-arg-crisp-clang SomeHICPPrules  \
      -emit-llvm hicpp_3_4_2.cpp

    $LLVM_OBJ_ROOT/$BUILD_MODE/bin/opt -analyze                       \
      -load $CRISP_INSTALL_ROOT/lib/crispllvm.so                      \
      -crisp-mod -crisp-rules-file SomeHICPPrules                     \
       -basicaa hicpp_3_4_2.ll > /dev/null

The first command is the same shown before, but applied to example
file `hicpp_3_4_2.cpp`. The second command runs the optimization and
analysis tool of LLVM, and executes an analysis pass for rule
validation based on alias analysis. The precision of rule checkers
depends on the sophistication of the alias analysis used. `-basicaa`
enables a simple alias analysis algorithm. Other useful alias analysis
implementations require installing loadable modules distributed
separately. The command takes as input a `.ll` file generated in the
previous step.
