#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "gatts_server.h"
#include "esp_gatt_common_api.h"

#include "sysenv.h"
#include "monitor.h"
#include "sti.h"
#include "e_pres.h"
#include "waveform.h"

#define TAG "GATTS_SERVER"

#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define ESP_APP_ID                  0x55
#define SVC_INST_ID                 0

static char device_name[6] = "null";
static char device_uid[13] = "xxxxxxxxxxxx";


/* The max length of characteristic value. When the GATT client performs a write or prepare write operation,
*  the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX. 
*/
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))

#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

static uint8_t adv_config_done       = 0;

uint16_t heart_rate_handle_table[HRS_IDX_NB];

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

static prepare_type_env_t prepare_write_env;



QueueHandle_t *queue_recv = NULL;
static int32_t ble_cmd_init(void);
static void push_cmd(ble_config_t *ble_cfg);
// static uint8_t load_id(void);
static void gen_raw_adv_data(void);

uint8_t tag[]="0";					
uint8_t mID[]="71Wh";				
uint8_t mType[]="1";				
uint8_t mNumber[]="19010001";		
uint8_t RunStatus[]="0";			
uint8_t RunMode[]="1";				
uint8_t mBat[]="3.70";				
uint8_t TreatMode[]="1";			
uint8_t TreatValue[]="2224";		
uint8_t PulseWide[]="0000";			
uint8_t Intermission[]="0.000000";	
uint8_t TreatTime[]="20.0";			
uint8_t TreatImpedance[]="10.0";	
uint8_t Time[]=__TIME__;			
uint8_t Date[]=__DATE__;			
uint8_t Version[]="V1.0";


uint8_t data_ok[4]={0x4F,0x4B,0x0D,0x0A};
uint8_t data_count = 0;
uint8_t receive_data[100];
uint8_t data_len = 0;
uint8_t data_single_count = 0;
uint8_t data_a = 0;
uint8_t data_b = 0;
uint8_t data_c = 0;
uint8_t Rece_data_flag= 0;




#define CONFIG_SET_RAW_ADV_DATA
#ifdef CONFIG_SET_RAW_ADV_DATA
static uint32_t raw_adv_data_len = 17;
static uint8_t raw_adv_data[31] = {
        /* flags */
        0x02, 0x01, 0x06,
        /* tx power*/
        0x02, 0x0a, 0xeb,
        /* service uuid */
        0x03, 0x03, 0xFF, 0x00,
        /* device name */
        0x0f, 0x09, 'N', 'U', 'L', 'L','\0'
};

static uint8_t raw_scan_rsp_data[] = {
        /* flags */
        0x02, 0x01, 0x06,
        /* tx power */
        0x02, 0x0a, 0xeb,
        /* service uuid */
        0x03, 0x03, 0xFF,0x00
};

#else
static uint8_t service_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

/* The length of adv data must be less than 31 bytes */
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp        = false,
    .include_name        = true,
    .include_txpower     = true,
    .min_interval        = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval        = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance          = 0x00,
    .manufacturer_len    = 0,    //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, //test_manufacturer,
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = sizeof(service_uuid),
    .p_service_uuid      = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp        = true,
    .include_name        = true,
    .include_txpower     = true,
    .min_interval        = 0x0006,
    .max_interval        = 0x0010,
    .appearance          = 0x00,
    .manufacturer_len    = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, //&test_manufacturer[0],
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = sizeof(service_uuid),
    .p_service_uuid      = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
#endif /* CONFIG_SET_RAW_ADV_DATA */

static esp_ble_adv_params_t adv_params = {
    .adv_int_min         = 0x20,
    .adv_int_max         = 0x40,
    .adv_type            = ADV_TYPE_IND,
    .own_addr_type       = BLE_ADDR_TYPE_PUBLIC,
    .channel_map         = ADV_CHNL_ALL,
    .adv_filter_policy   = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;

    uint8_t connected;
};


static int32_t update_notify_data(char *data);
void OpenPackge(uint8_t data[]);

// static int32_t packet_param(sti_send_param_t *param, char *p_out)
// {
//     if(param == NULL || p_out == NULL)
//     {
//         ESP_LOGE(TAG, "packet_param null pointer");
//         return -1;
//     }

