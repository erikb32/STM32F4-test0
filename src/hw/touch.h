#ifndef _TOUCH_H_
#define _TOUCH_H_

typedef enum {
    NO_ACTION,
    SWIPED_LEFT,
    SWIPED_RIGHT
} touch_cmd_t;

void touch_init(void);
void touch_info(void);
void touch_poll(void);

touch_cmd_t touch_get_command(void);

#endif // _TOUCH_H_
