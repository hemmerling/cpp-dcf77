/**
 *  @package   dcf77
 *  @file      dcf77.cpp
 *  @brief     Zeitsignal empfangen, Uhrzeit anzeigen   
 *  @author    Rolf Hemmerling <hemmerling@gmx.net>
 *  @version   1.00, 
 *             Programmiersprache "Borland C/C++" fuer MSDOS,
 *             Entwicklungswerkzeuge - TURBO C/C++ 2.0 / 3.0  
 *                                   - Borland C/C++ 3.1
 *                                   - Turbo C/C++ 4.5
 *    
 *  @date      2015-01-01
 *  @copyright Apache License, Version 2.0
 *
 *  dcf77.cpp - Zeitsignal empfangen, Uhrzeit anzeigen 
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
 *  ( u.a. ist die Pascal-Structure REGS unbekannt ).
 *
 *  Letztes Update: 2000-03-24
 */
 
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Include-Dateien                                                 */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "dcf77.h"

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Modulglobale Variablen                                          */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**
 *  @var      MV_feld1
 *  @brief    Feld1  
 */
unsigned int MV_feld1[59];

/**
 *  @var      MV_mindauer
 *  @brief    Mindestdauer  
 */
unsigned int MV_mindauer;

/**
 *  @var      MV_maxdauer
 *  @brief    Maximale Dauer  
 */
unsigned int MV_maxdauer;

/**
 *  @var      MV_feld2
 *  @brief    Feld2  
 */
unsigned int MV_feld2[59]=
             {0,0,0,0,0,0,0,0,0,0,0,     /* Struktur loeschen */
              0,0,0,0,0,0,0,0,0,0,
              1,0,0,1,1,0,1,0,           /* 59 Min */
              1,1,0,0,1,0,1,             /* 13 Std */
              1,1,0,0,0,1,               /* 23. */
              1,1,1,                     /* Sonntag */
              1,0,0,1,0,                 /* 09. */
              0,0,0,0,0,0,0,0,0};        /* 2000 */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Funktions-Deklarationen                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


void MP_cursor_entfernen(void)
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Der Cursor wird in einen nicht sichtbaren Bereich gesetzt.     */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
   union REGS PV_inregs,PV_outregs;
   PV_inregs.h.ah = 0x02; /* Funktion 2, Cursor setzen */
   PV_inregs.h.bh = 0x00; /* aktuelle Bildschirmseite */
   PV_inregs.h.dh = 0x25; /* y-Position = 25 */
   PV_inregs.h.dl = 0x0;  /* x-Position =  0 */
   /* Bios-Interupt 10h aufrufen */
   int86 (0x10, &PV_inregs, &PV_outregs);
 }


int MP_port()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Port lesen, Signalpegel auslesen                               */
/*  Es wird /ACK-Signal vom Port gelesen und der Wert              */
/*  zurueckgegeben                                                 */
/* Parameter: -                                                    */
/* Rueckgabewert:                                                  */
/*  1 wenn /ACK gesetzt ist                                        */
/*  0 wenn /ACK nicht gesetzt ist                                  */
/*******************************************************************/
 {
   return (!((inportb(MC_portaddr+1)&0x040)>0));
 }


