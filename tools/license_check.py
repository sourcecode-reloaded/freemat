import os



def visit( arg,  dirname,  names):
   out = open('miss', 'a')

   for name in names:
       (root, ext) = os.path.splitext(name)

       if dirname.find('llvm') != -1:
           continue
       if dirname.find('thirdparty') != -1:
           continue
       if name.find('moc') != -1:
           continue
       if ext=='.cpp' or ext=='.hpp':
           path = os.path.join(dirname, name)
           licensed = False
           f = open(path, 'r')
           for line in f:
               if line.find('Licensed under the GPL') !=-1 or line.find('GNU General Public License')!=-1:
                   licensed=True
           f.close()
           if licensed==False:
               print path
               out.write(path)
               out.write('\n')


   out.close()

os.path.walk('..',  visit,  '')
