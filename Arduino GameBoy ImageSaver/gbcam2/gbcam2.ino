
#define MOSI_ 2
#define MISO_ 3
#define SCLK_ 4

int bytes = 0;
uint8_t shift = 0;
uint8_t in_data = 0;
uint8_t out_data = 0;

// what to do with incoming data
byte command = 0;
byte compression = 0;
byte dataLengthLSB = 0;
byte dataLengthMSB = 0;
unsigned int dataPosition = 0;
byte acknowledgeCode = 129;
byte statusCode = 0;


unsigned long last_bit;

byte handleIncomingByte(byte inout) {
  switch (command){
  case 0:
    if (inout == 136){
      // Sync mask 1
      command = 1;
    }
    inout = 0;  
    break;
    
  case 1:
    if (inout == 51){
      // Sync mask 2
      command = 2;
      inout = 0;
    } else {
      command = 0;
      inout = 0;
    } 
    break;
    
  case 2:
    // GBcommand code:
    //=====================
    // 0x01 (1) = Initialize   [NO DATA]
    // 0x02 (2) = Print        [DATA = 0x1?]
    // 0x03 (3) = Unknown
    // 0x04 (4) = Data         [160x16 px]   
    // 0x0F (15) = Inquiry     [NO DATA]
    
    if (inout == 1){
      // Initialize
      command = 3;
      inout = 0;
      compression = 0;
      dataLengthLSB = 0;
      dataLengthMSB = 0;
    } else if (inout == 2){
      // Print
      command = 50;
      inout = 0;
    } else if (inout == 4){
      // Data
      command = 20;
      inout = 0;
    } else if (inout == 15){
      // Inquiry
      command = 90;
      inout = 0;
    } else {
      command = 0;
      inout = 5;
    } 
    break;

  case 3:
    // Initialize sequence - compression
    compression = inout;
    inout = 0;
    command = 4;
    break;
    
  case 4:
    // Initialize sequence - Body length LSB
    dataLengthLSB = inout;
    inout = 0;
    command = 5;
    break;

  case 5:
    // Initialize sequence - Body lengthMSB
    dataLengthMSB = inout;
    inout = 0;
    command = 6;
    break; 
    
  case 6:
    // Initialize sequence - Checksum LSB
    inout = 0;
    command = 7;
    break;
    
  case 7:
    // Initialize sequence - Checksum MSB
    // Print Acknowledgement code
    inout = 129; // 0x80 or 0x81
    command = 8;
    break;
    
  case 8:
    // Initialize sequence
    // Print Status code
    // =============================
    // 0000 0000 = Normal status
    // 1XXX XXXX = Error#1: Low batteries
    // X1XX XXXX = Error#4: Heat level at the printing head is unusual (too hot or too cold)
    // XX1X XXXX = Error#3: Paper jam
    // XXXX 1XXX = Ready to print
    // XXXX X1XX = Print request
    // XXXX XX1X = Printer busy
    // XXXX XXX1 = Bad checksum
    inout = statusCode;
    command = 0;
    break;
    
  case 20:
    // GB is sending image data.
    // Compression bit (GameBoy camara always sends 0)
    compression = inout;
    inout = 0;
    command = 21;
    break;

  case 21:
    // Data sequence - Data size LSB
    dataLengthLSB = inout;
    inout = 0;
    command = 22;
    break;
    
  case 22:
    // Data sequence - Data size MSB
    dataLengthMSB = inout;
    inout = 0;
    command = 23;
    dataPosition = 256 * dataLengthMSB + dataLengthLSB;
    break;
    
  case 23:
    // Data sequence - Data Body
    dataPosition = dataPosition -1;
    // Case 65535 used when data sequence has no body.
    if (dataPosition == 0 || dataPosition == 65535){
      command = 24;
    }else{
      command = 23;
    }
    inout = 0;
    break;
    
  case 24:
    // Data sequence - Checksum LSB
    inout = 0;
    command = 25;
    break;
    
  case 26:
    // Data sequence - Checksum MSB
    inout = acknowledgeCode;
    command = 27;
    break;

  case 27:
    // Data sequence - Acknowledgement code
    inout = statusCode;
    command = 0;
    break;
    
  case 50:
    // Print command - Compression byte
    inout = 0;
    command = 51;
    break;
    
  case 51:
    // Print command - Compression byte
    inout = 0;
    command = 52;
    break;
    
  case 52:
    // Print command - Body length LSB
    // Body data on a print command is always 0x04h.
    inout = 0;
    command = 53;
    break;
   
  case 53:
    // Print command - Body length LSB
    // Body data on a print command is always 0x04h.
    inout = 0;
    command = 54;
    break;
    
   case 54:
    // Print sequence - Margins. 
    inout = 0;
    command = 55;
    break;
    
  case 55:
    // Print sequence - Palette. 
    inout = 0;
    command = 56;
    break;

  case 56:
    // Print sequence - "Ink" density or heater power.
    inout = 0;
    command = 57;
    break;  
    
  case 57:
    // Print sequence - Checksum LSB. 
    inout = 0;
    command = 58;
    break;
    
  case 58:
    // Print sequence - Checksum MSB. 
    inout = acknowledgeCode;
    command = 59;
    break;
    
  case 59:
    // Print sequence - Checksum MSB. 
    inout = statusCode;
    command = 0;
    break;    
    
  case 90:
    // GB inquiry - Compression indicator.
    inout = 0;
    command = 91;
    break;  
    
  case 91:
    // GB Inquiry - Body Length LSB.
    inout = 0;
    command = 92;
    break; 
    
  case 92:
    // GB inquiry - Body Length MSB.
    inout = 0;
    command = 93;
    break;
    
  case 93:
    // GB inquiry - Checksum length LSB.
    inout = 0;
    command = 94;
    break;
    
  case 94:
    // GB inquiry - Checksum length MSB.
    inout = 129;
    command = 95;
    break;
    
  case 95:
    // GB inquiry - Acknowledgement code.
    inout = statusCode;
    command = 0;
    break;
    
  default: 
    inout = 0;
    command = 0;
    break;
  } // end of switch
  //Serial.println(command);
  return inout;
}


void setup() {
    Serial.begin(115200);
    pinMode(SCLK_, INPUT);
    pinMode(MISO_, INPUT);
    pinMode(MOSI_, OUTPUT);
    
    digitalWrite(MOSI_, LOW);
    out_data <<= 1;
    Serial.println("GameBoy ImageSaver v0.1: Setup done.");

}



void loop() {
  
  Serial.println("?");
  while(Serial.read()!=33){
    //LED OFF
  }
  
    while (1) {
    last_bit = micros();
    while(digitalRead(SCLK_)) {
      if (micros() - last_bit > 1000000) {
      }
    }
        transferBit();
    }
}

void transferBit(void) {
    in_data |= digitalRead(MISO_) << (7-shift);

    if(++shift > 7) {
        shift = 0;
        bytes++;
        out_data = handleIncomingByte(in_data);
        //Serial.print(bytes);
        //Serial.print(" ");
        //Serial.print(command);        
        //Serial.print(" ");
        //Serial.printl(in_data, HEX);
        Serial.println(in_data);
        //Serial.print(" ");
        //Serial.print(out_data, HEX);
        //Serial.print("\n");
        in_data = 0;
    }
    
    while(!digitalRead(SCLK_));
    //Serial.print(out_data & 0x80 ? HIGH : LOW);
    digitalWrite(MOSI_, out_data & 0x80 ? HIGH : LOW);
    out_data <<= 1;
}
