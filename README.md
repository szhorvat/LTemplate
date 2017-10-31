# LTemplate

**Note:** If you plan to use LTemplate, please send me an email and let me know about your use case. If I know how people use it, I can be more careful with breaking changes.

----

This is a package that simplifies extending [_Mathematica_][1] through [LibraryLink][2] by automating the generation of boilerplate code.

To install, drop the `LTemplate` directory in the directory opened by `SystemOpen@FileNameJoin[{$UserBaseDirectory, "Applications"}]`.

See `Documentation/LTemplateTutorial.nb` for a short introduction, and `Documentation/Examples` for many additional examples.

The `skeleton-project` directory contains a sample project that embeds LTemplate. Use this as a starting point for your own LTemplate-based package.

For an example of an application that uses and embeds LTemplate, see [IGraph/M][3].

 [1]: http://www.wolfram.com/mathematica/
 [2]: https://reference.wolfram.com/language/LibraryLink/tutorial/Overview.html
 [3]: https://github.com/szhorvat/IGraphM
