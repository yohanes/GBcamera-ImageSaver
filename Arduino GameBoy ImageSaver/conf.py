# Arduino GameBoy ImageSaver configuration file
FOLDER = "C:\\GB"
#specify a port for quicker startup
# SERIAL = "/dev/tty.usbmodem1a1231"
SERIAL = None



# How to connect?

#           _________
#          / 3  2  1 \
#          |_6__5__4_|
#   Game boy external link
#         wire connector

#    +-----------+---------+------+
#    | GB ExtLnk | Arduino | Name |
#    +-----------+---------+------+
#    | 1         | 13      | SCK  |
#    | 2         | 11      | SIN  |
#    | 4         | GND     | GND  |
#    | 6         | SOUT    | 12   |
#    +-----------+---------+------+
