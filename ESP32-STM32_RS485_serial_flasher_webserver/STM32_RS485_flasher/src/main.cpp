#include <Arduino.h>
#include <WiFi.h>

const char AP_DEMO_HTTP_200_IMAGE[] = "HTTP/1.1 200 OK\r\nPragma: public\r\nCache-Control: max-age=1\r\nExpires: Thu, 26 Dec 2016 23:59:59 GMT\r\nContent-Type: image/";

typedef enum {
    UPL_AP_STAT_MAIN = 1,             // GET /
    UPL_AP_STAT_LED_HIGH,             // GET /H
    UPL_AP_STAT_LED_LOW,              // GET /L
    UPL_AP_STAT_GET_IMAGE,            // GET /logo.bmp
    UPL_AP_STAT_GET_FAVICON,          // GET /favicon.ico
    UPL_AP_STAT_POST_UPLOAD,          // POST /upload
    UPL_AP_STAT_POST_START_BOUNDRY,   // POST /upload boundry
    UPL_AP_STAT_POST_GET_BOUNDRY,     // POST /upload boundry
    UPL_AP_STAT_POST_START_IMAGE,     // POST /upload image
    UPL_AP_STAT_POST_GET_IMAGE,       // POST /upload image
    UPL_AP_STAT_REFLASH_REQUEST_DETECTED
} UPL_AP_STAT_t;

const uint8_t favicon[] = {
0x00,0x00,0x01,0x00,0x02,0x00,0x0F,0x0E,0x02,0x00,0x01,0x00,0x01,0x00,0xA0,0x00,
0x00,0x00,0x26,0x00,0x00,0x00,0x10,0x10,0x02,0x00,0x01,0x00,0x01,0x00,0xB0,0x00,
0x00,0x00,0xC6,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x1C,0x00,
0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xBF,0xFE,
0x00,0x00,0xBF,0xFE,0x00,0x00,0xBF,0xFE,0x00,0x00,0x87,0xC6,0x00,0x00,0xBB,0xBA,
0x00,0x00,0x9D,0x9E,0x00,0x00,0xBD,0xBE,0x00,0x00,0x9D,0x9E,0x00,0x00,0xBB,0xBA,
0x00,0x00,0x83,0xC6,0x00,0x00,0xFF,0xFE,0x00,0x00,0xFF,0xFE,0x00,0x00,0xFF,0xFE,
0x00,0x00,0xFF,0xFE,0x00,0x00,0x28,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x20,0x00,
0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00
};


const char* ssid     = "STM32_reflash_AP";
const char* passphrase = "1234567890";


WiFiServer server(80);

#define reflashingIP      192,168,4,2
#define reflashingGateway 192,168,4,2
#define reflashingSubnet  255,255,255,0
#define reflashing
#define MAX_IMAGE_SIZE 65535
#define MAX_BUF_SIZE 1024
#define SERIAL_DEBUG
//#define IMAGE_DEBUG

#define RE_PIN 22
#define DE_PIN 23
#define RS485_port Serial2

int value = 0;
char boundbuf[MAX_BUF_SIZE];
int boundpos=0;
char imagetypebuf[MAX_BUF_SIZE];
int imagetypepos=0;
char imagebuf[MAX_IMAGE_SIZE];
int imagepos=0;
char checksum = 0;
int packetBytesCounter = 0;
int imageCounter = 0;
uint8_t currentAddress = 0;
uint8_t  isInReflashForm=0;
uint8_t reflashingResult = 0;


void setup() {
    bool ret;
    pinMode(RE_PIN, OUTPUT);
    pinMode(DE_PIN, OUTPUT);
    digitalWrite(RE_PIN, LOW);
    digitalWrite(DE_PIN, LOW);


    Serial.begin(115200);
    RS485_port.begin(115200);//115200
 
    // We start by connecting to a WiFi network
#ifdef  SERIAL_DEBUG
    Serial.println();
    Serial.println();
    Serial.print("Take Online ... ");
    Serial.println(ssid);
#endif

  /*
   * Set up an access point
   * @param ssid          Pointer to the SSID (max 63 char).
   * @param passphrase    (for WPA2 min 8 char, for open use NULL)
   * @param channel       WiFi channel number, 1 - 13.
   * @param ssid_hidden   Network cloaking (0 = broadcast SSID, 1 = hide SSID)
   */


    WiFi.mode(WIFI_AP);
    ret = WiFi.softAP(ssid, passphrase, 2, 0);
    delay(100);
    if(!WiFi.softAPConfig(IPAddress(reflashingIP),IPAddress(reflashingGateway),IPAddress(reflashingSubnet)))
    {
      Serial.println("AP Config Failed");
    }
    //WiFi.softAPConfig(IPAddress(reflashingIP),IPAddress(reflashingGateway),IPAddress(reflashingSubnet));
    
    

#ifdef  SERIAL_DEBUG
    Serial.println("");
    Serial.println("WiFi AP online ...");
    server.begin();
#endif

}