int MP_bezugspunkt()
/********************************************************************/
/* Aufgabe:                                                         */
/*  Bezugspunkt suchen                                              */
/*  Es wird auf den Anfang der Minute gewartet                      */
/* Rueckgabewert:                                                   */
/*  1 wenn Signal nicht gefunden wurde                              */
/*  0 wenn Signal und Minutenanfang gefunden wurden                 */
/*  2 wenn Signal gefunden, aber Minutenanfang NICHT gefunden wurde */
/********************************************************************/
 {
 unsigned int LV_ii;
 unsigned int PV_max;
 int  PV_signal=0;

 LV_ii=0;
 PV_max=0;

 /* Bildschirm loeschen */
 clrscr();

 printf("\n\n\n\nWarten auf Signal ...");

 /* Warten bis Schnittstellenpegel = HIGH oder
    Timeout nach 3000 ms = 3 s */
 do
   {
     /* 10 ms warten */
     delay(10); 
     LV_ii++;
     /* Port lesen */
     PV_signal=!MP_port();
   } while(LV_ii<300 && !PV_signal);


 if (PV_signal)
    {
      /* Signal gefunden */

      clrscr();
      printf("\n\n\n\nWarten auf Bezugspunkt ...\n\n\n");

      /* Balken malen */
      for(LV_ii=1;LV_ii<=60;LV_ii++)
        printf("X");

      LV_ii=0;

      /* Warten bis Timeout oder fertig */
      do
       {
         /* 10 ms warten */
         delay(10);
         if (MP_port()==1)
           { LV_ii=0; };

         ++LV_ii;
         ++PV_max;

         if (PV_max%100==0)
          {
            gotoxy(61-PV_max/100,8);
            printf(" ");
            MP_cursor_entfernen();
          }
       } while(LV_ii<120 && PV_max<6100);

      if(PV_max>=6100)
       { /* Keinen Minutenanfang gefunden */
         return(2); }
      else
       { /* Minutenanfang gefunden */
        return(0); }
    }
 else
    {
      /* Kein Signal gefunden */
      return(1);
    };
 }


void MP_zeit_lesen()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Zeit einlesen                                                  */
/*  Eine Minute lang werden alle 60 Bits als Zeiten eingelesen     */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
   int PV_fehler;
   int LV_ii;
   unsigned int PV_jj;
   MV_mindauer=65000;
   MV_maxdauer=0;
   PV_fehler=MP_bezugspunkt();
   if (PV_fehler!=0)
    {
      gotoxy(1,15);
      if(PV_fehler==2)
        printf("\n\nKeinen Minutenanfang gefunden!");
      if(PV_fehler==1)
        printf("\n\nKein Signal gefunden! Bitte Anschlu· ueberpruefen.");
      printf("\n\nWeiter mit Taste...");
      fflush(stdin);
      getch();
    }
   else
    {
      gotoxy(1,15);
      printf("Lese Zeitsignal ...");
      for (LV_ii=0; LV_ii<=58; LV_ii++)
        {
          PV_jj=0;
          while(MP_port()==0 && PV_jj<1200)
	     {
               PV_jj++;
               delay(1);
             };
         if (PV_jj>=1200)
           { PV_fehler=1; }
         else
           {
              while (MP_port()==1 && MV_feld1[LV_ii]<500)
		 {
                 MV_feld1[LV_ii]++;
		 delay(1);
                 };
             if (MV_feld1[LV_ii]>=500) PV_fehler=1;
             if (MV_feld1[LV_ii]<MV_mindauer) MV_mindauer=MV_feld1[LV_ii];
             if (MV_feld1[LV_ii]>MV_maxdauer) MV_maxdauer=MV_feld1[LV_ii];
           };
         if (PV_fehler)
           {
             clrscr();
             printf ("\n\n\n  Fehler beim Einlesen des Signals!");
             printf ("\n\n  Weiter mit Taste...");
             fflush(stdin);
             getch();
             return;
	   }
	}
    MP_umsetz();
    PV_fehler=MP_pruefen();
    if (PV_fehler)
       {
         clrscr();
         printf ("\n\n\n  ParityPV_fehler in den empfangenen Daten.");
         printf ("\n\n  Weiter mit Taste...");
         fflush(stdin);
         getch();
       }
    else
       {
         printf ("\n\n\n  Zeit erfolgreich gelesen. ");
         delay(1000);
         MP_auswertung();
         MP_zeit_anzeigen();
       }
    }
 }

void MP_umsetz()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Die eingelesenen Zeiten werden in "0" und "1" umgewandelt      */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 int LV_ii;
 unsigned int PV_schwelle;
 PV_schwelle=(MV_mindauer+MV_maxdauer)/2;
 for (LV_ii=0;LV_ii<=58;LV_ii++)
     {
     if (MV_feld1[LV_ii]<PV_schwelle)
       { MV_feld1[LV_ii]=0; }
      else
       { MV_feld1[LV_ii]=1; };
     }
 }


