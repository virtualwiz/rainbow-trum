#include <NeoPixelBus.h>

#define PIX_COUNT 60
#define PIX_PIN 2

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

  for(int i = 0; i < 180; i++){
    *(RT_Buffer + i) = 20;
  }

  RT_WriteAll(RT_Buffer);


}

void loop()
{

}