/**
 
 *	@brief	calculates the 16-bit CRC using modbus-RTU algorithm
 *
 *	@param  data - an array of bytes in modbus packet
 *	
 *	@return	CRC (16 bit)
 *
 */
uint16_t MRTU_CRC(uint8_t* data, ushort len)
{
  ushort res_CRC = 0xFFFF;
  ushort count = 0;
  uint8_t count_crc;
  uint8_t dt;
  while(count < len)
        {
                //
                count_crc = 0;
                dt = (uint8_t)(data[count]);
                res_CRC ^= (ushort)(dt);
                //
                while(count_crc < 8)
                {
                        if((res_CRC & 0x0001) < 1)
                        {
                                res_CRC = (res_CRC >> 1) & 0x7FFF;
                        }
                        else
                        {
                                res_CRC = (res_CRC >> 1) & 0x7FFF;
                                res_CRC ^= 0xA001;
                        };
                        count_crc++;
                };
                count++;
        }
        return (res_CRC);

}



/**
 
 *	@brief	Sends the reset command to a modbus device (before reflash).
 *  @brief  The reset command is in fact the command to set the coil#12 to "1".
 *  @brief  (See "Pins and registers match.xls" for reference)
 *
 *	@param  devAddr - a modbus device address
 *	
 *	@return	none
 *
 */
void resetModbusDevice(int8_t devAddr)
{
  uint8_t buf[16]={0};
  uint16_t  crc;

  buf[0]=devAddr; //device address
  buf[1]=0x05;    //05=command code to write coil
  buf[2]=0x00;  
  buf[3]=0x0C;
  buf[4]=0xFF;
  buf[5]=0x00;
  crc = MRTU_CRC(&buf[0],6);
  buf[6] = crc & 0xFF;
  buf[7]=(crc>>8) & 0xFF;

  //re- initialize an "RS485"  UART in order to change it's speed
  //changing speed is needed because the 19200 speed is being used for modbus communication
  //and the 115200 speed is being used for the bootloader mode communication
  RS485_port.end(); 
  delay(25);
  RS485_port.begin(19200);
  RS485_port.setTimeout(100);
  delay(25);

  digitalWrite(RE_PIN, HIGH);
  digitalWrite(DE_PIN, HIGH);
  delay(5);
  RS485_port.write(buf[0]);
  RS485_port.write(buf[1]);
  RS485_port.write(buf[2]);
  RS485_port.write(buf[3]);
  RS485_port.write(buf[4]);
  RS485_port.write(buf[5]);
  RS485_port.write(buf[6]);
  RS485_port.write(buf[7]);
  delay(50);  //without this delay all the 10 bytes will not be transmitted
  digitalWrite(RE_PIN, LOW);
  digitalWrite(DE_PIN, LOW);

    //re- initialize an "RS485"  UART in order to change it's speed
  //changing speed is needed because the 19200 speed is being used for modbus communication
  //and the 115200 speed is being used for the bootloader mode communication
  RS485_port.end(); 
  delay(25);
  RS485_port.begin(115200);
  RS485_port.setTimeout(100);
  delay(25);
#ifdef  SERIAL_DEBUG
  Serial.println("reset command transmitted chars:");
  for (size_t i = 0; i < 8; i++)
  {
    Serial.println(String(buf[i]));
  }
#endif
}

/**
 
 *	@brief	checks if the modbus device is present by trying to read it's coil#1
 *
 *	@param  devAddr - the modbus address
 *	
 *	@return	none
 *
 */
