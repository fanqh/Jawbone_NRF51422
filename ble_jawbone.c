#include "ble_jawbone.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "stdio.h"
#include "stdbool.h"


#define TRIGGER_EVENT_DATA_LEN  5
#define DEVICE_TIME_LEN  4

 uint8_t Drug_Name[] = "LA1";


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_jawbone       Battery Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_jawbone_t * p_jawbone, ble_evt_t * p_ble_evt)
{
    p_jawbone->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_jawbone       Battery Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_jawbone_t * p_jawbone,  ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_jawbone->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling the Write event.
 *
 * @param[in]   p_jawbone       Battery Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_jawbone_t * p_jawbone, ble_evt_t * p_ble_evt)
{
	  if (p_jawbone->is_notification_supported)
    {
        ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

        if ((p_evt_write->handle == p_jawbone->TriggerEvent_handle.cccd_handle))
            /////&&(p_evt_write->len == 5)//////////????????????????????????????????????????????
        {
            // CCCD written, call application event handler
            if (p_jawbone->evt_handler != NULL)
            {
                ble_sw_evt_t evt;

                if (ble_srv_is_notification_enabled(p_evt_write->data))
                {
                    evt.evt_type = BLE_SW_EVT_NOTIFICATION_ENABLED;
                }
                else
                {
                    evt.evt_type = BLE_SW_EVT_NOTIFICATION_DISABLED;
                }

                p_jawbone->evt_handler(p_jawbone, &evt);
            }
        }
    }
}


void ble_jawbone_on_ble_evt(ble_jawbone_t * p_jawbone, ble_evt_t * p_ble_evt)
{

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_jawbone, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_jawbone, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
						printf("BLE_GATTS_EVT_WRITE\r\n");
						if(p_ble_evt->evt.gatts_evt.params.write.handle==p_jawbone->TriggerEventHistoryReset_handle.value_handle)
							printf("p_ble_evt->evt.gatts_evt.write.handle =%d\r\n", p_ble_evt->evt.gatts_evt.params.write.handle);
            on_write(p_jawbone, p_ble_evt);
            break;

        default:
            break;
    }
}


/*
	UUID_DRUG_NAME
	uuid : oxfff1
	perm: read
*/
static uint32_t jawbone_char_DrugName_add(ble_jawbone_t * p_jawbone, const ble_jawbone_init_t * p_jawbone_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
		ble_srv_cccd_security_mode_t  char_attr_security_mode_md;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&char_attr_security_mode_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&char_attr_security_mode_md.write_perm);

    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read   = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_DRUG_NAME);

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = char_attr_security_mode_md.read_perm;
    attr_md.write_perm = char_attr_security_mode_md.cccd_write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(Drug_Name);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(Drug_Name);
		attr_char_value.p_value = Drug_Name;

    err_code = sd_ble_gatts_characteristic_add(p_jawbone->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_jawbone->DrugName_handle);	
		if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		return NRF_SUCCESS;
}	



/*
	TriggerEvent
	uuid : oxfff2
	perm: notify
*/
static uint32_t jawbone_char_TriggerEvent_add(ble_jawbone_t * p_jawbone, const ble_jawbone_init_t * p_jawbone_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
		ble_srv_cccd_security_mode_t  char_attr_security_mode_md;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&char_attr_security_mode_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&char_attr_security_mode_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&char_attr_security_mode_md.write_perm);

    // Add TriggerEvent characteristic
    if (p_jawbone->is_notification_supported)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));

        // According to BAS_SPEC_V10, the read operation on cccd should be possible without
        // authentication.
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        cccd_md.write_perm = char_attr_security_mode_md.cccd_write_perm;
        cccd_md.vloc       = BLE_GATTS_VLOC_STACK;
    }
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = (p_jawbone->is_notification_supported) ? 1 : 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = (p_jawbone->is_notification_supported) ? &cccd_md : NULL;
    char_md.p_sccd_md         = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_TRIGGER_EVENT_CHAR);

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = char_attr_security_mode_md.read_perm;
    attr_md.write_perm = char_attr_security_mode_md.cccd_write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = TRIGGER_EVENT_DATA_LEN;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = TRIGGER_EVENT_DATA_LEN;

    err_code = sd_ble_gatts_characteristic_add(p_jawbone->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_jawbone->TriggerEvent_handle);	
		if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		return NRF_SUCCESS;
}	


/*
	TriggerEventHistory
	uuid : oxfff3
	perm: read
*/
static uint32_t jawbone_char_TriggerEventHistory_add(ble_jawbone_t * p_jawbone, const ble_jawbone_init_t * p_jawbone_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
		ble_srv_cccd_security_mode_t  char_attr_security_mode_md;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&char_attr_security_mode_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&char_attr_security_mode_md.write_perm);

    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read   = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_TRIGGER_EVENT_HISTORY_CHAR);

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = char_attr_security_mode_md.read_perm;
    attr_md.write_perm = char_attr_security_mode_md.cccd_write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = TRIGGER_EVENT_DATA_LEN;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = TRIGGER_EVENT_DATA_LEN;
		attr_char_value.p_value = "abcde"; /////////////////////////////////////////////////////

    err_code = sd_ble_gatts_characteristic_add(p_jawbone->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_jawbone->TriggerEventHistory_handle);	
		if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		return NRF_SUCCESS;
}	


