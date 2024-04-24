idf_component_register(SRCS "lcd.c"
                            "lcd_hal.c"
                            "GUI.c"
			    "font.c"
                            "icon.c"
                            "pic.c"
                    INCLUDE_DIRS "."
                    REQUIRES spi_mng)