char scanForModbusDevices(int8_t devAddr){

  char recievedChars[6]={0x41,0x41,0x41,0x41,0x41,0x41};
  uint8_t buf[16]={0};
  uint16_t  crc;

  //re- initialize an "RS485"  UART in order to change it's speed
  //changing speed is needed because the 19200 speed is being used for modbus communication
  //and the 115200 speed is being used for the bootloader mode communication
  RS485_port.end(); 
  delay(25);
  RS485_port.begin(19200);
  RS485_port.setTimeout(100);
  delay(25);
  
  buf[0]=devAddr;
  buf[1]=0x01;
  buf[2]=0x00;
  buf[3]=0x00;
  buf[4]=0x00;
  buf[5]=0x01;
  crc = MRTU_CRC(&buf[0],6);
  buf[6] = crc & 0xFF;
  buf[7]=(crc>>8) & 0xFF;

#ifdef  SERIAL_DEBUG
  Serial.println("transmitted chars:");
  for (size_t i = 0; i < 8; i++)
  {
    Serial.println(String(buf[i]));
  }
#endif 


  digitalWrite(RE_PIN, HIGH);
  digitalWrite(DE_PIN, HIGH);
  delay(5);
  RS485_port.write(buf[0]);
  RS485_port.write(buf[1]);
  RS485_port.write(buf[2]);
  RS485_port.write(buf[3]);
  RS485_port.write(buf[4]);
  RS485_port.write(buf[5]);
  RS485_port.write(buf[6]);
  RS485_port.write(buf[7]);
  delay(5);//this delay is necessary to let all bytes being transmitted before RE and DE pins go low and cut transmit
   digitalWrite(RE_PIN, LOW);
  digitalWrite(DE_PIN, LOW);
  for (uint8_t i = 0; i < 5; i++)
  {
    RS485_port.readBytes(recievedChars,6);
  }
  #ifdef  SERIAL_DEBUG
  //Serial.println("recieved chars:");
  for (uint8_t i = 0; i < 6; i++)
  {
    //Serial.write(recievedChars[i]);
  }
  #endif
  if (recievedChars[1]==0x01)
  {
#ifdef  SERIAL_DEBUG
    Serial.println("response detected");
#endif
    return 0x01;
  }
  else
  {
    return 0x00;
  }

}


/**
 
 *	@brief	sends the HTML content whitch is a reflash page
 *
 *	@param  client - current wifi client connected
 *	
 *	@return	none
 *
 */
void printReflashForm(WiFiClient client){
#ifdef  SERIAL_DEBUG
  Serial.println("printReflashForm");
  Serial.println("Client IP is:");
  Serial.println(client.localIP());
#endif
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.println("<html>");
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: left;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 4px 10px;");
  client.println("text-decoration: none; font-size: 8px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #555555;}</style></head>");
  client.println("</style>");
  client.println("<body>");
  client.println();
  //client.println("<form action=\"upload\" method=\"post\" enctype=\"multipart/form-data\">");
  client.println("<form action=\"reflash\" method=\"post\" enctype=\"multipart/form-data\">");
  client.println("Modbus device address# "+ String(currentAddress) +" firmware update. Select the *.bin file to upload.");
  client.println("<br />");
  client.println("<br /> ");
  client.println("Modbus device firmware update. ");
  client.println();
  client.println("Select *.bin file to upload:");
  client.println("<input type=\"file\" name=\"fileToUpload\" id=\"fileToUpload\">");
  client.println("<input type=\"submit\" value=\"Upload file\" name=\"submit\">");
  //client.println("<p><a href=\"/\"><button class=\"button\">GO BACK</button></a></p>"); 
  client.println("</form>");
  client.println();
  client.println("</body>");
  client.println("</html>");
  client.println();
  
}

/**
 
 *	@brief	sends the HTML content whitch is an upload page
 *
 *	@param  client - current wifi client connected
 *	
 *	@return	none
 *
 */
