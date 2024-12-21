package demo.

def `func` name(`main`) code{
	def `var` name(`a`) value(0).
	print (`a`), for(5 `times`).
	// (`times`) is a variable, ("times") and ('times') are strings.
	// (5 `times`) is the same as (5*`times`).
	print (`i`), for(`i` from 0 to 4 step 0.05). // Special optimization for numbers. 
	print (`i`), for(`i` in [1,2,3,4,5] ).
	`a`: set to(5).
	`a`: delete.
}.

/*
def main():
	a = 0
	for i in range(5): print(a)
	for i in range(0, 20): print(i / 20)
	for i in [1,2,3,4,5]: print(i)
	a = 5
	del a
main()
*/