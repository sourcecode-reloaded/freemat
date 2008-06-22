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
  printf('Processing file %s...\n',filename);
  try 
    fp = fopen(filename,'r');
  catch
    return;
  end
  while (1)
    line = getline(fp);
    if (feof(fp))
      fclose(fp);
      return;
    end
    if (testmatch(line,
