per compilar:
	g++ -o practica practica.cpp
per executar:
	amb valors predeterminats executa:
	./practica	
 		// número de Docuemntos: 20
 		- número de funciones de Hash: 999
	 	- valor de K (palabras de cada Shingle): 7
 		- número de bandas: 333
 		- número de filas hasheadas para cada banda: 3
 		- valor de threshold: 0.1
	amb valors definits per usuario executa:
	./practica h k b r t
	on cada lletra es un nombre enter (excepte t que es float)
	./practica 20 999 7 333 3 0.1
Si falta algun parametre donarà segmentation fault.

El programa llegeix el archius ubicats a data/docs_requisits
	Es pot cambiar els archius canviant la línea de codi 209
	o afegint archius a aquesta carpeta (format docX.txt on X es un nombre igual o inferior als totals + 1 d'aquesta carpeta)



