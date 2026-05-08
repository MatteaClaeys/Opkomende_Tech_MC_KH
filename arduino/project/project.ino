#include "DFRobotDFPlayerMini.h"

HardwareSerial mySerial(2);
DFRobotDFPlayerMini player;

// DFPlayer pins
static const int PIN_MP3_TX = 17;
static const int PIN_MP3_RX = 16;

// Knoppen
const int BTN_1 = 14; // Reset
const int BTN_2 = 27; // Pauze / Loop-combo
const int BTN_3 = 26; // Pauze / Loop-combo
const int BTN_4 = 25; // Volgende track

// LED
const int LED_PIN = 2;

// States
bool isPaused     = false;
bool isLooping    = false;
int  currentTrack = 1;

// Vorige knoopstatus
bool lastBtn1 = HIGH;
bool lastBtn2 = HIGH;
bool lastBtn3 = HIGH;
bool lastBtn4 = HIGH;

// Timing
const unsigned long COMBO_WINDOW  = 80;  // ms tussen BTN_2 en BTN_3 om als combo te tellen
const unsigned long COMBO_HOLD    = 900; // ms vasthouden voor loop-toggle
const unsigned long DEBOUNCE_TIME = 250; // ms: minimale tijd tussen twee acties per knop

// Debounce timestamps per knop
unsigned long lastActionBtn1 = 0;
unsigned long lastActionBtn2 = 0;
unsigned long lastActionBtn3 = 0;
unsigned long lastActionBtn4 = 0;

// Pauze pending flags
bool btn2Pending      = false;
bool btn3Pending      = false;
unsigned long btn2PressTime = 0;
unsigned long btn3PressTime = 0;

// Combo state
bool comboActive      = false;
unsigned long comboStartTime = 0;


// ================= FUNCTIES =================

// Speelt de huidige track — respecteert of loop aan staat
void playCurrentTrack() {
  if (isLooping) {
    player.loop(currentTrack); // DFPlayer herhaalt track zelf
  } else {
    player.play(currentTrack);
  }
  isPaused = false;
}

// Reset: loop uit, track opnieuw starten
void restartTrack() {
  isLooping = false;
  digitalWrite(LED_PIN, LOW);
  player.play(currentTrack); // gewoon spelen, geen loop
  isPaused = false;
}

void togglePause() {
  if (isPaused) {
    player.start();
    isPaused = false;
  } else {
    player.pause();
    isPaused = true;
  }
}

// Loop toggle: schakel DFPlayer native loop in of uit
void toggleLoop() {
  isLooping = !isLooping;
  digitalWrite(LED_PIN, isLooping ? HIGH : LOW);

  if (isLooping) {
    player.loop(currentTrack); // DFPlayer loopt track zelf, geen software detectie nodig
  } else {
    player.play(currentTrack); // Terug naar normaal afspelen
  }
}

void switchTrack() {
  currentTrack = (currentTrack == 1) ? 2 : 1;
  playCurrentTrack(); // respecteert loop-status voor nieuwe track
}


// ================= SETUP =================
void setup() {
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  mySerial.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX);

  delay(1000); // Wacht tot DFPlayer opgestart is

  if (player.begin(mySerial)) {
    player.setTimeOut(500);
    player.outputDevice(DFPLAYER_DEVICE_SD);
    player.EQ(DFPLAYER_EQ_NORMAL);
    player.volume(25); 
    playCurrentTrack();
  }
}


