#include <blinklib.h>

#include "src/blinks-broadcast/manager.h"

void setup() {}

void loop() { broadcast::manager::Process(); }
