ast = input()

tabs = 0

def myPrint(a):
  print(a, end="") 

def printTabs():
  for i in range(tabs):
    myPrint(" ")

for char in ast:
  if (char == "("):
    myPrint(char)
    myPrint("\n")
    tabs = tabs + 2
    printTabs()
  elif (char == ","):
    myPrint(char)
    myPrint("\n")
    printTabs()
  elif (char == ")"):
    tabs = tabs - 2
    myPrint("\n")
    printTabs()
    myPrint(char)
  else: 
    myPrint(char)
    
myPrint("\n")
