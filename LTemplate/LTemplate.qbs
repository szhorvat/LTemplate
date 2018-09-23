import qbs

Product {
    type: ["unknown"] // E.g. "application", "dynamiclibrary", "staticlibrary"
    Depends { name: "cpp" }

    // Set the path to Mathematica's $InstallationDirectory here:
    property string mmaInstallDir: "/Applications/Mathematica 11.3.app/Contents/"

    cpp.cxxLanguageVersion: "c++11"

    cpp.includePaths: base.concat([
        mmaInstallDir + "SystemFiles/IncludeFiles/C/",
        mmaInstallDir + "SystemFiles/Links/MathLink/DeveloperKit/MacOSX-x86-64/CompilerAdditions",
        "IncludeFiles/",

        // The following are the locations of various libraries used by the example code,
        // such as boost, CGAL, etc.

        "/opt/local/include",
    ])

    cpp.defines: base.concat([
        'LTEMPLATE_MESSAGE_SYMBOL="LTemplate`LTemplate"',
        "LTEMPLATE_MMA_VERSION=1130",
    ])

    FileTagger {
        patterns: "*.inc"
        fileTags: ["cpp"]
    }

    files: [
        "IncludeFiles/*.h",
        "IncludeFiles/*.inc",
    ]

    Group {
        name: "Examples"
        prefix: "Documentation/Examples/"

        files: ["*/*.h", "*/*.cpp"]
    }
}