void printUploadForm(WiFiClient client) {
  uint8_t currentDeviceAddress = 1;
  #ifdef  SERIAL_DEBUG  
  Serial.println("printUploadForm");
  #endif
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.println("<html>");
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: left;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 4px 10px;");
  client.println("text-decoration: none; font-size: 8px; margin: 2px; cursor: pointer;}");
  client.println(".button2 {background-color: #555555;}</style></head>");
  client.println("</style>");
  client.println("<body>");
  client.println();
  //client.println("<form action=\"upload\" method=\"post\" enctype=\"multipart/form-data\">");
  client.println("Modbus device firmware update. Wait while scanning for active modbus devices...");
  client.println("<br />");
  Serial.println("scanning for modbus devices connected....");
  currentDeviceAddress = 1;

  //re- initialize an "RS485"  UART in order to change it's speed
  //changing speed is needed because the 19200 speed is being used for modbus communication
  //and the 115200 speed is being used for the bootloader mode communication
  RS485_port.end(); 
  delay(25);
  RS485_port.begin(19200);
  RS485_port.setTimeout(100);

  


  delay(25);
  do
  {
    //delay(1000);
    if (scanForModbusDevices(currentDeviceAddress) == 0x01)

    {
      //once the device is detected so creating the button with a link to the device reflashing page
      //ex. if the device address is 3 so the button will hold a link to web address
      //like 192.168.4.2/3/reflash
      //if you enter address manually, for example 192.168.4.2/10/reflash
      //the program will generate the page to reflash the modbus device with address 10 even if it was not found
      client.println("<p>Device address # " + String(currentDeviceAddress) +" is ONLINE<br /> "); 
      client.println("<p><a href=\"/" + String(currentDeviceAddress) + "/reflash\"><button class=\"button\">REFLASH</button></a></p>"); 
    }
    else
    {
      //printing dots while searching for devices
      client.println(".");
      //client.println("<p>Device address " + String(currentDeviceAddress) +" not found<br /> ");  

      //client.println("<br />");
    }
   currentDeviceAddress++; 
   //delay(1000);
  //the addresses search range is linited to 5 addresses due to no need to do search all the 249
  //addresses as it takes a long time
  } while (currentDeviceAddress<5);//while (currentDeviceAddress<249);

  RS485_port.end(); 
  delay(50);
  RS485_port.begin(115200);//115200

  //client.println("<br /> ");
  //client.println("Modbus device firmware update. ");
  //client.println();
  //client.println("Select *.bin file to upload:");
  //client.println("<input type=\"file\" name=\"fileToUpload\" id=\"fileToUpload\">");
  //client.println("<input type=\"submit\" value=\"Upload file\" name=\"submit\">");

  client.println("</form>");
  client.println();
  client.println("</body>");
  client.println("</html>");
  
  client.println();
  
}


/**
 
 *	@brief	sends the HTML content whitch is an icon for the page
 *
 *	@param  client - current wifi client connected
 *	
 *	@return	none
 *
 */
void printFavicon(WiFiClient client) {
#ifdef  SERIAL_DEBUG
  Serial.println("printFavicon");
#endif
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.print(AP_DEMO_HTTP_200_IMAGE);
  client.print("png\r\n\r\n");
#ifdef IMAGE_DEBUG
  Serial.print("AP_DEMO_HTTP_200_PNG");
#endif
  for (int i=0;i<sizeof(favicon);i++) {
    client.write(favicon[i]);
#ifdef IMAGE_DEBUG
    Serial.write(favicon[i]);
#endif
  }
  
}

/**
 
 *	@brief	The reflash procedure.
 *  @brief  First, it issues a reset to the modbus device with the "currentDeviceAddress" @19200 bps
 *  @brief  Second, it sends the reflashing sequence @115200 bps
 *
 *	@param  client - current wifi client connected
 *	
 *	@return	sets the global variable "reflashingResult" to 0 if the bootloader reports an error or reports nothing
 *  @return sets the global variable "reflashingResult" to 1 if the bootloader reports a success for flashing procedure
 *
 */
void printImage(WiFiClient client) {
char recievedChars[4]={0x41,0x41,0x41,0x41};
char  charBuf = 0x41;
  #ifdef  SERIAL_DEBUG
  Serial.println("printImage");
  #endif
  delay(3000);
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  ////client.print(AP_DEMO_HTTP_200_IMAGE);
  ////client.print(imagetypebuf);
  ////client.print("\r\n\r\n");

#ifdef IMAGE_DEBUG
  Serial.print("AP_DEMO_HTTP_200_PNG");
#endif
  for (int i=0;i<imagepos;i++) {
    ////client.write(imagebuf[i]);
#ifdef IMAGE_DEBUG
    Serial.write(imagebuf[i]);
#endif
  }
  ////////ModbusCommunication
  resetModbusDevice(currentAddress);
  delay(50);
  ////////end ModbusCommunication


  //re- initialize an "RS485"  UART in order to change it's speed
  //changing speed is needed because the 19200 speed is being used for modbus communication
  //and the 115200 speed is being used for the bootloader mode communication
  RS485_port.end(); 
  delay(25);
  RS485_port.begin(115200);
  RS485_port.setTimeout(100);
  delay(25);


    ///erase command
#ifdef  SERIAL_DEBUG
  Serial.write(0xFF);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x00);
  Serial.write(0x80);
  Serial.write(0x00);
  Serial.write(0x80);
