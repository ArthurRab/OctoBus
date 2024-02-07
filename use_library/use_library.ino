
#include "NintendoTeensy/src/Nintendo.h" 

#define max_num_controllers 7
#define controller_starting_pin 2

CGamecubeController gamecube_controllers[max_num_controllers] = 
{
  (2),
  (5),
  (8),
  (11),
  (14),
  (17),
  (20)
};

//CGamecubeController smfh(3);
Gamecube_Report_t zero_controller = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
CGamecubeConsole console(22);


#define button_bytes 2
#define continuum_bytes 6

int buttons[button_bytes][8];
int continuums[continuum_bytes];

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
  Serial.begin(19200);
  Serial.println("===== START =====");
  // Set up debug led
  
  

  //read(c1);
  //read(c2);
  
}


int num_connected = 0;
void read(CGamecubeController &c){
    //Serial.println(controller);
    //Serial.println(gamecube_controllers[controller].connected());
    if(c.read()){
      num_connected += 1;
      
      auto report = c.getReport();
      auto bytes = report.raw8;
      for (int byte = 0; byte < button_bytes; byte++){
        for (int bit = 0; bit < 8; bit++){
          buttons[byte][bit] += ((bytes[byte]>>(7-bit))%2);
        }
      }
      for (int continuum = 0; continuum < continuum_bytes; continuum++){
        continuums[continuum] += bytes[continuum+2];
      }
      
    }else{
      //any_reads_failed += 5;
    }
}

unsigned long last_c2_read = 0;



void loop()
{
  //delayMicroseconds(3500);
  
  for (int byte = 0; byte < button_bytes; byte++){
    for (int bit = 0; bit < 8; bit++){
      buttons[byte][bit] = 0;
    }
  }
  
  for (int continuum = 0; continuum < continuum_bytes; continuum++){
    continuums[continuum] = 0;
  }

  num_connected = 0;

  for (int controller = 0; controller < max_num_controllers; controller++){
    read(gamecube_controllers[controller]);
  }
  
  Serial.printf("Num connected %d\n", num_connected);

  auto virtual_controller = zero_controller; 
  for (int byte = 0; byte < button_bytes; byte++){
    for (int bit = 0; bit < 8; bit++){
      if (buttons[byte][bit]>(num_connected/2)){
        virtual_controller.raw8[byte]+=(1 << (7-bit));
      }
    }
  }

  for (int continuum = 0; continuum < continuum_bytes; continuum++){
    virtual_controller.raw8[continuum+2] = continuums[continuum]/num_connected;
  }
  
  //auto report = gamecube_controllers[0];
  if (num_connected > 0){
    if (!console.write(virtual_controller))
      {
        //Serial.println(millis());
        digitalWrite(LED_BUILTIN, HIGH);
        //delay(1000);
      }else{
        digitalWrite(LED_BUILTIN, LOW);
      }
  }else{
    Serial.println("SKIP");
  }
  
  
}