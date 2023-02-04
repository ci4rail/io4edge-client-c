#include "io4edge_internal.h"
#include "logging.h"

const char *TAG = "functionblock";

io4e_err_t io4edge_functionblock_client_new(io4edge_functionblock_client_t **handle_p, const char *host, int port)
{
    io4e_err_t err;
    // allocate memory for the functionblock client
    io4edge_functionblock_client_t *h = calloc(1, sizeof(io4edge_functionblock_client_t));
    if (h == NULL) {
        return IO4E_ERR_NO_MEM;
    }

    if ((err = io4edge_transport_new(host, port, &h->transport)) != IO4E_OK)
        goto ERREXIT;

    *handle_p = h;
ERREXIT:
    if (err != IO4E_OK) {
        free(h);
    }
    return err;
}

io4e_err_t io4edge_functionblock_client_delete(io4edge_functionblock_client_t **handle_p)
{
    if (*handle_p != NULL) {
        io4edge_functionblock_client_t *h = *handle_p;
        h->transport->destroy(&h->transport);
        free(h);
        *handle_p = NULL;
    }
    return IO4E_OK;
}

static io4e_err_t command(io4edge_functionblock_client_t *client,
    const Functionblock__Command *cmd,
    Functionblock__Response **res_p)
{
    io4e_err_t err;
    size_t marshaled_cmd_len = functionblock__command__get_packed_size(cmd);
    uint8_t marshaled_cmd_buffer[marshaled_cmd_len];
    functionblock__command__pack(cmd, marshaled_cmd_buffer);

    if ((err = client->transport->write_msg(client->transport, marshaled_cmd_buffer, marshaled_cmd_len)) != IO4E_OK) {
        IO4E_LOGE(TAG, "command write failed: %d", err);
        return err;
    }
    uint32_t res_len;
    uint8_t res_buffer[1024];  // TODO: allocate in read_msg

    if ((err = client->transport->read_msg(
             client->transport, res_buffer, sizeof(res_buffer), &res_len, client->cmd_timeout)) != IO4E_OK) {
        IO4E_LOGE(TAG, "command read failed: %d", err);
        return err;
    }
    Functionblock__Response *res = functionblock__response__unpack(NULL, res_len, res_buffer);
    if (res == NULL) {
        IO4E_LOGE(TAG, "command unpack failed");
        return IO4E_ERR_PROTOBUF;
    }
    if (res_p)
        *res_p = res;
    return IO4E_OK;
}

io4e_err_t io4e_functionblock_upload_configuration(io4edge_functionblock_client_t *client,
    uint8_t *marshaled_fs_config,
    size_t marshaled_fs_config_len,
    char *fs_proto_name)
{
    Google__Protobuf__Any pb_any = GOOGLE__PROTOBUF__ANY__INIT;
    pb_any.type_url = fs_proto_name;
    pb_any.value.len = marshaled_fs_config_len;
    pb_any.value.data = marshaled_fs_config;

    Functionblock__Configuration pb_config = FUNCTIONBLOCK__CONFIGURATION__INIT;
    pb_config.action_case = FUNCTIONBLOCK__CONFIGURATION__ACTION_FUNCTION_SPECIFIC_CONFIGURATION_SET;
    pb_config.functionspecificconfigurationset = &pb_any;

    Functionblock__Command pb_cmd = FUNCTIONBLOCK__COMMAND__INIT;
    pb_cmd.type_case = FUNCTIONBLOCK__COMMAND__TYPE_CONFIGURATION;
    pb_cmd.configuration = &pb_config;

    return command(client, &pb_cmd, NULL);
}
