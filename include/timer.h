#ifndef __TIMER_H__
#define __TIMER_H__

void init_timer(void);
void timer_event();
int sys_timer_query();
void set_timer_intr_handler(void (*)(void));
/*
 * 设置时钟中断的处理函数
 *
 */

#endif