//     char *output = p_out;
//     int32_t temp=0;

//     //head
//     *output++ = '0';
//     *output++ = ',';

//     //device addr
//     sprintf(output, "%s", "71Wh");
//     output += 4;
//     *output++ = ',';
    
//     //device type
//     *output++ = '1';
//     *output++ = ',';

//     //device number
//     sprintf(output, "%s", "000001");
//     output += 6;
//     *output++ = ',';

//     //run status
//     *output++ = (char)(param->sti_status) + '0';
//     *output++ = ',';

//     //run mode
//     *output++ = (char)(param->sti_mode) + '0';
//     *output++ = ',';

//     //bat vol
//     temp = sprintf(output, "%.2f", param->bat_vol);
//     if(temp > 0)
//         output += temp;
//     *output++ = ',';

//     //sti mode 
//     *output++ = (char)(param->sti_mode) + '0';
//     *output++ = ',';

//     //sti current
//     temp = sprintf(output, "%d", (int32_t)param->sti_cur);
//     if(temp > 0)
//         output += temp;
//     *output++ = ',';

//     //sti freq
//     temp = sprintf(output, "%d", (int32_t)param->sti_freq);
//     if(temp > 0)
//         output += temp;
//     *output++ = ',';

//     //sti offset
//     temp = sprintf(output, "%d", (int32_t)param->sti_offset);
//     if(temp > 0)
//         output += temp;
//     *output++ = ',';

//     //sti time
//     temp = sprintf(output, "%.1f", param->sti_remain_time);
//     if(temp > 0)
//         output += temp;
//     *output++ = ',';

//     //impedance
//     temp = sprintf(output, "%.1f", param->impedance);
//     if(temp > 0)
//         output += temp;
//     *output++ = ',';

//     //time
//     sprintf(output, "%s", "10:40:30");
//     output += 8;
//     *output++ = ',';
    
//     //date
//     sprintf(output, "%s", "06/15/2020");
//     output += 10;
//     *output++ = ',';
    
//     //version
//     sprintf(output, "%s", "V1.0");
//     output += 4;
//     *output++ = ',';

//     //rear
//     *output++ = '\r';
//     *output++ = '\n';

//     *output++ = '\0';

//     return output-p_out;
// }


// int32_t gatts_send(sti_send_param_t *param)
// {
//     if(param == NULL)
//     {
//         return -1;
//     }

//     if(send_param_queue == NULL)
//     {
//         return -2;
//     }

//     if(xQueueSend(send_param_queue, param, 0) != pdTRUE)
//     {
//         return -3;
//     }

//     return 0;
// }
void OpenPackge(uint8_t data[])
{
    ble_config_t ble_config;
    char tmpdata[9][12];
	uint8_t i=0,j=0,k=0;
    char *p = 0;
	
	for(i=0;i<9;i++)
    {
        for(j=0;j<12;j++)
        {
            tmpdata[i][j] = '\0';
        }
    }

	for(i=0,j=0,k=0;k<100;k++)
	{
		if(data[k] == '\0' || data[k] == '\r')
        {
            break;
        }

		if(data[k] == ',')
        {
			tmpdata[i][j] = '\0';
			j=0;
			i++;
		}
		else
        {
			tmpdata[i][j++] = data[k];
		}
	}
	
	memcpy(TreatMode, tmpdata[2], 1);
	memcpy(TreatValue, tmpdata[3], 4);
	memcpy(PulseWide, tmpdata[4], 4);
	memcpy(Intermission, tmpdata[5], 4);
	memcpy(TreatTime, tmpdata[6], 4);

    if(TreatMode[0]==0x31)
    {
	    ble_config.wave_id = 1;
    }
    else if(TreatMode[0]==0x33)
    {
        ble_config.wave_id = 2;
    }
    else if(TreatMode[0]==0x34)
    {
        ble_config.wave_id = 3;
    }
    else
    {
        // ble_config.wave_id = 0;
        return;
    }
    
    printf("ble_config.wave_id %d\n",ble_config.wave_id);
    
    p = (char *)TreatValue;
	ble_config.current = atof(p);
	printf("ble_config.current %f\n",ble_config.current);

	p =(char *) PulseWide;
	ble_config.frequency = atof(p);
	printf("ble_config.frequency %f\n",ble_config.frequency);
	
	p = (char *)Intermission;
    ble_config.offset =  atof(p);
	printf("ble_config.offset %f\n",ble_config.offset);

	p = (char *)TreatTime;
    ble_config.duration = atof(p);
	printf("ble_config.duration %f\n",ble_config.duration);

    push_cmd(&ble_config);
}


