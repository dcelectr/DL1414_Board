//**********************************************************************************
//    *   *   *                                                                    *
//  -------------                                                                  *
//  ---  ---  ---                                                                  *
// d c e l e c t r  Library for Litronix / Siemens DL1414 16-Segment LED Displays. *
//**********************************************************************************

#include <SPI.h>

const uint8_t RCLK_PIN = 10; //RCLK, for 74HC595N Storage Register Clock
const uint8_t SDA_PIN  = 11; //MOSI, for 74HC595N Serial Data Input
const uint8_t SCLK_PIN = 13; //SCLK, for 74HC595N Shift Register Clock

uint8_t SELECTED_DISP_NUM=0;

//The storage register transfers data to the output buffer when shift register clear (SRCLR) is high. 
//When SRCLR is low, the input shift register is cleared. The SRCLR is connected to Vcc permanently.
//When output enable (OE) is held high, all data in the output buffers is held low and all drain outputs are off. OE is connected to GND permanently.

void setup() {
  SPI.begin();                       //Start the SPI library
  SPI.setBitOrder(MSBFIRST);         //Set the bit order as MSB.
  //SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0)); // Uncomment this if there is a need to set SPI speed to 4MHz explicitly.  
  pinMode(RCLK_PIN, OUTPUT);        //Data transfers through the storage register on the rising edge of the register clock (RCK).
  pinMode(SDA_PIN, OUTPUT);
  pinMode(SCLK_PIN, OUTPUT);        //The device transfers data out the serial output (SER OUT) port on the rising edge of SRCK.
  digitalWrite(RCLK_PIN, HIGH);
}

void loop() {
  clearDisplay();
  delay(500);
  String messageString = "The quick brown fox jumps over the lazy dog";
  messageString.toUpperCase();
  writeDisplay(messageString);
}

void writeChar(uint8_t DATA, uint8_t ADDR, uint8_t DISP_NUM){

  uint8_t WR_ADDR = ~(ADDR & 0X03); // Address is swapped because D3 D2 D1 D0 is the order of digits and set all WR bits to 1 when inverting the mask
  SPI.transfer(DATA);
  SPI.transfer(WR_ADDR); 
  digitalWrite(RCLK_PIN, HIGH);
  digitalWrite(RCLK_PIN, LOW);

  WR_ADDR = WR_ADDR ^ (0X04<<DISP_NUM);  // flip WR bit to 0
  SPI.transfer(DATA);  
  SPI.transfer(WR_ADDR); 
  digitalWrite(RCLK_PIN, HIGH);
  digitalWrite(RCLK_PIN, LOW);

  WR_ADDR = WR_ADDR ^ (0X04<<DISP_NUM);  // flip WR bit to 1
  SPI.transfer(DATA);  
  SPI.transfer(WR_ADDR); 
  digitalWrite(RCLK_PIN, HIGH);
  digitalWrite(RCLK_PIN, LOW);
}

void clearDisplay(){
  uint8_t CLR_DATA=32;
  for (uint8_t i=0; i<4; i++){
    uint8_t CLR_ADDR = i;
    writeChar(CLR_DATA, CLR_ADDR, SELECTED_DISP_NUM);
  } 
}

void writeDisplay(String message){

  char displaybuffer[4] = {' ', ' ', ' ', ' '};
 
  for (uint8_t i=0; i<(message.length())+4; i++) {
    // scroll down display
    displaybuffer[0] = displaybuffer[1];
    displaybuffer[1] = displaybuffer[2];
    displaybuffer[2] = displaybuffer[3]; 
    if (i<(message.length())){
      displaybuffer[3] = message[i];
      }
    else{
      displaybuffer[3] = {' '}; 
      } 
    // send a message!
    for (uint8_t j=0; j<4; j++) {
    writeChar(displaybuffer[j],j,SELECTED_DISP_NUM);
    }
    delay(300);
  }
}

void charTest(){
  // Function for testing only (loop)
  writeChar('A',0,SELECTED_DISP_NUM);
  writeChar('B',1,SELECTED_DISP_NUM);
  writeChar('C',2,SELECTED_DISP_NUM);
  writeChar('D',3,SELECTED_DISP_NUM);
  delay(1000);
  for (uint8_t ii=32; ii < 93; ++ii){
    for (uint8_t i=0; i < 4; ++i){
      writeChar({ii+i},i,SELECTED_DISP_NUM);          
    }
    delay(500);   
  }
}
