#include "tweeny.h"

int main() {
  auto tween1 = tweeny::from(0.0, 1.0f).to(1.0f, 0.0f).via("stepped", "linear");
  return 0;
}