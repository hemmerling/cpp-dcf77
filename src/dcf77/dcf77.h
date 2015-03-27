#ifndef DCF77_H
#define DCF77_H

/**
 *  @package   dcf77
 *  @file      dcf77.h
 *  @brief     Include-Datei fuer das Programm zum DCF77 Zeitsignal empfangen   
 *  @author    Rolf Hemmerling <hemmerling@gmx.net>
 *  @version   1.00, 
 *             Programmiersprache "Borland C/C++" / "Turbo-C/C++" fuer MSDOS,
 *             Entwicklungswerkzeug "Borland C/C++ 3.1"
 *  @date      2015-01-01
 *  @copyright Apache License, Version 2.0
 *
 *  dcf77.c h- Include-Datei fuer das Programm zum DCF77 Zeitsignal empfangen
 *             Labor fuer Datenverarbeitung, FH Hannover,
 *             Versuch 1 "Normalzeit (DCF-77)"   
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
 *  ( u.a. ist die structure REGS unbekannt ).
 *
 *  Letztes Update: 2000-03-24
 */
 
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Debug-Schalter                                                  */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/**
 *  @def      DEBUG
 *  @brief    DEBUG
 */
#define DEBUG 1

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Include-Dateien                                                 */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include <dos.h>
#include <graphics.h>
#include <stdio.h>
#include <conio.h>

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Konstanten-Definitionen                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**
 *  @def      MC_portaddr
 *  @brief    Portadresse der Parallelen Schnittstelle im IBM-AT
 */
#define MC_portaddr 0x0278

/**
 *  @def      MC_treiberpfad
 *  @brief    Treiberpfad fuer Graphik
 *            *define MC_treiberpfad "c:\\borlandc\\bgi"
 */
#define MC_treiberpfad "c:\\borlandc\\bgi"

/**
 *  @def      MC_kbesc
 *  @brief    ESC-Code bei Tastatureingabe
 */
#define MC_kbesc    0x001B

/**
 *  @def      MC_on_colour
 *  @brief    MC_on_colour
 */
#define MC_on_colour    GREEN
/**
 *  @def      MC_off_colour
 *  @brief    MC_off_colour
 */
#define MC_off_colour   BLACK

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Funktions-Definitionen ( Prototypen )                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**
 *  @fn       MP_cursor_entfernen
 *  @brief    Der Cursor wird in einen nicht sichtbaren Bereich gesetzt.  
 */
void MP_cursor_entfernen(void);

/**
 *  @fn       MP_port
 *  @return   int 
 *            1 wenn /ACK gesetzt ist
 *            0 wenn /ACK nicht gesetzt ist
 *  @brief    Port lesen, Signalpegel auslesen
 *            Es wird /ACK-Signal vom Port gelesen 
 *            und der Wert zurueckgegeben    
 */
int MP_port(void);

/**
 *  @fn       MP_bezugspunkt
 *  @return   int 
 *            1 wenn Signal nicht gefunden wurde
 *            0 wenn Signal und Minutenanfang gefunden wurden
 *            2 wenn Signal gefunden, aber Minutenanfang 
 *              NICHT gefunden wurde
 *  @brief    Bezugspunkt suchen
 *            Es wird auf den Anfang der Minute gewartet
 */
int MP_bezugspunkt(void);

/**
 *  @fn       MP_zeit_lesen
 *  @brief    Zeit einlesen
 *            Eine Minute lang werden alle 60 Bits als Zeiten eingelesen
 */
void MP_zeit_lesen(void);

/**
 *  @fn       MP_zeit_anzeigen
 *  @brief    Die Systemzeit wird grafisch angezeigt
 */
void MP_zeit_anzeigen(void);

/**
 *  @fn       MP_umsetz
 *  @brief    Die eingelesenen Zeiten werden in "0" und "1" umgewandelt
 */
void MP_umsetz(void);

/**
 *  @fn       MP_pruefen
 *  @brief    Auswertung der Paritaetsbits
 */
int MP_pruefen(void);

/**
 *  @fn       MP_auswertung
 *  @brief    Ermittlung der Werte aus den einzelnen Bits und
 *            Abspeichern der Werte beim Minutenanfang
 */
void MP_auswertung(void);

/**
 *  @fn       MP_led_anzeige(unsigned int VV_zeichen, unsigned int VV_xx,
 *                           unsigned int VV_yy, float VV_kk)
 *  @param    VV_zeichen Auszugebenes Zeichen (ASCII-Wert)
 *  @param    VV_xx Skalierte X-Koordinate des Ausgabepunktes
 *  @param    VV_yy Skalierte Y-Koordinate des Ausgabepunktes
 *  @param    VV_kk Skalierungsfaktor
 *  @brief    LED Anzeige - Definition der 14-Segment-Anzeige
 */
void MP_led_anzeige(unsigned int VV_zeichen, unsigned int VV_xx,
                    unsigned int VV_yy, float VV_kk);

/**
 *  @fn       MP_led_punkt(unsigned int VV_xx, unsigned int VV_yy,
 *                         float VV_kk)
 *  @param    VV_xx Skalierte X-Koordinate des Ausgabepunktes
 *  @param    VV_yy Skalierte Y-Koordinate des Ausgabepunktes
 *  @param    VV_kk Skalierungsfaktor
 *  @brief    LED Punkt erzeugen.
 *            Zeichnung eines Kreises mit den uebergebenen Parametern
 */
void MP_led_punkt(unsigned int VV_xx, unsigned int VV_yy,
                  float VV_kk);
/**
 *  @fn       main
 *  @brief    Hauptprogramm    
 */
void main(void);

#endif