// ================= LOOP =================
void loop() {
  unsigned long now = millis();

  bool btn1 = digitalRead(BTN_1);
  bool btn2 = digitalRead(BTN_2);
  bool btn3 = digitalRead(BTN_3);
  bool btn4 = digitalRead(BTN_4);

  // ===== Vallende flanken (knop net ingedrukt) =====
  if (btn2 == LOW && lastBtn2 == HIGH) {
    btn2Pending   = true;
    btn2PressTime = now;
  }
  if (btn3 == LOW && lastBtn3 == HIGH) {
    btn3Pending   = true;
    btn3PressTime = now;
  }

  // ===== Combo-detectie: BTN_2 + BTN_3 bijna tegelijk =====
  if (btn2Pending && btn3Pending && !comboActive) { //deze code kijkt of knop 2 en knop 3 binnen een korte tijd van elkaar zijn ingedrukt. Als dat zo is, start hij een combo, slaat hij de starttijd op en voorkomt hij dat de twee knopdrukken ook nog apart worden verwerkt.
    unsigned long diff = (btn2PressTime > btn3PressTime)
                         ? (btn2PressTime - btn3PressTime)
                         : (btn3PressTime - btn2PressTime);
    if (diff <= COMBO_WINDOW) {
      comboActive    = true;
      comboStartTime = (btn2PressTime > btn3PressTime) ? btn2PressTime : btn3PressTime;
      btn2Pending    = false;
      btn3Pending    = false;
    }
  }

  // Ongeldige combo met BTN_1 of BTN_4: annuleer pending
  if (btn2Pending && (btn1 == LOW || btn4 == LOW)) {
    btn2Pending = false;
  }
  if (btn3Pending && (btn1 == LOW || btn4 == LOW)) {
    btn3Pending = false;
  }

  // ===== Combo loslaten =====
  if (comboActive && btn2 == HIGH && btn3 == HIGH) {  //Als de combo actief is, knop 2 en knop 3 nog steeds vastgehouden worden, ze lang genoeg zijn ingedrukt, knop 1 en knop 4 niet meedoen, én de debounce-tijd voorbij is, voer dan toggleLoop() uit en markeer knop 2 en knop 3 als net gebruikt.
    if ((now - comboStartTime >= COMBO_HOLD) && btn1 == HIGH && btn4 == HIGH) {
      if (now - lastActionBtn2 > DEBOUNCE_TIME && now - lastActionBtn3 > DEBOUNCE_TIME) {
        toggleLoop();
        lastActionBtn2 = now;
        lastActionBtn3 = now;
      }
    }
    comboActive = false;
  }

  // ===== Stijgende flanken: enkelvoudige pauze =====
  if (btn2 == HIGH && lastBtn2 == LOW) {  //Als knop 2 of knop 3 wordt losgelaten en die knop niet gebruikt is voor een combo, dan voert de code togglePause() uit. Maar alleen als de andere knoppen niet ingedrukt zijn en de debounce-tijd voorbij is.
    if (btn2Pending && !comboActive && btn1 == HIGH && btn3 == HIGH && btn4 == HIGH) {
      if (now - lastActionBtn2 > DEBOUNCE_TIME) {
        togglePause();
        lastActionBtn2 = now;
      }
    }
    btn2Pending = false;
  }
  if (btn3 == HIGH && lastBtn3 == LOW) {
    if (btn3Pending && !comboActive && btn1 == HIGH && btn2 == HIGH && btn4 == HIGH) {
      if (now - lastActionBtn3 > DEBOUNCE_TIME) {
        togglePause();
        lastActionBtn3 = now;
      }
    }
    btn3Pending = false;
  }

  // ===== BTN_1: Reset =====
  if (btn1 == LOW && lastBtn1 == HIGH) {
    if (btn2 == HIGH && btn3 == HIGH && btn4 == HIGH) {
      if (now - lastActionBtn1 > DEBOUNCE_TIME) {
        restartTrack();
        lastActionBtn1 = now;
      }
    }
  }

  // ===== BTN_4: Volgende track =====
  if (btn4 == LOW && lastBtn4 == HIGH) {
    if (btn1 == HIGH && btn2 == HIGH && btn3 == HIGH) {
      if (now - lastActionBtn4 > DEBOUNCE_TIME) {
        switchTrack();
        lastActionBtn4 = now;
      }
    }
  }

  // Sla statussen op
  lastBtn1 = btn1;
  lastBtn2 = btn2;
  lastBtn3 = btn3;
  lastBtn4 = btn4;
}