static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
					esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM] = {
    [PROFILE_APP_IDX] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
        .connected = 0,
    },
};

/* Service */
static const uint16_t GATTS_SERVICE_UUID_TEST      = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TEST_A       = 0xFF01;
static const uint16_t GATTS_CHAR_UUID_TEST_B       = 0xFF02;
static const uint16_t GATTS_CHAR_UUID_TEST_C       = 0xFF03;

static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read                =  ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_write               = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_read_write_notify   = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t heart_measurement_ccc[2]      = {0x00, 0x00};
static const uint8_t char_value[4]                 = {0x11, 0x22, 0x33, 0x44};


/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] =
{
    // Service Declaration
    [IDX_SVC]        =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_TEST), (uint8_t *)&GATTS_SERVICE_UUID_TEST}},

    /* Characteristic Declaration */
    [IDX_CHAR_A]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_notify}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_A] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_A, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

    /* Client Characteristic Configuration Descriptor */
    [IDX_CHAR_CFG_A]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(heart_measurement_ccc), (uint8_t *)heart_measurement_ccc}},

    /* Characteristic Declaration */
    [IDX_CHAR_B]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_B]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_B, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

    /* Characteristic Declaration */
    [IDX_CHAR_C]      =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write}},

    /* Characteristic Value */
    [IDX_CHAR_VAL_C]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATTS_CHAR_UUID_TEST_C, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

};

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    #ifdef CONFIG_SET_RAW_ADV_DATA
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
            adv_config_done &= (~ADV_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
            adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
    #else
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~ADV_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
    #endif
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            /* advertising start complete event to indicate advertising start successfully or failed */
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG, "advertising start failed");
            }else{
                ESP_LOGI(TAG, "advertising start successfully");
            }
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG, "Advertising stop failed");
            }
            else {
                ESP_LOGI(TAG, "Stop adv successfully\n");
            }
            break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
            break;
        default:
            break;
    }
}

void example_prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(TAG, "prepare write, handle = %d, value len = %d", param->write.handle, param->write.len);
    esp_gatt_status_t status = ESP_GATT_OK;
    if (prepare_write_env->prepare_buf == NULL) {
        prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
        prepare_write_env->prepare_len = 0;
        if (prepare_write_env->prepare_buf == NULL) {
            ESP_LOGE(TAG, "%s, Gatt_server prep no mem", __func__);
            status = ESP_GATT_NO_RESOURCES;
        }
    } else {
        if(param->write.offset > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_OFFSET;
        } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_ATTR_LEN;
        }
    }
    /*send response when param->write.need_rsp is true */
    if (param->write.need_rsp){
        esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
        if (gatt_rsp != NULL){
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK){
               ESP_LOGE(TAG, "Send response error");
            }
            free(gatt_rsp);
        }else{
            ESP_LOGE(TAG, "%s, malloc failed", __func__);
        }
    }
    if (status != ESP_GATT_OK){
        return;
    }
    memcpy(prepare_write_env->prepare_buf + param->write.offset,
           param->write.value,
           param->write.len);
    prepare_write_env->prepare_len += param->write.len;

}

