// Adafruit IO Publish & Subscribe Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/********* Configuration ************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/******** Example Starts Here ***********/

// this int will hold the current count for our sketch
int hora = 0;
int fecha = 0;
#define LED_PIN 2
#define LED_PIN1 5

// Track time of last published messages and limit feed->save events to once
// every IO_LOOP_DELAY milliseconds.
//
// Because this sketch is publishing AND subscribing, we can't use a long
// delay() function call in the main loop since that would prevent io.run()
// from being called often enough to receive all incoming messages.
//
// Instead, we can use the millis() function to get the current time in
// milliseconds and avoid publishing until IO_LOOP_DELAY milliseconds have
// passed.
#define IO_LOOP_DELAY 5000
unsigned long lastUpdate = 0;

// set up the 'counter' feed
AdafruitIO_Feed *enviarFeed = io.feed("sensor");
AdafruitIO_Feed *enviarFeed1 = io.feed("sensor2");
AdafruitIO_Feed *recibirFeed = io.feed("piloto1");
AdafruitIO_Feed *recibirFeed1 = io.feed("piloto2");

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN1, OUTPUT);
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while (! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // set up a message handler for the count feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  recibirFeed->onMessage(handleMessage);
  recibirFeed1->onMessage(handleMessage1);

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  recibirFeed->get();
  recibirFeed1->get();

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
  hora = random(0, 100);
  fecha = random(0, 100);
  if (millis() > (lastUpdate + IO_LOOP_DELAY)) {
    // save count to the 'counter' feed on Adafruit IO
    Serial.print("sending -> ");
    Serial.println(hora);
    enviarFeed->save(hora);
    Serial.print("sending -> ");
    Serial.println(fecha);
    enviarFeed1->save(fecha);

    // increment the count by 1
    //count++;

    // after publishing, store the current time
    lastUpdate = millis();
  }

}

// this function is called whenever a 'counter' message
// is received from Adafruit IO. it was attached to
// the counter feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {

  Serial.print("received <- ");
  Serial.println(data->value());

  if (data->toString() == "ON") {
    digitalWrite(LED_PIN, HIGH);
  }
  if (data->toString() == "OFF") {
    digitalWrite(LED_PIN, LOW);
  }

}
void handleMessage1(AdafruitIO_Data *data) {

  Serial.print("received <- ");
  Serial.println(data->value());

  if (data->toString() == "ON") {
    digitalWrite(LED_PIN1, HIGH);
  }
  if (data->toString() == "OFF") {
    digitalWrite(LED_PIN1, LOW);
  }

}
