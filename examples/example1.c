#include <stdio.h>
#include <binaryiotypea.h>
#include <malloc.h>

int main(void)
{
    io4e_err_t err;
    malloc_stats();
    io4edge_functionblock_client_t *client;
    if ((err = io4edge_functionblock_client_new(&client, "192.168.24.213", 10002)) != IO4E_OK) {
        printf("Failed to create client: %d\n", err);
        return 1;
    }
    // malloc_stats();

    BinaryIoTypeA__ConfigurationSet config = BINARY_IO_TYPE_A__CONFIGURATION_SET__INIT;
    config.outputwatchdogmask = 0x1;
    config.outputwatchdogtimeout = 1000;

    if ((err = io4edge_binaryiotypea_upload_configuration(client, &config)) != IO4E_OK) {
        printf("Failed to upload configuration: %d\n", err);
        return 1;
    }
    // printf("after upload\n");
    // malloc_stats();

    BinaryIoTypeA__ConfigurationGetResponse dl_config;
    if ((err = io4edge_binaryiotypea_download_configuration(client, &dl_config)) != IO4E_OK) {
        printf("Failed to upload configuration: %d\n", err);
        return 1;
    }
    printf("Downloaded configuration: %d %d\n", dl_config.outputwatchdogmask, dl_config.outputwatchdogtimeout);
    // printf("after first download\n");
    // malloc_stats();
    if ((err = io4edge_binaryiotypea_download_configuration(client, &dl_config)) != IO4E_OK) {
        printf("Failed to upload configuration: %d\n", err);
        return 1;
    }
    // printf("after second download\n");
    // malloc_stats();

    io4edge_functionblock_client_delete(&client);
    printf("after delete\n");
    // malloc_stats();

    return 0;
}
