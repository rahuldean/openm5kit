#include "../../src/OpenM5Kit.h"

openm5kit::App app(openm5kit::makeCoreS3Profile());

void setup() {
  app.begin();
}

void loop() {
  app.update();
}
