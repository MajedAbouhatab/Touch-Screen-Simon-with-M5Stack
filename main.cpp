#include <M5Unified.h>
#include "pitches.h"

#define MAX_GAME_LENGTH 100
#define LedState(b) M5.Display.fillCircle(LI % 2 * 120 + 60, int(LI / 2) * 120 + 60, b ? 55 : 45, b ? Colors[LI] : BLACK);
#define LoopLedSequence           \
  for (int i = 0; i < Level; i++) \
    LightAndTone(LedSequence[i]);
#define LoopGameTones(s, e, d) \
  for (int i = s; i < e; i++)  \
    SpeakerTone(GameTones[i], d);
// https://github.com/robsoncouto/arduino-songs/blob/master/thegodfather/thegodfather.ino
const int GameTones[] = {NOTE_G3, NOTE_C4, NOTE_E4, NOTE_G5,
                         NOTE_E4, NOTE_G4, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_G5,
                         NOTE_DS5, NOTE_D5, NOTE_CS5, NOTE_C5,
                         NOTE_E4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_C5, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_E4};
const int Colors[] = {RED, BLUE, GREEN, YELLOW};

// Global variables - store the game state
uint8_t LedSequence[MAX_GAME_LENGTH] = {0}, Level = 0;

void SpeakerTone(int f, int d)
{
  M5.Speaker.tone(f);
  delay(d);
  M5.Speaker.stop();
}

// Lights the given LED and plays a suitable tone
void LightAndTone(byte LI)
{
  delay(100);
  LedState(true);
  SpeakerTone(GameTones[LI], 100);
  LedState(false);
  delay(100);
}

// Get the user's input and compare it with the expected sequence.
bool CheckUserSequence()
{
  for (int i = 0; i < Level; i++)
  {
    byte ActualButton = 4;
    // Waits until the user pressed one of the buttons
    while (ActualButton == 4)
    {
      M5.update();
      auto t = M5.Touch.getDetail();
      // Touched
      if (t.state == 1)
        // Chnage volume
        if (t.x < 0)
        {
          M5.Speaker.setVolume(map(t.y, 0, 320, 0, 256));
          SpeakerTone(NOTE_E4, 300);
          delay(300);
        }
        else if (t.x < 240 && t.y < 240)
          // Get the button pressed
          ActualButton = 2 * int(t.y / 120) + int(t.x / 120);
        else
          // Say again
          LoopLedSequence;
    }
    LightAndTone(ActualButton);
    if (LedSequence[i] != ActualButton)
      return false;
  }
  return true;
}

void setup()
{
  M5.begin();
  M5.Display.setRotation(0);
  for (int LI = 0; LI < 4; LI++)
  {
    LedState(true);
    LedState(false);
  }
  M5.Speaker.setVolume(255);
  LoopGameTones(14, 26, 300);
  delay(2000);
}

void loop()
{
  // Add a random color to the end of the sequence
  LedSequence[Level++] = esp_random() % 4; // random(0, 4);
  constrain(Level, 0, MAX_GAME_LENGTH - 1);

  // Show game progress
  M5.Display.setFont(&fonts::Font7);
  M5.Display.setCursor(150, 250);
  M5.Display.printf("%02d", Level - 1);
  M5.Display.setFont(&fonts::Font4);
  M5.Display.setCursor(10, 260);
  M5.Display.print("Your score: ");

  // Plays the current sequence of notes that the user has to repeat
  LoopLedSequence;

  if (!CheckUserSequence())
  {
    Level = 0;
    M5.Display.setCursor(10, 260);
    M5.Display.print("Game over!");
    delay(200);
    // Wah-Wah-Wah-Wah sound
    LoopGameTones(10, 14, 300);
    delay(300);
  }
  else
    // Plays a hooray sound whenever the user finishes a level
    LoopGameTones(4, 10, 150);

  delay(300);
}
