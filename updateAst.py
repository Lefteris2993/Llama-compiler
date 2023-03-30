import os

files = [
 'bsort',
 'defThenUse',
 'dynamicMem',
 'evenOdd',
 'hanoi',
 'hello',
 'letin',
 'match',
 'sameNames',
 'variableNotDeclared', 
 'printInt',
 'ifThenElse',
 'forExpr',
 'mutable',
 'swap',
 'assignment',
 'dim',
 'array',
 'test',
]

for f in files:
    os.system(f"./Llama < ./example-programs/{f}.llama > .ast.temp 2>&1")
    os.system(f"python3 prettyPrint.py < .ast.temp > ./example-programs/.{f}.ast")
