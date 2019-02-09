#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#define PIX_COUNT 60
#define PIX_PIN 2

#include <NeoPixelBus.h>
#include <FFT.h>

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> RainbowTrum(PIX_COUNT, PIX_PIN);

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

  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0xC0; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0

  RainbowTrum.Begin();
  RainbowTrum.Show();
  // End of initialising

  uint8_t* RT_Buffer = (uint8_t*) malloc(3 * PIX_COUNT * sizeof(uint8_t));

  for(;;){
    cli();  // UDRE interrupt slows this way down on arduino1.0

    for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fft_input[i] = k; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
    }

    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    sei();

    for(int i=0; i<=180; i++){
      *(RT_Buffer + i) = 0;
    }

    for(int i=0; i<=60; i++){
      *(RT_Buffer + i) = fft_log_out[i] / 2;
    }

    RT_WriteAll(RT_Buffer);
  }

}



  void loop()
  {

  }