int MP_pruefen()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Auswertung der Paritaetsbits                                   */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 int LV_ii, PV_summe;
 int PV_fehler=0;

 PV_summe=0;

 for (LV_ii=21;LV_ii<=27;LV_ii++)
     PV_summe=PV_summe+MV_feld1[LV_ii];

 if (PV_summe%2!=MV_feld1[28])
   { PV_fehler=1; };

 PV_summe=0;

 for (LV_ii=29;LV_ii<=34;LV_ii++)
     { PV_summe=PV_summe+MV_feld1[LV_ii]; };

 if (PV_summe%2!=MV_feld1[35])
   { PV_fehler=1; };

 PV_summe=0;

 for (LV_ii=36;LV_ii<=57;LV_ii++)
   { PV_summe=PV_summe+MV_feld1[LV_ii]; };

 if (PV_summe%2!=MV_feld1[58])
   { PV_fehler=1; };
 return(PV_fehler);
 }


void MP_auswertung()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Ermittlung der Werte aus den einzelnen Bits und                */
/*  Abspeichern der Werte beim Minutenanfang                       */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 struct dosdate_t PV_datum;
 struct dostime_t PV_zeit;
 PV_zeit.hsecond   =0;
 PV_zeit.second    =0;
 PV_zeit.minute    =MV_feld1[21]*1 +MV_feld1[22]*2+MV_feld1[23]*4+
                    MV_feld1[24]*8+MV_feld1[25]*10+
                    MV_feld1[26]*20+MV_feld1[27]*40;
 PV_zeit.hour      =MV_feld1[29]*1 +MV_feld1[30]*2+MV_feld1[31]*4+
                    MV_feld1[32]*8+MV_feld1[33]*10+
                    MV_feld1[34]*20;
 PV_datum.day      =MV_feld1[36]*1 +MV_feld1[37]*2+MV_feld1[38]*4+
                    MV_feld1[39]*8+MV_feld1[40]*10+
                    MV_feld1[41]*20;
 PV_datum.dayofweek=MV_feld1[42]*1 +MV_feld1[43]*2+MV_feld1[44]*4;

 if (PV_datum.dayofweek==7)
   { PV_datum.dayofweek=0;};

 PV_datum.month = MV_feld1[45]*1 +MV_feld1[46]*2
                  +MV_feld1[47]*4+MV_feld1[48]*8+MV_feld1[49]*10;
 PV_datum.year = MV_feld1[50]*1 +MV_feld1[51]*2
                 +MV_feld1[52]*4+MV_feld1[53]*8+MV_feld1[54]*10+
                 MV_feld1[55]*20+MV_feld1[56]*40+MV_feld1[57]*80;

 /* Dieses Programm funktioniert so von 1999..2098 */
 if (PV_datum.year<99)
    { PV_datum.year=PV_datum.year+2000; }
  else
    { PV_datum.year=PV_datum.year+1900; };

  /* Uhrzeit des PCs setzen */
  _dos_settime(&PV_zeit);
  _dos_setdate(&PV_datum);
 }