#endif

  digitalWrite(RE_PIN, HIGH);
  digitalWrite(DE_PIN, HIGH);
  delay(100);

  RS485_port.write(0xFF);
  RS485_port.write(0x00);
  RS485_port.write(0x00);
  RS485_port.write(0x00);
  RS485_port.write(0x80);
  RS485_port.write(0x00);
  RS485_port.write(0x80);
  //digitalWrite(RE_PIN, LOW);
  //digitalWrite(DE_PIN, LOW);
    ///end erase command

  delay(3000);//wait for an erase procedure on the STM32 side ends

    ///sending firmware
  packetBytesCounter=0;
  imageCounter=0;

  while (imageCounter < imagepos)
  {
    packetBytesCounter = 0;
    checksum = 0xFF;
    checksum ^= 0xFF;
    checksum ^= 0x01;
    RS485_port.write(0xFF);
    RS485_port.write(0x01);
    while (packetBytesCounter < 128)
    {
      if ( imageCounter < imagepos )
      {
        
        RS485_port.write(imagebuf[imageCounter]);
        checksum ^= imagebuf[imageCounter];
      }
      else
      {
      RS485_port.write(0x00);
      checksum ^= 0x00;
      }
    packetBytesCounter++;
    imageCounter++;  
    }
    RS485_port.write(checksum);
    delay(15);//a small delay to let the STM32 write the recieved packet to FLASH
    delay(15);
   
  }

  delay(100);
  //check if flashing was successfull////////////
  delay(2000);
  checksum = 0xFF;
  checksum ^= 0xFF;
  checksum ^= 0x04;
  RS485_port.write(0xFF);
  RS485_port.write(0x04);
  RS485_port.write(0x04);
    //RS485_port.write(checksum);
  delay(5);
  digitalWrite(RE_PIN, LOW);
  digitalWrite(DE_PIN, LOW);

  //delay(3000);
  reflashingResult = 0;
  for (uint16_t i = 0; i < 32; i++)
  {
    RS485_port.readBytes(recievedChars,4);
    if ((recievedChars[0] == 0x00) && (recievedChars[1] == 0x01))
    {
      reflashingResult = 1;
#ifdef SERIAL_DEBUG
      Serial.println("");
      Serial.write(recievedChars[0]);
      Serial.write(recievedChars[1]);
#endif
    }
    
  }
  

  //end check if flashing was successfull////////////

  digitalWrite(RE_PIN, LOW);
  digitalWrite(DE_PIN, LOW);
  
    ///end sending firmware
  delay(3000);//wait for the STM32 bootloader resets and goes to main (modbus) program
  
}


/**
 
 *	@brief	sends the HTML content whitch is an wait while reflashing message
 *
 *	@param  client - current wifi client connected
 *	
 *	@return	none
 *
 */
void printWait(WiFiClient client) {
#ifdef  SERIAL_DEBUG
  Serial.println("printWait");
#endif
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.println("<html>");
  client.println("<body>");
  client.println();
  client.println("The uploading process is in progress....");
  
  client.println();
  //client.println("</body>");
  //client.println("</html>");

  
  // The HTTP response ends with another blank line:
  client.println();
  
}


/**
 
 *	@brief	sends the HTML content whitch is a message about reflashing have ended
 *
 *	@param  client - current wifi client connected
 *	
 *	@return	none
 *
 */
