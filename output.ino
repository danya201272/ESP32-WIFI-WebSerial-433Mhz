static const char* bin2tristate(const char* bin);
static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength);

void output(unsigned long decimal, unsigned int length, unsigned int delay, unsigned int* raw, unsigned int protocol) {

  const char* b = dec2binWzerofill(decimal, length);
  WebSerial.print("Decimal: ");
  WebSerial.print(decimal);
  WebSerial.print(" (");
  WebSerial.print( length );
  WebSerial.print("Bit) Binary: ");
  WebSerial.print( b );
  WebSerial.print(" Tri-State: ");
  WebSerial.print( bin2tristate( b) );
  WebSerial.print(" PulseLength: ");
  WebSerial.print(delay);
  WebSerial.print(" microseconds");
  WebSerial.print(" Protocol: ");
  WebSerial.println(protocol);
  
  WebSerial.print("Raw data: ");
  for (unsigned int i=0; i<= length*2; i++) {
    WebSerial.print(raw[i]);
    WebSerial.print(",");
  }
  WebSerial.println();
  WebSerial.println();
}

static const char* bin2tristate(const char* bin) {
  static char returnValue[50];
  int pos = 0;
  int pos2 = 0;
  while (bin[pos]!='\0' && bin[pos+1]!='\0') {
    if (bin[pos]=='0' && bin[pos+1]=='0') {
      returnValue[pos2] = '0';
    } else if (bin[pos]=='1' && bin[pos+1]=='1') {
      returnValue[pos2] = '1';
    } else if (bin[pos]=='0' && bin[pos+1]=='1') {
      returnValue[pos2] = 'F';
    } else {
      return "not applicable";
    }
    pos = pos+2;
    pos2++;
  }
  returnValue[pos2] = '\0';
  return returnValue;
}

static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength) {
  static char bin[64]; 
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    } else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';
  
  return bin;
}
