import os
import filecmp

# fileName, should pass
tests = [
  ['bsort', True],
  ['defThenUse', True],
  ['dynamicMem', True],
  ['evenOdd', True],
  ['hanoi', True],
  ['hello', True],
  ['letin', True],
  ['match', False],                   # different return types in clauses
  ['sameNames', False],               # identifier re declaration
  ['variableNotDeclared', False],     # Unknown identifier: b
  ['printInt', True],
  ['ifThenElse', True],
  ['forExpr', True],
  ['mutable', True],
  ['swap', True],
  ['assignment', True],
  ['dim', True],
  ['array', True],
  ['newDelete', True],
  ['2DArray', True],
  ['workingMatch', True],
]

failed = 0

for [test, shouldPass] in tests:
  os.system(f"./do.sh ./example-programs/{test}.llama > .test.temp 2>&1")
  with open(".test.temp") as f:
    lines = f.readlines()
    if (not lines) != (shouldPass):
      failed = failed + 1
      print(f"{test}: failed with output: {lines}")
      continue
    if not shouldPass:
      continue

  os.system("scp a.out ubuntu16:~/.")
  os.system(f"ssh ubuntu16 './a.out' > .test.temp 2>&1")
  comp = filecmp.cmp('.test.temp', f"./example-programs/outputs/{test}")
  if (not comp):
    print(f"{test}: failed output mismatched")
    failed = failed + 1
 

print(f"Passed: {len(tests) - failed}. Failed: {failed}")
