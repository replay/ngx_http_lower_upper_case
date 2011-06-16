#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <parser.c>

#define UPPER 0
#define LOWER 1

typedef struct {
    ngx_conf_t                     *cf;
    ngx_array_t                    *ulcases;
} ngx_http_lower_upper_case_conf_t;

typedef struct {
    uintptr_t                       action:1
    ngx_int_t                       src_variable_index;
    ngx_http_variable_t            *dst_variable;
} ngx_http_ulcase_t;

// create confs
static void *ngx_http_lower_upper_case_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_lower_upper_case_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

static char *ngx_http_lower_upper_directive(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);


static ngx_command_t ngx_http_lower_upper_case_commands[] = {
  { ngx_string("string_to_uppercase"),
    NGX_HTTP_LOC_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
    ngx_http_lower_upper_directive,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_http_lower_upper_case_conf_t, action),
    NULL },
  { ngx_string("string_to_lowercase"),
    NGX_HTTP_LOC_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
    ngx_http_lower_upper_directive,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_http_lower_upper_case_conf_t, action),
    NULL },
    ngx_null_command
  };
};

ngx_http_module_t ngx_http_lower_upper_case_module = {
    NULL,
    NULL,

    NULL,
    NULL,

    NULL,
    NULL,

    ngx_http_lower_upper_case_create_loc_conf,
    ngx_http_lower_upper_case_merge_loc_conf
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
    lucf->ulcases = NULL;

    return lucf;
}

static void *
ngx_http_lower_upper_case_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_lower_upper_case_conf_t   *prev = parent;
    ngx_http_lower_upper_case_conf_t   *conf = child;

    if (prev->ulcases != NULL)
    {
            conf->ulcases = prev->ulcases;
    }

    return NGX_CONF_OK;
}

static char *
ngx_http_lower_upper_directive(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_lower_upper_case_conf_t       *lucf = conf;
    ngx_http_ulcase_t                      *ulcase;
    ngx_str_t                               variable;
    ngx_http_variable_t                    *ngx_http_variable_t;

    if (ulcf->cases == NULL) {
        ulcf->cases = ngx_array_create(cf->pool, 1, sizeof(ngx_http_ulcase_t));
        if (ulcf->cases == NULL) {
            return NGX_CONF_ERROR;
        }
    }

    ulcase = ngx_array_push(lucf->cases);
    if (ulcase == NULL) {
        return NGX_CONF_ERROR;
    }

    variable = cf->args->elts;
    if (variable[1].data[0] != '$') {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0, "invalid variable name \"%V\"", &value[1]);
        return NGX_CONF_ERROR;
    }

    variable[1].len--;
    variable[1].data++;

    v = ngx_http_add_variable(cf, &variable[1], NGX_HTTP_VAR_CHANGEABLE);
    if (v == NULL) {
        return NGX_CONF_ERROR;
    }

    if (variable[0].data[0] == 's') {
        v->get_handler = ngx_http_to_uppercase;
    } else {
        v->get_handler = ngx_http_to_lowercase;
    }
    return NGX_CONF_OK;
}


static ngx_int_t
ngx_http_to_uppercase(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data)
{
    
}
