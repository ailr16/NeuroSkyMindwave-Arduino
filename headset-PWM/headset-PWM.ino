////////////////////////////////////////////////////////////////////////
// Arduino Bluetooth Interface with Mindwave
// 
// This is example code provided by NeuroSky, Inc. and is provided
// license free.
//
// This modification allows control devices through PWM
// Lozano Ramirez Angel Ivan
// Mexico  4.07.2021
////////////////////////////////////////////////////////////////////////

#define LED 13
#define output 11
#define BAUDRATE 57600
#define DEBUGOUTPUT 0

// checksum variables
byte  generatedChecksum = 0;
byte  checksum = 0; 
int   payloadLength = 0;
byte  payloadData[64] = {0};
byte  poorQuality = 0;
byte  attention = 0;
byte  meditation = 0;

// system variables
long    lastReceivedPacket = 0;
boolean bigPacket = false;

//////////////////////////
// Microprocessor Setup //
//////////////////////////
void setup(){
  pinMode(LED, OUTPUT);
  pinMode(output, OUTPUT);
  Serial.begin(BAUDRATE);
}

////////////////////////////////
// Read data from Serial UART //
////////////////////////////////
byte ReadOneByte() {
  int ByteRead;
  while(!Serial.available());
  ByteRead = Serial.read();
  return ByteRead;
}

/////////////
//MAIN LOOP//
/////////////
void loop() {
  // Look for sync bytes
  if(ReadOneByte() == 170){
    if(ReadOneByte() == 170){
      payloadLength = ReadOneByte();
      if(payloadLength > 169) return;                      //Payload length can not be greater than 169
      generatedChecksum = 0;        
      for(int i = 0; i < payloadLength; i++) {  
        payloadData[i] = ReadOneByte();            //Read payload into memory
        generatedChecksum += payloadData[i];
      }   

      checksum = ReadOneByte();                      //Read checksum byte from stream      
      generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum
      if(checksum == generatedChecksum) {    
        poorQuality = 200;
        attention = 0;
        meditation = 0;

        for(int i = 0; i < payloadLength; i++) {    // Parse the payload
          switch (payloadData[i]) {
          case 2:
            i++;            
            poorQuality = payloadData[i];
            bigPacket = true;            
            break;
          case 4:
            i++;
            attention = payloadData[i];                        
            break;
          case 5:
            i++;
            meditation = payloadData[i];
            break;
          case 0x80:
            i = i + 3;
            break;
          case 0x83:
            i = i + 25;      
            break;
          default:
            break;
          } // switch
        } // for loop
        #if !DEBUGOUTPUT
            // *** Add your code here ***
          if(bigPacket) {
            if(poorQuality == 0)  digitalWrite(LED, HIGH);
            else digitalWrite(LED, LOW);
            
            analogWrite(output, (int)attention);
           }                     
        }
        #endif      
        bigPacket = false;        
      }
      else; // Checksum Error
    } // end if read 0xAA byte
} // end if read 0xAA byte
