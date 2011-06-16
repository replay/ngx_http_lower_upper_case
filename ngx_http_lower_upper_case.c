#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define UPPER 0
#define LOWER 1

typedef struct {
    ngx_conf_t                     *cf;
    ngx_array_t                    *lucases;
} ngx_http_lower_upper_case_conf_t;

typedef struct {
    uintptr_t                       action:1;
    uintptr_t                       src_variable_index;
    uintptr_t                       dst_variable_index;
} ngx_http_lucase_t;

// create confs
static void *ngx_http_lower_upper_case_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_lower_upper_case_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

static char *ngx_http_lower_upper_directive(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_do_lower_upper(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);


static ngx_command_t ngx_http_lower_upper_case_commands[] = {
  { ngx_string("string_to_uppercase"),
    NGX_HTTP_LOC_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
    ngx_http_lower_upper_directive,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },
  { ngx_string("string_to_lowercase"),
    NGX_HTTP_LOC_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
    ngx_http_lower_upper_directive,
    NGX_HTTP_LOC_CONF_OFFSET,
    0,
    NULL },
    ngx_null_command
};

static ngx_http_module_t ngx_http_lower_upper_case_module_ctx = {
    NULL,
    NULL,

    NULL,
    NULL,

    NULL,
    NULL,

    ngx_http_lower_upper_case_create_loc_conf,
    ngx_http_lower_upper_case_merge_loc_conf
};

ngx_module_t ngx_http_lower_upper_case_module = {
    NGX_MODULE_V1,
    &ngx_http_lower_upper_case_module_ctx,
    ngx_http_lower_upper_case_commands,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING
};

static void *
ngx_http_php_session_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_lower_upper_case_conf_t   *lucf;

    lucf = ngx_pcalloc(cf->pool, sizeof(ngx_http_lower_upper_case_conf_t));
    if (lucf == NULL)
    {
        return NGX_CONF_ERROR;
    }
    
    lucf->cf = cf;
    lucf->lucases = NULL;

    return lucf;
}

static char *
ngx_http_lower_upper_case_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_lower_upper_case_conf_t   *prev = parent;
    ngx_http_lower_upper_case_conf_t   *conf = child;

    if (prev->lucases != NULL)
    {
            conf->lucases = prev->lucases;
    }

    return NGX_CONF_OK;
}

static char *
ngx_http_lower_upper_directive(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_lower_upper_case_conf_t       *lucf = conf;
    ngx_http_lucase_t                      *lucase;
    ngx_str_t                              *variable;
    ngx_http_variable_t                    *v;

    if (lucf->lucases == NULL) {
        lucf->lucases = ngx_array_create(cf->pool, 1, sizeof(ngx_http_lucase_t));
        if (lucf->lucases == NULL) {
            return NGX_CONF_ERROR;
        }
    }

    lucase = ngx_array_push(lucf->lucases);
    if (lucase == NULL) {
        return NGX_CONF_ERROR;
    }

    variable = cf->args->elts;
    if (variable[1].data[0] != '$') {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "invalid variable name \"%V\"", variable[1]);
        return NGX_CONF_ERROR;
    }

    variable[1].len--;
    variable[1].data++;

    v = ngx_http_add_variable(cf, &variable[1], NGX_HTTP_VAR_CHANGEABLE);
    if (v == NULL) {
        return NGX_CONF_ERROR;
    }

    lucase->src_variable_index = ngx_http_get_variable_index(cf, &variable[2]);

    if (variable[0].data[0] == 'u') {
        lucase->action = UPPER;
    } else {
        lucase->action = LOWER;
    }

    lucase->dst_variable_index = ngx_http_get_variable_index(cf, &variable[1]);

    v->get_handler = ngx_http_do_lower_upper;

    return NGX_CONF_OK;
}


static ngx_int_t
ngx_http_do_lower_upper(ngx_http_request_t *r, ngx_http_variable_value_t *dst_v, uintptr_t data)
{
    ngx_http_lower_upper_case_conf_t       *lucf = ngx_http_get_module_loc_conf(r, ngx_http_lower_upper_case_module);
    ngx_uint_t                              i;
    u_char                                 *tmp_void;
    ngx_http_lucase_t                      *tmp_value;
    ngx_http_lucase_t                      *lucase;
    ngx_http_variable_value_t              *src_v;

    tmp_void = (u_char*) lucf->lucases->elts;

    for (i = 0; i < lucf->lucases->nelts; i++) {
        tmp_value = (ngx_http_lucase_t*) tmp_void;
        if (tmp_value->dst_variable_index == data) {
            lucase = tmp_value;
            break;
        }
    }
    if (i == lucf->lucases->nelts) {
        return NGX_ERROR;
    }
    
    src_v = ngx_http_get_indexed_variable(r, lucase->src_variable_index);

    dst_v->len = src_v->len;

    dst_v->data = ngx_pcalloc(r->pool, src_v->len);
    if (dst_v->data == NULL) {
        return NGX_ERROR;
    }

    ngx_cpymem(dst_v->data, src_v->data, dst_v->len);

    if (lucase->action == LOWER) {
        for (i = 0; i < dst_v->len; i++) {
            ngx_tolower(dst_v->data[i]);
        }
    } else {
        for (i = 0; i < dst_v->len; i++) {
            ngx_toupper(dst_v->data[i]);
        }
    }

    dst_v->valid = 1;

    return NGX_OK;
}
