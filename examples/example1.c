#include <stdio.h>
#include <binaryiotypea.h>
#include <malloc.h>

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

    BinaryIoTypeA__ConfigurationGetResponse *dl_config;
    if ((err = io4edge_binaryiotypea_download_configuration(client, &dl_config)) != IO4E_OK) {
        printf("Failed to upload configuration: %d\n", err);
        return 1;
    }
    printf("Downloaded configuration: %d %d\n", dl_config->outputwatchdogmask, dl_config->outputwatchdogtimeout);
    binary_io_type_a__configuration_get_response__free_unpacked(dl_config, NULL);

#if 0
    if ((err = io4edge_binaryiotypea_set_output(client, 0, true)) != IO4E_OK) {
        printf("Failed to set output: %d\n", err);
        return 1;
    }

    if ((err = io4edge_binaryiotypea_set_all_outputs(client, 0x1, 0x1)) != IO4E_OK) {
        printf("Failed to set all outputs: %d\n", err);
        return 1;
    }
#endif

    bool state;
    if ((err = io4edge_binaryiotypea_input(client, 0, &state)) != IO4E_OK) {
        printf("Failed to get input: %d\n", err);
        return 1;
    }
    printf("Input state: %d\n", state);

    io4edge_functionblock_client_delete(&client);

    return 0;
}
