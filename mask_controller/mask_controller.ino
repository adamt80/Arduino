/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

/*
 * This sketch demonstrate the central API(). A additional bluefruit
 * that has bleuart as peripheral is required for the demo.
 */
#include <bluefruit.h>

#define Buttons A0

int ButtonRead = 0;
int current = 0;
int last = -1;


BLEClientDis  clientDis;
BLEClientUart clientUart;

void setup()
{
  Serial.begin(115200);

  Serial.println("Mask Controller - Serial Connected");
  Serial.println("-----------------------------------\n");

  
  // Enable both peripheral and central
  Bluefruit.begin(true, true);
  Bluefruit.setName("Mask Controller");

  // Configure DIS client
  clientDis.begin();

  // Init BLE Central Uart Serivce
  clientUart.begin();
  clientUart.setRxCallback(bleuart_rx_callback);

  // Increase Blink rate to different from PrPh advertising mode
  Bluefruit.setConnLedInterval(250);

  // Callbacks for Central
  Bluefruit.Central.setConnectCallback(connect_callback);
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);

  /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Interval = 100 ms, window = 80 ms
   * - Don't use active scan
   * - Start(timeout) with timeout = 0 will scan forever (until connected)
   */
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
  Bluefruit.Scanner.useActiveScan(true);
  Bluefruit.Scanner.start(0);                   // // 0 = Don't stop scanning after n seconds
}

/**
 * Callback invoked when scanner pick up an advertising data
 * @param report Structural advertising data
 */
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  uint8_t len = 0;
  uint8_t buffer[32];
  memset(buffer, 0, sizeof(buffer));
  String complete_name = "None";
  
/* Display the timestamp and device address */
  if (report->scan_rsp)
  {
    //Serial.printf("[SR%10d] Packet received from ", millis());
  }
  else
  {
    //Serial.printf("[ADV%9d] Packet received from ", millis());
  }
  //Serial.printBuffer(report->peer_addr.addr, 6, ':');
  //Serial.print("\n");

  /* Raw buffer contents */
  //Serial.printf("%14s %d bytes\n", "PAYLOAD", report->dlen);
  if (report->dlen)
  {
    //Serial.printf("%15s", " ");
    //Serial.printBuffer(report->data, report->dlen, '-');
    //Serial.println();
  }

  /* RSSI value */
  //Serial.printf("%14s %d dBm\n", "RSSI", report->rssi);

  /* Adv Type */
  //Serial.printf("%14s ", "ADV TYPE");
  switch (report->type)
  {
    case BLE_GAP_ADV_TYPE_ADV_IND:
      //Serial.printf("Connectable undirected\n");
      break;
    case BLE_GAP_ADV_TYPE_ADV_DIRECT_IND:
      //Serial.printf("Connectable directed\n");
      break;
    case BLE_GAP_ADV_TYPE_ADV_SCAN_IND:
      //Serial.printf("Scannable undirected\n");
      break;
    case BLE_GAP_ADV_TYPE_ADV_NONCONN_IND:
      //Serial.printf("Non-connectable undirected\n");
      break;
  }

  /* Shortened Local Name */
  if(Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, buffer, sizeof(buffer)))
  {
    //Serial.printf("%14s %s\n", "SHORT NAME", buffer);
    memset(buffer, 0, sizeof(buffer));
  }

  /* Complete Local Name */
  if(Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, buffer, sizeof(buffer)))
  {
    //Serial.printf("%14s %s\n", "COMPLETE NAME", buffer);
    complete_name = (char*)buffer;
    memset(buffer, 0, sizeof(buffer));
  }

  /* TX Power Level */
  if (Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_TX_POWER_LEVEL, buffer, sizeof(buffer)))
  {
    //Serial.printf("%14s %i\n", "TX PWR LEVEL", buffer[0]);
    memset(buffer, 0, sizeof(buffer));
  }

  /* Check for UUID16 Complete List */
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE, buffer, sizeof(buffer));
  if ( len )
  {
    //printUuid16List(buffer, len);
  }

  /* Check for UUID16 More Available List */
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE, buffer, sizeof(buffer));
  if ( len )
  {
    //printUuid16List(buffer, len);
  }

  /* Check for UUID128 Complete List */
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE, buffer, sizeof(buffer));
  if ( len )
  {
    //printUuid128List(buffer, len);
  }

  /* Check for UUID128 More Available List */
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE, buffer, sizeof(buffer));
  if ( len )
  {
    //printUuid128List(buffer, len);
  }  

  /* Check for BLE UART UUID */
  if ( Bluefruit.Scanner.checkReportForUuid(report, BLEUART_UUID_SERVICE) )
  {
    //Serial.printf("%14s %s\n", "BLE UART", "UUID Found!");
  }

  /* Check for DIS UUID */
  if ( Bluefruit.Scanner.checkReportForUuid(report, UUID16_SVC_DEVICE_INFORMATION) )
  {
    //Serial.printf("%14s %s\n", "DIS", "UUID Found!");
  }

  /* Check for Manufacturer Specific Data */
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, buffer, sizeof(buffer));
  if (len)
  {
    //Serial.printf("%14s ", "MAN SPEC DATA");
    //Serial.printBuffer(buffer, len, '-');
    //Serial.println();
    memset(buffer, 0, sizeof(buffer));
  }  

  //Serial.println();

  if(complete_name == "Mask") {
    Serial.print("Mask detected. Connecting ... ");
    // Connect to device with bleuart service in advertising
    Bluefruit.Central.connect(report);
  } 
  
}

