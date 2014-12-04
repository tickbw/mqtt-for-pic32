sbit Net_Wireless_MCW1001_Chip_RST           at LATC1_bit;
sbit Net_Wireless_MCW1001_Chip_RST_Direction at TRISC1_bit;

char channels[11]    = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
char strSSID[15]     = "yourSSID";
char wpaPassword[11] = "yourPASSWORD";

unsigned char   myMacAddr[6] = {0x22, 0x33, 0x44, 0x55, 0x66, 0x88};    // my MAC address
unsigned char   myIpAddr[4]  = {192, 168,  1, 8 } ;                   // my IP address
unsigned char   gwIpAddr[4]  = {192, 168,  1, 1 } ;                   // gateway (router) IP address
unsigned char   ipMask[4]    = {255, 255, 255,  0 } ;                   // network mask (for example : 255.255.255.0)
char wifistatus;

////////////////////////////////////////////////////////////////////////////////
// Initialization of WiFi module

void InitWiFi() {
 int response;

  Net_Wireless_MCW1001_TimeToWait = 10;
  response = 1;
  Net_Wireless_MCW1001_SocketAllocate(1, 1, 0, 0, 1024, 1024);
  while(response != 0)
    response = Net_Wireless_MCW1001_SetMode(_NET_WIRELESS_MCW1001_CP_1, _NET_WIRELESS_MCW1001_MODE_INFRASTRUCTURE); // Set "Connection Profile 1" mode (Ad-Hoc mode)

  Net_Wireless_MCW1001_TimeToWait = 5;
  Net_Wireless_MCW1001_SetChannelList(11, channels);
  Net_Wireless_MCW1001_SetSecurity_WPA(_NET_WIRELESS_MCW1001_CP_1, _NET_WIRELESS_MCW1001_WPA_PSK2, wpaPassword);    // Set WPA_PSK2 security
  Net_Wireless_MCW1001_SetSSID(_NET_WIRELESS_MCW1001_CP_1, strSSID);    // Set SSID of network

  // Set network parameters
  Net_Wireless_MCW1001_SetNetworkMask(ipMask);
  Net_Wireless_MCW1001_SetGatewayIP(gwIpAddr);
  Net_Wireless_MCW1001_SetMAC(myMacAddr);
  Net_Wireless_MCW1001_SetIP(myIPAddr);

  response = Net_Wireless_MCW1001_SetArpTime(1);                                   // Set gratuitous ARP timing
  response = Net_Wireless_MCW1001_SetRetryCount(5, 255);                             // Set number of retries for the connection
  //response = Net_Wireless_MCW1001_SocketAllocate(1, 1, 1024, 1024, 1024, 1024);          // Allocate byffers for sockets
}

void SearchSSID() {
char markFound, numOfScanRes, response, i;
char strTmp[33];

  Delay_ms(2000);
  markFound = 0;
  while(markFound != 1) {
    while(1) {
      response = Net_Wireless_MCW1001_ScanStart(255, &numOfScanRes);                           // Scan network
      if(response == 0)                                                                        // Check if scanning successful
        break;
      Delay_ms(500);
    }
    // Check whether is there our ("mikroe_net") network
    for(i = 1; i <= numOfScanRes; i++) {
      response = Net_Wireless_MCW1001_GetScanResult(i);
      strncpy(strTmp, Net_Wireless_MCW1001_ScanResult.SSID, Net_Wireless_MCW1001_ScanResult.SSIDLength);

      if(Net_Wireless_MCW1001_ScanResult.SSIDLength > 13) {
        strTmp[14] = '\0';
        strTmp[13] = '.';
        strTmp[12] = '.';
        strTmp[11] = '.';
      }
      else
        strTmp[Net_Wireless_MCW1001_ScanResult.SSIDLength] = '\0';

      if(strncmp(Net_Wireless_MCW1001_ScanResult.SSID, strSSID, Net_Wireless_MCW1001_ScanResult.SSIDLength) == 0) {
        markFound = 1;
        Delay_ms(700);
        break;
      }
      Delay_ms(700);
    }
  }
}

void ConnectToAp() {

  // Connect to AP..............................................................
  Net_Wireless_MCW1001_TimeToWait = 1;
  Net_Wireless_MCW1001_Connect(_NET_WIRELESS_MCW1001_CP_1, &wifiStatus);
  // ASCII WPA-2 pass phrase will be provided in security Key,
  // and while connecting, the MRF24WB0M will calculate the
  // PSK key (which can take up to 30 seconds).
  Net_Wireless_MCW1001_Properties.networkStatus = _NET_WIRELESS_MCW1001_STATUS_NOTCONN_STATIC_IP;
  while(1) {
    Net_Wireless_MCW1001_GetNetworkStatus();
    if(Net_Wireless_MCW1001_Properties.networkStatus == _NET_WIRELESS_MCW1001_STATUS_CONN_STATIC_IP)
      break;
  }
  Net_Wireless_MCW1001_TimeToWait = 10;
  Sound_Play(800, 40);            // Play sound at 800Hz for 40ms
  Delay_ms(50);
  Sound_Play(800, 40);            // Play sound at 800Hz for 40ms
}

char wificonnect() {
  //InitMcu();
  UART2_Init(115200);
  Delay_ms(1000);
  InitWiFi();
  SearchSSID();
  ConnectToAp();
  return(1);
}
