(* Mathematica Package *)
(* Based on LTemplate, https://github.com/szhorvat/LTemplate *)

(**************************************************************************************

This is a skeleton for an LTemplate project designed to run on multiple platforms.

To use it as the basis of your own project:

 - Change all occurrences of "SkeletonProject" in all files to the desired name of
     your package.  Find & Replace is safe to use for this (match everything, not just
     whole words).  Check the following files in particular:
     SkeletonProject.m, SkeletonProject.h, Kernel/init.m
 - Rename files accordingly.
 - Copy the LTemplate directory within your project directory.
 - Edit BuildSettings.m to set compilation and linking flags if necessary.
 - Search below for comments that are marked TODO
     and make adjustments as necessary.
 - Only if your projects depends on separately compiled shared libraries (DLLs):
     Copy them into the appropriate directory within LibraryResources.
     Create a file called dependencies.m in the same directory and
     add LibraryLoad calls for the dependencies in the correct order.
 - Edit YourName below.
 - Remove these instructions.

***************************************************************************************)


(* :Title: SkeletonProject *)
(* :Context: SkeletonProject` *)
(* :Author: YourName *)
(* :Date:  *)

(* :Package Version: 0.1 *)
(* :Mathematica Version: 10.0 *)
(* :Copyright: (c) 2017 YourName *)
(* :Keywords: *)
(* :Discussion: *)

BeginPackage["SkeletonProject`"]

(* Privately load LTemplate. Note the leading ` character which ensures that the embedded LTemplate gets loaded. *)
Get["`LTemplate`LTemplatePrivate`"];

(* ConfigureLTemplate[] *must* be called at this point. You should also supply a symbol from the SkeletonProject`
   context (called SkeletonProject here) to associate LTemplate's standard ::error, ::warning, etc. messages with. *)
ConfigureLTemplate["MessageSymbol" -> SkeletonProject];

(* Public package symbols go here: *)

SkeletonProject::usage = "SkeletonProject is a symbol to which package messages are associated.";

SkeletonProjectVersion::usage = "SkeletonProjectVersion[]"; (* TODO example function, remove if not used *)

`Developer`Recompile::usage = "SkeletonProject`Developer`Recompile[] recompiles the SkeletonProject library and reloads the functions.";
PrependTo[$ContextPath, $Context <> "Developer`"];


Begin["`Private`"]

(* Helper function: abort package loading and leave a clean $ContextPath behind *)
packageAbort[] := (End[]; EndPackage[]; Abort[])

$packageVersion    = "0.1";
$packageDirectory  = DirectoryName[$InputFileName];

$systemID = $SystemID;

(* On OS X libraries use libc++ ABI since M10.4 and libstdc++ ABI up to M10.3.
   We need separate binaries to support M10.3 and earlier.
   http://community.wolfram.com/groups/-/m/t/816033 *)
If[$OperatingSystem === "MacOSX", $systemID = $systemID <> If[$VersionNumber <= 10.3, "-libstdc++", "-libc++"]];

$libraryDirectory  = FileNameJoin[{$packageDirectory, "LibraryResources", $systemID}];
$sourceDirectory   = FileNameJoin[{$packageDirectory, "LibraryResources", "Source"}];
$buildSettingsFile = FileNameJoin[{$packageDirectory, "BuildSettings.m"}];

(* Add $libraryDirectory to $LibraryPath in case package is not installed in $UserBaseDirectory/Applications. *)
If[Not@MemberQ[$LibraryPath, $libraryDirectory],
  PrependTo[$LibraryPath, $libraryDirectory]
]


(***** The library template *****)

template =
    LClass["SkeletonProject",
      {
        LFun["version", {}, Real]
      }
    ];


(***** Compilation, loading and initialization *****)

$buildSettings = None;
If[FileExistsQ[$buildSettingsFile], Get[$buildSettingsFile] ]


Recompile::build = "No build settings found. Please check BuildSettings.m."

Recompile[] :=
    Module[{},
      (* abort compilation if no build settings are present *)
      If[$buildSettings === None,
        Message[Recompile::build];
        Return[$Failed]
      ];
      (* create directory for binary if it doesn't exist yet *)
      If[Not@DirectoryQ[$libraryDirectory],
        CreateDirectory[$libraryDirectory]
      ];
      (* compile code *)
      SetDirectory[$sourceDirectory];
      Quiet@UnloadTemplate[template];
      CompileTemplate[template, { (* TODO add any extra .cpp source files to be included in the compilation *) },
        "ShellCommandFunction" -> Print, "ShellOutputFunction" -> Print,
        "TargetDirectory" -> $libraryDirectory,
        Sequence @@ $buildSettings
      ];
      ResetDirectory[];
      (* load library *)
      loadSkeletonProject[] (* defined below *)
    ]


loadSkeletonProject[] :=
    Module[{deps},
      (* mechanism for loading shared library dependencies, if necessary *)
      deps = FileNameJoin[{$libraryDirectory, "dependencies.m"}];
      Check[
        If[FileExistsQ[deps], Get[deps]],
        Return[$Failed]
      ];
      (* load the library *)
      If[LoadTemplate[template] === $Failed,
        Return[$Failed]
      ];
      (* TODO add any other post-loading initialization if necessary *)
    ]


(* Load library, compile if necessary. *)
If[LoadTemplate[template] === $Failed,
  Print[Style["Loading failed, trying to recompile ...", Red]];
  If[Recompile[] === $Failed
    ,
    Print[Style["Cannot load or compile library. \[SadSmiley] Aborting.", Red]];
    packageAbort[] (* cleanly abort package loading *)
    ,
    Print[Style["Successfully compiled and loaded the library. \[HappySmiley]", Red]];
  ]
]


(***** Definitions of package functions *****)


SkeletonProjectVersion[] := Block[{obj = Make["SkeletonProject"]}, obj@"version"[]] (* TODO example function, remove if not used *)


End[] (* `Private` *)

EndPackage[]
