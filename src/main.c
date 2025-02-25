/* nRF5340 DK Dual Button-LED Toggle Example
 * Toggles LED1 on each Button 1 press
 * Toggles LED2 on each Button 2 press
 */

 #include <zephyr/kernel.h>
 #include <zephyr/drivers/gpio.h>
 #include <zephyr/logging/log.h>
 
 LOG_MODULE_REGISTER(button_led_toggle, LOG_LEVEL_INF);
 
 /* LED1 on nRF5340 DK is on P0.28 */
 #define LED1_NODE DT_ALIAS(led0)
 #define LED1_PIN  DT_GPIO_PIN(LED1_NODE, gpios)
 
 /* LED2 on nRF5340 DK is on P0.29 */
 #define LED2_NODE DT_ALIAS(led1)
 #define LED2_PIN  DT_GPIO_PIN(LED2_NODE, gpios)
 
 /* Button 1 on nRF5340 DK is on P0.23 */
 #define BUTTON1_NODE DT_ALIAS(sw0)
 #define BUTTON1_PIN  DT_GPIO_PIN(BUTTON1_NODE, gpios)
 
 /* Button 2 on nRF5340 DK is on P0.24 */
 #define BUTTON2_NODE DT_ALIAS(sw1)
 #define BUTTON2_PIN  DT_GPIO_PIN(BUTTON2_NODE, gpios)
 
 /* Debounce time in milliseconds */
 #define DEBOUNCE_TIME_MS 50
 
 static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
 static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
 static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET(BUTTON1_NODE, gpios);
 static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET(BUTTON2_NODE, gpios);
 
 /* Button interrupt callback data */
 static struct gpio_callback button1_cb_data;
 static struct gpio_callback button2_cb_data;
 
 /* LED states */
 static bool led1_state = false;
 static bool led2_state = false;
 
 /* Debounce timestamps */
 static int64_t last_time_button1 = 0;
 static int64_t last_time_button2 = 0;
 
 void button1_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
 {
	 int64_t now = k_uptime_get();
	 
	 /* Simple debounce mechanism */
	 if ((now - last_time_button1) < DEBOUNCE_TIME_MS) {
		 return;
	 }
	 last_time_button1 = now;
 
	 /* Toggle LED1 state */
	 led1_state = !led1_state;
	 gpio_pin_set_dt(&led1, led1_state);
	 
	 LOG_INF("Button 1 pressed, LED1 is %s", led1_state ? "ON" : "OFF");
 }
 
 void button2_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
 {
	 int64_t now = k_uptime_get();
	 
	 /* Simple debounce mechanism */
	 if ((now - last_time_button2) < DEBOUNCE_TIME_MS) {
		 return;
	 }
	 last_time_button2 = now;
 
	 /* Toggle LED2 state */
	 led2_state = !led2_state;
	 gpio_pin_set_dt(&led2, led2_state);
	 
	 LOG_INF("Button 2 pressed, LED2 is %s", led2_state ? "ON" : "OFF");
 }
 
 void main(void)
 {
	 int ret;
 
	 /* Check if devices are ready */
	 if (!device_is_ready(led1.port)) {
		 LOG_ERR("LED1 device not ready");
		 return;
	 }
 
	 if (!device_is_ready(led2.port)) {
		 LOG_ERR("LED2 device not ready");
		 return;
	 }
 
	 if (!device_is_ready(button1.port)) {
		 LOG_ERR("Button1 device not ready");
		 return;
	 }
 
	 if (!device_is_ready(button2.port)) {
		 LOG_ERR("Button2 device not ready");
		 return;
	 }
 
	 /* Configure LEDs as outputs */
	 ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
	 if (ret < 0) {
		 LOG_ERR("Error configuring LED1 pin: %d", ret);
		 return;
	 }
 
	 ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);
	 if (ret < 0) {
		 LOG_ERR("Error configuring LED2 pin: %d", ret);
		 return;
	 }
 
	 /* Configure Buttons as inputs with pull-up */
	 ret = gpio_pin_configure_dt(&button1, GPIO_INPUT | GPIO_PULL_UP);
	 if (ret < 0) {
		 LOG_ERR("Error configuring button1 pin: %d", ret);
		 return;
	 }
 
	 ret = gpio_pin_configure_dt(&button2, GPIO_INPUT | GPIO_PULL_UP);
	 if (ret < 0) {
		 LOG_ERR("Error configuring button2 pin: %d", ret);
		 return;
	 }
 
	 /* Configure Button interrupts */
	 ret = gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_TO_ACTIVE);
	 if (ret < 0) {
		 LOG_ERR("Error configuring button1 interrupt: %d", ret);
		 return;
	 }
 
	 ret = gpio_pin_interrupt_configure_dt(&button2, GPIO_INT_EDGE_TO_ACTIVE);
	 if (ret < 0) {
		 LOG_ERR("Error configuring button2 interrupt: %d", ret);
		 return;
	 }
 
	 /* Initialize button callbacks */
	 gpio_init_callback(&button1_cb_data, button1_pressed, BIT(button1.pin));
	 gpio_add_callback(button1.port, &button1_cb_data);
 
	 gpio_init_callback(&button2_cb_data, button2_pressed, BIT(button2.pin));
	 gpio_add_callback(button2.port, &button2_cb_data);
 
	 LOG_INF("nRF5340 Dual Button-LED toggle application started");
	 LOG_INF("Press Button 1 to toggle LED1");
	 LOG_INF("Press Button 2 to toggle LED2");
 
	 /* Main loop - do nothing here as interrupts handle the button presses */
	 while (1) {
		 k_sleep(K_SECONDS(1));
	 }
 }