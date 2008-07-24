function siggen(source_path)
  diary([source_path,'/siggen_diary.txt']);
  genfiles = {};
  file_list = {};
  file_list = [file_list;helpgen_rdir([source_path,'/libs'])];
  file_list = [file_list;helpgen_rdir([source_path,'/src'])];
  s = [];
  for i=1:numel(file_list)
    siggen_processfile(file_list{i},s);
  end
  siggen_genloaders(s);
end
  
function siggen_processfile(filename,&s)
  [path,name,suffix] = fileparts(filename);
  if (~strcmp(suffix,'.cpp')) return; end
  %  printf('Processing file %s...\n',filename);
  try 
    fp = fopen(filename,'r');
  catch
    return;
  end
  while (1)
    line = fgetline(fp);
    if (feof(fp))
      fclose(fp);
      return;
    end
    if (testmatch(line,'@@Signature\s*(\w*)'))
      try
      fline = fgetline(fp);
      iline = fgetline(fp);
      oline = fgetline(fp);
      ftokens = regexp(fline,'//\s*(\w*)\s*([@\w:]*)\s*(\w*)','tokens');
      function_type = ftokens{1}{1};
      function_name = ftokens{1}{2};
      function_internal_name = ftokens{1}{3};
      if (isempty(function_name) | isempty(function_internal_name))
         printf('MISSING Function Name: %s %s %s\n',fline,function_name,function_internal_name);
      end
      iline = strtrim(strrep(iline,'//inputs',''));
      iline = strtrim(strrep(iline,'//input',''));
      iline = regexprep(iline,'\s+','","');
      oline = strtrim(strrep(oline,'//outputs',''));
      oline = strtrim(strrep(oline,'//output',''));
      oline = regexprep(oline,'\s+','","');
      if (testmatch(iline,'input') || testmatch(oline,'output'))
         printf('MALFORMED Function I/O spec: %s, %s\n',iline,oline);
      end
      catch
         printf('MALFORMED Function Signature: %s\n',fline);
      end
      %     printf('function line: %s',fline);
      %     printf('input line: %s',iline);
      %     printf('output line: %s',oline);
      %     printf('function code: %s\n',function_type);
      p.function_type = function_type;
      p.function_name = function_name;
      p.function_internal_name = function_internal_name;
      p.iline = iline;
      p.oline = oline;
      if (isempty(s))
         s = p;
      else
         s = [s,p];
      end
   end
  end
end

function siggen_genloaders(s)
  fp = fopen('libs/libCore/Loader.cpp','w');
  fprintf(fp,'// WARNING - This file is automatically generated by\n');
  fprintf(fp,'// the siggen.m script in src/toolbox/help.  Do not\n');
  fprintf(fp,'// edit it by hand, or changes will be lost!  If you\n');
  fprintf(fp,'// need to add a built in function to FreeMat, add a\n');
  fprintf(fp,'// signature block to its documentation and rerun siggen.\n');
  fprintf(fp,'\n\n// First each function gets prototyped.\n');
  fprintf(fp,'#include "Array.hpp"\n');
  fprintf(fp,'#include "Context.hpp"\n');
  fprintf(fp,'\n\n');
  for i=1:numel(s)
     switch (s(i).function_type)
        case 'function'
        fprintf(fp,'ArrayVector %s(int, const ArrayVector&);\n',s(i).function_internal_name);
        case 'sfunction'
        fprintf(fp,'ArrayVector %s(int, const ArrayVector&, Interpreter*);\n',s(i).function_internal_name);
        case 'gfunction'
        fprintf(fp,'ArrayVector %s(int, const ArrayVector&);\n',s(i).function_internal_name);
        case 'sgfunction'
        fprintf(fp,'ArrayVector %s(int, const ArrayVector&, Interpreter*);\n',s(i).function_internal_name);
        otherwise
        error(['Unsupported function type ' s(i).function_type]);
     end
  end
  fprintf(fp,'\n\nvoid LoadBuiltinFunctions(Context *context, bool guiflag) {\n');
  for i=1:numel(s)
    switch (s(i).function_type)
      case 'function'
      fstring = 'Function';
      case 'sfunction'
      fstring = 'SpecialFunction';
      case 'gfunction'
      fstring = 'GfxFunction';
      case 'sgfunction'
      fstring = 'GfxSpecialFunction';
      otherwise
      error(['Unsupported function type ' s(i).function_type]);
    end
    if (strcmp(s(i).function_type,'gfunction') || strcmp(s(i).function_type,'sgfunction'))
      fprintf(fp,'   if (guiflag)\n   ');
    end
    fprintf(fp,'  context->add%s("%s",%s,',fstring,s(i).function_name,s(i).function_internal_name);
    if (strcmp(s(i).iline,'none')==1)
      input_count = 0;
    elseif (testmatch(s(i).iline,'varargin'))
      input_count = -1;
    else
      input_count = numel(find(s(i).iline==','))+1;
    end
    if (strcmp(s(i).oline,'none')==1)
      output_count = 0;
    elseif (testmatch(s(i).oline,'varargout'))
      output_count = -1;
    else
      output_count = numel(find(s(i).oline==','))+1;
    end
    fprintf(fp,'%d,%d,',input_count,output_count);
    if (input_count > 0)
      fprintf(fp,'"%s",',s(i).iline);
    end
    fprintf(fp,'NULL);\n');
  end
  fprintf(fp,'}\n\n');
  fclose(fp);
end
