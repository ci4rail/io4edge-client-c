#include <stdio.h>
#include <binaryiotypea.h>

int main(void)
{
    io4e_err_t err;
    io4edge_functionblock_client_t *client;
    if ((err = io4edge_functionblock_client_new(&client, "192.168.24.213", 10002)) != IO4E_OK) {
        printf("Failed to create client: %d\n", err);
        return 1;
    }

    BinaryIoTypeA__ConfigurationSet config = BINARY_IO_TYPE_A__CONFIGURATION_SET__INIT;
    config.outputwatchdogmask = 0x1;
    config.outputwatchdogtimeout = 1000;

    if ((err = io4edge_binaryiotypea_upload_configuration(client, &config)) != IO4E_OK) {
        printf("Failed to upload configuration: %d\n", err);
        return 1;
    }
    return 0;
}
