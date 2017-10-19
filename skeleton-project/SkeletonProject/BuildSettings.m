
(* Specify build settings such as compiler and linker flags, libraries to be linked, etc. here *)

Switch[$OperatingSystem,
  "MacOSX", (* Compilation settings for OS X *)
  $buildSettings = {
    "CompileOptions" -> {(* "-std=c++14", "-mmacosx-version-min=10.9" *)}

    (*
    , "IncludeDirectories" -> {}
    , "LibraryDirectories" -> {}
    *)
  },

  "Unix", (* Compilation settings for Linux *)
  $buildSettings = {
    "CompileOptions" -> {(* "-std=c++14" *)}

    (*
    , "IncludeDirectories" -> {}
    , "LibraryDirectories" -> {}
    *)
  },

  "Windows", (* Compilation settings for Windows *)
  $buildSettings = { 
    "CompileOptions" -> {"/EHsc", "/wd4244", "/DNOMINMAX"}

    (*
    , "IncludeDirectories" -> {}
    , "LibraryDirectories" -> {}
    *)
  }
]
