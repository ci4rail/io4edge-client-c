#include <stdio.h>
#include <io4edge_client.h>

int main(void)
{
    io4e_err_t err;
    io4edge_functionblock_client_t *client;
    if ((err = io4edge_functionblock_client_new(&client, "192.168.24.213", 10002)) != IO4E_OK) {
        printf("Failed to create client: %d\n", err);
        return 1;
    }
}
