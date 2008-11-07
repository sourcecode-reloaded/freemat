// WARNING - This file is automatically generated by
// the siggen.m script in src/toolbox/help.  Do not
// edit it by hand, or changes will be lost!  If you
// need to add a built in function to FreeMat, add a
// signature block to its documentation and rerun siggen.


// First each function gets prototyped.
#include "Array.hpp"
#include "Context.hpp"


ArrayVector AbsFunction(int, const ArrayVector&);
ArrayVector PCodeFunction(int, const ArrayVector&, Interpreter*);
ArrayVector PermuteFunction(int, const ArrayVector&);
ArrayVector RepMatFunction(int, const ArrayVector&);
ArrayVector DiagFunction(int, const ArrayVector&);
ArrayVector WavPlayFunction(int, const ArrayVector&);
ArrayVector WavRecordFunction(int, const ArrayVector&);
ArrayVector BitandFunction(int, const ArrayVector&);
ArrayVector BitorFunction(int, const ArrayVector&);
ArrayVector BitxorFunction(int, const ArrayVector&);
ArrayVector BitcmpFunction(int, const ArrayVector&);
ArrayVector Int2BinFunction(int, const ArrayVector&);
ArrayVector Bin2IntFunction(int, const ArrayVector&);
ArrayVector LogicalFunction(int, const ArrayVector&);
ArrayVector StringFunction(int, const ArrayVector&);
ArrayVector UInt8Function(int, const ArrayVector&);
ArrayVector UInt16Function(int, const ArrayVector&);
ArrayVector UInt32Function(int, const ArrayVector&);
ArrayVector UInt64Function(int, const ArrayVector&);
ArrayVector Int8Function(int, const ArrayVector&);
ArrayVector Int16Function(int, const ArrayVector&);
ArrayVector Int32Function(int, const ArrayVector&);
ArrayVector Int64Function(int, const ArrayVector&);
ArrayVector FloatFunction(int, const ArrayVector&);
ArrayVector FloatFunction(int, const ArrayVector&);
ArrayVector DoubleFunction(int, const ArrayVector&);
ArrayVector ComplexFunction(int, const ArrayVector&);
ArrayVector DcomplexFunction(int, const ArrayVector&);
ArrayVector TypeOfFunction(int, const ArrayVector&);
ArrayVector CeilFunction(int, const ArrayVector&);
ArrayVector CellFunction(int, const ArrayVector&);
ArrayVector ClearFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ConjFunction(int, const ArrayVector&);
ArrayVector InfFunction(int, const ArrayVector&);
ArrayVector InfFunction(int, const ArrayVector&);
ArrayVector NaNFunction(int, const ArrayVector&);
ArrayVector NaNFunction(int, const ArrayVector&);
ArrayVector IFunction(int, const ArrayVector&);
ArrayVector IFunction(int, const ArrayVector&);
ArrayVector PiFunction(int, const ArrayVector&);
ArrayVector EFunction(int, const ArrayVector&);
ArrayVector EpsFunction(int, const ArrayVector&);
ArrayVector FepsFunction(int, const ArrayVector&);
ArrayVector TrueFunction(int, const ArrayVector&);
ArrayVector FalseFunction(int, const ArrayVector&);
ArrayVector JITControlFunction(int, const ArrayVector&, Interpreter*);
ArrayVector DbAutoFunction(int, const ArrayVector&, Interpreter*);
ArrayVector Conv2Function(int, const ArrayVector&);
ArrayVector CenumFunction(int, const ArrayVector&);
ArrayVector CtypeDefineFunction(int, const ArrayVector&);
ArrayVector CtypePrintFunction(int, const ArrayVector&, Interpreter*);
ArrayVector CtypeFreezeFunction(int, const ArrayVector&, Interpreter*);
ArrayVector CtypeSizeFunction(int, const ArrayVector&);
ArrayVector CtypeThawFunction(int, const ArrayVector&, Interpreter*);
ArrayVector CumprodFunction(int, const ArrayVector&);
ArrayVector CumsumFunction(int, const ArrayVector&);
ArrayVector DbDeleteFunction(int, const ArrayVector&, Interpreter*);
ArrayVector DbListFunction(int, const ArrayVector&, Interpreter*);
ArrayVector DbStopFunction(int, const ArrayVector&, Interpreter*);
ArrayVector FdumpFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ChangeDirFunction(int, const ArrayVector&, Interpreter*);
ArrayVector DirFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ListFilesFunction(int, const ArrayVector&, Interpreter*);
ArrayVector DirSepFunction(int, const ArrayVector&);
ArrayVector PrintWorkingDirectoryFunction(int, const ArrayVector&);
ArrayVector RMDirFunction(int, const ArrayVector&);
ArrayVector MKDirCoreFunction(int, const ArrayVector&);
ArrayVector FilePartsFunction(int, const ArrayVector&);
ArrayVector DeleteFunction(int, const ArrayVector&);
ArrayVector CopyFileFunction(int, const ArrayVector&);
ArrayVector DotFunction(int, const ArrayVector&);
ArrayVector EigFunction(int, const ArrayVector&);
ArrayVector EigsFunction(int, const ArrayVector&);
ArrayVector LasterrFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ErrorCountFunction(int, const ArrayVector&, Interpreter*);
ArrayVector WarningFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ErrorFunction(int, const ArrayVector&);
ArrayVector EvalFunction(int, const ArrayVector&, Interpreter*);
ArrayVector EvalInFunction(int, const ArrayVector&, Interpreter*);
ArrayVector AssignInFunction(int, const ArrayVector&, Interpreter*);
ArrayVector FevalFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ExistFunction(int, const ArrayVector&, Interpreter*);
ArrayVector FFTFunction(int, const ArrayVector&);
ArrayVector IFFTFunction(int, const ArrayVector&);
ArrayVector FopenFunction(int, const ArrayVector&);
ArrayVector FcloseFunction(int, const ArrayVector&);
ArrayVector FreadFunction(int, const ArrayVector&);
ArrayVector FwriteFunction(int, const ArrayVector&);
ArrayVector FflushFunction(int, const ArrayVector&);
ArrayVector FtellFunction(int, const ArrayVector&);
ArrayVector FeofFunction(int, const ArrayVector&);
ArrayVector FseekFunction(int, const ArrayVector&);
ArrayVector FgetlineFunction(int, const ArrayVector&);
ArrayVector FprintfFunction(int, const ArrayVector&, Interpreter*);
ArrayVector FscanfFunction(int, const ArrayVector&);
ArrayVector FindFunction(int, const ArrayVector&);
ArrayVector FloorFunction(int, const ArrayVector&);
ArrayVector FixFunction(int, const ArrayVector&);
ArrayVector HelpFunction(int, const ArrayVector&, Interpreter*);
ArrayVector HelpWinFunction(int, const ArrayVector&, Interpreter*);
ArrayVector Hex2DecFunction(int, const ArrayVector&);
ArrayVector Dec2HexFunction(int, const ArrayVector&);
ArrayVector Num2HexFunction(int, const ArrayVector&);
ArrayVector ImReadFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ImWriteFunction(int, const ArrayVector&, Interpreter*);
ArrayVector EndFunction(int, const ArrayVector&);
ArrayVector WhoFunction(int, const ArrayVector&, Interpreter*);
ArrayVector WhosFunction(int, const ArrayVector&, Interpreter*);
ArrayVector FieldNamesFunction(int, const ArrayVector&);
ArrayVector WhereFunction(int, const ArrayVector&, Interpreter*);
ArrayVector WhichFunction(int, const ArrayVector&, Interpreter*);
ArrayVector MFilenameFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ComputerFunction(int, const ArrayVector&);
ArrayVector FormatFunction(int, const ArrayVector&);
ArrayVector SetPrintLimitFunction(int, const ArrayVector&, Interpreter*);
ArrayVector GetPrintLimitFunction(int, const ArrayVector&, Interpreter*);
ArrayVector SaveFunction(int, const ArrayVector&, Interpreter*);
ArrayVector DlmReadFunction(int, const ArrayVector&);
ArrayVector LoadFunction(int, const ArrayVector&, Interpreter*);
ArrayVector GetLineFunction(int, const ArrayVector&, Interpreter*);
ArrayVector IsSameFunction(int, const ArrayVector&);
ArrayVector IsSetFunction(int, const ArrayVector&, Interpreter*);
ArrayVector IsSparseFunction(int, const ArrayVector&);
ArrayVector IsNaNFunction(int, const ArrayVector&);
ArrayVector IsNaNFunction(int, const ArrayVector&);
ArrayVector IsInfFunction(int, const ArrayVector&);
ArrayVector IsInfFunction(int, const ArrayVector&);
ArrayVector IsRealFunction(int, const ArrayVector&);
ArrayVector IsEmptyFunction(int, const ArrayVector&);
ArrayVector LUFunction(int, const ArrayVector&);
ArrayVector InvFunction(int, const ArrayVector&);
ArrayVector MaxFunction(int, const ArrayVector&);
ArrayVector MeanFunction(int, const ArrayVector&);
ArrayVector MinFunction(int, const ArrayVector&);
ArrayVector SimKeysFunction(int, const ArrayVector&, Interpreter*);
ArrayVector DiaryFunction(int, const ArrayVector&, Interpreter*);
ArrayVector QuietFunction(int, const ArrayVector&, Interpreter*);
ArrayVector SourceFunction(int, const ArrayVector&, Interpreter*);
ArrayVector BuiltinFunction(int, const ArrayVector&, Interpreter*);
ArrayVector DoCLIFunction(int, const ArrayVector&, Interpreter*);
ArrayVector SystemFunction(int, const ArrayVector&);
ArrayVector OnesFunction(int, const ArrayVector&);
ArrayVector GetPathFunction(int, const ArrayVector&, Interpreter*);
ArrayVector SetPathFunction(int, const ArrayVector&, Interpreter*);
ArrayVector PathToolFunction(int, const ArrayVector&, Interpreter*);
ArrayVector SprintfFunction(int, const ArrayVector&);
ArrayVector PrintfFunction(int, const ArrayVector&, Interpreter*);
ArrayVector DispFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ProdFunction(int, const ArrayVector&);
ArrayVector QRDFunction(int, const ArrayVector&);
ArrayVector SeedFunction(int, const ArrayVector&);
ArrayVector RandBetaFunction(int, const ArrayVector&);
ArrayVector RandIFunction(int, const ArrayVector&);
ArrayVector RandChiFunction(int, const ArrayVector&);
ArrayVector RandExpFunction(int, const ArrayVector&);
ArrayVector RandPoissonFunction(int, const ArrayVector&);
ArrayVector RandBinFunction(int, const ArrayVector&);
ArrayVector RandNBinFunction(int, const ArrayVector&);
ArrayVector RandFFunction(int, const ArrayVector&);
ArrayVector RandGammaFunction(int, const ArrayVector&);
ArrayVector RandMultiFunction(int, const ArrayVector&);
ArrayVector RandNChiFunction(int, const ArrayVector&);
ArrayVector RandNFFunction(int, const ArrayVector&);
ArrayVector RandnFunction(int, const ArrayVector&);
ArrayVector RandFunction(int, const ArrayVector&);
ArrayVector RcondFunction(int, const ArrayVector&);
ArrayVector RealFunction(int, const ArrayVector&);
ArrayVector ImagFunction(int, const ArrayVector&);
ArrayVector ReshapeFunction(int, const ArrayVector&);
ArrayVector RoundFunction(int, const ArrayVector&);
ArrayVector SizeFunction(int, const ArrayVector&);
ArrayVector SortFunction(int, const ArrayVector&);
ArrayVector SponesFunction(int, const ArrayVector&);
ArrayVector SparseFunction(int, const ArrayVector&, Interpreter*);
ArrayVector FullFunction(int, const ArrayVector&);
ArrayVector StrCmpFunction(int, const ArrayVector&);
ArrayVector StrCmpiFunction(int, const ArrayVector&);
ArrayVector StrnCmpFunction(int, const ArrayVector&);
ArrayVector StrStrFunction(int, const ArrayVector&);
ArrayVector StrRepFunction(int, const ArrayVector&);
ArrayVector RegExpFunction(int, const ArrayVector&);
ArrayVector RegExpIFunction(int, const ArrayVector&);
ArrayVector RegExpRepDriverFunction(int, const ArrayVector&);
ArrayVector DeblankFunction(int, const ArrayVector&);
ArrayVector StrTrimFunction(int, const ArrayVector&);
ArrayVector StrFindFunction(int, const ArrayVector&);
ArrayVector Num2StrFunction(int, const ArrayVector&);
ArrayVector Str2NumFunction(int, const ArrayVector&);
ArrayVector SscanfFunction(int, const ArrayVector&);
ArrayVector StructFunction(int, const ArrayVector&);
ArrayVector SumFunction(int, const ArrayVector&);
ArrayVector SVDFunction(int, const ArrayVector&);
ArrayVector TicFunction(int, const ArrayVector&);
ArrayVector ClockFunction(int, const ArrayVector&);
ArrayVector ClockToTimeFunction(int, const ArrayVector&);
ArrayVector TocFunction(int, const ArrayVector&);
ArrayVector Log1PFunction(int, const ArrayVector&);
ArrayVector LogFunction(int, const ArrayVector&);
ArrayVector SqrtFunction(int, const ArrayVector&);
ArrayVector TanhFunction(int, const ArrayVector&);
ArrayVector ArccoshFunction(int, const ArrayVector&);
ArrayVector ArcsinhFunction(int, const ArrayVector&);
ArrayVector ArcsechFunction(int, const ArrayVector&);
ArrayVector ArctanhFunction(int, const ArrayVector&);
ArrayVector CoshFunction(int, const ArrayVector&);
ArrayVector SinhFunction(int, const ArrayVector&);
ArrayVector ExpFunction(int, const ArrayVector&);
ArrayVector ExpM1Function(int, const ArrayVector&);
ArrayVector CosdFunction(int, const ArrayVector&);
ArrayVector CosFunction(int, const ArrayVector&);
ArrayVector SindFunction(int, const ArrayVector&);
ArrayVector SinFunction(int, const ArrayVector&);
ArrayVector TanFunction(int, const ArrayVector&);
ArrayVector TandFunction(int, const ArrayVector&);
ArrayVector CscFunction(int, const ArrayVector&);
ArrayVector CscdFunction(int, const ArrayVector&);
ArrayVector SecFunction(int, const ArrayVector&);
ArrayVector CotFunction(int, const ArrayVector&);
ArrayVector ArccosFunction(int, const ArrayVector&);
ArrayVector ArcsinFunction(int, const ArrayVector&);
ArrayVector ArcTanFunction(int, const ArrayVector&);
ArrayVector Arctan2Function(int, const ArrayVector&);
ArrayVector UniqueFunction(int, const ArrayVector&);
ArrayVector VarFunction(int, const ArrayVector&);
ArrayVector VersionFunction(int, const ArrayVector&);
ArrayVector VerStringFunction(int, const ArrayVector&);
ArrayVector XMLReadFunction(int, const ArrayVector&);
ArrayVector HTMLReadFunction(int, const ArrayVector&);
ArrayVector URLWriteFunction(int, const ArrayVector&);
ArrayVector XNrm2Function(int, const ArrayVector&);
ArrayVector ZerosFunction(int, const ArrayVector&);
ArrayVector ErfcFunction(int, const ArrayVector&);
ArrayVector ErfFunction(int, const ArrayVector&);
ArrayVector GammaFunction(int, const ArrayVector&);
ArrayVector GammaLnFunction(int, const ArrayVector&);
ArrayVector Interplin1Function(int, const ArrayVector&);
ArrayVector FitFunFunction(int, const ArrayVector&, Interpreter*);
ArrayVector AnonFuncDispFunction(int, const ArrayVector&, Interpreter*);
ArrayVector AnonFuncSubsrefFunction(int, const ArrayVector&, Interpreter*);
ArrayVector AnonFuncFevalFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ClassFunction(int, const ArrayVector&);
ArrayVector FuncPtrDispFunction(int, const ArrayVector&, Interpreter*);
ArrayVector FuncPtrHorzCatFunction(int, const ArrayVector&);
ArrayVector FuncPtrSubsrefFunction(int, const ArrayVector&, Interpreter*);
ArrayVector FuncPtrFevalFunction(int, const ArrayVector&, Interpreter*);
ArrayVector FuncPtrSubsasgnFunction(int, const ArrayVector&, Interpreter*);
ArrayVector LoadLibFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ImportFunction(int, const ArrayVector&, Interpreter*);
ArrayVector DrawNowFunction(int, const ArrayVector&);
ArrayVector HFigureFunction(int, const ArrayVector&);
ArrayVector HAxesFunction(int, const ArrayVector&);
ArrayVector HSetFunction(int, const ArrayVector&);
ArrayVector HGetFunction(int, const ArrayVector&);
ArrayVector HLineFunction(int, const ArrayVector&);
ArrayVector HContourFunction(int, const ArrayVector&);
ArrayVector HUIControlFunction(int, const ArrayVector&, Interpreter*);
ArrayVector HImageFunction(int, const ArrayVector&);
ArrayVector HTextFunction(int, const ArrayVector&);
ArrayVector HSurfaceFunction(int, const ArrayVector&);
ArrayVector FigRaiseFunction(int, const ArrayVector&);
ArrayVector FigLowerFunction(int, const ArrayVector&);
ArrayVector HGCFFunction(int, const ArrayVector&);
ArrayVector HGCAFunction(int, const ArrayVector&);
ArrayVector HPropertyValidateFunction(int, const ArrayVector&);
ArrayVector HCloseFunction(int, const ArrayVector&);
ArrayVector HCopyFunction(int, const ArrayVector&);
ArrayVector HPrintFunction(int, const ArrayVector&);
ArrayVector HPointFunction(int, const ArrayVector&);
ArrayVector HIs2DViewFunction(int, const ArrayVector&);
ArrayVector EditorFunction(int, const ArrayVector&, Interpreter*);
ArrayVector EditFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ThreadIDFunction(int, const ArrayVector&, Interpreter*);
ArrayVector PauseFunction(int, const ArrayVector&, Interpreter*);
ArrayVector SleepFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ThreadNewFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ThreadStartFunction(int, const ArrayVector&, Interpreter*);
ArrayVector ThreadValueFunction(int, const ArrayVector&);
ArrayVector ThreadWaitFunction(int, const ArrayVector&);
ArrayVector ThreadKillFunction(int, const ArrayVector&);
ArrayVector ThreadFreeFunction(int, const ArrayVector&);
ArrayVector ClcFunction(int, const ArrayVector&);
ArrayVector ProfilerFunction(int, const ArrayVector&);


