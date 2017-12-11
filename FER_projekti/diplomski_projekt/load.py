filein = open("fhrwac.1.0.tok.seg.txt", "r", encoding="utf-8")
fileout = open("input.txt", "w", encoding="utf-8")

for i in range(25000000): #55412961
  fileout.write(filein.readline())