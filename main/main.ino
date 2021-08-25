#include "Phone.h" // Phone class
#include <memory>

std::unique_ptr<Phone> phone; // create pointer to Phone class

void setup() {
  phone = std::unique_ptr<Phone>(new Phone());
}

void loop() {
  phone->loop();
}
