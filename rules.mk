BOOTLOADER = qmk-hid
BOOTLOADER_SIZE = 512

RGBLIGHT_ENABLE = no
RGB_MATRIX_ENABLE = yes

WPM_ENABLE = yes
OLED_ENABLE = yes
OLED_DRIVER = SSD1306
ENCODER_ENABLE = yes
SWAP_HANDS_ENABLE = yes
MOUSEKEY_ENABLE = yes

MAGIC_ENABLE = yes

GRAVE_ESC_ENABLE = no
SPACE_CADET_ENABLE = no
MUSIC_ENABLE = no

SRC += kyria.c kyria_rgb_matrix.c $(KEYMAP_OUTPUT)/src/keymap_configurator.c

.SECONDEXPANSION:
$(KEYMAP_OUTPUT)/src/keymap_configurator.c: $$(KEYMAP_PATH)/pfn.json
	$(QMK_BIN) json2c --quiet --output $@ $<
