#include <NeoPixelBus.h>

#define PIX_COUNT 60
#define PIX_PIN 2

#define BEAT 0 // 3
#define SECTION 4 // 11
#define RHYTHM 12 // 23

#define BEAT_IDLE 0,10,10
#define SECTION_IDLE 15,10,0
#define RHYTHM_IDLE 15,5,5

#define BEAT_ACT 60,0,0
#define SECTION_ACT 0,60,0
#define RHYTHM_ACT 0,20,60

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> RainbowTrum(PIX_COUNT, PIX_PIN);

int intensity = 50;

RgbColor red(127, 0, 0);
RgbColor green(0, 127, 0);
RgbColor blue(0, 0, 127);
RgbColor white(127);
RgbColor black(5);

void RT_WriteOne(uint8_t position, uint8_t r, uint8_t g, uint8_t b){
  RainbowTrum.SetPixelColor(position, RgbColor(r, g, b));
  RainbowTrum.Show();
}

void RT_WriteAll(uint8_t* Buffer_Ptr){
  int position;
  for(position = 0; position < PIX_COUNT; position++){
    RainbowTrum.SetPixelColor(position,
                              RgbColor(
                                       *(Buffer_Ptr + position),
                                       *(Buffer_Ptr + position + 60),
                                       *(Buffer_Ptr + position + 120)
                                       ));
  }
  RainbowTrum.Show();
}

void setup()
{
  // Initialising
  Serial.begin(115200);
  while (!Serial);
  Serial.flush();
  RainbowTrum.Begin();
  RainbowTrum.Show();
  // End of initialising

  uint8_t* RT_Buffer = (uint8_t*) malloc(3 * PIX_COUNT * sizeof(uint8_t));

  int index;

  for(index = BEAT; index <= BEAT + 3; index++){
    RT_WriteOne(index, BEAT_IDLE);
  }

  for(index = SECTION; index <= SECTION + 8; index++){
    RT_WriteOne(index, SECTION_IDLE);
  }

  for(index = RHYTHM; index <= RHYTHM + 12; index++){
    RT_WriteOne(index, RHYTHM_IDLE);
  }

  // THE SHOW BEGINS : Preparing Beats
  for(int i = 0; i < 8; i++){
    for(index = BEAT; index <= BEAT + 3; index++){
      RT_WriteOne(index, 100, 50, 0);
    }
    delay(100);
    for(index = BEAT; index <= BEAT + 3; index++){
      RT_WriteOne(index, BEAT_IDLE);
    }
    delay(900);
  }

  uint8_t Melody[32] = {
    3,2,3,4,5,4,3,2,3,6,5,4,5,6,7,8,6,8,7,6,5,4,3,5,4,5,6,5,4,3,2,0
  };

  //Music Starts
  uint8_t Last_Melody = 0;
  for(index = 0; index <= 31; index++){
    // Beat
    switch(index % 4){
    case 0:
      RT_WriteOne(BEAT + 0, BEAT_ACT);
      RT_WriteOne(BEAT + 3, BEAT_IDLE);
      break;
    case 1:
      RT_WriteOne(BEAT + 1, BEAT_ACT);
      RT_WriteOne(BEAT + 0, BEAT_IDLE);
      break;
    case 2:
      RT_WriteOne(BEAT + 2, BEAT_ACT);
      RT_WriteOne(BEAT + 1, BEAT_IDLE);
      break;
    case 3:
      RT_WriteOne(BEAT + 3, BEAT_ACT);
      RT_WriteOne(BEAT + 2, BEAT_IDLE);
      break;
    }

    // Section
    RT_WriteOne(SECTION + index / 4, SECTION_ACT);

    // Rhythm
    RT_WriteOne(RHYTHM + Last_Melody + 2, RHYTHM_IDLE);
    Last_Melody = Melody[index];
    RT_WriteOne(RHYTHM + Last_Melody + 2, RHYTHM_ACT);

    delay(480);
  }

  //Ending

  for(index = BEAT; index <= BEAT + 3; index++){
    RT_WriteOne(index, BEAT_IDLE);
  }

  for(index = SECTION; index <= SECTION + 8; index++){
    RT_WriteOne(index, SECTION_IDLE);
  }

  for(index = RHYTHM; index <= RHYTHM + 12; index++){
    RT_WriteOne(index, RHYTHM_IDLE);
  }

  for(int i = 0; i < 4; i++){
    for(index = SECTION; index <= SECTION + 7; index++){
      RT_WriteOne(index, 50, 0, 50);
    }
    delay(500);
    for(index = SECTION; index <= SECTION + 7; index++){
      RT_WriteOne(index, SECTION_IDLE);
    }
    delay(500);
  }


}

void loop()
{

}
