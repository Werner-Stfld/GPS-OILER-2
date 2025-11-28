# GPS-Oiler 3.0 #

## WEB-Server ##

The web content uses vueJs as described in [How to use VueJs with ESP8266 or ESP32](https://www.luisllamas.es/en/esp8266-vuejs/).
The web servers files are stored as LittleFs filesystem on the target. The filesystem must be created and uploaded. The content is taken from the data folder.

### Create and upload The filesystem with platform.io ###

Important: platformio.ini needs an entry to use the correct filesystem type: board_build.filesystem = littlefs

Use the platform.io project task 'Upload Filesystem Image'.






