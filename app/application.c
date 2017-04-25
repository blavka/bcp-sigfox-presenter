#include <application.h>
#include <bcl.h>

#define PIR_COOLDOWN_RESET 30000
#define PIR_COOLDOWN (15 * 60 * 1000)

enum
{
    SIGFOX_PREV = 0x00,
    SIGFOX_NEXT = 0x01,

};

bc_scheduler_task_id_t transmit_task_id;

// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

// SigFox Module instance
bc_module_sigfox_t sigfox_module;

uint8_t buffer[1];

void transmit_task(void *param)
{
    (void) param;

    if (!bc_module_sigfox_is_ready(&sigfox_module))
    {
        bc_scheduler_plan_current_now();
        return;
    }

    bc_led_set_mode(&led, BC_LED_MODE_ON);

    bc_module_sigfox_send_rf_frame(&sigfox_module, buffer, sizeof(buffer));
}

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    (void) self;
    (void) event_param;

    if (event == BC_BUTTON_EVENT_CLICK)
    {
        buffer[0] = SIGFOX_NEXT;
        bc_scheduler_plan_now(transmit_task_id);
    }

}

void sigfox_module_event_handler(bc_module_sigfox_t *self, bc_module_sigfox_event_t event, void *event_param)
{
    (void) self;
    (void) event_param;

    static bool has_device_id = false;
    static bool has_device_pac = false;

    // If event is ready...
    if (event == BC_MODULE_SIGFOX_EVENT_READY)
    {
        if (!has_device_id)
        {
            bc_module_sigfox_read_device_id(self);
        }
        else if (!has_device_pac)
        {
            bc_module_sigfox_read_device_pac(self);
        }
    }
    // If event is error...
    else if (event == BC_MODULE_SIGFOX_EVENT_ERROR)
    {
        bc_led_pulse(&led, 0);
        bc_led_set_mode(&led, BC_LED_MODE_BLINK);
    }
    // If event is end of transmission...
    else if (event == BC_MODULE_SIGFOX_EVENT_SEND_RF_FRAME_DONE)
    {
        bc_led_set_mode(&led, BC_LED_MODE_OFF);
    }
    // If event is read device ID...
    else if (event == BC_MODULE_SIGFOX_EVENT_READ_DEVICE_ID)
    {
        has_device_id = true;
    }
    // If event is read device PAC...
    else if (event == BC_MODULE_SIGFOX_EVENT_READ_DEVICE_PAC)
    {
        has_device_pac = true;
    }
}

void application_init(void)
{
    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    // Initialize SigFox Module
    bc_module_sigfox_init(&sigfox_module, BC_MODULE_SIGFOX_REVISION_R1);
    bc_module_sigfox_set_event_handler(&sigfox_module, sigfox_module_event_handler, NULL);

    buffer[0] = SIGFOX_PREV;
    transmit_task_id = bc_scheduler_register(transmit_task, NULL, 0);

    bc_led_set_mode(&led, BC_LED_MODE_OFF);
}
