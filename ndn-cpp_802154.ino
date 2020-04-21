/*
 *
 * Basic NDN-802.15.4 sketch 
 * Interact directly with IEEE 802.15.4 radio
 *
 */

#ifdef ARDUINO

#define greenLed 10
#define blueLed 11
#define redLed 12

#include "ndn-802154.h"

#include "ndn-cpp/lite/data-lite.hpp"
#include "ndn-cpp/lite/interest-lite.hpp"
#include "ndn-cpp/lite/util/crypto-lite.hpp"
#include "ndn-cpp/lite/encoding/tlv-0_2-wire-format-lite.hpp"

using namespace ndn;

IEEE802154 radio_ = IEEE802154();

struct ndn_NameComponent dataNameComponents[4];
DataLite data(dataNameComponents, sizeof(dataNameComponents) / sizeof(dataNameComponents[0]), 0, 0);

  
uint8_t encoding[100];
DynamicUInt8ArrayLite output(encoding, sizeof(encoding), 0);
size_t encodingLength;

const char contentBuffer[] = "01234567890123456789";

const uint8_t keyBytes[] = {
       0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
      16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    };

bool prod = true;

void blink(byte led, byte times){
  for(int i=0; i < times; i++){
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(50);
  }
}

void handleInterest(){ 
  if(radio_.getResponse().getData()[0] != 5){
    blink(redLed,1);
    return;
  }
  
  ndn_NameComponent interestNameComponents[3];
  InterestLite interest(interestNameComponents, sizeof(interestNameComponents) / sizeof(interestNameComponents[0]), 0, 0, 0, 0);
  size_t signedPortionBeginOffset, signedPortionEndOffset;
  ndn_Error error;
  if ((error = Tlv0_2WireFormatLite::decodeInterest(interest, radio_.getResponse().getData(), radio_.getResponse().getDataLength(), &signedPortionBeginOffset, &signedPortionEndOffset))){
    blink(redLed,3);
    return;
  }
  
  data.setName(interest.getName());

  ndn_Error dError;
  size_t dSignedPortionBeginOffset, dSignedPortionEndOffset;
  if ((dError = Tlv0_2WireFormatLite::encodeData(data, &dSignedPortionBeginOffset, &dSignedPortionEndOffset, output, &encodingLength))){    
    digitalWrite(redLed,HIGH);
    return;
  }

  uint8_t signatureValue[ndn_SHA256_DIGEST_SIZE];
  CryptoLite::computeHmacWithSha256(keyBytes, sizeof(keyBytes), encoding + signedPortionBeginOffset,
     signedPortionEndOffset - signedPortionBeginOffset, signatureValue);
  data.getSignature().setSignature(BlobLite(signatureValue, ndn_SHA256_DIGEST_SIZE));
  
  // Encode again to include the signature.
  if ((error = Tlv0_2WireFormatLite::encodeData(data, &signedPortionBeginOffset, &signedPortionEndOffset, output, &encodingLength))){
    digitalWrite(redLed,HIGH);
    return;
  }
  
  if (encodingLength > 100){   
    blink(blueLed,3);
    return;
  }
  
  radio_.send(encoding, encodingLength, 0);  
  blink(greenLed,2);

  memset(contentBuffer, 0, sizeof(contentBuffer));
  data.getName().clear(); 
}

void setup() {
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  blink(blueLed,3);
  
  Serial.begin(57600);
  radio_.begin(Serial);

  //data.getSignature().getKeyLocator().setType(ndn_KeyLocatorType_KEYNAME);
  //data.getSignature().getKeyLocator().setKeyName(NameLite("key1"));
  data.setContent(BlobLite((const uint8_t*) contentBuffer, strlen(contentBuffer)));
  data.getSignature().setType(ndn_SignatureType_HmacWithSha256Signature);
  delay(500);
  
  blink(redLed,1);
  blink(blueLed,1);
  blink(greenLed,1);  
}

void loop() {
  radio_.readPacket();  
  if (radio_.getResponse().isAvailable()) 
  {  
    handleInterest();
  }

  /* fill contentBuffer... */
  
  delay(500);
  
} //loop

#endif // ARDUINO
