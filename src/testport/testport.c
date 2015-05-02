/**
 *  @package   dcf77
 *  @file      testport.c
 *  @brief     Wert von PC 80x86 IO-Port einlesen   
 *  @author    Rolf Hemmerling <hemmerling@gmx.net>
 *  @version   1.00, 
 *             Programmiersprache "Borland C/C++" fuer MSDOS,
 *             Entwicklungswerkzeuge 
 *                                   - TURBO C/C++ 2.0 / 3.0 
 *                                   - Borland C/C++ 3.1
 *                                   - Turbo C/C++ 4.5
 *  @date      2015-01-01
 *  @copyright Apache License, Version 2.0
 *
 *  testport.c - Wert von PC 80x86 IO-Port einlesen 
 *              Labor fuer Datenverarbeitung, FH Hannover,
 *              Versuch 1 "Normalzeit (DCF-77)"   
 *  
 *  Copyright 2000-2015 Rolf Hemmerling
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *  either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *  Diese Software laesst sich nicht mit dem freien 
 *  Entwicklungswerkzeug "Borland C/C++ 5.5" uebersetzen, 
 *  da dieses nur Windows- und keine DOS-Programme erzeugt 
 *  ( u.a. ist die C-Structure REGS unbekannt ).
 *
 *  Haupt-Entwicklungszeit: 2000-03-01 - 2000-03-24
 */
 
#include <stdio.h>
#include <dos.h>
#include <string.h>

main(int argc, char **argv)
{
	int port;
	unsigned char result;

	port = atoi(argv[1]);

	do{
	result = inportb(port);

	printf("%d\n", result);

	}while( !kbhit() );
	printf ("Fertig. Port war %d (dezimal), bzw %x (hexadez.).", port, port);

}