void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param){
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC && prepare_write_env->prepare_buf){
        esp_log_buffer_hex(TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    }else{
        ESP_LOGI(TAG,"ESP_GATT_PREP_WRITE_CANCEL");
    }
    if (prepare_write_env->prepare_buf) {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT:{
            esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(device_name);
            if (set_dev_name_ret){
                ESP_LOGE(TAG, "set device name failed, error code = %x", set_dev_name_ret);
            }
    #ifdef CONFIG_SET_RAW_ADV_DATA
            esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, raw_adv_data_len);
            if (raw_adv_ret){
                ESP_LOGE(TAG, "config raw adv data failed, error code = %x ", raw_adv_ret);
            }
            adv_config_done |= ADV_CONFIG_FLAG;
            esp_err_t raw_scan_ret = esp_ble_gap_config_scan_rsp_data_raw(raw_scan_rsp_data, sizeof(raw_scan_rsp_data));
            if (raw_scan_ret){
                ESP_LOGE(TAG, "config raw scan rsp data failed, error code = %x", raw_scan_ret);
            }
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
    #else
            //config adv data
            esp_err_t err = esp_ble_gap_config_adv_data(&adv_data);
            if (err){
                ESP_LOGE(TAG, "config adv data failed, error code = %x", err);
            }
            adv_config_done |= ADV_CONFIG_FLAG;
            //config scan response data
            err = esp_ble_gap_config_adv_data(&scan_rsp_data);
            if (err){
                ESP_LOGE(TAG, "config scan response data failed, error code = %x", err);
            }
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
    #endif
            esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, HRS_IDX_NB, SVC_INST_ID);
            if (create_attr_ret){
                ESP_LOGE(TAG, "create attr table failed, error code = %x", create_attr_ret);
            }
        }
       	    break;
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_READ_EVT");
        
       	    break;
        case ESP_GATTS_WRITE_EVT:
            if (!param->write.is_prep)
            {
                // the data length of gattc write  must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
                ESP_LOGI(TAG, "GATT_WRITE_EVT, handle = %d, value len = %d, value :", param->write.handle, param->write.len);
                esp_log_buffer_hex(TAG, param->write.value, param->write.len);

               // if (heart_rate_handle_table[IDX_CHAR_CFG_A] == param->write.handle)// && param->write.len >= 5){
             //   {
		            if( param->write.len >= 5)
                   	{
                        if(*(param->write.value+1) == 44 && *(param->write.value+3) == 44)
                        {
                            for(uint8_t i = 0; i < 20; i++)
                            {
                                receive_data[i] = *param->write.value++;
                            }
                            data_a++;
                        }
                        else if( param->write.len < 20)
                        {
                            for(uint8_t i = 40; i < 60; i++)
                            {
                                receive_data[i] = *param->write.value++;
                            }
                            data_b++;

                            // esp_ble_gatts_send_indicate(gatts_if, 
                            //                             param->write.conn_id, 
                            //                             heart_rate_handle_table[IDX_CHAR_VAL_A],
                            //                             sizeof(data_ok), 
                            //                             data_ok, 
                            //                             false); 
                        }
                        else
                        {
                            for(uint8_t i = 20; i < 40; i++)
                            {
                                receive_data[i] = *param->write.value++;
                            }
                            data_c++;
                        }

                        if(data_c >= 1 && data_b >= 1 && data_a >= 1)
                        {
                            printf("\nrecive_data:%s",receive_data);

					        Rece_data_flag = 1;

                            //  printf("\nRunStatus:%s\nTreatMode:%s\nTreatValue:%s\nPulseWide:%s\nIntermission:%s\nTreatTime:%s\nTime:%s\nDate:%s\n",RunStatus,TreatMode,TreatValue,PulseWide,Intermission,TreatTime,Time,Date);
                            //??ok

                            //ESP_LOGI(GATTS_TABLE_TAG, "??OK");
                            data_a = 0;
                            data_b = 0;
                            data_c = 0;
                        }
		           }
		//		}

               /*      uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
                   if (descr_value == 0x0001){
                        ESP_LOGI(TAG, "notify enable");
                        uint8_t notify_data[15];
                        for (int i = 0; i < sizeof(notify_data); ++i)
                        {
                            notify_data[i] = i % 0xff;
                        }
                        //the size of notify_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, heart_rate_handle_table[IDX_CHAR_VAL_A],
                                                sizeof(notify_data), notify_data, false);
                    }else if (descr_value == 0x0002){
                        ESP_LOGI(TAG, "indicate enable");
                        uint8_t indicate_data[15];
                        for (int i = 0; i < sizeof(indicate_data); ++i)
                        {
                            indicate_data[i] = i % 0xff;
                        }
                        //the size of indicate_data[] need less than MTU size
                        esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, heart_rate_handle_table[IDX_CHAR_VAL_A],
                                            sizeof(indicate_data), indicate_data, true);
                    }
                    else if (descr_value == 0x0000){
                        ESP_LOGI(TAG, "notify/indicate disable ");
                    }else{
                        ESP_LOGE(TAG, "unknown descr value");
                        esp_log_buffer_hex(TAG, param->write.value, param->write.len);
                    }

                }
                //send response when param->write.need_rsp is true
                if (param->write.need_rsp){
                    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
                }*/
            }
            else
            {
                /* handle prepare write */
                example_prepare_write_event_env(gatts_if, &prepare_write_env, param);
            }
      	    break;
        case ESP_GATTS_EXEC_WRITE_EVT: 
            // the length of gattc prepare write data must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX. 
            ESP_LOGI(TAG, "ESP_GATTS_EXEC_WRITE_EVT");
            example_exec_write_event_env(&prepare_write_env, param);
            break;
        case ESP_GATTS_MTU_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
            break;
        case ESP_GATTS_CONF_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d", param->conf.status, param->conf.handle);
            break;
        case ESP_GATTS_START_EVT:
            ESP_LOGI(TAG, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
            break;
        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
            esp_log_buffer_hex(TAG, param->connect.remote_bda, 6);
            esp_ble_conn_update_params_t conn_params = {0};
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            /* For the iOS system, please refer to Apple official documents about the BLE connection parameters restrictions. */
            conn_params.latency = 0;
            conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
            conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
            conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
            //start sent the update connection parameters to the peer device.
            esp_ble_gap_update_conn_params(&conn_params);

            heart_rate_profile_tab[PROFILE_APP_IDX].connected = 1;
            
            break;
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
            esp_ble_gap_start_advertising(&adv_params);

            heart_rate_profile_tab[PROFILE_APP_IDX].connected = 0;
            
            break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:{
            if (param->add_attr_tab.status != ESP_GATT_OK){
                ESP_LOGE(TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
            }
            else if (param->add_attr_tab.num_handle != HRS_IDX_NB){
                ESP_LOGE(TAG, "create attribute table abnormally, num_handle (%d) \
                        doesn't equal to HRS_IDX_NB(%d)", param->add_attr_tab.num_handle, HRS_IDX_NB);
            }
            else {
                ESP_LOGI(TAG, "create attribute table successfully, the number handle = %d\n",param->add_attr_tab.num_handle);
                memcpy(heart_rate_handle_table, param->add_attr_tab.handles, sizeof(heart_rate_handle_table));
                esp_ble_gatts_start_service(heart_rate_handle_table[IDX_SVC]);
            }
            break;
        }
        case ESP_GATTS_STOP_EVT:
        case ESP_GATTS_OPEN_EVT:
        case ESP_GATTS_CANCEL_OPEN_EVT:
        case ESP_GATTS_CLOSE_EVT:
        case ESP_GATTS_LISTEN_EVT:
        case ESP_GATTS_CONGEST_EVT:
        case ESP_GATTS_UNREG_EVT:
        case ESP_GATTS_DELETE_EVT:
        default:
            break;
    }
}


