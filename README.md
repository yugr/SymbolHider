[![License](http://img.shields.io/:license-MIT-blue.svg)](https://github.com/yugr/SymbolHider/blob/master/LICENSE.txt)

# What's this?

SymbolHider is a simple tool which hides symbols in shared library's public interface
(by rewriting their visibility to `hidden`).

The tool is inspired by @EmployedRussian suggestion in [Is there any way to override the -fvisibility=hidden at link time?](https://stackoverflow.com/questions/36273404/is-there-any-way-to-override-the-fvisibility-hidden-at-link-time) although I also enabled it for a more common case of fully linked binaries.

# How to use

To hide some symbols in a library, run tool like
```
$ sym-hider libxyz.so foo bar
```

For more details run
```
$ sym-hider -h
```

# TODO

* Support 32-bit ELFs
* Hide by wildcards
* More tests (e.g. system test)
