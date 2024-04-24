#include <stdint.h>
#include <string.h>

#include "esp_log.h"

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#define TAG "sysenv"

#define NVS_NAMESPACE    "std"

#define NVS_MODE_KEY     "dev_mode"
#define NVS_TS_KEY     "dev_ts"
#define NVS_DEVID_KEY  "dev_id"
#define NVS_DEVNAME_KEY "dev_name"

static int32_t dev_mode = 0; //tDCS
static int32_t dev_ts = 1; //true
static int32_t dev_id = 0; 
static char *dev_name = NULL;
static char *dev_uid = NULL;


void sysenv_load(void)
{
    esp_err_t err;
    nvs_handle nvs_handle;

    uint8_t mac[6];
    
    err = esp_efuse_mac_get_default(mac);
    if(err != ESP_OK)
    {
        ESP_ERROR_CHECK(err);
    }
    char *uid = calloc(1, 13);
    if(uid == NULL)
    {
        ESP_ERROR_CHECK(ESP_FAIL);
    }
    sprintf(uid, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    dev_uid = uid;

    err = nvs_flash_init();
    if(err != ESP_OK)
    {
        return;
    }
    
    // if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    //     // NVS partition was truncated and needs to be erased
    //     // Retry nvs_flash_init
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     err = nvs_flash_init();
    // }
    // ESP_ERROR_CHECK( err );

    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if(err != ESP_OK)
    {
        //ESP_ERROR_CHECK(err);
        return;
    }

    err = nvs_get_i32(nvs_handle, NVS_MODE_KEY, &dev_mode);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "load_id: get mode failed.(%d)", err);
    }


    err = nvs_get_i32(nvs_handle, NVS_TS_KEY, &dev_ts);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "load_id: get ts failed.(%d)", err);
    }


    err = nvs_get_i32(nvs_handle, NVS_DEVID_KEY, &dev_id);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "load_id: get device id failed.(%d)", err);
    }


    // size_t len;
    // err = nvs_get_str(nvs_handle, NVS_DEVNAME_KEY, NULL, &len);
    // if(err == ESP_OK)
    // {
    //     char *str = (char*)calloc(1, len);
    //     if(str == NULL)
    //     {
    //         ESP_ERROR_CHECK(ESP_FAIL);
    //     }
    //     err = nvs_get_str(nvs_handle, NVS_DEVNAME_KEY, str, &len);
    //     if(err != ESP_OK)
    //     {
    //         ESP_LOGE(TAG, "load_id: get device name failed.(%d)", err);
    //     }
    //     dev_name = str;
    // }
    // ESP_LOGE(TAG, "load_id: get device name length failed.(%d)", err);

    nvs_close(nvs_handle);
}


int32_t sysenv_save_device_mode(int32_t mode)
{
    esp_err_t err;
    int32_t ret = 0;
    nvs_handle nvs_handle;

    // err = nvs_flash_init();
    // if(err != ESP_OK)
    // {
    //     ESP_ERROR_CHECK(err);
    // }

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if(err != ESP_OK)
    {
        return -1;
    }

    err = nvs_set_i32(nvs_handle, NVS_MODE_KEY, mode);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "load_id: set device mode failed.(%d)", err);
        ret = -2;
        goto exit;
    }

    ESP_LOGI(TAG, "save device mode(%d) success.", mode);

exit:
    nvs_close(nvs_handle);
    return ret;
}


int32_t sysenv_get_device_mode(void)
{
    return dev_mode;
}

int32_t sysenv_save_ts(int32_t ts)
{
    esp_err_t err;
    int32_t ret = 0;
    nvs_handle nvs_handle;

    // err = nvs_flash_init();
    // if(err != ESP_OK)
    // {
    //     ESP_ERROR_CHECK(err);
    // }

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if(err != ESP_OK)
    {
        return -1;
    }

    err = nvs_set_i32(nvs_handle, NVS_TS_KEY, ts);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "save device ts failed.(%d)", err);
        ret = -2;
        goto exit;
    }
    ESP_LOGI(TAG, "save device ts(%d) success.", ts);

exit:
    nvs_close(nvs_handle);
    return ret;
}


int32_t sysenv_get_ts(void)
{
    return dev_ts;
}


int32_t sysenv_save_device_id(int32_t id)
{
    esp_err_t err;
    int32_t ret = 0;
    nvs_handle nvs_handle;

    // err = nvs_flash_init();
    // if(err != ESP_OK)
    // {
    //     ESP_ERROR_CHECK(err);
    // }

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if(err != ESP_OK)
    {
        return -1;
    }

    err = nvs_set_i32(nvs_handle, NVS_DEVID_KEY, id);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "load_id: set device id failed.(%d)", err);
        ret = -2;
        goto exit;
    }
    ESP_LOGI(TAG, "save device id(%d) success.", id);

exit:
    nvs_close(nvs_handle);
    return ret;
}

int32_t sysenv_get_device_id(void)
{
    return dev_id;
}

int32_t sysenv_save_device_name(char *name)
{
    if(name == NULL)
    {
        return -1;
    }

    esp_err_t err;
    int32_t ret = 0;
    nvs_handle nvs_handle;

    // err = nvs_flash_init();
    // if(err != ESP_OK)
    // {
    //     ESP_ERROR_CHECK(err);
    // }

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if(err != ESP_OK)
    {
        return -1;
    }

    err = nvs_set_str(nvs_handle, NVS_DEVNAME_KEY, name);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "load_id: set device name failed.(%d)", err);
        ret = -2;
        goto exit;
    }
    ESP_LOGI(TAG, "save device name(%s) success.", name);

exit:
    nvs_close(nvs_handle);
    return ret;
}
const char* sysenv_get_device_name(void)
{
    return dev_name;
}

const char* sysenv_get_uid_a(void)
{
    return dev_uid;
}