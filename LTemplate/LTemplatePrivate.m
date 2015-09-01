(* Mathematica Package *)

(*
 * To include LTemplate privately in another package, load LTemplatePrivate.m using Get[],
 * then immediately call ConfigureLTemplate[].
 *)

BeginPackage["`LTemplate`", {"SymbolicC`", "CCodeGenerator`", "CCompilerDriver`"}]

Get@FileNameJoin[{DirectoryName[$InputFileName], "LTemplateInner.m"}]

EndPackage[]
