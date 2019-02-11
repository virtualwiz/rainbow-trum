#define FFT_N 256 // set to 256 point fft
#define LOG_OUT 1

#define PIX_COUNT 60
#define PIX_PIN 2

#include <NeoPixelBus.h>
#include <FFT.h>

//#define THRESHOLD_R 20
//#define THRESHOLD_G 30
//#define THRESHOLD_B 40

#define THRESHOLD 50

#define RANGE_R (THRESHOLD + 3)
#define RANGE_O (THRESHOLD + 6)
#define RANGE_Y (THRESHOLD + 9)
#define RANGE_G (THRESHOLD + 12)
#define RANGE_C (THRESHOLD + 15)
#define RANGE_B (THRESHOLD + 18)
#define RANGE_P (THRESHOLD + 21)

#define OFF 0, 0, 0
#define RED 20, 0, 0
#define ORANGE 40, 30, 0
#define YELLOW 50, 50, 0
#define GREEN 0, 70, 0
#define CYAN 0, 80, 80
#define BLUE 0, 80, 150
#define PURPLE 150, 100, 150

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> RainbowTrum(PIX_COUNT, PIX_PIN);

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

void Colour_To_Buffer(uint8_t* Buffer_Ptr, uint8_t position, uint8_t r, uint8_t g, uint8_t b){
  *(Buffer_Ptr + position) = r;
  *(Buffer_Ptr + position + PIX_COUNT) = g;
  *(Buffer_Ptr + position + (2 * PIX_COUNT)) = b;
}

void setup()
{
  // Initialising
  Serial.begin(115200);
  while (!Serial);
  Serial.flush();

  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe7;
  ADMUX = 0x40;
  DIDR0 = 0x01;

  RainbowTrum.Begin();
  RainbowTrum.Show();
  // End of initialising

  uint8_t* RT_Buffer = (uint8_t*) malloc(3 * PIX_COUNT * sizeof(uint8_t));
  uint8_t Spectrum[60];

  for(int i=0; i<=180; i++){
    *(RT_Buffer + i) = 0;
  }

  for(;;){
    cli();

    for (int i = 0 ; i < 512 ; i += 2) {
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf7; // restart adc
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf7; // restart adc
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf7; // restart adc

      byte m = ADCL;
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
    fft_mag_log(); // log output
    sei();

    for(uint8_t i = 0; i < PIX_COUNT; i++){
      *(Spectrum + i) = *(fft_log_out + (i + i / 2 + 33));
      //Serial.println(*(Spectrum + i));
    }

    for(uint8_t i = 0; i < PIX_COUNT; i++){
      if(*(Spectrum + i) <= THRESHOLD){
        Colour_To_Buffer(RT_Buffer, i, OFF);
      }
      else if(*(Spectrum + i) <= RANGE_R){
        Colour_To_Buffer(RT_Buffer, i, RED);
      }
      else if(*(Spectrum + i) <= RANGE_O){
        Colour_To_Buffer(RT_Buffer, i, ORANGE);
      }
      else if(*(Spectrum + i) <= RANGE_Y){
        Colour_To_Buffer(RT_Buffer, i, YELLOW);
      }
      else if(*(Spectrum + i) <= RANGE_G){
        Colour_To_Buffer(RT_Buffer, i, GREEN);
      }
      else if(*(Spectrum + i) <= RANGE_C){
        Colour_To_Buffer(RT_Buffer, i, CYAN);
      }
      else if(*(Spectrum + i) <= RANGE_B){
        Colour_To_Buffer(RT_Buffer, i, BLUE);
      }
      else{
        Colour_To_Buffer(RT_Buffer, i, PURPLE);
      }
    }

    RT_WriteAll(RT_Buffer); // Write to Rainbowtrum

  }
}


void loop()
{

}
