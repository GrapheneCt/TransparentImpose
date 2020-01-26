# TransparentImpose

Plugin for PS Vita that makes impose menu (the one that appears after long PS button press) and power off menu transparent.

This plugin doesn't work in system applications (exceptions are Videos app and Package Installer).

Install under *main

# How to set custom transparency value

Using Vitashell, create config file: `ur0:data/trimpose.txt`.
Edit this file and enter your transparency value. Valid values are in 0-255 range:

255 - 100% transparent
.
.
.
0 - 0% transparent

Values below 30 are not recommended.

Reboot is required to apply changes.
