#include <IotaWallet.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char ca[] PROGMEM = "-----BEGIN CERTIFICATE-----\n" \
"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
"-----END CERTIFICATE-----";


BearSSL::WiFiClientSecure networkClient;
IotaClient iotaClient(networkClient, "nodes.thetangle.org", 443);

const String ADDRESS = "DWNLYMTY9ANNAJ9ALK9DJNJRSMCAKLUQBWJPFXCORTVWYKYNECGJXCYSIVXCXSJIFDNAP9VYAJCCDPKKB";
const String WIFI_SSID = "SSID";
const String WIFI_PASSWORD = "PASSWORD"; 
const int INTERVAL = 5000;

unsigned long lastBalance = 0;

std::vector<String> addrs;
std::vector<uint64_t> balances;

void setup() {
  Serial.begin(115200);

  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  BearSSL::X509List *serverTrustedCA = new BearSSL::X509List(ca);
  networkClient.setTrustAnchors(serverTrustedCA);

  connectToWiFi();

  //clearEEPROM();

  addrs.push_back(ADDRESS);

  unsigned long result = 0;
  EEPROM.begin(4);
  EEPROM.get( 0, result );
  EEPROM.end();

  printf("Last balance from EEPROM: %lu i\n", result);

  if(result == ULONG_MAX) {
    storeLastBalance(lastBalance);
  } else if(result > lastBalance) {
    lastBalance = result;
  }
}

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  do {
    digitalWrite(13, LOW);
    delay(300);
    digitalWrite(13, HIGH);
    delay(300);
  } while (WiFi.status() != WL_CONNECTED);
}

bool storeLastBalance(unsigned long value) {
  lastBalance = value;
  
  EEPROM.begin(8);
  EEPROM.put( 0, value );
  EEPROM.commit();
  EEPROM.end();    
}

void loop() {
  if (iotaClient.getBalances(addrs, balances)) {    
    printf("Current balance: %llu i\n", balances[0]);

    if(balances[0] > lastBalance) {
      switchOn();
      delay((balances[0] - lastBalance) * 1000);
      switchOff();

      storeLastBalance(balances[0]);
    } else {
      delay(INTERVAL);
    }
  } else {
    printf("Connecting to node was not successful \n");
  }  
}

void switchOn() {
  if(digitalRead(12) == LOW) {
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    printf("Switched ON\n");
  }
}

void switchOff() {
  if(digitalRead(12) == HIGH) {
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
    printf("Switched OFF\n");
  }
}

bool clearEEPROM() {  
  EEPROM.begin(512);
  for (int i = 0 ; i < 512 ; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
  
  printf("Cleared EEPROM\n");
}
