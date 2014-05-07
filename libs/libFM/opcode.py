#!/usr/bin/python
import csv


# Pass 1 - define the opcodes
f = open("OpCodes.h","w")
with open('OpCodes.csv', 'rb') as csvfile:
  datareader = csv.reader(csvfile, delimiter=',', quotechar='"')
  for row in datareader:
    f.write('const op_t ' + row[0] + ' = ' + row[1] + ';\n')
f.close()

# Pass 2 - write the two utility functions
f = open("OpCodes.cpp","w")

f.write('opcodemode getOpCodeMode(op_t opcode)\n')
f.write('{\n')
f.write('  switch (opcode)\n')
f.write('  {\n');
with open('OpCodes.csv', 'rb') as csvfile:
  datareader = csv.reader(csvfile, delimiter=',', quotechar='"')
  for row in datareader:
    f.write('    case ' + row[0] + ': return ' + row[2] + ';\n')
f.write('    default:\n')
f.write('      return three_registers;\n')
f.write('  }\n')
f.write('}\n')
f.write('\n\n')
f.write('std::string getOpCodeName(op_t opcode)\n')
f.write('{\n')
f.write('  switch(opcode)\n')
f.write('  {\n')
with open('OpCodes.csv', 'rb') as csvfile:
  datareader = csv.reader(csvfile, delimiter=',', quotechar='"')
  for row in datareader:
    f.write('    case ' + row[0] + ': return "' + row[3] + '";\n')
f.write('  }\n')
f.write('}\n')

f.close()
