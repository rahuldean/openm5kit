#include "../../src/OpenM5Kit.h"

openm5kit::App app(openm5kit::makeM5StickProfile());

void setup() {
  app.begin();
}

void loop() {
  app.update();
}
