#ifndef BLE_JAWBONE__H
#define BLE_JAWBONE__H

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"




#define  UUID_JAWBONE_SERVICE  								 	 0xfff0
#define  UUID_DRUG_NAME													 0XFFF1
#define  UUID_TRIGGER_EVENT_CHAR			      		 0XFFF2
#define  UUID_TRIGGER_EVENT_HISTORY_CHAR				 0XFFF3
#define  UUID_TRIGGER_EVENT_HISTORY_RESET_CHAR   0XFFF4
#define  UUID_TRIGGER_EVENT_HISTORY_CLEAR_CHAR   0XFFF5
#define  UUID_DEVICE_TIME												 0XFFF6
#define  UUID_UPDATA_DEVICE_TIME								 0XFFF7


/**@brief Battery Service event type. */
typedef enum
{
    BLE_SW_EVT_NOTIFICATION_ENABLED,                             /**< Battery value notification enabled event. */
    BLE_SW_EVT_NOTIFICATION_DISABLED                             /**< Battery value notification disabled event. */
} ble_sw_evt_type_t;

/**@brief Battery Service event. */
typedef struct
{
    ble_sw_evt_type_t evt_type;                                  /**< Type of event. */
} ble_sw_evt_t;

// Forward declaration of the ble_jawbone_t type. 
typedef struct ble_jawbone_s ble_jawbone_t;

/**@brief Battery Service event handler type. */
typedef void (*ble_jawbone_evt_handler_t) (ble_jawbone_t * p_jawbone, ble_sw_evt_t * p_evt);

/**@brief Battery Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_jawbone_evt_handler_t     evt_handler;                    /**< Event handler to be called for handling events in the Battery Service. */
    bool                          support_notification;           /**< TRUE if notification of Battery Level measurement is supported. */
    ble_srv_report_ref_t *        p_report_ref;                   /**< If not NULL, a Report Reference descriptor with the specified value will be added to the Battery Level characteristic */

} ble_jawbone_init_t;

/**@brief Battery Service structure. This contains various status information for the service. */
struct ble_jawbone_s
{
    ble_jawbone_evt_handler_t     evt_handler;                    /**< Event handler to be called for handling events in the Battery Service. */
    uint16_t                      service_handle;                 /**< Handle of Battery Service (as provided by the BLE stack). */
		ble_gatts_char_handles_t      TriggerEvent_handle;
		ble_gatts_char_handles_t      DrugName_handle;
		ble_gatts_char_handles_t			TriggerEventHistory_handle;
		ble_gatts_char_handles_t      DeviceTime_handle;
		ble_gatts_char_handles_t      TriggerEventHistoryReset_handle;
//    ble_gatts_char_handles_t      battery_level_handles;          /**< Handles related to the Battery Level characteristic. */
//    uint16_t                      report_ref_handle;              /**< Handle of the Report Reference descriptor. */
//    uint8_t                       battery_level_last;             /**< Last Battery Level measurement passed to the Battery Service. */
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    bool                          is_notification_supported;      /**< TRUE if notification of Battery Level is supported. */
};

void ble_jawbone_on_ble_evt(ble_jawbone_t * p_jawbone, ble_evt_t * p_ble_evt);
uint32_t ble_jawbone_init(ble_jawbone_t * p_jawbone, const ble_jawbone_init_t * p_jawbone_init);
uint32_t TriggerEventData_Send(ble_jawbone_t * p_jawbone, uint8_t *TiggerEventData);




#endif


