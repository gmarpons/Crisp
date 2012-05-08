Crisp
=====

A coding rule validation add-on for **LLVM/clang**, GPL v3
licensed.

**Coding Rules** constrain admissible constructs of a language to help
produce better code. Crisp can be used to define and enforce rules for
C and C++, like those in
[High Integrity C++](http://www.codingstandard.com/), and
[CERT's Secure Coding Standards](http://www.cert.org/secure-coding/).

Crisp rules are written in Prolog. A high-level declarative DSL to
easily write new rules is under development. It will be called CRISP,
an acronym for *Coding Rules in Sugared Prolog*.


Installation
============

Although Crisp should be quite portable, it has been tested on
GNU/Linux only so far.

Prerequisites
-------------

1. Install and build **LLVM/clang sources**. You can follow
   instructions in [http://clang.llvm.org/get_started.html](). There
   are also Git repositories available for both LLVM and clang:
   - [http://llvm.org/git/llvm.git]()
   - [http://llvm.org/git/clang.git]()

   Crisp build system expects to find LLVM/clang commands compiled in
   *Release* mode, so you need to set environment variables
   `ENABLE_OPTIMIZED=1` and `DISABLE_ASSERTIONS=1` when running `make`
   in step 5 of the "Getting Started" documentation above (see more
   information about these variables at
   [http://www.llvm.org/docs/GettingStarted.html#compile]()).

2. Install **SWI-Prolog**. It is most likely provided as a
   pre-compiled package for your GNU/Linux distribution. On
   Debian/Ubuntu (and derivatives) you simply need to type `sudo
   apt-get install swi-prolog`.

3. Install **Boost C++ Libraries**. General installation instructions
   for Unix/Linux can be found here:
   [http://www.boost.org/doc/libs/1_49_0/more/getting_started/unix-variants.html]().
   Crisp relies on header-only libraries. On Debian/Ubuntu (and
   derivatives) you only need to type `sudo apt-get install
   libboost-dev`.

Download Crisp source code
--------------------------

Source code is available at GitHub:

    git clone git://github.com/gmarpons/Crisp.git

Build Crisp
-----------

Let `LOCAL_LLVM_SRC_ROOT` (resp. `LOCAL_LLVM_OBJ_ROOT`) the root of
your LLVM/clang source (resp. build) tree, and `LOCAL_CRISP_SRC_ROOT`
the root of your Crisp source tree. Then do the following:

    mkdir LOCAL_CRISP_SRC_ROOT/projects/crisp
    cd LOCAL_CRISP_SRC_ROOT/projects/crisp
    LOCAL_CRISP_SRC_ROOT/configure \
      --with-llvmsrc=LOCAL_LLVM_SRC_ROOT \
      --with-llvmobj=LOCAL_LLVM_OBJ_ROOT
      
You will probably need also to set the header directory for
SWI-Prolog, with the `configure`'s extra argument
`--with-swipl-includes`. E.g., in Debian/Ubuntu (and derivatives) you
need to use
`--with-swipl-includes=/usr/lib/swi-prolog/include/`. Documentation
for other `configure` command line options (such as `--prefix`) can be
obtained typing

    LOCAL_CRISP_SRC_ROOT/configure --help

In order to build and install Crisp, type:

    make
    make install

Compilation is possible with a recent version of LLVM/clang (it does
not work with GCC). As usual, you can specify a particular building
compiler with environment variables `CC` and `CXX`. E.g., write

    make CC=LOCAL_LLVM_OBJ_ROOT/BUILD_MODE/bin/clang \
      CXX=LOCAL_LLVM_OBJ_ROOT/BUILD_MODE/bin/clang++

to compile Crisp using the same compiler you are building an add-on
for, where `BUILD_MODE` can be `Release`, `Debug+Asserts`, or another
combination describing the debugging/optimizing/profiling options you
have used to build LLVM/clang (default is `Debug+Asserts`).

If you want to test Crisp without installing it, instead of running
`make install` you can simply add variable `ENABLE_DATA_OBJ_ROOT=1` to
your make command. This will allow Crisp access to necessary
initialization data directly from the build tree.


Basic Usage
===========

*TODO*


Known Issues
============

- Building Crisp in *Release* mode does not currently work.