void printUuid16List(uint8_t* buffer, uint8_t len)
{
  Serial.printf("%14s %s", "16-Bit UUID");
  for(int i=0; i<len; i+=2)
  {
    uint16_t uuid16;
    memcpy(&uuid16, buffer+i, 2);
    Serial.printf("%04X ", uuid16);
  }
  Serial.println();
}

void printUuid128List(uint8_t* buffer, uint8_t len)
{
  (void) len;
  Serial.printf("%14s %s", "128-Bit UUID");

  // Print reversed order
  for(int i=0; i<16; i++)
  {
    const char* fm = (i==4 || i==6 || i==8 || i==10) ? "-%02X" : "%02X";
    Serial.printf(fm, buffer[15-i]);
  }

  Serial.println();  
}

/**
 * Callback invoked when an connection is established
 * @param conn_handle
 */
void connect_callback(uint16_t conn_handle)
{
  Serial.println("Connected");

  Serial.print("Dicovering DIS ... ");
  if ( clientDis.discover(conn_handle) )
  {
    Serial.println("Found it");


    Serial.println();
  }  

  Serial.print("Discovering BLE Uart Service ... ");

  if ( clientUart.discover(conn_handle) )
  {
    Serial.println("Found it");

    Serial.println("Enable TXD's notify");
    clientUart.enableTXD();

    Serial.println("Ready to receive from peripheral");
  }else
  {
    Serial.println("Found NONE");
    
    // disconect since we couldn't find bleuart service
    Bluefruit.Central.disconnect(conn_handle);
  }  
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle
 * @param reason
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
  
  Serial.println("Disconnected");
}

/**
 * Callback invoked when uart received data
 * @param uart_svc Reference object to the service where the data 
 * arrived. In this example it is clientUart
 */
void bleuart_rx_callback(BLEClientUart& uart_svc)
{
  Serial.print("[RX]: ");
  
  while ( uart_svc.available() )
  {
    Serial.print( (char) uart_svc.read() );
  }

  Serial.println();
}

void loop()
{
  if ( Bluefruit.Central.connected() )
  {
    // Not discovered yet
    if ( clientUart.discovered() )
    {
      // Discovered means in working state
      // Get Serial input and send to Peripheral
      if ( Serial.available() )
      {
        delay(2); // delay a bit for all characters to arrive
        String s;
        while(Serial.available()) {
          s = s + String((char)Serial.read());    
        }
        Serial.println(s);
        clientUart.print(s+'\n');
      }
      
    }
    
  }
  readButtons();
  delay(1);
}

void readButtons() {
    int b = 0;
    int i = 0;
    String s;
    ButtonRead = analogRead(Buttons);
    while(ButtonRead > 50) {
       ButtonRead = analogRead(Buttons);
       b = b + ButtonRead;
       i++;
    }
    if(i < 10) {
      current = 0;
      ButtonRead = 0;
    } else {
      ButtonRead = b/i;
    }
    // grab the current state of the remote-buttons
    if (ButtonRead < 120) {
      current = 0;
    }
    if (ButtonRead > 150) {
      current = 1;
      s = "!B110";
    }
    if (ButtonRead > 220) {
      current = 2;
      s = "!B219";
    }
    if (ButtonRead > 350) {
      current = 3;
      s = "!B318";
    }
    if (ButtonRead > 500) {
      current = 4;
      s= "!B417";
    }
 
  // return if the value hasn't changed
  if(current == last)
    return;

  if(current != 0 && last != 0)
    return;
    
  int32_t value = current;
  last = current;

  if(current > 0 && uartUp())
    clientUart.print(s+'\n');

  Serial.print(ButtonRead);
  Serial.print(": ");
  Serial.println(value);
}

bool uartUp() {
  return (Bluefruit.Central.connected() && clientUart.discovered());
}