void MP_zeit_anzeigen()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Die Systemzeit wird grafisch angezeigt                         */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 int            PV_graphdriver, PV_graphmode;
 float          PV_faktor_datum, PV_faktor_zeit;
 unsigned int   PV_max_x, PV_max_y;
 struct         dostime_t PV_zeit;
 struct         dosdate_t PV_datum;
 char           PV_old_second;

 /* Wochentage mit je 2 Buchstaben */
 int            PV_zeichen[7][2]=           
                 {{17,15},    /* SO */
                  {14,15},    /* MO */
                  {11,13},    /* DI */
                  {14,13},    /* MI */
                  {11,15},    /* DO */
                  {12,16},    /* FR */
                  {17,10}};   /* SA */

 /* Graphikmodus einschalten */
 detectgraph(&PV_graphdriver,&PV_graphmode);
 initgraph (&PV_graphdriver,&PV_graphmode, MC_treiberpfad);
 cleardevice();
 fflush(stdin);
 do
   {
   if (graphresult()==grOk)
     {
       /* Im Graphikmodus */
       PV_max_x = getmaxx();
       PV_max_y = getmaxy();
       /* 640 x 480 Punkte ? */
       if ((PV_max_x>=639) && (PV_max_y>=479))         
         {
         /* VGA = 640x480 Punkte */
         PV_faktor_datum = 1.2;
         PV_faktor_zeit  = 2;
	 }
      else
         {
         /* CGA,EGA,HERCULES.. */
         PV_faktor_datum = 0.5;
         PV_faktor_zeit  = 0.8;
	 }

      _dos_gettime(&PV_zeit);

      if (PV_old_second != PV_zeit.second)
         {
         PV_old_second = PV_zeit.second;
         MP_led_anzeige (PV_zeit.hour/10,    25*PV_faktor_zeit,
                         150*PV_faktor_zeit,PV_faktor_zeit);
         MP_led_anzeige (PV_zeit.hour%10,    57*PV_faktor_zeit,
                         150*PV_faktor_zeit,PV_faktor_zeit);
         MP_led_punkt   (69*PV_faktor_zeit, 122*PV_faktor_zeit,
                         PV_faktor_zeit);
         MP_led_punkt   (69*PV_faktor_zeit, 139*PV_faktor_zeit,
                         PV_faktor_zeit);
         MP_led_anzeige (PV_zeit.minute/10, 123*PV_faktor_zeit,
                         150*PV_faktor_zeit,PV_faktor_zeit);
         MP_led_anzeige (PV_zeit.minute%10, 157*PV_faktor_zeit,
                         150*PV_faktor_zeit,PV_faktor_zeit);
         MP_led_punkt   (169*PV_faktor_zeit,122*PV_faktor_zeit,
                         PV_faktor_zeit);
         MP_led_punkt   (169*PV_faktor_zeit,139*PV_faktor_zeit,
                         PV_faktor_zeit);
         MP_led_anzeige (PV_zeit.second/10, 223*PV_faktor_zeit,
                         150*PV_faktor_zeit,PV_faktor_zeit);
         MP_led_anzeige (PV_zeit.second%10, 255*PV_faktor_zeit,
                         150*PV_faktor_zeit,PV_faktor_zeit);

         _dos_getdate(&PV_datum);
         MP_led_anzeige (PV_zeichen[PV_datum.dayofweek][0],
                         50*PV_faktor_datum,130*PV_faktor_datum,
                         PV_faktor_datum);
         MP_led_anzeige (PV_zeichen[PV_datum.dayofweek][1],
                         85*PV_faktor_datum,130*PV_faktor_datum,
                         PV_faktor_datum);
         MP_led_anzeige (PV_datum.day/10,
                         170*PV_faktor_datum,130*PV_faktor_datum,
                         PV_faktor_datum);
         MP_led_anzeige (PV_datum.day%10,
                         205*PV_faktor_datum,130*PV_faktor_datum,
                         PV_faktor_datum);
         MP_led_punkt   (205*PV_faktor_datum,
                         130*PV_faktor_datum,PV_faktor_datum);
         MP_led_anzeige (PV_datum.month/10,
                         250*PV_faktor_datum,130*PV_faktor_datum,
                         PV_faktor_datum);
         MP_led_anzeige (PV_datum.month%10,
                         285*PV_faktor_datum,130*PV_faktor_datum,
                         PV_faktor_datum);
         MP_led_punkt   (285*PV_faktor_datum,
                         130*PV_faktor_datum,PV_faktor_datum);
         MP_led_anzeige (PV_datum.year/1000,
                         330*PV_faktor_datum,130*PV_faktor_datum,
                         PV_faktor_datum);
         MP_led_anzeige ((PV_datum.year/ 100)%10,
                         365*PV_faktor_datum,130*PV_faktor_datum,
                         PV_faktor_datum);
         MP_led_anzeige ((PV_datum.year/  10)%10,
                         400*PV_faktor_datum,130*PV_faktor_datum,
                         PV_faktor_datum);
         MP_led_anzeige (PV_datum.year%10,
                         435*PV_faktor_datum,130*PV_faktor_datum,
                         PV_faktor_datum);
         setcolor(WHITE);

         if ((PV_max_x>=639) && (PV_max_y>=479))
            {
              outtextxy(  2*PV_faktor_zeit,10*PV_faktor_zeit,
                        "Versuch 1: Normalzeit (DCF 77)");
              outtextxy(195*PV_faktor_zeit,10*PV_faktor_zeit,
                        "Gruppe:");
            }
          else
            { outtextxy(2*PV_faktor_zeit,10*PV_faktor_zeit,"DCF-77 Uhr"); };
         outtextxy(240*PV_faktor_zeit,10*PV_faktor_zeit,"Rolf Hemmerling");

         if (MV_feld1[17])
            { outtextxy(0*PV_faktor_zeit,140*PV_faktor_zeit,
                        "Sommerzeit (MESZ)");}
         if (MV_feld1[18])
            { outtextxy(0*PV_faktor_zeit,140*PV_faktor_zeit,
              "Winterzeit (MEZ)"); };
	 }
     }
    else
     {
       clrscr();
       printf ("\n\n\n  Wechsel in Grafikmodus ging schief !");
       printf ("\n\n  Weiter mit Taste...");
       fflush(stdin);
       getch();
     }
 }
 while(!kbhit());                       /* auf Tastendruck warten */
 PV_old_second=getch();                    /* Tastaturpuffer leeren */
 closegraph();
 }


