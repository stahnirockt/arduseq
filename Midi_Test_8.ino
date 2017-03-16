//Biblitheken einbinden
#include <OneButton.h>      //Biblithok für die Buttons
#include <TimerOne.h>       //Bibliothek für den Hardwaretimer
#include <SoftwareSerial.h> //Bibliothek für die serielle Verbindung

SoftwareSerial midiSerial(14, 2); // RX, TX

//Button-Objekte initialisieren (pinNummer, false)
OneButton buttonInstrument(13, false);
OneButton buttonBPM(3, false);
OneButton buttonStep1(12, false);
OneButton buttonStep2(11, false);
OneButton buttonStep3(10, false);
OneButton buttonStep4( 9, false);
OneButton buttonStep5( 8, false);
OneButton buttonStep6( 7, false);
OneButton buttonStep7( 6, false);
OneButton buttonStep8( 5, false);

/*
 * Festlegung wichtiger Variablen für das Sequencermuster
*/
//Array mit dem abgespielten Pattern - am Anfang Null, erster Wert ist die Midinote
int patternArray[13][17] =  {{60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {61,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {62,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {65,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {66,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {67,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {68,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {70,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {71,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                           {72,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

int instrumentNumber = 0; //Wert für die aktuelle "Zeile" des Patternsp
int seqStep = 1; //Varialble für den aktuellen Sequencer Step (1-16)

/*
 * Festlegung wichtiger Variablen für die Syncronisation
 * und das Timing
*/

int bpm; // Beats per Minutes
float bpm_pot_faktor = 4.2625; //Faktor um BPM über Potentiometerwert 
                               //des analogen Inputs (0-1023) auszulesen


int pot1_value;

int ppqnpm = bpm*24; //soviele MidiClock-Signale werden pro Minute als Midimaster versendet
uint16_t timeForPulse = 60000000/ppqnpm; //Zeit für das versenden eines MidiClocksignals 
                                         //-> entsprechender Wert für den Timeroverflow

int ppqnCounter = 0;   //Zähler für gesendete MidiClocksignale



void setBPM(){
  int bpmOld = bpm;
  pot1_value = analogRead(5);
  bpm = (int) pot1_value/bpm_pot_faktor;

  if(bpm != bpmOld){
  Serial.print("BPM: ");
  Serial.print(bpm);
  Serial.print("\n \n");
  
  ppqnpm = bpm*24;
  timeForPulse = 60000000/ppqnpm;
  Timer1.initialize(timeForPulse); // Timeroverflow nach entsprechender Zeit 
                                   // -> die AttachInterrupt-Fn wird ausgeführt
    }
  }
//Funktion um Midinoten zu senden
void noteOn(byte cmd, byte data1, byte data2) {
    midiSerial.write(cmd);
    midiSerial.write(data1);
    midiSerial.write(data2);
}

void syncMidi(){
  midiSerial.write(0xf8);
  playMidi();//es wird ein MidiClocksignal gesendet
  
  if(ppqnCounter<24){     // Hochzählen des ppqnCounters
    ppqnCounter++;
    }
  else{
    ppqnCounter = 0;
    }
  }
  
void playMidi(){
  if(ppqnCounter == 0 || ppqnCounter == 11){
        for (int k = 0; k<13; k++) {
          if (patternArray[k][seqStep] != 0){
            noteOn(0x90, patternArray[k][0], 0x45);
            }
        }
        if (seqStep < 16){
         seqStep++;
         }
        else{
          seqStep = 1;
        }}
/*
        if(seqStep%16 == 0){
          digitalWrite(4,HIGH);
          }
        else if(seqStep%4 == 0){
          digitalWrite(4,LOW);
          digitalWrite(3,HIGH);
          }
        else{digitalWrite(3,LOW);digitalWrite(4,LOW);}

        } */
   else if(ppqnCounter == 6 || ppqnCounter == 18){
        for (int k = 0; k<13; k++) {
            noteOn(0x80, patternArray[k][0], 0x45);
            }
        }
          
}


//Funktion um die Buttonsauszulesen
void buttonRead(){
  buttonStep1.tick();
  buttonStep2.tick();
  buttonStep3.tick();
  buttonStep4.tick();
  buttonStep5.tick();
  buttonStep6.tick();
  buttonStep7.tick();
  buttonStep8.tick();
  buttonInstrument.tick();
  buttonBPM.tick();
  }

//Funktion beim einfachen Klick des InstrumenteButtons
//erhöht das ausgewählte Instrument um 1
void changeInstrument(){
  if(instrumentNumber < 12){instrumentNumber++;}
  else{instrumentNumber = 0;}
  Serial.print("InstrumentNumber = ");
  Serial.print(instrumentNumber);
  Serial.print(" ||  ");
  printArray();
}

//Funktion beim Doppelklick des InstrumenteButtons
//verringert das ausgewählte Instrument um 1
void changeInstrument_dc(){
  if(instrumentNumber > 0){instrumentNumber--;}
  else{instrumentNumber = 12;}
  Serial.print("InstrumentNumber = ");
  Serial.print(instrumentNumber);
  Serial.print(" ||  ");
  printArray();
}


/* Verändert das Pattern beim jeweils ausgewählten Element, 
 * an der jeweiligen Stelle (entsprechend der Buttonnummer)
 */
void fn_button(int bnNmbr){
      if(patternArray[instrumentNumber][bnNmbr]==0){patternArray[instrumentNumber][bnNmbr]=1;}
      else{patternArray[instrumentNumber][bnNmbr] = 0;}
      printArray();
      }


//Funktion um alle Werte, außer den ersten Wert des PatternArrays auf 0 zu setzen
//Der erste Wert ist die Midinote
void resetPattern(){
  for(int i = 0; i<13; i++){
    for(int k = 1; k<17; k++){
      patternArray[i][k] = 0;
      }
  }
  Serial.print("Reset done!");
 }


//Serielle Ausgabe, wird später vom LCD übernommen//
void printArray(){
      Serial.print("aktuelles Array: ");
      Serial.print(patternArray[instrumentNumber][0]);
      Serial.print(" | ");
      Serial.print(patternArray[instrumentNumber][1]);
      Serial.print(patternArray[instrumentNumber][2]);
      Serial.print(patternArray[instrumentNumber][3]);
      Serial.print(patternArray[instrumentNumber][4]);
      Serial.print("|");
      Serial.print(patternArray[instrumentNumber][5]);
      Serial.print(patternArray[instrumentNumber][6]);
      Serial.print(patternArray[instrumentNumber][7]);
      Serial.print(patternArray[instrumentNumber][8]);
      Serial.print("|");
      Serial.print(patternArray[instrumentNumber][9]);
      Serial.print(patternArray[instrumentNumber][10]);
      Serial.print(patternArray[instrumentNumber][11]);
      Serial.print(patternArray[instrumentNumber][12]);
      Serial.print("|");
      Serial.print(patternArray[instrumentNumber][13]);
      Serial.print(patternArray[instrumentNumber][14]);
      Serial.print(patternArray[instrumentNumber][15]);
      Serial.print(patternArray[instrumentNumber][16]);
      Serial.print("|");
      
      Serial.print("\n \n");
  }


void setup() {
  midiSerial.begin(31250);
  Serial.begin(9600);
  setBPM();
  Timer1.attachInterrupt(syncMidi);

  buttonInstrument.attachClick(changeInstrument);
  buttonInstrument.attachDoubleClick(changeInstrument_dc);
  buttonInstrument.attachLongPressStop(resetPattern);
  buttonBPM.attachDuringLongPress(setBPM);
  buttonStep1.attachMidiClick(fn_button, 1);
  buttonStep1.attachDoubleMidiClick(fn_button, 9);
  buttonStep2.attachMidiClick(fn_button, 2);
  buttonStep2.attachDoubleMidiClick(fn_button, 10);
  buttonStep3.attachMidiClick(fn_button, 3);
  buttonStep3.attachDoubleMidiClick(fn_button, 11);
  buttonStep4.attachMidiClick(fn_button, 4);
  buttonStep4.attachDoubleMidiClick(fn_button, 12);
  buttonStep5.attachMidiClick(fn_button, 5);
  buttonStep5.attachDoubleMidiClick(fn_button, 13);
  buttonStep6.attachMidiClick(fn_button, 6);
  buttonStep6.attachDoubleMidiClick(fn_button, 14);
  buttonStep7.attachMidiClick(fn_button, 7);
  buttonStep7.attachDoubleMidiClick(fn_button, 15);
  buttonStep8.attachMidiClick(fn_button, 8);
  buttonStep8.attachDoubleMidiClick(fn_button, 16);

  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
}


void loop() {
  buttonRead();
}