static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    
        /* If event is register event, store the gatts_if for each profile */
        if (event == ESP_GATTS_REG_EVT) {
            if (param->reg.status == ESP_GATT_OK) {
                heart_rate_profile_tab[PROFILE_APP_IDX].gatts_if = gatts_if;
            } else {
                ESP_LOGE(TAG, "reg app failed, app_id %04x, status %d",
                        param->reg.app_id,
                        param->reg.status);
                return;
            }
        }
 
        int idx;
           for (idx = 0; idx < PROFILE_NUM; idx++) 
           {
               /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
               if (gatts_if == ESP_GATT_IF_NONE || gatts_if == heart_rate_profile_tab[idx].gatts_if) 
               {
                    if (heart_rate_profile_tab[idx].gatts_cb) 
                    {
                        heart_rate_profile_tab[idx].gatts_cb(event, gatts_if, param); 
                    } 
       
                }
                if(Rece_data_flag == 1)
                {
                    OpenPackge(receive_data);
                    int free_buff_num = esp_ble_get_cur_sendable_packets_num(param->write.conn_id);
                    if(free_buff_num >0){
                    esp_ble_gatts_send_indicate(gatts_if,
                                                param->write.conn_id,
                                                heart_rate_handle_table[IDX_CHAR_VAL_A],
                                                sizeof(data_ok),
                                                data_ok,
                                                false);
                    }
                    Rece_data_flag = 0;
                }
           }
		  

}
static char notify_buf[300];

