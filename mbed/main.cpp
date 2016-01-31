#include "mbed.h"
#include "EthernetInterface.h"
#include "Websocket.h"
#include "string"
#include "M2XStreamClient.h"
#include "FXAS21002.h"
#include "FXOS8700.h"  


DigitalOut R(PTB22);                          
DigitalOut G(PTE26); 
DigitalOut B(PTB21);  
DigitalIn button(A1);
PwmOut IN1(PTC2); // Pin IN1 input to MC34931 (FRDM PIN Name)
PwmOut IN2 (PTA2); // Pin IN2 input to MC34931 (FRDM PIN Name)
DigitalOut EN(PTE25); // Pin EN input to MC34931 (FRDM PIN Name)
DigitalOut DIS(PTC17); //  Pin D1 input to MC34931 (FRDM PIN Name)
//DigitalIn sw(PTA4);

// Initialize Serial port
Serial pc(USBTX, USBRX);

// Initialize pins for I2C communication for sensors. Set jumpers J6,J7 in FRDM-STBC-AGM01 board accordingly.
FXOS8700 accel(D14,D15);
FXOS8700 mag(D14,D15);
FXAS21002 gyro(D14,D15);

//Variables
int pwm_freq_lo;
int pwm_freq_hi;
int frequencyHz = 2500;
int runstop = 0;
int direction = 1;
int braking;
int dutycycle = 75;
int newDataFlag = 0;
int status = 0;
uint16_t CurrFB; 
uint16_t CFBArray[101];
uint32_t CFBTotal;
uint16_t CFBAvg;
uint16_t CFBtemp;

float accel_data[3]; float accel_rms=0.0;
    float previous=0.0;
    int counter = 0;
    int changesCounted = 0;

char feedId[] = "c62bdcd7c15260520c6ce2b39c171971"; // Feed you want to post to
char m2xKey[] = "cc15e6803ede831fbe02a77050d0b107"; // Your M2X access key
char streamName[100] = "ButtonPushed"; // Stream you want to post to

Client client;
M2XStreamClient m2xClient(&client, m2xKey);

int main() {
    //int res;
    //char buf[100];
    //char msg[100];
    
    bool pb = 1;
    
    R = 1;
    G = 1;
    B = 0;
    
    printf("\nSTART!!!\n\r");
    EthernetInterface eth;
    eth.init();
    printf("Ethernet init is GO\n\r");
    eth.connect();
    printf("IP Address is %s\n\r", eth.getIPAddress());
    
    accel.accel_config();
    
    //wait(1.0);
    
    // Change YOURLOGIN for your mbed login so you can communicate to the board
    // using the mbed websocket at:
    // http://sockets.mbed.org/YOURLOGIN
    
    
    //char connectionString[1000] = "ws://echo.websocket.org";
    
    //Websocket ws("ws://kentative.azurewebsites.net/WebServices/chatserver");
    //Websocket ws("ws://echo.websocket.org");
    Websocket ws("ws://52.34.251.80:9006/");
    
    //REMEMBER! NEED TO USE IP and / at the end for AWS server
    
    ws.connect();
    
    //printf(connectionString);
    printf("\n\n");
    
    char recv_data[100] = "emptyRec";
    char send_data[100] = "emptySend";
    int stillLocked = 1;
    
    char m2xMsg[100] = "noInfo";
    
    while (pb) {
        printf("\nIs Connected: %i\n\r", ws.is_connected());
            
        ws.send(send_data); 
        printf("\nData sent is: %s\n\r", send_data);
        wait(1);
        ws.read(recv_data);    
        printf("\nData received is: %s\n\r", recv_data);
        
        if (strcmp(recv_data,"unlocked") == 0){
            printf("\nLIGHT Greene: Unlocked...\n\r");
            if (stillLocked == 1){
                
                stillLocked = 0;
                
                printf("\r\n!!!!!!!!!!!!!!!!!!!!!UNLOCKING!!!!!!!!!!!!!!!!!!!!!\n\r\n\r");
                
                IN1.period(1/(float)frequencyHz);    
                IN1 = (float)dutycycle/100.0;
                IN2 = 1;
                EN = 1;
                DIS = 0;  
             
                wait(1);
             
                IN1.period(0);
                IN2.period(0);                 
                IN1.write(0);
                IN2.write(0);
                EN = 0;
                DIS = 0; 
            }
                
            R = 1;
            G = 0;
            B = 1;
        }
            
        if (strcmp(recv_data,"locked") == 0){
            stillLocked = 1;
            printf("\nLIGHT Red: LOCKED\n\r");
            
            R = 0;
            G = 1;
            B = 1;
        }
        
        if(button){
            printf("Door Sensor Triggered!\n\r");
            
            strcpy(m2xMsg,"doorTamperTrigger");
            strcpy(streamName,"OpenTamper");
            
            int response = m2xClient.updateStreamValue(feedId, streamName, m2xMsg);
            printf("Post response code: %d\r\n", response);
            if (response == -1) while (true);
        }      
        
        
        for(int i = 0; i < 6; i = i +1){
            accel.acquire_accel_data_g(accel_data);
            printf("Change :%4.2f\n\r",accel_data[0] - previous);
            if (((accel_data[0] - previous) > 0.5) || ((accel_data[0] - previous) < -0.5)){
                changesCounted = changesCounted + 1;
                printf("Changes Detected :%i\n\r", changesCounted);
                if (changesCounted % 5 == 0){
                    printf("Door Sensor Triggered!\n\r");
            
                    strcpy(m2xMsg,"PositionTamperTrigger");
                    strcpy(streamName,"PositionTamper");
            
                    int response = m2xClient.updateStreamValue(feedId, streamName, m2xMsg);
                    printf("Post response code: %d\r\n", response);
                    if (response == -1) while (true);
                }
            }
            previous = accel_data[0];
            wait(1);
       }
            
       wait(3);
    }
    ws.close();
    eth.disconnect();
}
