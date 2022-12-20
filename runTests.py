import os

# fileName, should pass
tests = [
  ['bsort.llama', True],
  ['defThenUse.llama', True],
  ['dynamicMem.llama', True],
  ['evenOdd.llama', True],
  ['hanoi.llama', True],
  ['hello.llama', True],
  ['letin.llama', True],
  ['match.llama', False],       # different return types in clauses
  ['sameNames.llama', False],   # identifier re declaration
]

for [test, shouldPass] in tests:
  os.system(f"./Llama < ./example-programs/{test} > .test.temp 2>&1")
  with open(".test.temp") as f:
    lines = f.readlines()
    if (not lines) != (shouldPass):
      print(f"{test}: failed with output: {lines}")
