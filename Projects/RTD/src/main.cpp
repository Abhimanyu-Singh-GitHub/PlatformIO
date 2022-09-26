#include <Arduino.h>
#include <Tone.h> 


int Buzzer = 11;
int songLength = 6;
int switchPin = 9; //change if need be 
char notes[] = "gggg gg gg "; // triplet quarter, eight quarter, eight quarter
int beats[] = {3,1,2,1,2,1}; //3,1,2,1,2,1

int frequency(char note) 
{
  int i;
  const int numNotes = 8; // number of notes stored
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};
  for (i = 0; i < numNotes; i++)  // Step through the notes
  {
    if (names[i] == note)         // Is this the one?
    {
      return(frequencies[i]);     // Yes! Return the frequency
    }
  }
  return(0);  // looked through everything and didn't find it,
              // but we still need to return a value, so return 0.
}

// A "1" represents a quarter-note, 2 a half-note, 3 a triplet??
int tempo = 116; //tempo for the aggie war hymn
void setup() {
  // put your setup code here, to run once:
   pinMode(switchPin, INPUT);
   pinMode(Buzzer, OUTPUT);
}
void loop() {
  // put your main code here, to run repeatedly:
  int i, duration;
  if (digitalRead(switchPin) == HIGH)
  {
    for (i = 0; i < songLength; i++)
    {
      duration = beats[i]*tempo;
      if (notes[i] == ' ')
      {
        delay(duration);
      }
      else
      {
        tone(Buzzer, frequency(notes[i]), duration);
      delay(duration);
      }
      delay(tempo/10);
      // brief pause between notes
    }
  }
}
