BOOTLOADER = qmk-hid
BOOTLOADER_SIZE = 512

WPM_ENABLE = yes
OLED_ENABLE = yes
LTO_ENABLE = yes
OLED_DRIVER = SSD1306   # Enables the use of OLED displays
ENCODER_ENABLE = yes       # Enables the use of one or more encoders
RGBLIGHT_ENABLE = yes      # Enable keyboard RGB underglow
SWAP_HANDS_ENABLE = yes
MOUSEKEY_ENABLE = yes

GRAVE_ESC_ENABLE = no
SPACE_CADET_ENABLE = no
MAGIC_ENABLE = yes
MUSIC_ENABLE = no
SRC += kyria.c keymap_configurator.c

.SECONDEXPANSION:
keymap_configurator.c: $$(KEYMAP_PATH)/pfn.json
	$(QMK_BIN) json2c --quiet --output $@ $<