void MP_led_anzeige(unsigned int VV_zeichen,
                    unsigned int VV_xx,
                    unsigned int VV_yy,
                    float VV_kk)
/*******************************************************************/
/* Aufgabe:                                                        */
/*  LED Anzeige - Definition der 14-Segment-Anzeige                */
/* Parameter:                                                      */
/*  VV_zeichen = Auszugebenes Zeichen (ASCII-Wert)                 */
/*  VV_xx = Skalierte X-Koordinate des Ausgabepunktes              */
/*  VV_yy = Skalierte Y-Koordinate des Ausgabepunktes              */
/*  VV_kk = Skalierungsfaktor                                      */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 int PV_segment;
 int   PV_border;
 int PV_segpunkte[14]=                   
       {5,5,5,5,5,5,6,6,7,5,7,7,5,7};
 /* Anzahl der Punkte im Segment */

 int PV_punktfeld[14][16]=
     {{( 2*VV_kk)+VV_xx,( 1*VV_kk)+VV_yy,(26*VV_kk)+VV_xx,
       ( 1*VV_kk)+VV_yy,(22*VV_kk)+VV_xx,( 5*VV_kk)+VV_yy,
       ( 6*VV_kk)+VV_xx,( 5*VV_kk)+VV_yy,( 2*VV_kk)+VV_xx,
       ( 1*VV_kk)+VV_yy,0,0,0,0,
       (10*VV_kk)+VV_xx,( 3*VV_kk)+VV_yy},
      {(27*VV_kk)+VV_xx,( 2*VV_kk)+VV_yy,(27*VV_kk)+VV_xx,
       (26*VV_kk)+VV_yy,(23*VV_kk)+VV_xx,(24*VV_kk)+VV_yy,
       (23*VV_kk)+VV_xx,( 6*VV_kk)+VV_yy,(27*VV_kk)+VV_xx,
       ( 2*VV_kk)+VV_yy,0,0,0,0,
       (25*VV_kk)+VV_xx,(15*VV_kk)+VV_yy},
      {(23*VV_kk)+VV_xx,(30*VV_kk)+VV_yy,(27*VV_kk)+VV_xx,
       (28*VV_kk)+VV_yy,(27*VV_kk)+VV_xx,(51*VV_kk)+VV_yy,
       (23*VV_kk)+VV_xx,(47*VV_kk)+VV_yy,(23*VV_kk)+VV_xx,
       (30*VV_kk)+VV_yy,0,0,0,0,
       (25*VV_kk)+VV_xx,(38*VV_kk)+VV_yy},
      {( 6*VV_kk)+VV_xx,(48*VV_kk)+VV_yy,(22*VV_kk)+VV_xx,
       (48*VV_kk)+VV_yy,(26*VV_kk)+VV_xx,(52*VV_kk)+VV_yy,
       ( 2*VV_kk)+VV_xx,(52*VV_kk)+VV_yy,( 6*VV_kk)+VV_xx,
       (48*VV_kk)+VV_yy,0,0,0,0,
       (14*VV_kk)+VV_xx,(50*VV_kk)+VV_yy},
      {( 1*VV_kk)+VV_xx,(28*VV_kk)+VV_yy,
       ( 5*VV_kk)+VV_xx,(30*VV_kk)+VV_yy,( 5*VV_kk)+VV_xx,
       (47*VV_kk)+VV_yy,( 1*VV_kk)+VV_xx,(51*VV_kk)+VV_yy,
       ( 1*VV_kk)+VV_xx,(28*VV_kk)+VV_yy,0,0,0,0,
       ( 3*VV_kk)+VV_xx,(38*VV_kk)+VV_yy},
      {( 1*VV_kk)+VV_xx,( 2*VV_kk)+VV_yy,( 5*VV_kk)+VV_xx,
       ( 6*VV_kk)+VV_yy,( 5*VV_kk)+VV_xx,(24*VV_kk)+VV_yy,
       ( 1*VV_kk)+VV_xx,(26*VV_kk)+VV_yy,( 1*VV_kk)+VV_xx,
       ( 2*VV_kk)+VV_yy,0,0,0,0,
       ( 3*VV_kk)+VV_xx,(13*VV_kk)+VV_yy},
      {( 6*VV_kk)+VV_xx,(25*VV_kk)+VV_yy,(13*VV_kk)+VV_xx,
       (25*VV_kk)+VV_yy,(13*VV_kk)+VV_xx,(29*VV_kk)+VV_yy,
       ( 6*VV_kk)+VV_xx,(29*VV_kk)+VV_yy,( 2*VV_kk)+VV_xx,
       (27*VV_kk)+VV_yy,( 6*VV_kk)+VV_xx,(25*VV_kk)+VV_yy,0,0,
       ( 9*VV_kk)+VV_xx,(27*VV_kk)+VV_yy},
      {(15*VV_kk)+VV_xx,(25*VV_kk)+VV_yy,(22*VV_kk)+VV_xx,
       (25*VV_kk)+VV_yy,(26*VV_kk)+VV_xx,(27*VV_kk)+VV_yy,
       (22*VV_kk)+VV_xx,(29*VV_kk)+VV_yy,(15*VV_kk)+VV_xx,
       (29*VV_kk)+VV_yy,(15*VV_kk)+VV_xx,(25*VV_kk)+VV_yy,0,0,
       (20*VV_kk)+VV_xx,(27*VV_kk)+VV_yy},
      {( 7*VV_kk)+VV_xx,( 7*VV_kk)+VV_yy,( 8*VV_kk)+VV_xx,
       ( 7*VV_kk)+VV_yy,(10*VV_kk)+VV_xx,(10*VV_kk)+VV_yy,
       (11*VV_kk)+VV_xx,(23*VV_kk)+VV_yy,( 9*VV_kk)+VV_xx,
       (23*VV_kk)+VV_yy,( 7*VV_kk)+VV_xx,(13*VV_kk)+VV_yy,
       ( 7*VV_kk)+VV_xx,( 7*VV_kk)+VV_yy,( 8*VV_kk)+VV_xx,
       (10*VV_kk)+VV_yy},
      {(12*VV_kk)+VV_xx,( 7*VV_kk)+VV_yy,(16*VV_kk)+VV_xx,
       ( 7*VV_kk)+VV_yy,(15*VV_kk)+VV_xx,(23*VV_kk)+VV_yy,
       (13*VV_kk)+VV_xx,(23*VV_kk)+VV_yy,(12*VV_kk)+VV_xx,
       ( 7*VV_kk)+VV_yy,0,0,0,0,
       (14*VV_kk)+VV_xx,( 9*VV_kk)+VV_yy},
      {(18*VV_kk)+VV_xx,(10*VV_kk)+VV_yy,(20*VV_kk)+VV_xx,
       ( 7*VV_kk)+VV_yy,(21*VV_kk)+VV_xx,( 7*VV_kk)+VV_yy,
       (21*VV_kk)+VV_xx,(13*VV_kk)+VV_yy,(19*VV_kk)+VV_xx,
       (23*VV_kk)+VV_yy,(17*VV_kk)+VV_xx,(23*VV_kk)+VV_yy,
       (18*VV_kk)+VV_xx,(10*VV_kk)+VV_yy,(19*VV_kk)+VV_xx,
       (11*VV_kk)+VV_yy},
      {(17*VV_kk)+VV_xx,(31*VV_kk)+VV_yy,(19*VV_kk)+VV_xx,
       (31*VV_kk)+VV_yy,(21*VV_kk)+VV_xx,(40*VV_kk)+VV_yy,
       (21*VV_kk)+VV_xx,(46*VV_kk)+VV_yy,(20*VV_kk)+VV_xx,
       (46*VV_kk)+VV_yy,(18*VV_kk)+VV_xx,(43*VV_kk)+VV_yy,
       (17*VV_kk)+VV_xx,(31*VV_kk)+VV_yy,(18*VV_kk)+VV_xx,
       (32*VV_kk)+VV_yy},
      {(13*VV_kk)+VV_xx,(31*VV_kk)+VV_yy,(15*VV_kk)+VV_xx,
       (31*VV_kk)+VV_yy,(16*VV_kk)+VV_xx,(46*VV_kk)+VV_yy,
       (12*VV_kk)+VV_xx,(46*VV_kk)+VV_yy,(13*VV_kk)+VV_xx,
       (31*VV_kk)+VV_yy,0,0,0,0,
       (14*VV_kk)+VV_xx,(42*VV_kk)+VV_yy},
      {( 9*VV_kk)+VV_xx,(31*VV_kk)+VV_yy,(11*VV_kk)+VV_xx,
       (31*VV_kk)+VV_yy,(10*VV_kk)+VV_xx,(43*VV_kk)+VV_yy,
       ( 8*VV_kk)+VV_xx,(46*VV_kk)+VV_yy,( 7*VV_kk)+VV_xx,
       (46*VV_kk)+VV_yy,( 7*VV_kk)+VV_xx,(40*VV_kk)+VV_yy,
       ( 9*VV_kk)+VV_xx,(31*VV_kk)+VV_yy,(10*VV_kk)+VV_xx,
       (32*VV_kk)+VV_yy}};
 /* 14 Segmente mit je max. 7 Punkte und Punkt zum AusfÅllen */

 int PV_zeichenfeld[18][14]=  /* Ascii-Zeichen von 0-17 mit je 14 Segmenten */
    {{1,1,1,1,1,1,0,0,0,0,0,0,0,0},    /* 0 */
     {0,1,1,0,0,0,0,0,0,0,0,0,0,0},    /* 1 */
     {1,1,0,1,1,0,1,1,0,0,0,0,0,0},    /* 2 */
     {1,1,1,1,0,0,1,1,0,0,0,0,0,0},    /* 3 */
     {0,1,1,0,0,1,1,1,0,0,0,0,0,0},    /* 4 */
     {1,0,1,1,0,1,1,1,0,0,0,0,0,0},    /* 5 */
     {1,0,1,1,1,1,1,1,0,0,0,0,0,0},    /* 6 */
     {1,1,1,0,0,0,0,0,0,0,0,0,0,0},    /* 7 */
     {1,1,1,1,1,1,1,1,0,0,0,0,0,0},    /* 8 */
     {1,1,1,1,0,1,1,1,0,0,0,0,0,0},    /* 9 */
     {1,1,1,0,1,1,1,1,0,0,0,0,0,0},    /* A */
     {1,1,1,1,0,0,0,0,0,1,0,0,1,0},    /* D */
     {1,0,0,0,1,1,1,1,0,0,0,0,0,0},    /* F */
     {0,0,0,0,0,0,0,0,0,1,0,0,1,0},    /* I */
     {0,1,1,0,1,1,0,0,1,0,1,0,0,0},    /* M */
     {1,1,1,1,1,1,0,0,0,0,0,0,0,0},    /* O */
     {1,1,0,0,1,1,1,1,0,0,0,1,0,0},    /* R */
     {1,0,1,1,0,1,1,1,0,0,0,0,0,0}};   /* S */

  for (PV_segment=0;PV_segment<=13;PV_segment++) /* alle 14 Segmente */
     {
       /* Segment aktiv? */
       if (PV_zeichenfeld[VV_zeichen][PV_segment]==1)
         {
          setcolor(MC_on_colour);
          setfillstyle(SOLID_FILL,MC_on_colour);
          PV_border=MC_on_colour;
         }
       else
         {
          setcolor(MC_off_colour);
          setfillstyle(SOLID_FILL,MC_off_colour);
          PV_border=MC_off_colour;
         };
       /* Segment zeichnen */
       drawpoly(PV_segpunkte[PV_segment],PV_punktfeld[PV_segment]);
       /* Segment ausfuellen */
       floodfill(PV_punktfeld[PV_segment][14],
                 PV_punktfeld[PV_segment][15],PV_border);
     }
 }


