#include <Arduino.h>
#include <lilka.h>
#include <AlfredoCRSF.h>
#include <HardwareSerial.h>

#define PIN_RX_OUT 44
#define PIN_TX_OUT 43

#define PIN_RX 14
#define PIN_TX 13

#define TX12_MIN 990
#define TX12_MAX 1998
#define TX12_MID 1500

HardwareSerial crsfSerialOut(0);
AlfredoCRSF crsfOut;

// Set up a new Serial object
HardwareSerial crsfSerial(1);
AlfredoCRSF crsf;

int channelValue = CRSF_CHANNEL_VALUE_MID;

int gear = 0;

int prevPitch = 0;
int prevPitchStartMillis = 0;

// Method to send channels based on CRSF instance
void sendChannels(AlfredoCRSF& crsf) {
  const crsf_channels_t* channels_ptr = crsf.getChannelsPacked();
  crsfOut.writePacket(CRSF_SYNC_BYTE, CRSF_FRAMETYPE_RC_CHANNELS_PACKED, channels_ptr, sizeof(*channels_ptr));
}

// Fallback method to send default channel values
void sendFallbackChannels() {
  crsf_channels_t crsfChannels = { 0 };
  crsfChannels.ch0 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch1 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch2 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch3 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch4 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch5 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch6 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch7 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch8 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch9 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch10 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch11 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch12 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch13 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch14 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch15 = CRSF_CHANNEL_VALUE_MIN;

  crsfOut.writePacket(CRSF_SYNC_BYTE, CRSF_FRAMETYPE_RC_CHANNELS_PACKED, &crsfChannels, sizeof(crsfChannels));
}

// Fallback method to send default channel values
void sendModifiedChannels(
  int roll,
  int pitch,
  int yaw,
  int throttle,
  int ch5_E,
  int ch6_F,
  int ch7_B,
  int ch8_C,
  int ch9_A,
  int ch10_D,
  int ch11_S1,
  int ch12_S2
) {
  crsf_channels_t crsfChannels = { 0 };
  crsfChannels.ch0 = roll;
  crsfChannels.ch1 = pitch;
  crsfChannels.ch2 = yaw;
  crsfChannels.ch3 = throttle;
  crsfChannels.ch4 = ch5_E;
  crsfChannels.ch5 = ch6_F;
  crsfChannels.ch6 = ch7_B;
  crsfChannels.ch7 = ch8_C;
  crsfChannels.ch8 = ch9_A;
  crsfChannels.ch9 = ch10_D;
  crsfChannels.ch10 = ch11_S1;
  crsfChannels.ch11 = ch12_S2;
  crsfChannels.ch12 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch13 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch14 = CRSF_CHANNEL_VALUE_MIN;
  crsfChannels.ch15 = CRSF_CHANNEL_VALUE_MIN;

  crsfOut.writePacket(CRSF_SYNC_BYTE, CRSF_FRAMETYPE_RC_CHANNELS_PACKED, &crsfChannels, sizeof(crsfChannels));
}

void setup() {
  lilka::begin();

  Serial.begin(9600);

  crsfSerialOut.begin(CRSF_BAUDRATE, SERIAL_8N1, PIN_RX_OUT, PIN_TX_OUT);
  crsfOut.begin(crsfSerialOut);

  delay(300);

  crsfSerial.begin(CRSF_BAUDRATE, SERIAL_8N1, PIN_RX, PIN_TX);
  crsf.begin(crsfSerial);
}

int convertCh(int chValue) {
  return map(chValue, TX12_MIN, TX12_MAX, CRSF_CHANNEL_VALUE_MIN, CRSF_CHANNEL_VALUE_2000);
}