void LoadBuiltinFunctions(Context *context, bool guiflag) {
  context->addFunction("abs",AbsFunction,1,1,"x",NULL);
  context->addSpecialFunction("pcode",PCodeFunction,-1,0,NULL);
  context->addFunction("permute",PermuteFunction,2,1,"x","p",NULL);
  context->addFunction("repmat",RepMatFunction,3,1,"x","rows","cols",NULL);
  context->addFunction("diag",DiagFunction,2,1,"x","n",NULL);
  context->addFunction("wavplay",WavPlayFunction,3,0,"y","sampling_rate","mode",NULL);
  context->addFunction("wavrecord",WavRecordFunction,-1,1,NULL);
  context->addFunction("bitand",BitandFunction,2,1,"a","b",NULL);
  context->addFunction("bitor",BitorFunction,2,1,"a","b",NULL);
  context->addFunction("bitxor",BitxorFunction,2,1,"a","b",NULL);
  context->addFunction("bitcmp_cpp",BitcmpFunction,1,1,"a",NULL);
  context->addFunction("int2bin",Int2BinFunction,2,1,"x","bits",NULL);
  context->addFunction("bin2int",Bin2IntFunction,2,1,"x","flag",NULL);
  context->addFunction("logical",LogicalFunction,1,1,"x",NULL);
  context->addFunction("string",StringFunction,1,1,"x",NULL);
  context->addFunction("uint8",UInt8Function,1,1,"x",NULL);
  context->addFunction("uint16",UInt16Function,1,1,"x",NULL);
  context->addFunction("uint32",UInt32Function,1,1,"x",NULL);
  context->addFunction("uint64",UInt64Function,1,1,"x",NULL);
  context->addFunction("int8",Int8Function,1,1,"x",NULL);
  context->addFunction("int16",Int16Function,1,1,"x",NULL);
  context->addFunction("int32",Int32Function,1,1,"x",NULL);
  context->addFunction("int64",Int64Function,1,1,"x",NULL);
  context->addFunction("float",FloatFunction,1,1,"x",NULL);
  context->addFunction("single",FloatFunction,1,1,"x",NULL);
  context->addFunction("double",DoubleFunction,1,1,"x",NULL);
  context->addFunction("complex",ComplexFunction,2,1,"x","z",NULL);
  context->addFunction("dcomplex",DcomplexFunction,1,1,"x",NULL);
  context->addFunction("typeof",TypeOfFunction,1,1,"x",NULL);
  context->addFunction("ceil",CeilFunction,1,1,"x",NULL);
  context->addFunction("cell",CellFunction,-1,1,NULL);
  context->addSpecialFunction("clear",ClearFunction,-1,0,NULL);
  context->addFunction("conj",ConjFunction,1,1,"x",NULL);
  context->addFunction("inf",InfFunction,0,1,NULL);
  context->addFunction("Inf",InfFunction,0,1,NULL);
  context->addFunction("nan",NaNFunction,0,1,NULL);
  context->addFunction("NaN",NaNFunction,0,1,NULL);
  context->addFunction("i",IFunction,0,1,NULL);
  context->addFunction("j",IFunction,0,1,NULL);
  context->addFunction("pi",PiFunction,0,1,NULL);
  context->addFunction("e",EFunction,0,1,NULL);
  context->addFunction("eps",EpsFunction,-1,1,NULL);
  context->addFunction("feps",FepsFunction,-1,1,NULL);
  context->addFunction("true",TrueFunction,0,1,NULL);
  context->addFunction("false",FalseFunction,0,1,NULL);
  context->addSpecialFunction("jitcontrol",JITControlFunction,1,1,"flag",NULL);
  context->addSpecialFunction("dbauto",DbAutoFunction,1,1,"flag",NULL);
  context->addFunction("conv2",Conv2Function,4,1,"hcol","hrow","X","shape",NULL);
  context->addFunction("cenum",CenumFunction,2,1,"enumtype","enumint",NULL);
  context->addFunction("ctypedefine",CtypeDefineFunction,-1,0,NULL);
  context->addSpecialFunction("ctypeprint",CtypePrintFunction,1,0,"typename",NULL);
  context->addSpecialFunction("ctypefreeze",CtypeFreezeFunction,2,1,"mystruct","typename",NULL);
  context->addFunction("ctypesize",CtypeSizeFunction,2,1,"type","count",NULL);
  context->addSpecialFunction("ctypethaw",CtypeThawFunction,3,2,"x","type","count",NULL);
  context->addFunction("cumprod",CumprodFunction,2,1,"x","dimensions",NULL);
  context->addFunction("cumsum",CumsumFunction,2,1,"x","dimension",NULL);
  context->addSpecialFunction("dbdelete",DbDeleteFunction,1,0,"num",NULL);
  context->addSpecialFunction("dblist",DbListFunction,0,0,NULL);
  context->addSpecialFunction("dbstop",DbStopFunction,2,0,"funcname","linenumber",NULL);
  context->addSpecialFunction("fdump",FdumpFunction,1,0,"fname",NULL);
  context->addSpecialFunction("cd",ChangeDirFunction,1,0,"path",NULL);
  context->addSpecialFunction("dir",DirFunction,1,1,"name",NULL);
  context->addSpecialFunction("ls",ListFilesFunction,-1,0,NULL);
  context->addFunction("dirsep",DirSepFunction,0,1,NULL);
  context->addFunction("pwd",PrintWorkingDirectoryFunction,0,1,NULL);
  context->addFunction("rmdir",RMDirFunction,2,0,"dirname","flag",NULL);
  context->addFunction("mkdir_core",MKDirCoreFunction,1,1,"dir",NULL);
  context->addFunction("fileparts",FilePartsFunction,1,4,"filename",NULL);
  context->addFunction("delete",DeleteFunction,1,0,"filename",NULL);
  context->addFunction("copyfile",CopyFileFunction,3,0,"pattern","directory_out","forceflag",NULL);
  context->addFunction("dot",DotFunction,3,1,"x1","x2","dim",NULL);
  context->addFunction("eig",EigFunction,2,2,"A","flag",NULL);
  context->addFunction("eigs",EigsFunction,3,2,"A","k","sigma",NULL);
  context->addSpecialFunction("lasterr",LasterrFunction,1,1,"msg",NULL);
  context->addSpecialFunction("errorcount",ErrorCountFunction,0,1,NULL);
  context->addSpecialFunction("warning",WarningFunction,1,0,"msg",NULL);
  context->addFunction("error",ErrorFunction,1,0,"string",NULL);
  context->addSpecialFunction("eval",EvalFunction,2,-1,"try_clause","catch_clause",NULL);
  context->addSpecialFunction("evalin",EvalInFunction,2,3,"workspace","expression",NULL);
  context->addSpecialFunction("assignin",AssignInFunction,3,0,"workspace","variablename","value",NULL);
  context->addSpecialFunction("feval",FevalFunction,-1,-1,NULL);
  context->addSpecialFunction("exist",ExistFunction,2,1,"item","kind",NULL);
  context->addFunction("fft",FFTFunction,3,1,"x","len","dim",NULL);
  context->addFunction("ifft",IFFTFunction,3,1,"x","len","dim",NULL);
  context->addFunction("fopen",FopenFunction,3,1,"fname","mode","byteorder",NULL);
  context->addFunction("fclose",FcloseFunction,1,0,"handle",NULL);
  context->addFunction("fread",FreadFunction,3,2,"handle","size","precision",NULL);
  context->addFunction("fwrite",FwriteFunction,2,1,"handle","A",NULL);
  context->addFunction("fflush",FflushFunction,1,0,"handle",NULL);
  context->addFunction("ftell",FtellFunction,1,1,"handle",NULL);
  context->addFunction("feof",FeofFunction,1,1,"handle",NULL);
  context->addFunction("fseek",FseekFunction,3,0,"handle","offset","style",NULL);
  context->addFunction("fgetline",FgetlineFunction,1,1,"handle",NULL);
  context->addSpecialFunction("fprintf",FprintfFunction,-1,0,NULL);
  context->addFunction("fscanf",FscanfFunction,2,-1,"handle","format",NULL);
  context->addFunction("find",FindFunction,3,3,"x","keep","flag",NULL);
  context->addFunction("floor",FloorFunction,1,1,"x",NULL);
  context->addFunction("fix",FixFunction,1,1,"x",NULL);
  context->addSpecialFunction("help",HelpFunction,1,0,"function",NULL);
   if (guiflag)
     context->addGfxSpecialFunction("helpwin",HelpWinFunction,1,0,"functionname",NULL);
  context->addFunction("hex2dec",Hex2DecFunction,1,1,"x",NULL);
  context->addFunction("dec2hex",Dec2HexFunction,2,1,"x","digits",NULL);
  context->addFunction("num2hex",Num2HexFunction,1,1,"x",NULL);
   if (guiflag)
     context->addGfxSpecialFunction("imread",ImReadFunction,1,3,"filename",NULL);
   if (guiflag)
     context->addGfxSpecialFunction("imwrite",ImWriteFunction,4,0,"filename","A","map","alpha",NULL);
  context->addFunction("p_end",EndFunction,3,1,"x","dim","subindexes",NULL);
  context->addSpecialFunction("who",WhoFunction,-1,0,NULL);
  context->addSpecialFunction("whos",WhosFunction,-1,0,NULL);
  context->addFunction("fieldnames",FieldNamesFunction,1,1,"y",NULL);
  context->addSpecialFunction("where",WhereFunction,0,0,NULL);
  context->addSpecialFunction("which",WhichFunction,1,1,"functionname",NULL);
  context->addSpecialFunction("mfilename",MFilenameFunction,0,1,NULL);
  context->addFunction("computer",ComputerFunction,0,1,NULL);
  context->addFunction("format",FormatFunction,2,1,"format","exptype",NULL);
  context->addSpecialFunction("setprintlimit",SetPrintLimitFunction,1,0,"linecount",NULL);
  context->addSpecialFunction("getprintlimit",GetPrintLimitFunction,0,1,NULL);
  context->addSpecialFunction("save",SaveFunction,-1,0,NULL);
  context->addFunction("dlmread",DlmReadFunction,4,1,"filename","delimiter","startrow","startcol",NULL);
  context->addSpecialFunction("load",LoadFunction,-1,1,NULL);
  context->addSpecialFunction("getline",GetLineFunction,1,1,"prompt",NULL);
  context->addFunction("issame",IsSameFunction,2,1,"a","b",NULL);
  context->addSpecialFunction("isset",IsSetFunction,1,2,"name",NULL);
  context->addFunction("issparse",IsSparseFunction,1,1,"x",NULL);
  context->addFunction("isnan",IsNaNFunction,1,1,"x",NULL);
  context->addFunction("IsNaN",IsNaNFunction,1,1,"x",NULL);
  context->addFunction("isinf",IsInfFunction,1,1,"x",NULL);
  context->addFunction("IsInf",IsInfFunction,1,1,"x",NULL);
  context->addFunction("isreal",IsRealFunction,1,1,"x",NULL);
  context->addFunction("isempty",IsEmptyFunction,1,1,"x",NULL);
  context->addFunction("lu",LUFunction,1,5,"x",NULL);
  context->addFunction("inv",InvFunction,1,1,"x",NULL);
  context->addFunction("max",MaxFunction,3,2,"x","z","dim",NULL);
  context->addFunction("mean",MeanFunction,2,1,"x","dim",NULL);
  context->addFunction("min",MinFunction,3,2,"x","z","dim",NULL);
  context->addSpecialFunction("simkeys",SimKeysFunction,1,1,"itext",NULL);
  context->addSpecialFunction("diary",DiaryFunction,1,1,"x",NULL);
  context->addSpecialFunction("quiet",QuietFunction,1,1,"mode",NULL);
  context->addSpecialFunction("source",SourceFunction,1,0,"filename",NULL);
  context->addSpecialFunction("builtin",BuiltinFunction,-1,-1,NULL);
  context->addSpecialFunction("docli",DoCLIFunction,0,0,NULL);
  context->addFunction("system",SystemFunction,1,1,"cmd",NULL);
  context->addFunction("ones",OnesFunction,-1,1,NULL);
  context->addSpecialFunction("getpath",GetPathFunction,0,1,NULL);
  context->addSpecialFunction("setpath",SetPathFunction,1,0,"y",NULL);
   if (guiflag)
     context->addGfxSpecialFunction("pathtool",PathToolFunction,0,0,NULL);
  context->addFunction("sprintf",SprintfFunction,-1,1,NULL);
  context->addSpecialFunction("printf",PrintfFunction,-1,2,NULL);
  context->addSpecialFunction("disp",DispFunction,-1,0,NULL);
  context->addFunction("prod",ProdFunction,2,1,"x","dimension",NULL);
  context->addFunction("qr",QRDFunction,2,-1,"x","n",NULL);
  context->addFunction("seed",SeedFunction,2,0,"s","t",NULL);
  context->addFunction("randbeta",RandBetaFunction,2,1,"alpha","beta",NULL);
  context->addFunction("randi",RandIFunction,2,1,"low","high",NULL);
  context->addFunction("randchi",RandChiFunction,1,1,"n",NULL);
  context->addFunction("randexp",RandExpFunction,1,1,"lambda",NULL);
  context->addFunction("randp",RandPoissonFunction,1,1,"n",NULL);
  context->addFunction("randbin",RandBinFunction,2,1,"N","p",NULL);
  context->addFunction("randnbin",RandNBinFunction,2,1,"r","p",NULL);
  context->addFunction("randf",RandFFunction,2,1,"n","m",NULL);
  context->addFunction("randgamma",RandGammaFunction,2,1,"a","r",NULL);
  context->addFunction("randmulti",RandMultiFunction,2,1,"N","pvec",NULL);
  context->addFunction("randnchi",RandNChiFunction,2,1,"n","mu",NULL);
  context->addFunction("randnf",RandNFFunction,3,1,"n","m","c",NULL);
  context->addFunction("randn",RandnFunction,-1,1,NULL);
  context->addFunction("rand",RandFunction,-1,1,NULL);
  context->addFunction("rcond",RcondFunction,1,1,"A",NULL);
  context->addFunction("real",RealFunction,1,1,"x",NULL);
  context->addFunction("imag",ImagFunction,1,1,"x",NULL);
  context->addFunction("reshape",ReshapeFunction,-1,1,NULL);
  context->addFunction("round",RoundFunction,1,1,"x",NULL);
  context->addFunction("size",SizeFunction,2,-1,"x","dim",NULL);
  context->addFunction("sort",SortFunction,3,2,"A","dim","mode",NULL);
  context->addFunction("spones",SponesFunction,1,1,"x",NULL);
  context->addSpecialFunction("sparse",SparseFunction,5,1,"i","j","v","m","n",NULL);
  context->addFunction("full",FullFunction,1,1,"x",NULL);
  context->addFunction("strcmp",StrCmpFunction,2,1,"string1","string2",NULL);
  context->addFunction("strcmpi",StrCmpiFunction,2,1,"string1","string2",NULL);
  context->addFunction("strncmp",StrnCmpFunction,3,1,"string1","string2","len",NULL);
  context->addFunction("strstr",StrStrFunction,2,1,"x","y",NULL);
  context->addFunction("strrep",StrRepFunction,3,1,"source","pattern","replace",NULL);
  context->addFunction("regexp",RegExpFunction,-1,6,NULL);
  context->addFunction("regexpi",RegExpIFunction,-1,6,NULL);
  context->addFunction("regexprepdriver",RegExpRepDriverFunction,-1,1,NULL);
  context->addFunction("deblank",DeblankFunction,1,1,"x",NULL);
  context->addFunction("strtrim",StrTrimFunction,1,1,"x",NULL);
  context->addFunction("strfind",StrFindFunction,2,1,"x","pattern",NULL);
  context->addFunction("num2str",Num2StrFunction,2,1,"x","format",NULL);
  context->addFunction("str2num",Str2NumFunction,1,1,"string",NULL);
  context->addFunction("sscanf",SscanfFunction,2,-1,"text","format",NULL);
  context->addFunction("struct",StructFunction,-1,1,NULL);
  context->addFunction("sum",SumFunction,2,1,"x","dim",NULL);
  context->addFunction("svd",SVDFunction,2,3,"A","flag",NULL);
  context->addFunction("tic",TicFunction,0,0,NULL);
  context->addFunction("clock",ClockFunction,0,1,NULL);
  context->addFunction("clocktotime",ClockToTimeFunction,1,1,"x",NULL);
  context->addFunction("toc",TocFunction,0,1,NULL);
  context->addFunction("log1p",Log1PFunction,1,1,"x",NULL);
  context->addFunction("log",LogFunction,1,1,"x",NULL);
  context->addFunction("sqrt",SqrtFunction,1,1,"x",NULL);
  context->addFunction("tanh",TanhFunction,1,1,"x",NULL);
  context->addFunction("acosh",ArccoshFunction,1,1,"x",NULL);
  context->addFunction("asinh",ArcsinhFunction,1,1,"x",NULL);
  context->addFunction("asech",ArcsechFunction,1,1,"x",NULL);
  context->addFunction("atanh",ArctanhFunction,1,1,"x",NULL);
  context->addFunction("cosh",CoshFunction,1,1,"x",NULL);
  context->addFunction("sinh",SinhFunction,1,1,"x",NULL);
  context->addFunction("exp",ExpFunction,1,1,"x",NULL);
  context->addFunction("expm1",ExpM1Function,1,1,"x",NULL);
  context->addFunction("cosd",CosdFunction,1,1,"x",NULL);
  context->addFunction("cos",CosFunction,1,1,"x",NULL);
  context->addFunction("sind",SindFunction,1,1,"x",NULL);
  context->addFunction("sin",SinFunction,1,1,"x",NULL);
  context->addFunction("tan",TanFunction,1,1,"x",NULL);
  context->addFunction("tand",TandFunction,1,1,"x",NULL);
  context->addFunction("csc",CscFunction,1,1,"x",NULL);
  context->addFunction("cscd",CscdFunction,1,1,"x",NULL);
  context->addFunction("sec",SecFunction,1,1,"x",NULL);
  context->addFunction("cot",CotFunction,1,1,"x",NULL);
  context->addFunction("acos",ArccosFunction,1,1,"x",NULL);
  context->addFunction("asin",ArcsinFunction,1,1,"x",NULL);
  context->addFunction("atan",ArcTanFunction,1,1,"x",NULL);
  context->addFunction("atan2",Arctan2Function,2,1,"y","x",NULL);
  context->addFunction("unique",UniqueFunction,2,3,"A","mode",NULL);
  context->addFunction("var",VarFunction,2,1,"x","dim",NULL);
  context->addFunction("version",VersionFunction,0,1,NULL);
  context->addFunction("verstring",VerStringFunction,0,1,NULL);
  context->addFunction("xmlread",XMLReadFunction,1,1,"filename",NULL);
  context->addFunction("htmlread",HTMLReadFunction,1,1,"filename",NULL);
  context->addFunction("urlwrite",URLWriteFunction,3,2,"url","filename","timeout",NULL);
  context->addFunction("xnrm2",XNrm2Function,1,1,"A",NULL);
  context->addFunction("zeros",ZerosFunction,-1,1,NULL);
  context->addFunction("erfc",ErfcFunction,1,1,"x",NULL);
  context->addFunction("erf",ErfFunction,1,1,"x",NULL);
  context->addFunction("gamma",GammaFunction,1,1,"x",NULL);
  context->addFunction("gammaln",GammaLnFunction,1,1,"x",NULL);
  context->addFunction("interplin1",Interplin1Function,4,1,"x1","y1","xi","extrapflag",NULL);
  context->addSpecialFunction("fitfun",FitFunFunction,-1,2,NULL);
  context->addSpecialFunction("@anonfunction:display",AnonFuncDispFunction,1,0,"x",NULL);
  context->addSpecialFunction("@anonfunction:subsref",AnonFuncSubsrefFunction,2,-1,"x","s",NULL);
  context->addSpecialFunction("@anonfunction:feval",AnonFuncFevalFunction,-1,-1,NULL);
  context->addFunction("class",ClassFunction,-1,1,NULL);
  context->addSpecialFunction("@functionpointer:display",FuncPtrDispFunction,1,0,"x",NULL);
  context->addFunction("@functionpointer:horzcat",FuncPtrHorzCatFunction,-1,1,NULL);
  context->addSpecialFunction("@functionpointer:subsref",FuncPtrSubsrefFunction,2,-1,"x","s",NULL);
  context->addSpecialFunction("@functionpointer:feval",FuncPtrFevalFunction,-1,-1,NULL);
  context->addSpecialFunction("@functionpointer:subsasgn",FuncPtrSubsasgnFunction,3,1,"x","s","y",NULL);
  context->addSpecialFunction("loadlib",LoadLibFunction,5,0,"libfile","symbolname","functionname","nargin","nargout",NULL);
  context->addSpecialFunction("import",ImportFunction,5,0,"libraryname","symbol","function","returntype","arguments",NULL);
   if (guiflag)
     context->addGfxFunction("drawnow",DrawNowFunction,0,0,NULL);
   if (guiflag)
     context->addGfxFunction("figure",HFigureFunction,1,1,"number",NULL);
   if (guiflag)
     context->addGfxFunction("axes",HAxesFunction,-1,1,NULL);
   if (guiflag)
     context->addGfxFunction("set",HSetFunction,-1,0,NULL);
   if (guiflag)
     context->addGfxFunction("get",HGetFunction,2,1,"handle","property",NULL);
   if (guiflag)
     context->addGfxFunction("hline",HLineFunction,-1,1,NULL);
   if (guiflag)
     context->addGfxFunction("hcontour",HContourFunction,-1,1,NULL);
   if (guiflag)
     context->addGfxSpecialFunction("uicontrol",HUIControlFunction,-1,1,NULL);
   if (guiflag)
     context->addGfxFunction("himage",HImageFunction,-1,1,NULL);
   if (guiflag)
     context->addGfxFunction("htext",HTextFunction,-1,1,NULL);
   if (guiflag)
     context->addGfxFunction("surface",HSurfaceFunction,-1,1,NULL);
   if (guiflag)
     context->addGfxFunction("figraise",FigRaiseFunction,1,0,"handle",NULL);
   if (guiflag)
     context->addGfxFunction("figlower",FigLowerFunction,1,0,"handle",NULL);
   if (guiflag)
     context->addGfxFunction("gcf",HGCFFunction,0,1,NULL);
   if (guiflag)
     context->addGfxFunction("gca",HGCAFunction,0,1,NULL);
   if (guiflag)
     context->addGfxFunction("pvalid",HPropertyValidateFunction,2,1,"type","property",NULL);
   if (guiflag)
     context->addGfxFunction("close",HCloseFunction,1,0,"handle",NULL);
   if (guiflag)
     context->addGfxFunction("copy",HCopyFunction,0,0,NULL);
   if (guiflag)
     context->addGfxFunction("print",HPrintFunction,-1,0,NULL);
   if (guiflag)
     context->addGfxFunction("hpoint",HPointFunction,0,1,NULL);
   if (guiflag)
     context->addGfxFunction("is2dview",HIs2DViewFunction,1,1,"handle",NULL);
   if (guiflag)
     context->addGfxSpecialFunction("editor",EditorFunction,0,0,NULL);
   if (guiflag)
     context->addGfxSpecialFunction("edit",EditFunction,-1,0,NULL);
  context->addSpecialFunction("threadid",ThreadIDFunction,0,1,NULL);
   if (guiflag)
     context->addGfxSpecialFunction("pause",PauseFunction,1,0,"flag",NULL);
   if (guiflag)
     context->addGfxSpecialFunction("sleep",SleepFunction,1,0,"n",NULL);
  context->addSpecialFunction("threadnew",ThreadNewFunction,0,1,NULL);
  context->addSpecialFunction("threadstart",ThreadStartFunction,-1,0,NULL);
  context->addFunction("threadvalue",ThreadValueFunction,2,-1,"handle","timeout",NULL);
  context->addFunction("threadwait",ThreadWaitFunction,2,1,"handle","timeout",NULL);
  context->addFunction("threadkill",ThreadKillFunction,1,0,"handle",NULL);
  context->addFunction("threadfree",ThreadFreeFunction,2,0,"handle","timeout",NULL);
   if (guiflag)
     context->addGfxFunction("clc",ClcFunction,0,0,NULL);
  context->addFunction("profiler",ProfilerFunction,-1,0,NULL);
}

