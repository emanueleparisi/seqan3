# Changelog {#about_changelog}

[TOC]

This changelog contains a top-level entry for each release with sections on new features, API changes and notable
bug-fixes (not all bug-fixes will be listed).
See the documentation on [api stability](http://docs.seqan.de/seqan/3-master-user/about_api.html) to learn about
when API changes are allowed.

<!--
The following API changes should be documented as such:
  * a previously experimental interface now being marked as stable
  * an interface being removed
  * syntactical changes to an interface (e.g. renaming or reordering of files, functions, parameters)
  * semantical changes to an interface (e.g. a function's result is now always one larger) [DANGEROUS!]

If possible, provide tooling that performs the changes, e.g. a shell-script.
-->

# 3.0.1

## New features

#### Argument parser
* Simplified reading file extensions from formatted files in the input/output file validators.

#### Core
* Added traits for "metaprogramming" with `seqan3::type_list` and type packs.

## API changes

#### Argument parser

* **Changed class signature of input/output file validators:**
  Most user code will be unaffected; to fix possible compiler errors you need to add an empty template parameter list to
  the respective instances (e.g. change `input_file_validator` to `input_file_validator<>`).

#### Core

* **The `type_list` header has moved:**
  If you included `<seqan3/core/type_list.hpp>` you need to change the path to `<seqan3/core/type_list/type_list.hpp>`.

## Notable Bug-fixes

# 3.0.0 ("Escala")

Initial release of SeqAn3.
