#include <Wire.h>
#include <SD.h>
int CS_Pin = 10;

void Init_YUV422(){
  WriteOV7670(0x12, 0x00);//COM7
  WriteOV7670(0x8C, 0x00);//RGB444
  WriteOV7670(0x04, 0x00);//COM1
  WriteOV7670(0x40, 0xC0);//COM15
  WriteOV7670(0x14, 0x1A);//COM9
  WriteOV7670(0x3D, 0x40);//COM13
  }

void Init_QVGA(){
  WriteOV7670(0x0C, 0x04);//COM3 - Enable Scaling
  WriteOV7670(0x3E, 0x19);//COM14
  WriteOV7670(0x72, 0x11);//
  WriteOV7670(0x73, 0xF1);//
  WriteOV7670(0x17, 0x16);//HSTART
  WriteOV7670(0x18, 0x04);//HSTOP
  WriteOV7670(0x32, 0xA4);//HREF
  WriteOV7670(0x19, 0x02);//VSTART
  WriteOV7670(0x1A, 0x7A);//VSTOP
  WriteOV7670(0x03, 0x0A);//VREF
  }

void Init_OV7670(){
  WriteOV7670(0x12,0x80);
  delay(100);
  WriteOV7670(0x3A, 0x04); //TSLB
 
  WriteOV7670(0x13, 0xC0); //COM8
  WriteOV7670(0x00, 0x00); //GAIN
  WriteOV7670(0x10, 0x00); //AECH
  WriteOV7670(0x0D, 0x40); //COM4
  WriteOV7670(0x14, 0x18); //COM9
  WriteOV7670(0x24, 0x95); //AEW
  WriteOV7670(0x25, 0x33); //AEB
  WriteOV7670(0x13, 0xC5); //COM8
  WriteOV7670(0x6A, 0x40); //GGAIN
  WriteOV7670(0x01, 0x40); //BLUE
  WriteOV7670(0x02, 0x60); //RED
  WriteOV7670(0x13, 0xC7); //COM8
  WriteOV7670(0x41, 0x08); //COM16
  WriteOV7670(0x15, 0x20); //COM10 - PCLK does not toggle on HBLANK
  }

void WriteOV7670(byte regID, byte regVal){
  Wire.beginTransmission(0x21);
  Wire.write(regID);  
  Wire.write(regVal);
  Wire.endTransmission();
  delay(1); 
  }

void ReadOV7670(byte regID){
  Wire.beginTransmission(0x21); // 7-bit Slave address
  Wire.write(regID);  // reading from register 0x11
  Wire.endTransmission();
  Wire.requestFrom(0x21, 1);
  Serial.print("Read request Status:");
  Serial.println(Wire.available());
  Serial.print(regID,HEX);
  Serial.print(":");
  Serial.println(Wire.read(),HEX);
  }

void XCLK_SETUP(void){
  pinMode(9, OUTPUT); //Set pin 9 to output
  TCCR1A = (1 << COM1A0) | (1 << WGM11) | (1 << WGM10);
  //SET CS10 bit for clock select with no prescaling
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10); 
  OCR1A = 0;
}

void TWI_SETUP(void){
  //Set prescaler bits in TWI Status Register (TWSR) to 0
  TWSR &= ~3;
  //Set SCL frequency to 100KHz
  //SCLfreq = CPUfreq/(16 + 2(TWBR) - 4^(TWPS))
  //TWBR = 72, TWPS(prescaler) = 0
  TWBR = 72;
}

void OV7670_PINS(void){
  
  DDRC &= ~15;//low d0-d3 camera
  DDRD &= ~252;
}

void QVGA_Image(String title){
  int h,w;
  
  File dataFile = SD.open(title, FILE_WRITE);
  while (!(PIND & 8));//wait for high
  while ((PIND & 8));//wait for low

    h = 240;
  while (h--){
        w = 320;
       byte dataBuffer[320];
    while (w--){
      while ((PIND & 4));   //wait for low
        dataBuffer[319-w] = (PINC & 15) | (PIND & 240);
      while (!(PIND & 4));  //wait for high
      while ((PIND & 4));   //wait for low
      while (!(PIND & 4));  //wait for high
    }
    dataFile.write(dataBuffer,320);
    
  }

    dataFile.close();
    delay(100);
}

void setup(){
 
  noInterrupts(); //Disable all interrupts
  XCLK_SETUP();   //Setup 8MHz clock at pin 11
  OV7670_PINS();  // Setup Data-in and interrupt pins from camera
  delay(1000);
  TWI_SETUP();    // Setup SCL for 100KHz
  interrupts();
  Wire.begin();
  
  
  Init_OV7670();
  Init_QVGA();
  Init_YUV422();
  WriteOV7670(0x11, 0x1F); //Range 00-1F
  noInterrupts();
  Serial.begin(9600);
  pinMode(CS_Pin, OUTPUT);
  SD.begin(CS_Pin);
 
}

void loop(){
  QVGA_Image("0.bmp");
  QVGA_Image("1.bmp");
  QVGA_Image("2.bmp");
  QVGA_Image("3.bmp");
  QVGA_Image("4.bmp");
  QVGA_Image("5.bmp");
  while(1);
}