void ble_task(void *arg)
{
    esp_err_t err;
    int32_t ret;

    ret = ble_cmd_init();
    if(ret < 0)
    {
        goto exit;
    }

    /* Initialize NVS. */
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    err = esp_bt_controller_init(&bt_cfg);
    if (err) {
        ESP_LOGE(TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(err));
        return;
    }

    err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (err) {
        ESP_LOGE(TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(err));
        return;
    }

    err = esp_bluedroid_init();
    if (err) {
        ESP_LOGE(TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(err));
        return;
    }

    err = esp_bluedroid_enable();
    if (err) {
        ESP_LOGE(TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(err));
        return;
    }

    err = esp_ble_gatts_register_callback(gatts_event_handler);
    if (err){
        ESP_LOGE(TAG, "gatts register error, error code = %x", err);
        return;
    }

    err = esp_ble_gap_register_callback(gap_event_handler);
    if (err){
        ESP_LOGE(TAG, "gap register error, error code = %x", err);
        return;
    }

    err = esp_ble_gatts_app_register(ESP_APP_ID);
    if (err){
        ESP_LOGE(TAG, "gatts app register error, error code = %x", err);
        return;
    }

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

    // send_param_queue = xQueueCreate(10, sizeof(sti_send_param_t));
    // if(send_param_queue == NULL)
    // {
    //     return;
    // }

    while(1)
    {

        // sti_send_param_t send_param;
        // int32_t length=0;
        memset(notify_buf, 0, sizeof(notify_buf));

        int32_t length = update_notify_data(notify_buf);

        int32_t i=0;

        while(1)
        {
            

            if (heart_rate_profile_tab[PROFILE_APP_IDX].connected) 
            {
                // if(xQueueReceive(send_param_queue, &send_param, 0) != pdFALSE)
                // {
                //     ESP_LOGI(TAG, "update send param");

                //     int32_t length_temp = packet_param(&send_param, notify_buf);

                //     if(length_temp > 0)
                //     {
                //         length = length_temp;
                //     }
                // }

                
                if(i < length)
                {
                    if(i==0) 
                    {
                        printf("%s\n", notify_buf);
                    }

                    int free_buff_num = esp_ble_get_cur_sendable_packets_num(heart_rate_profile_tab[PROFILE_APP_IDX].conn_id);
                    printf("free_buff_num %d\n", free_buff_num);
                    if(free_buff_num > 0)
                    {
                        esp_ble_gatts_send_indicate(heart_rate_profile_tab[PROFILE_APP_IDX].gatts_if,
                                                    heart_rate_profile_tab[PROFILE_APP_IDX].conn_id,
                                                    heart_rate_handle_table[IDX_CHAR_VAL_A],
                                                    (length-i<20) ? (length-i) : 20,
                                                    (uint8_t *)(notify_buf+i),
                                                    false);
                        i += 20;
                    }
                    else{
                        vTaskDelay(pdMS_TO_TICKS(50));
                         ESP_LOGE(TAG, "ble buff <= 0");
                    }

//                    esp_ble_gatts_send_indicate(heart_rate_profile_tab[PROFILE_APP_IDX].gatts_if,
//                                                heart_rate_profile_tab[PROFILE_APP_IDX].conn_id,
//                                                heart_rate_handle_table[IDX_CHAR_VAL_A],
//                                                (length-i<20) ? (length-i) : 20,
//                                                (uint8_t *)(notify_buf+i),
//                                                false);
                }
                else
                {
                    i = 0;
                    break;
                }
            }
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

exit:
    ESP_LOGE(TAG, "error: %d", ret);

    if(queue_recv != NULL)
    {
        vQueueDelete(queue_recv);
        queue_recv = NULL;
    }

    vTaskDelete(NULL);
}

uint8_t gatts_is_connected(void)
{
    if(heart_rate_profile_tab[PROFILE_APP_IDX].connected == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


static int32_t update_notify_data(char *data)
{
    char box[]= "0,71Wh,%d,19010001,%c,1,%1.2f,%c,%.0f,%.0f,%.0f,%.1f,%.1f,10:41:30,May 27 2020,V1.0,\r\n\0";
    
    int32_t dev_type = sysenv_get_device_mode();
    
    char status;
    sti_status_t sti_status = sti_get_running_status();
    
    if((STI_STATUS_BUSY == sti_status) || (STI_STATUS_ABORTING == sti_status) || (STI_STATUS_STARTING == sti_status))
    {
        status = '1';
    }
    else
    {
        status = '0';
    }

    float bat;
    mon_get_battery_v(&bat);
    bat = bat/1000;

    char wave = '0';
    float current = 0;
    uint32_t time = 0;
    float imp = 0;
    float freq = 0;
    float offset = 0;

    waveform_t *wave_ptr;
    sti_context_t *ctx = sti_get_active_ctx();
    if(ctx != NULL)
    {
        wave_ptr = ctx->ports[0]->seq->wave;
        switch(wave_ptr->id)
        {
            case TDCS_WAVE_ID:
                wave = '1';
                mon_get_current(&current);
                // wave_get_dc_cur(wave_ptr, &current);
            break;

            case TACS_WAVE_ID:
                wave = '3';
                tacs_get_current(wave_ptr, &current);
                tacs_get_freq(wave_ptr, &freq);
                tacs_get_offset(wave_ptr, &offset);
            break;

            case CES_WAVE_ID:
                wave = '4';
                ces_get_current(wave_ptr, &current);
            break;

            default:
                wave = '0';
                tdcs_get_current(wave_ptr, &current);
            break;
        }
        
        sti_get_running_time_sec(&time);
        mon_get_impedance(&imp);

        if(imp < 0)
        {
            imp = 0;
        }
        else if(imp > 99.9)
        {
            imp = 99.9;
        }

    }
    else
    {
        wave = 0;
        current = 0;
        time = 0;
        imp = 0;
    }
    
    return 1 + sprintf((char*)data, box, dev_type, status, bat, wave, current, freq, offset, time, imp);
}

int32_t ble_check_cmd(void)
{
    if(queue_recv == NULL)
    {
        return -1;
    }

    ble_config_t ble_cfg;
    BaseType_t bt_ret = xQueuePeek(queue_recv, &ble_cfg, 0);
    if(bt_ret == pdPASS)
    {
        return ble_cfg.wave_id;
    }
    else
    {
        return 0;
    }
}


int32_t ble_get_cmd(ble_config_t *ble_cfg)
{
    if(queue_recv == NULL)
    {
        return -1;
    }

    BaseType_t bt_ret = xQueueReceive(queue_recv, ble_cfg, 0);
    if(bt_ret == pdPASS)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void ble_skip_cmd(void)
{
    ble_config_t ble_cfg;
    ble_get_cmd(&ble_cfg);
}

static void gen_raw_adv_data(void)
{
    uint8_t *ptr = &raw_adv_data[10];

    *ptr = 1 + sprintf((char*)(ptr + 2), "%s", device_name) + 1;
    *(ptr + 1) = 0x09;
    ptr += (1 + *ptr);

    raw_adv_data_len = ptr - raw_adv_data;

    ESP_LOGI(TAG, "adv_data name-> %s", &raw_adv_data[12]);
}

static int32_t ble_cmd_init(void)
{
    queue_recv = xQueueCreate(10, sizeof(ble_config_t));
    if(queue_recv == NULL)
    {
        return -1;
    }

    sprintf(device_name, "DEV%d", sysenv_get_device_id());

    memcpy(device_uid, sysenv_get_uid_a(), 12);
    device_uid[12] = '\0';

    gen_raw_adv_data();
    
    return 0;
}

static void push_cmd(ble_config_t *ble_cfg)
{
    if(queue_recv == NULL)
    {
        return;
    }

    BaseType_t bt_ret = xQueueSend(queue_recv, ble_cfg, pdMS_TO_TICKS(10));
    if(bt_ret != pdTRUE)
    {
        ESP_LOGW(TAG, "push cmd failed.");
    }
}
