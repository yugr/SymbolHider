[![License](http://img.shields.io/:license-MIT-blue.svg)](https://github.com/yugr/SymbolHider/blob/master/LICENSE.txt)
[![Build Status](https://github.com/yugr/SymbolHider/actions/workflows/ci.yml/badge.svg)](https://github.com/yugr/SymbolHider/actions)
[![codecov](https://codecov.io/gh/yugr/SymbolHider/branch/master/graph/badge.svg)](https://codecov.io/gh/yugr/SymbolHider)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/yugr/SymbolHider.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/yugr/SymbolHider/alerts/)
[![Coverity Scan](https://scan.coverity.com/projects/yugr-SymbolHider/badge.svg)](https://scan.coverity.com/projects/yugr-SymbolHider)

# What's this?

SymbolHider is a simple tool which hides symbols in shared library's public interface
(by rewriting their visibility to `hidden`).

The tool is inspired by @EmployedRussian suggestion in [Is there any way to override the -fvisibility=hidden at link time?](https://stackoverflow.com/questions/36273404/is-there-any-way-to-override-the-fvisibility-hidden-at-link-time) although I also enabled it for a more common case of fully linked binaries.

# How to use

To hide some symbols in a library, run tool like
```
$ sym-hider libxyz.so foo bar
```

To instead _unhide_ some symbols, run tool like
```
$ sym-hider --unhide libxyz.so foo bar
```
(usually this will not work for already linked files i.e. executables and shlibs).

For more details run
```
$ sym-hider -h
```

# TODO

* Support 32-bit ELFs
* Hide by wildcards
* More tests
* Support static libs (?)
* Warn on missing symbols