void printThanks(WiFiClient client) {
#ifdef  SERIAL_DEBUG
  Serial.println("printThanks");
#endif
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  //client.println("HTTP/1.1 200 OK");
  //client.println("Content-type:text/html");
  //client.println();
  //client.println("<html>");
  //client.println("<body>");
  client.println();
  if (reflashingResult == 1)
  {
    client.println("The uploading process is finished successfully");
  }
  else
  {
    client.println("An error occured while reflashing");
  }
  
  client.println("<p><a href=\"/\"><button class=\"button\">GO BACK</button></a></p>"); 
  //client.println("<a id=\"logo\" href=\"/\"><img src=\"logo.bmp\" alt=\"logo\" border=\"0\"></a>");
  client.println();
  client.println("</body>");
  client.println("</html>");
  // the content of the HTTP response follows the header:
  //client.print("Click <a href=\"/H\">here</a> turn the LED on pin 5 on<br>");
  //client.print("Click <a href=\"/L\">here</a> turn the LED on pin 5 off<br>");
  
  // The HTTP response ends with another blank line:
  client.println();
  
}








void loop(){
  
  String  header = "";
  String  strbuf = "";
  int     cnt;
  bool    newconn=false;
  int     stat;
  

  WiFiClient client = server.available();   // listen for incoming clients
  client.localIP();

  if (client) {                             // if you get a client,
    stat=0;
    boundpos=0;
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";
    String currentLine1 = "";               // make a String to hold incoming data from the client

    while (client.connected()) {            // loop while the client's connected
      cnt = client.available();
      if (cnt) {                            // if there're bytes to read from the client,
#ifdef IMAGE_DEBUG
          if (newconn==false) {
            Serial.println(cnt);
            newconn=true;
          }
#endif
          char c = client.read();             // read a byte, then
          header+=c;
          currentLine1+=c;
#ifdef  SERIAL_DEBUG
          Serial.write("_");
          Serial.write(c);
#endif
                    
          if (stat==UPL_AP_STAT_POST_GET_IMAGE) {
            if (imagepos<MAX_IMAGE_SIZE) {
              imagebuf[imagepos]=c;
              imagepos++;
            }
          }

          strbuf = "/reflash";//this is a sign of we're on the reflash page

          if (currentLine1.endsWith(strbuf) ){
          for (int16_t i = 249; i > 0; i--) //this cycle in in down direction in order to avoid triggering
                                            //on for ex. address 2 if the actual address is 212, and so on...
          {
            strbuf ="GET /"+ String(i)+"/reflash";
            if (currentLine1.endsWith(strbuf) )
            
            {
              stat = UPL_AP_STAT_MAIN;      //switch to initial state to get ready for FW data to be uploaded
#ifdef  SERIAL_DEBUG
              Serial.write("stat = UPL_AP_STAT_MAIN");
#endif
              isInReflashForm = 1;          //a flag that we're on reflash page
#ifdef  SERIAL_DEBUG
              for (uint16_t i = 0; i < 300; i++)
              {
                 if (isInReflashForm == 0){
                   Serial.write("+");
                 }
                 if (isInReflashForm == 1){
                   Serial.write("!");
                 }
                
              }
#endif
              currentLine1=""; 
              currentAddress = (int8_t)i;
              break;
            }
          }}

          if (c == '\n') {  // if the byte is a newline character
            
            currentLine1="";                   
#ifdef IMAGE_DEBUG
            Serial.print("stat is equal=");
            Serial.println(stat);
#endif
            if (stat==UPL_AP_STAT_POST_START_BOUNDRY) {
              boundbuf[boundpos]='\0';
              boundpos++;
#ifdef IMAGE_DEBUG
              Serial.println("&&&&&&&&&&&&&&&&&");
              Serial.println(boundbuf);
              Serial.println("&&&&&&&&&&&&&&&&&");
#endif
              stat=UPL_AP_STAT_POST_UPLOAD;
              Serial.println("stats=UPL_AP_STAT_POST_UPLOAD");
            }
            if (stat==UPL_AP_STAT_POST_START_IMAGE && currentLine.length() == 0) {
              imagetypebuf[imagetypepos]='\0';
              imagetypepos++;
#ifdef IMAGE_DEBUG
              Serial.println("&&&&&&&&&&&&&&&&&");
              Serial.println(imagetypebuf);
              Serial.println("&&&&&&&&&&&&&&&&&");
#endif
              imagepos=0;
              stat=UPL_AP_STAT_POST_GET_IMAGE;
              Serial.println("stats=UPL_AP_STAT_POST_GET_IMAGE");
            }
              // if you got a newline, then clear currentLine:
            currentLine = "";
            newconn=false;
          } else if (c != '\r') {  // if you got anything else but a carriage return character,
            currentLine += c;      // add it to the end of the currentLine
            if (stat==UPL_AP_STAT_POST_START_BOUNDRY) {
              if (boundpos<MAX_BUF_SIZE) {
                boundbuf[boundpos]=c;
                boundpos++;
              }
            }
            if (stat==UPL_AP_STAT_POST_START_IMAGE) {
              if (imagetypepos<MAX_BUF_SIZE) {
                imagetypebuf[imagetypepos]=c;
                imagetypepos++;
              }
            }
          }

          

          

          // Check to see if the client request was "GET / " whitch means the request for the main page
          if (currentLine.endsWith("GET / ")) {
             stat=UPL_AP_STAT_MAIN;
             isInReflashForm = 0;
#ifdef  SERIAL_DEBUG
             Serial.println("stats=UPL_AP_STAT_MAIN");
#endif
         }
            if (currentLine.endsWith("GET /logo.bmp ")) {
            stat=UPL_AP_STAT_GET_IMAGE;
#ifdef  SERIAL_DEBUG
            Serial.println("stats=UPL_AP_STAT_GET_IMAGE");
#endif
         }
          if (currentLine.endsWith("GET /favicon.ico ")) {
            stat=UPL_AP_STAT_GET_FAVICON;
#ifdef  SERIAL_DEBUG
            Serial.println("stats=UPL_AP_STAT_GET_FAVICON");
#endif
         }
          strbuf="POST /"+ String(currentAddress)+"/reflash ";
          //if (currentLine.endsWith("POST /upload ")) {
          if (currentLine.endsWith(strbuf)) {
            stat=UPL_AP_STAT_POST_UPLOAD;
#ifdef  SERIAL_DEBUG
            Serial.println("stats=UPL_AP_STAT_POST_UPLOAD");
#endif
          }
         if (stat==UPL_AP_STAT_POST_UPLOAD && currentLine.endsWith("Content-Type: multipart/form-data; boundary=")) {
            stat=UPL_AP_STAT_POST_START_BOUNDRY;
#ifdef  SERIAL_DEBUG
           Serial.println("stats=UPL_AP_STAT_POST_START_BOUNDRY");
#endif
         }
         //if (stat==UPL_AP_STAT_POST_UPLOAD && currentLine.endsWith("Content-Type: image/")) {
         if (stat==UPL_AP_STAT_POST_UPLOAD && currentLine.endsWith("Content-Type: application/octet-stream")) {
            stat=UPL_AP_STAT_POST_START_IMAGE;
#ifdef  SERIAL_DEBUG
           Serial.println("stats=UPL_AP_STAT_POST_START_IMAGE");
#endif
          }
         if (stat==UPL_AP_STAT_POST_UPLOAD && boundpos>0 && currentLine.endsWith(boundbuf)) {
#ifdef  SERIAL_DEBUG
            Serial.println("found boundry");
#endif
          }
         if (stat==UPL_AP_STAT_POST_GET_IMAGE && boundpos>0 && currentLine.endsWith(boundbuf)) {
#ifdef  SERIAL_DEBUG
            Serial.println("found image boundry");
            Serial.println(imagepos);
#endif
            stat=UPL_AP_STAT_POST_UPLOAD;
            imagepos=imagepos-boundpos-3;
#ifdef IMAGE_DEBUG
            Serial.println(imagepos);
            for (int i=0;i<imagepos;i++) {
              Serial.write(imagebuf[i]);
            }
#endif
#ifdef  SERIAL_DEBUG
             Serial.println("stats=UPL_AP_STAT_POST_UPLOAD");
#endif
          }
      } else {
            if ((stat==UPL_AP_STAT_MAIN) && (isInReflashForm == 0)) {

              printUploadForm(client);
              break;
            }
            if ((stat==UPL_AP_STAT_MAIN) && (isInReflashForm == 1)) {

              printReflashForm(client);
              break;
            }
            if (stat==UPL_AP_STAT_POST_UPLOAD) {
              printWait(client);
              printImage(client);
              printThanks(client);
              
              isInReflashForm = 0;
              break;
            }
            if (stat==UPL_AP_STAT_GET_IMAGE) {
              printImage(client);

              break;
            }
            if (stat==UPL_AP_STAT_GET_FAVICON) {
              printFavicon(client);
              break;
            }
            
            Serial.println("stat unknown");
            delay(1000);
            break;
      }
    }
    header = "";
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}