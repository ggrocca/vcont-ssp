import os
import shutil
import sys
from subprocess import call

def main():

	if(len(sys.argv)==2):
		bauerMaxIndex=120
		pngCommonName=sys.argv[1]
		bauerOutFile="../bauerData.ssp"
		gaussOutFile="../gaussData.ssp"
		pngRange="../pngRanges.txt"

		filesList=[]
		bauerIndexes=[]
		bauerIndexes.append(0)
		
		for i in range(1,20):
			if(2**(i/2.0) <= bauerMaxIndex):
				bauerIndexes.append(int(round(2**(i/2.0))))

		# Elimina il file che contiene i range di colore delle png
		if(os.path.isfile(pngRange)):
			os.remove(pngRange)

		print bauerIndexes
		for i in bauerIndexes:
			# Converte le PNG in MESH
			call(["../pngToMesh/pngToMesh", pngCommonName + str(i) + ".png", "mesh_" + str(i) + ".tri", pngRange])

			# Filtra le MESH per eliminare le aree piatte
			call(["../magillo/risolvi_ext", "mesh_" + str(i) + ".tri", "mesh_out_" + str(i) + ".tri"])
			filesList.append("mesh_out_" + str(i) + ".tri")

		# Stampa l'elenco di MESH prodotte
		print ""
		print "Total mesh created: " + str(len(filesList))
		for f in filesList:
			print "Created " + f
		
		# Converte la sequenza di MESH in due file SSP
		call(["../meshToSSp/meshToSSp", str(bauerMaxIndex), "mesh_out_", bauerOutFile, gaussOutFile, pngRange])

		print ""	
		print "Init Completed"
		
		print "Run scale-tracking..."
		print ""	
		
		# Esecuzione delle chiamate allo scaletracker
		scaletrackerPath = "../scalespace/scaletracker/scaletracker"

		# Chiama lo scaletracker su bauer
		# -a -r /home/claudio/tesi/bauerData.ssp -o outBauer -t imgBauer -m 255
		call([scaletrackerPath, "-a", "-r", "/home/claudio/tesi/bauerData.ssp", "-o", "outBauer", "-t", "imgBauer", "-m", "255"])

		# Chiama lo scaletracker su gauss
		# -a -d /home/claudio/tesi/gaussData.ssp -n 12 -o outGauss -t imgGauss -m 255
		call([scaletrackerPath, "-a", "-d", "/home/claudio/tesi/gaussData.ssp", "-n", "12", "-o", "outGauss", "-t", "imgGauss", "-m", "255"])

		print "Process Completed"

if __name__ == '__main__':
	main()

