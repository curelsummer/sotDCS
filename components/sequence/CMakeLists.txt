idf_component_register(SRCS "seq_common.c"
                            "seq.c"
                            "tDCS/tdcs_gseq.c"
                            "tACS/tacs_gseq.c"
                            "tACS/tacs_para_table.c"
                            "CES/ces_gseq.c"
                            "rtACS/rtacs_gseq.c"

                    INCLUDE_DIRS "."
                                 "tDCS"
                                 "tACS"
                                 "CES"
                                 "rtACS"
                    REQUIRES waveform current)

