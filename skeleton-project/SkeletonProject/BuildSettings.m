
(* Specify build settings such as compiler and linker flags, libraries to be linked, etc. here *)

Switch[$OperatingSystem,
  "MacOSX", (* Compilation settings for OS X *)
  $buildSettings = {
    "CompileOptions" -> {(* "-std=c++11", "-mmacosx-version-min=10.9" *)}

    (*
    , "IncludeDirectories" -> {}
    , "LibraryDirectories" -> {}
    *)
  },

  "Unix", (* Compilation settings for Linux *)
  $buildSettings = {
    "CompileOptions" -> {(* "-std=c++11" *)}

    (*
    , "IncludeDirectories" -> {}
    , "LibraryDirectories" -> {}
    *)

    (* The following is only necessary if you are including mlstream.h
       and compiling on 32-bit Linux, e.g. on a Raspberry Pi. *)
    , If[$SystemWordLength == 32,
        "Defines" -> {"MLSTREAM_32BIT_INT_AND_LONG"},
        Unevaluated@Sequence[]
      ]
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
