#include <stdint-gcc.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

// UUID: 8C:86:4B:49-9A:27-47:2F-A9:28-BD:9C:54:4C:74:8E (colon-separated for clarity)
#define CUSTOM_SERVICE_UUID_BASE {0x8E, 0x74, 0x4C, 0x54, 0x9C, 0xBD, 0x28, 0xA9, \
                                  0x2F, 0x47, 0x27, 0x9A, 0x49, 0x4B, 0x86, 0x8C}

#define CUSTOM_SERVICE_UUID 0x1400
#define CUSTOM_VALUE_CHAR_UUID 0x1401

/**
 * @brief Custom Service init structure. This contains all options and data needed for initialization of the service.
 */
typedef struct
{
    uint8_t initial_custom_value;                           /**< Initial custom value */
    ble_srv_cccd_security_mode_t custom_value_char_attr_md; /*< Initial security level for Custom characteristics attribute */
} ble_cus_init_t;

/**
 * @brief Custom Service structure. This contains various status information for the service.
 */
struct ble_cus_s
{
    uint16_t service_handle;                       /**< Handle of Custom Service. */
    ble_gatts_char_handles_t custom_value_handles; /**< Handles related to the Custom Value characteristic. */
    uint16_t conn_handle;                          /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t uuid_type;
};

// Forward declaration of the ble_cus_t type.
typedef struct ble_cus_s ble_cus_t;

/**
 * @brief Macro for defining a ble_cus instance.
 * 
 * @param _name Name of the instance.
 * @hideinitializer
 */
#define BLE_CUS_DEF(_name) static ble_cus_t _name;

/**
 * @brief Function for initializing the Custom Service.
 * 
 * @param[out] p_cus Custom Service structure. This structure will have to be supplied by the applization. It will be initialized by this function, and will later be used to identify this particular service instance.
 * 
 * @param[in] p_cus_init Information needed to initialize the service.
 * 
 * @return NRF_SUCCESS on successful initialization of service, otherwise ar error code.
 */
uint32_t ble_cus_init(ble_cus_t *p_cus, const ble_cus_init_t *p_cus_init);