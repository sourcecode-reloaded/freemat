import os
import re

p='C:\\freemat_src\\Freemat4\\src\\toolbox\\octave\\nnet'

keyword_dict = { 'endif':'end',  'endfor':'end',  'endfunction':'end' }
syntax_dict = { '#':'%', '!':'~', '"':"'" }

def process_comments( script ):
    return script
    
def process_globals( script ):
    glob_re = re.compile("(?<=global).+=.*(?=\s|\Z|;])") #look for global keyword
    matches = glob_re.search(script) 
    if matches != None:
        print matches.start(0), matches.end(0)
        # parse 'global a, b=2;'
        clauses = re.split('[,;\r\n]*', script[ matches.start(0) : matches.end(0) ]) 
        for expr in clauses:
            if expr.find('='):
                
                
    
    return script
    
def process_syntax( script ):
    for kwd,  subst in syntax_dict.items():
        script, nsub=re.subn('%s'%kwd,  subst,  script)
    return script

def process_keywords( script ):
    for kwd,  subst in keyword_dict.items():
        script, nsub=re.subn('(?<=\s)%s(?=\s)'%kwd,  subst,  script)
    return script
    
def process_script( script ):
    script = process_keywords( script )
    script = process_syntax( script )
    script = process_globals( script )
    script = process_comments( script )
    return script 
    
def process_files( fpaths ):

    for fname,  fpath in fpaths.items():
        f = open(fpath, 'r')
        script = f.read()
        f.close()
        script = process_script( script )
        f = open(fpath+'.pp', 'w')
        f.write(script)
        f.close()
    

exclude_pth = re.compile(r"""\.svn""")
mat_files = dict()

for root, dirs, files in os.walk(p):
    if exclude_pth.search(root)==None:
        for file in files:
            if file[-2:]=='.m':
                print file
                mat_files[file] = os.path.join(root, file)

process_files(mat_files)