/*
	TriggerEventHistoryReset
	uuid : oxfff4
	perm: write
*/
static uint32_t jawbone_char_TriggerEventHistoryReset_add(ble_jawbone_t * p_jawbone, const ble_jawbone_init_t * p_jawbone_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
		ble_srv_cccd_security_mode_t  char_attr_security_mode_md;

    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&char_attr_security_mode_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&char_attr_security_mode_md.write_perm);
	

    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read   = 0;
		char_md.char_props.write   = 1;
		char_md.char_props.write_wo_resp = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_TRIGGER_EVENT_HISTORY_RESET_CHAR);

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = char_attr_security_mode_md.read_perm;
    attr_md.write_perm = char_attr_security_mode_md.cccd_write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 0;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = TRIGGER_EVENT_DATA_LEN;
		attr_char_value.p_value = "abcde"; /////////////////////////////////////////////////////

    err_code = sd_ble_gatts_characteristic_add(p_jawbone->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_jawbone->TriggerEventHistoryReset_handle);	
		if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		return NRF_SUCCESS;
}	

/*
	DEVICE_TIME
	uuid : oxfff6
	perm: read
*/
static uint32_t jawbone_char_DEVICE_TIME_add(ble_jawbone_t * p_jawbone, const ble_jawbone_init_t * p_jawbone_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
		ble_srv_cccd_security_mode_t  char_attr_security_mode_md;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&char_attr_security_mode_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&char_attr_security_mode_md.write_perm);

    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read   = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_DEVICE_TIME);

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = char_attr_security_mode_md.read_perm;
    attr_md.write_perm = char_attr_security_mode_md.cccd_write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = DEVICE_TIME_LEN;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = DEVICE_TIME_LEN;
		attr_char_value.p_value = "abcd"; /////////////////////////////////////////////////////

    err_code = sd_ble_gatts_characteristic_add(p_jawbone->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_jawbone->DeviceTime_handle);	
		if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		return NRF_SUCCESS;
}

uint32_t ble_jawbone_init(ble_jawbone_t * p_jawbone, const ble_jawbone_init_t * p_jawbone_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;


    // Initialize service structure
    p_jawbone->evt_handler               = p_jawbone->evt_handler;
    p_jawbone->conn_handle               = BLE_CONN_HANDLE_INVALID;
    p_jawbone->is_notification_supported = p_jawbone_init->support_notification;


    // Add service									
    BLE_UUID_BLE_ASSIGN(ble_uuid, UUID_JAWBONE_SERVICE);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_jawbone->service_handle);
    if (err_code != NRF_SUCCESS)
        return err_code;

    // Add TriggerEvent characteristic
		err_code = jawbone_char_TriggerEvent_add(p_jawbone, p_jawbone_init);
		if (err_code != NRF_SUCCESS)
        return err_code;
		// Add DrugName characteristic
		err_code = jawbone_char_DrugName_add(p_jawbone, p_jawbone_init);
		if (err_code != NRF_SUCCESS)
        return err_code;
				// Add DrugName characteristic
		err_code = jawbone_char_TriggerEventHistoryReset_add(p_jawbone, p_jawbone_init);
		if (err_code != NRF_SUCCESS)
        return err_code;
		
		return NRF_SUCCESS;
		
}


uint32_t TriggerEventData_Send(ble_jawbone_t * p_jawbone, uint8_t *TiggerEventData)
{
    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

		// Initialize value struct.
		memset(&gatts_value, 0, sizeof(gatts_value));

		gatts_value.len     = TRIGGER_EVENT_DATA_LEN;
		gatts_value.offset  = 0;
		gatts_value.p_value = TiggerEventData;

		// Update database.
		err_code = sd_ble_gatts_value_set(p_jawbone->conn_handle,
																			p_jawbone->TriggerEvent_handle.value_handle,
																			&gatts_value);
	
	
	
	
/**************************************************************************	 ///内部数据写操作
			gatts_value.len     = sizeof(Drug_Name);;
		gatts_value.offset  = 0;
		gatts_value.p_value = "123";
		err_code = sd_ble_gatts_value_set(p_jawbone->conn_handle,
																			p_jawbone->DrugName_handle.value_handle,
																			&gatts_value);
	
*****************************************************************************/	
	
	
	
	
	
	
	
	
	
	
		if (err_code != NRF_SUCCESS)
		{
				return err_code;
		}

		// Send value if connected and notifying.
		if ((p_jawbone->conn_handle != BLE_CONN_HANDLE_INVALID) && p_jawbone->is_notification_supported)
		{
				ble_gatts_hvx_params_t hvx_params;

				memset(&hvx_params, 0, sizeof(hvx_params));

				hvx_params.handle = p_jawbone->TriggerEvent_handle.value_handle;
				hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
				hvx_params.offset = gatts_value.offset;
				hvx_params.p_len  = &gatts_value.len;
				hvx_params.p_data = gatts_value.p_value;

				err_code = sd_ble_gatts_hvx(p_jawbone->conn_handle, &hvx_params);
		}
		else
		{
				err_code = NRF_ERROR_INVALID_STATE;
		}

    return err_code;
}




