int transport_sendPacketBuffer(char, unsigned char*, int );
int transport_getdata(char, unsigned char*, int);
char transport_open(char * remoteIP, int remotePort, int localport);
void transport_close(char);
