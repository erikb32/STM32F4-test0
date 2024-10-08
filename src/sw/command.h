#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <stdbool.h>

typedef enum {
    DISPLAY,
} cmd_exec_t;

void command_prompt(void);
void command_check(void);
bool command_exec(cmd_exec_t cmd);

#endif // _COMMAND_H_
