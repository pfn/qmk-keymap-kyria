CONVERT_TO=kb2040

RGBLIGHT_ENABLE = no
RGB_MATRIX_ENABLE = yes

WPM_ENABLE = yes
OLED_ENABLE = yes
ENCODER_ENABLE = yes
SWAP_HANDS_ENABLE = yes
MOUSEKEY_ENABLE = yes

MAGIC_ENABLE = yes

GRAVE_ESC_ENABLE = no
SPACE_CADET_ENABLE = no
MUSIC_ENABLE = no
INTROSPECTION_KEYMAP_C=$(INTERMEDIATE_OUTPUT)/src/keymap_configurator.c

SRC += kyria.c kyria_rgb_matrix.c

quantum/keymap_introspection.c: $(INTERMEDIATE_OUTPUT)/src/keymap_configurator.c

.SECONDEXPANSION:
$(INTERMEDIATE_OUTPUT)/src/keymap_configurator.c: $$(KEYMAP_PATH)/pfn.json
	$(QMK_BIN) json2c --quiet --output $@ $<
