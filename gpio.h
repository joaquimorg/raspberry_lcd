//
//  GPIO registers access on the Raspberry-Pi
//



void gpio_setup();
void gpio_output_pin(int pin);
void gpio_input_pin(int pin);

// Warning : the following function takes a shifted pin argument.
// i.e. : gpio_set(1 << 6) to set the 6th pin !
inline void gpio_set(int pin);
inline void gpio_clr(int pin);
inline void gpio_put(int pin,short value);

