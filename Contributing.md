Contributing to PixelCayman
===========================

PixelCayman is just starting out and contributions of any kind are very welcome.

Issue Tracker
-------------

If you find a bug, or have a suggestion for a new feature you can report it
in the [issue tracker](https://github.com/mbasaglia/PixelCayman/issues) or
on IRC (#PixelCaymen on FreeNode, highlight Melanosuchus).

Non-Technical Contributions
---------------------------

PixelCayman needs can benefit from contributions which don't require
technical skills including (but not limited to):

 * Feedback regarding bugs, missing features,
   or anything that can be improved upon
 * Artwork for icons, logos and similar
   (both vector and pixel artists are welcome)
 * Translations into other languages
 * Spreading the word
 * Creating tutorials, overviews and other documentation
 * Just using PixelCayman also helps keeping it going

Source Documentation
--------------------

The code is documented using Doxygen, `make doc` will generate the HTML files.
`make doc-view` uses `xdg-open` to display the documentation.
This includes some details on how to write plugins (see src/plugin.dox)

License
-------

PixelCayman is released under the terms of the GPLv3+ (see COPYING), all
contributions must be under the same license to be included into PixelCayman.

Coding Standards
----------------
The main core of PixelCayman is written in modern C++11, using the Qt5
framework.

### Modularity

For an overview of the directory structure and namespaces, see src/directory.dox.

If it's possible to add a new feature using a plugin (because it uses the
available registrable services documented in src/plugin.dox)
is advised to do so.

### Typographical Style

When implementing interfaces similar to the concepts described in the C++
standard library, names should follow the conventions used by standard C++
(lowercase names and underscores), otherwise use the following conventions.

User-defined types and enum should have an uppercase CamelCase name.
Member functions should have a lowercase camelCase name.
Data members should be in lowercase and use underscores to separate words,
if a name might clash with a function, append an extra underscore.
Namespaces shall have a lowercase name.

Indentation is performed using 4 plain spaces.
Braces go on a new line for most constructs (like classes, functions and
control structures), possible exceptions are brace initializers and
one-line lambda literals.

### Object Orientation

When inheriting polymorphic classes, overridden functions must be marked
**override** (or **final**).

For classes that have virtual functions, member functions that might change
its internal state or inherit other classes; all data members must be
**private** and access to them shall be local to the current class or done
via accessor functions.

### Qt

Classes inhering from QObject either directly or indirectly shall contain
the Q_OBJECT macro.

When connecting signals to slots, the connect overloads taking a
function pointer or a function object are to be preferred to those which
use the function names (with SIGNAL and SLOT macros).
