function siggen(source_path)
  diary([source_path,'/siggen_diary.txt']);
  genfiles = {};
  file_list = {};
  file_list = [file_list;helpgen_rdir([source_path,'/libs'])];
  file_list = [file_list;helpgen_rdir([source_path,'/src'])];
  for i=1:numel(file_list)
    siggen_processfile(file_list{i});
  end

function siggen_processfile(filename)
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
      ftokens = regexp(fline,'//\s*(\w*)\s*(\w*)\s*(\w*)','tokens');
      function_type = ftokens{1}{1};
      function_name = ftokens{1}{2};
      function_internal_name = ftokens{1}{3};
      iline = strtrim(strrep(iline,'//inputs',''));
      iline = strtrim(strrep(iline,'//input',''));
      iline = regexprep(iline,'\s+',',');
      oline = strtrim(strrep(oline,'//outputs',''));
      oline = strtrim(strrep(oline,'//output',''));
      oline = regexprep(oline,'\s+',',');
      if (testmatch(iline,'input') || testmatch(oline,'output'))
	printf('MALFORMED Function I/O spec: %s, %s\n',iline,oline);
      end
      catch
	printf('MALFORMED Function Signature: %s\n',fline);
      end
%     printf('function line: %s',fline);
%     printf('input line: %s',iline);
%     printf('output line: %s',oline);
      printf('function code: %s\n',function_type);
   end
  end


