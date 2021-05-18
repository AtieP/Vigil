#ifndef __SYS_KCON_H__
#define __SYS_KCON_H__

#include <stdarg.h>
#include <stdint.h>

enum kcon_log_level {
    KCON_LOG_NONE,
    KCON_LOG_DEBUG,
    KCON_LOG_INFO,
    KCON_LOG_WARN
};

void kcon_init(uint32_t *framebuffer, uint16_t width, uint16_t height, uint16_t pitch);
void kcon_set_font(uint8_t *font, uint16_t width, uint16_t height);
void kcon_set_color(uint32_t foreground, uint32_t background);
void kcon_set_cursor(int x, int y);
void kcon_cls();
void kcon_scroll();
void kcon_putc(int ch);
void kcon_puts(const char *string);
void kcon_printf(const char *fmt, va_list args);
void kcon_log(enum kcon_log_level level, const char *module, const char *fmt, ...);

#endif
