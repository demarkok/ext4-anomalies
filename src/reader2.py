def is_consistent(s):
	return not (len(s) == 10 and 'x' in s)

results = set() 

while True:
	f = open("test.txt", "r")
	s = f.read()
	if not s in results: 
		print(s)
		results.add(s)
	f.close()
	
