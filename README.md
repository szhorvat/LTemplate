# LTemplate

**Note:** If you plan to use LTemplate, please send me an email and let me know about your use case. If I know how people use it, I can be more careful with breaking changes.

----

LTemplate is a package that simplifies extending [_Mathematica_][1] through its [LibraryLink C/C++ API][2]. It automates the generation of boilerplate code, the loading of library functions, and using [managed library expressions][4]. LTemplate is designed to be embeddable into a larger Mathematica application that relies on LibraryLink, but it can also be used standalone in an interactive session.

### Installation

Drop the `LTemplate` directory in the directory opened by `SystemOpen@FileNameJoin[{$UserBaseDirectory, "Applications"}]`.

### Getting started

See `Documentation/LTemplateTutorial.nb` for a short introduction, and `Documentation/Examples` for many additional examples.

The `skeleton-project` directory contains a sample project that embeds LTemplate. Use this as a starting point for your own LTemplate-based package.

For an example of a mature application that uses and embeds LTemplate, see [IGraph/M][3].

See [CHANGELOG.md](CHANGELOG.md) for a list of changes between versions.

 [1]: http://www.wolfram.com/mathematica/
 [2]: https://reference.wolfram.com/language/LibraryLink/tutorial/Overview.html
 [3]: https://szhorvat.net/mathematica/IGraphM
 [4]: https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#353220453

