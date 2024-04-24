#include "dac8562_hal.h"


#define CMD_SETA_UPDATEA      		0x18	// A�����̨�?����?+16??A?����y?Y
#define CMD_SETB_UPDATEB      		0x19	// B�����̨�?����?+16??B?����y?Y
#define CMD_UPDATE_ALL_DACS	   		0x0F	// ?��D?��??��??��??��?����?��?o��16????D������?��?��?��

#define CMD_GAIN	    						0x02	// ?��2?��?�䨮��?��y?����?
#define DATA_GAIN_B2_A2    				0x0000	// B?��2��?��?A?��1��?
#define DATA_GAIN_B2_A1       		0x0001	// B?��1��?��?A?��2��?
#define DATA_GAIN_B1_A2         	0x0002	// B?��2��?��?A?��2��?
#define DATA_GAIN_B1_A1       		0x0003	// B?��1��?��?A?��1��?				  

#define CMD_PWR_UP_A_B		      	0x20	// ?����?��o��?��?A?��B?��
#define DATA_PWR_UP_A_B		      	0x0003	// ��y?Y��oPower up DAC-A and DAC-B	data

#define CMD_RESET_ALL_REG	      	0x28	// ?����?��o?����D??��??��?��???��??????��??��
#define DATA_RESET_ALL_REG     		0x0001	// ��y?Y��o?����D??��??��?��???��??????��??��

#define CMD_LDAC_DIS	        		0x30	// LDAC??1|?��?����?
#define DATA_LDAC_DIS       			0x0003	// LDAC??2??e���¨�?

#define CMD_INTERNAL_REF_DIS    	0x38	// ?����?��oDisable internal reference and reset DACs to gain = 1
#define DATA_INTERNAL_REF_DIS    	0x0000	// ��y?Y��oDisable internal reference and reset DACs to gain = 1
#define CMD_INTERNAL_REF_EN       0x38	// ?����?��oEnable Internal Reference & reset DACs to gain = 2
#define DATA_INTERNAL_REF_EN    	0x0001	// ��y?Y��oEnable Internal Reference & reset DACs to gain = 2



static void dac8562_wr_reg(uint8_t cmd_byte, uint16_t data_byte)
{
    uint8_t buf[3];
    buf[0] = cmd_byte;
    buf[1] = (data_byte>>8)&0xFF;
    buf[2] = data_byte&0xFF;
	dac8562_hal_write_bytes(buf, 3);
}


void dac8562_init(void)
{
    dac8562_hal_init();

    dac8562_wr_reg(CMD_RESET_ALL_REG, DATA_RESET_ALL_REG);
    dac8562_wr_reg(CMD_GAIN, DATA_GAIN_B1_A1);// ?��??
	dac8562_wr_reg(CMD_PWR_UP_A_B, DATA_PWR_UP_A_B);				// ��?��?
	dac8562_wr_reg(CMD_INTERNAL_REF_DIS, DATA_INTERNAL_REF_DIS);	    // ��1?��?��2?REF
	dac8562_wr_reg(CMD_LDAC_DIS, DATA_LDAC_DIS);					// ��?2??o��??��D?
}



void dac8562_output_b(uint16_t data)
{						
	dac8562_wr_reg(CMD_SETB_UPDATEB, data);	
}



