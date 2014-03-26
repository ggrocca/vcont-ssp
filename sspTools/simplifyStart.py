import os
import shutil
import sys
from subprocess import call

def main():

	if(len(sys.argv)==5):
		fromValue=(int)(sys.argv[1])
		toValue=(int)(sys.argv[2])
		baseImageName=sys.argv[3]
		clonedImageName=sys.argv[4]

		for i in range(fromValue,toValue+1):
			shutil.copy2(baseImageName, clonedImageName)
			call(["../simplify", "--threshold=" + str(i), clonedImageName])

if __name__ == '__main__':
	main()


