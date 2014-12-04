/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *    Luca Massarelli (associated with Nergal Consulting S.R.L. - porting the library in mikroC
 *******************************************************************************/

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTSNPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/


//unsigned int remotePort = 1883, localPort = 80;
//unsigned char   remoteIP[4]  = {192, 168,  178, 84 } ;



int Socket_error(char* aString, int sock)
{

}


int transport_sendPacketBuffer(char socketHandle, unsigned char* buf, int buflen){
    int sendBytes;
    Net_Wireless_MCW1001_TCP_SendBytes(socketHandle, buf, buflen, &sendBytes);
    return(sendBytes);
}


int transport_getdata(char socketHandle, unsigned char* buf, int amountOfDataToBeRead){
    int receiveBytes;
    Net_Wireless_MCW1001_TCP_ReadBytes(&socketHandle, amountOfDataToBeRead, buf, &receiveBytes);
    return(receiveBytes);
}


char transport_open(char * remoteIP, int remotePort, int localport) {
    char socketHandle;
    char bindResponse;
    char connectResponse;
    char numOfSendBytes = 1;
    char a[5] = {'s','t','a','r','t'};
    char read[50] = {0};
    int result = 0;
    int res = 0;
    int amountOfDataToBeRead = 10;
    int numOfReceiveBytes = 0;
    
    Net_Wireless_MCW1001_TimeToWait = 30;
    
    Net_Wireless_MCW1001_SocketClose(socketHandle);
    res = Net_Wireless_MCW1001_SocketCreate(&socketHandle, _NET_WIRELESS_MCW1001_SOCKET_TYPE_TCP);   // Create TCP socket
    res = Net_Wireless_MCW1001_SocketBind(socketHandle, &localPort, &bindResponse);                  // Bind socket to the listen port
    
    do {
       result = Net_Wireless_MCW1001_TCP_Connect(socketHandle, remotePort, remoteIP, &connectResponse);
       delay_ms(200);
    } while(connectResponse != 0);
    //Net_Wireless_MCW1001_TCP_SendBytes(socketHandle, "hello", 5, &numOfSendBytes);
    return(socketHandle);
    
}

void transport_close(char socketHandle) {
    Net_Wireless_MCW1001_SocketClose(socketHandle);
}
