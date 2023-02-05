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

static io4e_err_t map_fb_error(Functionblock__Status status)
{
    switch (status) {
    case FUNCTIONBLOCK__STATUS__OK:
        return IO4E_OK;
    case FUNCTIONBLOCK__STATUS__UNSPECIFIC_ERROR:
        return IO4E_ERR_FB_UNSPECIFIC_ERROR;
    case FUNCTIONBLOCK__STATUS__UNKNOWN_COMMAND:
        return IO4E_ERR_FB_UNKNOWN_COMMAND;
    case FUNCTIONBLOCK__STATUS__NOT_IMPLEMENTED:
        return IO4E_ERR_FB_NOT_IMPLEMENTED;
    case FUNCTIONBLOCK__STATUS__WRONG_CLIENT:
        return IO4E_ERR_FB_WRONG_CLIENT;
    case FUNCTIONBLOCK__STATUS__INVALID_PARAMETER:
        return IO4E_ERR_FB_INVALID_PARAMETER;
    case FUNCTIONBLOCK__STATUS__HW_FAULT:
        return IO4E_ERR_FB_HW_FAULT;
    case FUNCTIONBLOCK__STATUS__STREAM_ALREADY_STARTED:
        return IO4E_ERR_FB_STREAM_ALREADY_STARTED;
    case FUNCTIONBLOCK__STATUS__STREAM_ALREADY_STOPPED:
        return IO4E_ERR_FB_STREAM_ALREADY_STOPPED;
    case FUNCTIONBLOCK__STATUS__STREAM_START_FAILED:
        return IO4E_ERR_FB_STREAM_START_FAILED;
    case FUNCTIONBLOCK__STATUS__TEMPORARILY_UNAVAILABLE:
        return IO4E_ERR_FB_TEMPORARILY_UNAVAILABLE;
    default:
        return IO4E_ERR_FB_UNKNOWN;
    }
}

// caller must free the response if it is not NULL
static io4e_err_t command(io4edge_functionblock_client_t *client,
    const Functionblock__Command *cmd,
    Functionblock__Response **res_p)
{
    io4e_err_t err;
    size_t marshaled_cmd_len = functionblock__command__get_packed_size(cmd);
    size_t header_size = client->transport->get_write_offset(client->transport);
    uint8_t marshaled_cmd_buffer[header_size + marshaled_cmd_len];
    functionblock__command__pack(cmd, &marshaled_cmd_buffer[header_size]);

    if (res_p)
        *res_p = NULL;
    if ((err = client->transport->write_msg(
             client->transport, marshaled_cmd_buffer, marshaled_cmd_len + header_size)) != IO4E_OK) {
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
    // check status
    if (res->status != FUNCTIONBLOCK__STATUS__OK) {
        IO4E_LOGE(TAG,
            "command failed: %s: %s",
            functionblock__status__descriptor.values[res->status].name,
            res->error->error);
        functionblock__response__free_unpacked(res, NULL);

        return map_fb_error(res->status);
    }

    if (res_p)
        *res_p = res;
    else
        functionblock__response__free_unpacked(res, NULL);

    return IO4E_OK;
}

// #define SET_ANY

io4e_err_t io4e_functionblock_upload_configuration(io4edge_functionblock_client_t *client, io4e_protomsg_t *fs_config)
{
    Google__Protobuf__Any pb_any = GOOGLE__PROTOBUF__ANY__INIT;
    pb_any.type_url = (char *)fs_config->proto_name;
    pb_any.value.len = fs_config->marshaled_msg_len;
    pb_any.value.data = fs_config->marshaled_msg;

    Functionblock__Configuration pb_config = FUNCTIONBLOCK__CONFIGURATION__INIT;
    pb_config.action_case = FUNCTIONBLOCK__CONFIGURATION__ACTION_FUNCTION_SPECIFIC_CONFIGURATION_SET;
    pb_config.functionspecificconfigurationset = &pb_any;

    Functionblock__Command pb_cmd = FUNCTIONBLOCK__COMMAND__INIT;
    pb_cmd.type_case = FUNCTIONBLOCK__COMMAND__TYPE_CONFIGURATION;
    pb_cmd.configuration = &pb_config;

    return command(client, &pb_cmd, NULL);
}
