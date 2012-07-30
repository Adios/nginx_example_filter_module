#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_hash.h>

#define	IS_METHOD(x)									\
	r->method & NGX_HTTP_ ## x
#define	IS_CONTENT_TYPE(x)								\
	r->headers_in.content_type != NULL &&						\
	r->headers_in.content_type->value.data != NULL &&				\
	r->headers_in.content_type->value.len == sizeof ((x)) - 1 &&			\
	ngx_strcasecmp(r->headers_in.content_type->value.data, (u_char *) (x)) == 0

static void	*ngx_http_example_create_conf();
static char	*ngx_http_example_merge_conf();
static ngx_int_t ngx_http_example_filter_init();

typedef struct {
	ngx_flag_t enable;
} ngx_http_example_conf_t;

static ngx_command_t ngx_http_example_filter_commands[] = {
	{ ngx_string("example"), NGX_HTTP_LOC_CONF | NGX_CONF_FLAG, ngx_conf_set_flag_slot, NGX_HTTP_LOC_CONF_OFFSET, 0, NULL },
	ngx_null_command
};

static ngx_http_module_t ngx_http_example_filter_module_ctx = {
	NULL,				/* preconfiguration */
	ngx_http_example_filter_init,	/* postconfiguration */

	NULL,				/* create main configuration */
	NULL,				/* init main configuration */

	NULL,				/* create server configuration */
	NULL,				/* merge server configuration */

	ngx_http_example_create_conf,	/* create location configuration */
	ngx_http_example_merge_conf	/* merge location configuration */
};

ngx_module_t ngx_http_example_filter_module = {
	NGX_MODULE_V1,
	&ngx_http_example_filter_module_ctx,	/* module context */
	ngx_http_example_filter_commands,	/* module directives */
	NGX_HTTP_MODULE,			/* module type */
	NULL,					/* init master */
	NULL,					/* init module */
	NULL,					/* init process */
	NULL,					/* init thread */
	NULL,					/* exit thread */
	NULL,					/* exit process */
	NULL,					/* exit master */
	NGX_MODULE_V1_PADDING
};

static ngx_http_output_body_filter_pt ngx_http_next_body_filter;

static void example(ngx_http_request_t *r, ngx_chain_t *in)
{
	ngx_chain_t *chain_link;

	// print out all buffers on the buffer chain.
	for (chain_link = in; chain_link; chain_link = chain_link->next)
		ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "http example filter: %s", chain_link->buf->pos);
}

static ngx_int_t ngx_http_example_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
	ngx_http_example_conf_t *conf;

	conf = ngx_http_get_module_loc_conf(r, ngx_http_example_filter_module);

	if (conf->enable && IS_METHOD(POST) && IS_CONTENT_TYPE("application/x-www-form-urlencoded"))
	{
		ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "http example filter: lady go!");
		example(r, in);
	}
	return ngx_http_next_body_filter(r, in);
}

static ngx_int_t ngx_http_example_filter_init(ngx_conf_t *cf)
{
	ngx_http_next_body_filter = ngx_http_top_body_filter;
	ngx_http_top_body_filter = ngx_http_example_body_filter;

	return NGX_OK;
}

static void *ngx_http_example_create_conf(ngx_conf_t *cf)
{
	ngx_http_example_conf_t *conf;

	if ((conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_example_conf_t))) == NULL)
		return NGX_CONF_ERROR;
	conf->enable = NGX_CONF_UNSET;

	return conf;
}

static char *ngx_http_example_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
	ngx_http_example_conf_t *prev = parent;
	ngx_http_example_conf_t *conf = child;

	ngx_conf_merge_value(conf->enable, prev->enable, 0);

	return NGX_CONF_OK;
}
/* End */
// vim:ts=8 sts=8 sw=8:
