import glob
import popen2
import threading

def coder(lst):
	for i in lst:
		j = "mgl/" + i + ".png"
		print "processing %s -> %s" % (i, j)
		stdout, stdin, stderr = popen2.popen3("mgl2png '" + i + "' '" + j + "'")
		for i in stdout:
			pass

lst = []
for i in glob.glob("*.mgl"):
	lst.append(i)

m = len(lst) / 2
lst1 = lst[0:m]
lst2 = lst[m:]

p1 = threading.Thread(target=coder, name="t1", args = [lst1])
p2 = threading.Thread(target=coder, name="t1", args = [lst2])

p1.start()
p2.start()

p1.join()
p2.join()

