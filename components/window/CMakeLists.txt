idf_component_register(SRCS "win_basic.c"
                            "dispwin.c"
                            "win_debug.c"
                            "win_tdcs.c"
                            "win_tacs.c" 
                            "win_ces.c"
                            "win_rtacs.c"
                            "win_home_tdcs.c"
                            "win_home_ces.c"
                            
                    INCLUDE_DIRS "."
                    REQUIRES sysenv lcd button beep waveform e_pres stimulation monitor ble nvs_flash calibration)
