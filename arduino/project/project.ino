#include "DFRobotDFPlayerMini.h" // Library om de dfplayer mini aan te sturen

HardwareSerial mySerial(2); // Gebruik UART2 van de ESP32 voor communicatie, ook hoofdreden waarom ik esp heb gekocht
DFRobotDFPlayerMini player;  // Maak een dfplayer object aan

// Dfplayer pins (verbinding tussen ESP32 en dfplayer)
static const int PIN_MP3_TX = 17; // ESP32 transmitter naar dfplayer receiver
static const int PIN_MP3_RX = 16; // ESP32 receiver → dfplayer transmitter (volgens forums zou uint8_t eigenlijk optimaler zijn omdat dat maar 8 bits geheugen nodig heeft ipv 32, maar anders weet ik nie wat dat doet.)

// Knoppen (pins op ESP32)
const int BTN_PAUSE = 14; // knop voor pauze / hervatten
const int BTN_LOOP  = 27; // knop voor loop aan/uit
const int BTN_NEXT  = 26; // knop om naar andere track te gaan
const int BTN_RESET = 25; // knop om track opnieuw te starten

// LED 
const int LED_PIN = 2; // toont loop status

// States (houden bij wat de huidige status is)
bool isPaused = false;   // of de muziek gepauzeerd is
bool isLooping = false;  // of looping aan staat
int currentTrack = 1;    // welke track er aan het spele is

// Voor knopdetectie (om te zien of knop net is ingedrukt)
bool lastPauseState = HIGH;
bool lastLoopState  = HIGH;
bool lastNextState  = HIGH;
bool lastResetState = HIGH;


// ================= FUNCTIES =================

// speel huidige track
void playCurrentTrack() {
  player.play(currentTrack); // speel track 1 of 2
  isPaused = false;          // reset pause status
}

// restart huidige track
void restartTrack() {
  playCurrentTrack(); // gewoon opnieuw starten
}

// pauze toggle functie
void togglePause() {
  if (isPaused) {
    player.start(); // hervatten
    isPaused = false;
  } else {
    player.pause(); // pauzeren
    isPaused = true;
  }
}

// loop toggle functie
void toggleLoop() {
  isLooping = !isLooping; // switch loop aan/uit

  // LED aan/uit afhankelijk van loop status
  digitalWrite(LED_PIN, isLooping ? HIGH : LOW); // als loop aan staat staat led aan en omgekeerd
}

// switch track functie (1 naar 2 en dan 2 naar 1)
void switchTrack() {
  if (currentTrack == 1) {
    currentTrack = 2; // ga naar track 2
  } else {
    currentTrack = 1; // ga terug naar track 1
  }

  playCurrentTrack(); // speel nieuwe track
}


// ================= SETUP =================
void setup() {
  mySerial.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX); // start dfplayer communicatie (serial_8N1 is standaard volgens forums voor UART toepassingen.)

  // knoppen als input met pull-up
  pinMode(BTN_PAUSE, INPUT_PULLUP);
  pinMode(BTN_LOOP, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  // LED output
  pinMode(LED_PIN, OUTPUT);

  // start dfplayer
  if (player.begin(mySerial)) {
    player.volume(10);   // volume instellen
    playCurrentTrack();  // start eerste track
  }
}


// ================= LOOP =================
void loop() {

  // ===== PAUSE KNOP =====
  bool pauseState = digitalRead(BTN_PAUSE); // lees de status van de knop

  if (pauseState == LOW && lastPauseState == HIGH) {
    togglePause(); // pauzeren of hervatten (functie)
    delay(200);    // debounce
  }
  lastPauseState = pauseState; // De laatste status is de nieuwe status

  // ===== LOOP KNOP =====
  bool loopState = digitalRead(BTN_LOOP); // lees status van de knop

  if (loopState == LOW && lastLoopState == HIGH) {
    toggleLoop(); // loop aan/uit + LED (funtie)
    delay(200);
  }
  lastLoopState = loopState; // De laatste status is de nieuwe status

  // ===== NEXT TRACK (TOGGLE 1 ↔ 2) =====
  bool nextState = digitalRead(BTN_NEXT); // lees de status van deknop

  if (nextState == LOW && lastNextState == HIGH) {
    switchTrack(); // wissel tussen track 1 en 2 (functie)
    delay(200);
  }
  lastNextState = nextState; // De laatste status is de nieuwe status

  // ===== RESET KNOP =====
  bool resetState = digitalRead(BTN_RESET); // lees de status van de knop

  if (resetState == LOW && lastResetState == HIGH) {
    restartTrack(); // herstart huidige track (Functie)
    delay(200);
  }
  lastResetState = resetState; // De laatste status is de nieuwe status

  // ===== EINDE TRACK DETECTIE =====
  if (player.available()) { // checkt of de dfplayer iets nieuws te melden heeft (vb. is de track gedaan met spelen)
    int type = player.readType(); // lees welk type er gemeld is

    if (type == DFPlayerPlayFinished) { // als track klaar is

      if (isLooping) {
        restartTrack(); // opnieuw starten bij loop
      }

      // anders: niets doen
    }
  }
}
