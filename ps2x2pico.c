/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 No0ne (https://github.com/No0ne)
 *           (c) 2023 Dustin Hoffman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "hardware/gpio.h"
#include "bsp/board.h"
#include "tusb.h"
#include "ps2x2pico.h"

void tuh_hid_mount_cb(u8 dev_addr, u8 instance, u8 const* desc_report, u16 desc_len) {
  if(DEBUG) printf("HID device address = %d, instance = %d is mounted\n", dev_addr, instance);

  switch(tuh_hid_interface_protocol(dev_addr, instance)) {
    case HID_ITF_PROTOCOL_KEYBOARD:
      if(DEBUG) printf("HID Interface Protocol = Keyboard\n");
      kb_usb_mount(dev_addr, instance);
      tuh_hid_receive_report(dev_addr, instance);
    break;
    
    case HID_ITF_PROTOCOL_MOUSE:
      if(DEBUG) printf("HID Interface Protocol = Mouse\n");
      //tuh_hid_set_protocol(dev_addr, instance, HID_PROTOCOL_REPORT);
      ms_usb_mount(dev_addr, instance);
      tuh_hid_receive_report(dev_addr, instance);
    break;
  }
}

void tuh_hid_umount_cb(u8 dev_addr, u8 instance) {
  if(DEBUG) printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
  kb_usb_umount(dev_addr, instance);
  ms_usb_umount(dev_addr, instance);
}

void tuh_hid_report_received_cb(u8 dev_addr, u8 instance, u8 const* report, u16 len) {
  board_led_write(1);
  
  switch(tuh_hid_interface_protocol(dev_addr, instance)) {
    case HID_ITF_PROTOCOL_KEYBOARD:
      kb_usb_receive(report);
      tuh_hid_receive_report(dev_addr, instance);
    break;
    
    case HID_ITF_PROTOCOL_MOUSE:
      ms_usb_receive(report);
      tuh_hid_receive_report(dev_addr, instance);
    break;
  }
  
  board_led_write(0);
}

void main() {
  board_init();
  printf("\n%s-%s DEBUG=%s\n", PICO_PROGRAM_NAME, PICO_PROGRAM_VERSION_STRING, DEBUG ? "true" : "false");
  
  gpio_init(LVPWR);
  gpio_set_dir(LVPWR, GPIO_OUT);
  gpio_put(LVPWR, 1);
  
  tusb_init();
  kb_init();
  ms_init();
  
  while(true) {
    tuh_task();
    kb_task();
    ms_task();
  }
}