void loop() {
  lilka::Canvas canvas; 
  int batteryLevel = lilka::battery.readLevel();
  String batteryMessage = "Bat: " + String(batteryLevel) + "%";

  // put your main code here, to run repeatedly:

  crsf.update();

  Serial.println(crsf.isLinkUp());

  //sendChannels(crsf);

  // Serial.print(crsf.getChannel(1));
  // Serial.println(',');

  int roll = convertCh(crsf.getChannel(1));
  int pitch = convertCh(crsf.getChannel(2));
  int yaw = convertCh(crsf.getChannel(3));
  int throttle = convertCh(crsf.getChannel(4));

  int ch5_E = convertCh(crsf.getChannel(5));
  int ch6_F = convertCh(crsf.getChannel(6));
  int ch7_B = convertCh(crsf.getChannel(7));
  int ch8_C = convertCh(crsf.getChannel(8));
  int ch9_A = convertCh(crsf.getChannel(9));
  int ch10_D = convertCh(crsf.getChannel(10));
  int ch11_S1 = convertCh(crsf.getChannel(11));
  int ch12_S2 = convertCh(crsf.getChannel(12));


  // auto transmition
  boolean autoTransmition = false;

  if (ch6_F > 1600) {
    autoTransmition = true;
  }
  // END. auto transmition

  canvas.fillScreen(lilka::colors::Black);
  canvas.setTextColor(lilka::colors::White);

  int spacer = 17;
  int topRowStart = 30;
  int leftCellStart = 5;

  int rightCellStart = 150;

  canvas.setCursor(leftCellStart, topRowStart);
  canvas.print("roll: " + String(roll));

  canvas.setCursor(leftCellStart, topRowStart + spacer * 1);
  canvas.print("pitch: " + String(pitch));

  canvas.setCursor(leftCellStart, topRowStart + spacer * 2);
  canvas.print("throttle: " + String(yaw));

  canvas.setCursor(leftCellStart, topRowStart + spacer * 3);
  canvas.print("yaw: " + String(throttle));

  canvas.setCursor(leftCellStart, topRowStart + spacer * 4);
  canvas.print("ch5_E: " + String(ch5_E));

  canvas.setCursor(leftCellStart, topRowStart + spacer * 5);
  canvas.print("ch6_F: " + String(ch6_F));

  canvas.setCursor(leftCellStart, topRowStart + spacer * 6);
  canvas.print("ch7_B: " + String(ch7_B));

  canvas.setCursor(leftCellStart, topRowStart + spacer * 7);
  canvas.print("ch8_C: " + String(ch8_C));

  canvas.setCursor(leftCellStart, topRowStart + spacer * 8);
  canvas.print("ch9_A: " + String(ch9_A));

  canvas.setCursor(leftCellStart, topRowStart + spacer * 9);
  canvas.print("ch10_D: " + String(ch10_D));

  canvas.setCursor(leftCellStart, topRowStart + spacer * 10);
  canvas.print("ch11_S1: " + String(ch11_S1));

  canvas.setCursor(leftCellStart, topRowStart + spacer * 11);
  canvas.print("ch12_S2: " + String(ch12_S2));

  canvas.setCursor(rightCellStart, topRowStart);
  canvas.print(batteryMessage);

  if (autoTransmition) {

    canvas.setCursor(rightCellStart, topRowStart + spacer * 1);
    canvas.print("AUTO MODE");

    canvas.setCursor(rightCellStart, topRowStart + spacer * 2);
    canvas.print("GEAR: " + String(gear));

    if (gear == 0) {
      yaw = 206;
    } else if (gear == 1) {
      yaw = 400;
    } else if (gear == 2) {
      yaw = 600;
    } else if (gear == 3) {
      yaw = 1000;
    } else {
      yaw = 206;
    }

    if (pitch > 1010 && prevPitch == 0) {
      prevPitch = pitch;
      prevPitchStartMillis = millis();
    } else if (prevPitch && pitch > 980 && pitch < 1010) {
      if (millis() - prevPitchStartMillis < 300) {
        if (gear < 3) {
          gear++;
        }
      }

      prevPitch = 0;
      prevPitchStartMillis = 0;
    }
  }

  lilka::display.drawCanvas(&canvas);

  if (crsf.isLinkUp()) {
      sendModifiedChannels(
        roll,
        pitch,
        yaw,
        throttle,
        ch5_E,
        ch6_F,
        ch7_B,
        ch8_C,
        ch9_A,
        ch10_D,
        ch11_S1,
        ch12_S2
      );
  } else {
    gear = 0;
    sendFallbackChannels();
  }
}

