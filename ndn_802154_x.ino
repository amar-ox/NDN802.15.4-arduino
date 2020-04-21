
 /*
  * 
  *  Cow sensor with my NDN-802.15.4 and ndn-cpp Lite library
  *  Attached to an accelerometer 
  *  For Arduino DUE
  *  
  */

#ifdef ARDUINO

#define greenLed 10
#define blueLed 11
#define redLed 12

#include "node.h"    /* NDN-802.15.4 library */
#include "MPU9250.h" /* Acceleromoter library */
#include "ndn-cpp/lite/data-lite.hpp"
#include "ndn-cpp/lite/interest-lite.hpp"
#include "ndn-cpp/lite/util/crypto-lite.hpp"
#include "ndn-cpp/lite/encoding/tlv-0_2-wire-format-lite.hpp"

MPU9250 myIMU;

using namespace ndn;

/* NDN producer node */
Node node = Node();

/* signature key */
const uint8_t keyBytes[] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

/* app prefix if producer */
struct ndn_NameComponent prefixComponents[3];
NameLite  prefix( prefixComponents, sizeof(prefixComponents) / sizeof(prefixComponents[0]) );

/* movement detection algorithm variables */
/* ... */

/* onInterest callback for producer */
void onInterest(const InterestLite& interest) 
{   
  digitalWrite(greenLed, HIGH);  
  ndn_Error error;
  uint64_t cowID = 0;
  if ( (error = interest.getName().get(2).toSegment(cowID)) )
  {
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, HIGH);
    return;
  }
  /* some control on Interest name */
  /* ... */
    
  /* If OK: Create and send Data */
  ndn_NameComponent dataNameComponents[3];
  DataLite data(dataNameComponents, sizeof(dataNameComponents) / sizeof(dataNameComponents[0]), 0, 0);
  data.setName(interest.getName());
  data.setContent(BlobLite((const uint8_t*) contentBuffer, strlen(contentBuffer)));
  data.getSignature().setType(ndn_SignatureType_HmacWithSha256Signature);

  /* First encoding */
  uint8_t encoding[100];
  DynamicUInt8ArrayLite output(encoding , sizeof(encoding), 0);
  size_t encodingLength, dSignedPortionBeginOffset, dSignedPortionEndOffset;
  if ( (error = Tlv0_2WireFormatLite::encodeData(data, &dSignedPortionBeginOffset, &dSignedPortionEndOffset, output, &encodingLength))){
    blink(redLed,3);
    return;
  }

  /* Data signature */
  uint8_t signatureValue[ndn_SHA256_DIGEST_SIZE];
  CryptoLite::computeHmacWithSha256(keyBytes, sizeof(keyBytes), encoding + signedPortionBeginOffset,
     signedPortionEndOffset - signedPortionBeginOffset, signatureValue);
  data.getSignature().setSignature(BlobLite(signatureValue, ndn_SHA256_DIGEST_SIZE));
  
  /* Encode again to include the signature.*/
  if ((error = Tlv0_2WireFormatLite::encodeData(data, &signedPortionBeginOffset, &signedPortionEndOffset, output, &encodingLength))){
    blink(redLed,3);
    return;
  }

  /* Compress and send Data */
  node.compressAndPutData(data);
  digitalWrite(greenLed, LOW);
}
 
void setup() {
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  blink(blueLed,3);
  Serial.begin(57600);
  node.begin(Serial);  /* init connection to 802.15.4 radio */
  
  prefix.append("2");
  prefix.append("mvmnt");
  node.setPrefix(prefix, &onInterest); /* set producer prefix */
  delay(500);

  /* Acceleromoter init and calibration */
  /* ... */
  
  randomSeed(analogRead(0));
  blink(blueLed,3);
}

void loop() 
{
  node.processEvents();
    
  /* read and rocess acceleromoter data */
  /* ... */
  
} //loop

void blink(byte led, byte times){
  for(int i=0; i < times; i++){
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(50);
  }
}

#endif // ARDUINO

// uint8_t data_[] = {6,45,7,10,8,1,49,8,5,109,118,109,110,116,20,0,21,20,48,49,50,51,52,53,54,55,56,57,48,49,50,51,52,53,54,55,56,57,22,5,27,1,4,28,0,23,0};

/*encoding[1] = encoding[1] - 13;
  encoding[3] = encoding[3] - 13;
  for(uint8_t i = 4 ; i < encodingLength-13 ; i++){
    encoding[i] = encoding[i+13];
  }*/
  /*
   * data.getName().append("1");
  data.getName().append("mvmnt");
  data.getName().append(interest.getName().get(2));
  */

  /*ndn_NameComponent dataNameComponents[3];
  DataLite data(dataNameComponents, sizeof(dataNameComponents) / sizeof(dataNameComponents[0]), 0, 0);
  data.getName().append(interest.getName());
  data.setContent(BlobLite((const uint8_t*) contentBuffer, strlen(contentBuffer)));
  data.getSignature().setType(ndn_SignatureType_HmacWithSha256Signature);*/  

  