void MP_led_punkt(unsigned int VV_xx, unsigned int VV_yy,
                  float VV_kk)
/*******************************************************************/
/* Aufgabe:                                                        */
/*  LED Punkt erzeugen.                                            */
/*  Zeichnung eines Kreises mit den uebergebenen Parametern        */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
   setcolor(MC_on_colour);
   setfillstyle(SOLID_FILL,MC_on_colour);
   /* Punkt zeichnen */
   fillellipse((36*VV_kk)+VV_xx,(48*VV_kk)+VV_yy,4*VV_kk,4*VV_kk);
 }

void main()
/*******************************************************************/
/* Hauptprogramm                                                   */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 char PV_key;
 int PV_status;
 int LV_ii;
 do
   {
   clrscr();
   printf("\n\n\n\n      Hemmerling (R) Normalzeit (DCF-77)");
   printf("\n      ==================================");
   printf("\n      Copyright 2000-2015 Rolf Hemmerling");
   printf("\n      Licensed under the Apache License, Version 2.0");
   printf("\n\n\n   1) Zeitsignal empfangen");
   printf("\n   2) Uhrzeit anzeigen");
   printf("\n   t) Test/Debug-Funktion: Loeschen des Datenblocks");
   printf("\n ESC) Programm beenden");
   printf("\n\n Bitte waehlen Sie: ");
   do
     {
     /* Standardeingabe-Puffer loeschen */
     fflush(stdin);
     /* auf Tastendruck warten */
     do{}                       
      while(!kbhit());
     /* Zeichen von Tastatur holen */
     PV_key=getch();
     }
   while(PV_key!=MC_kbesc && PV_key!='1' && PV_key!='2' && PV_key!='t');

   /* t loeschen */

   /* Block loeschen */
   /* Sommerzeit aktiviert */
   /* MV_feld1[17]=1; */            

   if(PV_key=='t')
     {
     do
       {
       gotoxy(1,1);
       if (MP_port()) printf("1 ");
       else PV_key='a';
       }
     while(PV_key=='t');

     for (LV_ii=0;LV_ii<=58;LV_ii++)
         MV_feld1[LV_ii]=MV_feld2[LV_ii];

     PV_status=MP_pruefen();

     MP_auswertung();

     if(PV_status)
       {
       printf("\n\nFehler!!!!!!");
       fflush(stdin);
       getch();
       }
     }
   /* loeschen Ende */

   if(PV_key=='1') MP_zeit_lesen();
   if(PV_key=='2') MP_zeit_anzeigen();
   }
 while(PV_key!=MC_kbesc);             /* mit ESC Programm beenden */
 }

/* EOF */

