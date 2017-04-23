#include "../includes.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*****************************************************
_vi通道配置加载流程：
1、加载和初始化vi的json时，都以vi配置的jso数组作为接口的实参；
2、接口内部通过通道的vi_json_id去指定加载json数组中的固定的配置；
3、


*****************************************************/

int core_channel_init_vi_cfg(int channel_index, cJSON *arry_json)
{
	if (channel_index < 0 || channel_index >= g_av_platform_ctx.m_vichn_cnt || !arry_json)
	{
		return AV_ERR_INVALID_PARAM;
	}
	if (CHANNEL_TYPE_LOCAL_CHANNEL != g_av_platform_ctx.m_all_channel_ptr[channel_index]->m_channel_type)
	{
		DBG_PRT("chn[%d] is vi chn\n");
		return AV_ERR_INVALID_PARAM;
	}
	cJSON *sub_json;
	char str[32];
	vi_cfg_t *p_vi_cfg = (vi_cfg_t *)&g_av_platform_ctx.m_local_channel_ptr[channel_index].m_cfg.m_vi_cfg;
	
	p_vi_cfg->m_mask0 = 0xff0000;
	p_vi_cfg->m_mask1 = 0x0;
	p_vi_cfg->m_width = HD_WIDTH;
	p_vi_cfg->m_height = HD_HEIGHT;
	p_vi_cfg->m_fps = 30;
	p_vi_cfg->m_interface = BT1120I;
	p_vi_cfg->m_videv = channel_index - VI_CHN_START;
	p_vi_cfg->m_vichn = (channel_index - VI_CHN_START) * 4;
	p_vi_cfg->m_colorspace = YUV420;
	p_vi_cfg->m_mirrorflip = VI_CHN_SET_NORMAL;
	p_vi_cfg->m_interlaced = 0;//_逐行
	p_vi_cfg->m_src_fps = -1;
	p_vi_cfg->m_dst_fps = -1;
	sub_json = cJSON_CreateObject();
	if (!sub_json)
	{
		return AV_ERR_OPEN_JSON_FAILED;
	}
	snprintf(str, sizeof(str), "0x%x", p_vi_cfg->m_mask0);
	cJSON_AddStringToObject(sub_json,"mask0", str);
	memset(str, 0, sizeof(str));
	snprintf(str, sizeof(str), "0x%x", p_vi_cfg->m_mask1);
	cJSON_AddStringToObject(sub_json,"mask1", str);
	cJSON_AddNumberToObject(sub_json,"width", p_vi_cfg->m_width);
	cJSON_AddNumberToObject(sub_json,"height", p_vi_cfg->m_width);
	cJSON_AddNumberToObject(sub_json,"fps", p_vi_cfg->m_fps);
	cJSON_AddNumberToObject(sub_json,"interface", p_vi_cfg->m_interface);
	cJSON_AddNumberToObject(sub_json,"videv", p_vi_cfg->m_videv);
	cJSON_AddNumberToObject(sub_json,"vichn", p_vi_cfg->m_vichn);
	cJSON_AddNumberToObject(sub_json,"colorspace", p_vi_cfg->m_colorspace);
	cJSON_AddNumberToObject(sub_json,"mirrorflip", p_vi_cfg->m_mirrorflip);
	cJSON_AddNumberToObject(sub_json,"src_fps", p_vi_cfg->m_src_fps);
	cJSON_AddNumberToObject(sub_json,"dst_fps", p_vi_cfg->m_dst_fps);
	cJSON_AddItemToArray(arry_json, sub_json);
	return AV_OK;
}

int core_channel_uninit_vi_cfg()
{

	return AV_OK;
}

int core_channel_load_vi_cfg(int channel_index, int vi_josn_id, cJSON *arry_json)
{
	if (channel_index < 0 || channel_index >= g_av_platform_ctx.m_vichn_cnt || \
		vi_josn_id < 0 || vi_josn_id >= g_av_platform_ctx.m_vichn_cnt || arry_json)
	{
		return AV_ERR_INVALID_PARAM;
	}
	channel_data_t *p_chn_data = g_av_platform_ctx.m_all_channel_ptr[channel_index];
	channel_cfg_t *p_chn_cfg = &p_chn_data->m_cfg;

	if (CHANNEL_TYPE_LOCAL_CHANNEL != p_chn_data->m_channel_type)
	{
		DBG_PRT("chn[%d] is vi chn\n");
		return AV_ERR_INVALID_PARAM;
	}
	cJSON *json, *sub_json;
	char str[32];
	vi_cfg_t *p_vi_cfg = (vi_cfg_t *)&p_chn_cfg->m_vi_cfg;

	json = cJSON_GetArrayItem(arry_json, vi_josn_id);
	if (!sub_json)
	{
		return AV_FALSE;
	}
	sub_json = cJSON_GetObjectItem(json, "mask0");
	if (!sub_json)
	{
		p_vi_cfg->m_mask0 = 0xff0000;
		snprintf(str, sizeof(str), "0x%x", p_vi_cfg->m_mask0);
		cJSON_AddStringToObject(sub_json,"mask0", str);
	}
	else
	{
		StrToNumber(sub_json->valuestring, &p_vi_cfg->m_mask0);
	}
	sub_json = cJSON_GetObjectItem(json, "mask1");
	if (!sub_json)
	{
		p_vi_cfg->m_mask0 = 0xff0000;
		snprintf(str, sizeof(str), "0x%x", p_vi_cfg->m_mask0);
		cJSON_AddStringToObject(sub_json,"mask1", str);
	}
	else
	{
		StrToNumber(sub_json->valuestring, &p_vi_cfg->m_mask1);
	}
	sub_json = cJSON_GetObjectItem(json, "width");
	if (!sub_json)
	{
		p_vi_cfg->m_width = HD_WIDTH;
		cJSON_AddNumberToObject(sub_json,"width", p_vi_cfg->m_width);
	}
	else
	{
		p_vi_cfg->m_width = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "height");
	if (!sub_json)
	{
		p_vi_cfg->m_height = HD_HEIGHT;
		cJSON_AddNumberToObject(sub_json,"height", p_vi_cfg->m_width);
	}
	else
	{
		p_vi_cfg->m_height = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "fps");
	if (!sub_json)
	{
		p_vi_cfg->m_fps = 30;
		cJSON_AddNumberToObject(sub_json,"fps", p_vi_cfg->m_width);
	}
	else
	{
		p_vi_cfg->m_fps = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "interface");
	if (!sub_json)
	{
		p_vi_cfg->m_interface = BT1120I;
		cJSON_AddNumberToObject(sub_json,"interface", p_vi_cfg->m_interface);
	}
	else
	{
		p_vi_cfg->m_interface = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "videv");
	if (!sub_json)
	{
		p_vi_cfg->m_videv = channel_index - VI_CHN_START;
		cJSON_AddNumberToObject(sub_json,"videv", p_vi_cfg->m_videv);
	}
	else
	{
		p_vi_cfg->m_videv = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "vichn");
	if (!sub_json)
	{
		p_vi_cfg->m_vichn = (channel_index - VI_CHN_START) * 4;
		cJSON_AddNumberToObject(sub_json,"vichn", p_vi_cfg->m_vichn);
	}
	else
	{
		p_vi_cfg->m_vichn = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "colorspace");
	if (!sub_json)
	{
		p_vi_cfg->m_colorspace = YUV420;
		cJSON_AddNumberToObject(sub_json,"colorspace", p_vi_cfg->m_colorspace);
	}
	else
	{
		p_vi_cfg->m_colorspace = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "mirrorflip");
	if (!sub_json)
	{
		p_vi_cfg->m_mirrorflip = VI_CHN_SET_NORMAL;
		cJSON_AddNumberToObject(sub_json,"mirrorflip", p_vi_cfg->m_mirrorflip);
	}
	else
	{
		p_vi_cfg->m_mirrorflip = (vi_chn_set_e)sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "interlaced");
	if (!sub_json)
	{
		p_vi_cfg->m_interlaced = 0;
		cJSON_AddNumberToObject(sub_json,"interlaced", p_vi_cfg->m_interlaced);
	}
	else
	{
		p_vi_cfg->m_interlaced = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "src_fps");
	if (!sub_json)
	{
		p_vi_cfg->m_src_fps = 0;
		cJSON_AddNumberToObject(sub_json,"src_fps", p_vi_cfg->m_src_fps);
	}
	else
	{
		p_vi_cfg->m_src_fps = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "dst_fps");
	if (!sub_json)
	{
		p_vi_cfg->m_dst_fps = 0;
		cJSON_AddNumberToObject(sub_json,"dst_fps", p_vi_cfg->m_dst_fps);
	}
	else
	{
		p_vi_cfg->m_dst_fps = sub_json->valueint;
	}
	
	return AV_OK;
}

int core_channel_save_vpss_cfg(int channel_index, cJSON *json)
{
	if (channel_index < 0 || channel_index >= g_av_platform_ctx.m_vichn_cnt || !json)
	{
		return AV_ERR_INVALID_PARAM;
	}
	cJSON *sub_json;
	channel_data_t *p_chn_data = g_av_platform_ctx.m_all_channel_ptr[channel_index];
	channel_cfg_t *p_chn_cfg = &p_chn_data->m_cfg;
	vpss_cfg_t *p_vpss_cfg = &p_chn_cfg->m_vpss_cfg;

	sub_json = cJSON_GetObjectItem(json, "cfStrength");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"cfStrength", p_vpss_cfg->m_cfStrength);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_cfStrength);
	}
	sub_json = cJSON_GetObjectItem(json, "contrast");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"contrast", p_vpss_cfg->m_contrast);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_contrast);
	}
	sub_json = cJSON_GetObjectItem(json, "cTfStrength");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"cTfStrength", p_vpss_cfg->m_cTfStrength);
	}
	else
	{
		cJSON_SetNumberValue(sub_json,p_vpss_cfg->m_cTfStrength);
	}
	sub_json = cJSON_GetObjectItem(json, "cvbsStrength");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"cvbsStrength", p_vpss_cfg->m_cvbsStrength);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_cvbsStrength);
	}
	sub_json = cJSON_GetObjectItem(json, "deMotionBlurring");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"deMotionBlurring", p_vpss_cfg->m_deMotionBlurring);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_deMotionBlurring);
	}
	sub_json = cJSON_GetObjectItem(json, "dieStrength");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"dieStrength", p_vpss_cfg->m_dieStrength);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_dieStrength);
	}
	sub_json = cJSON_GetObjectItem(json, "en_dci");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"en_dci", p_vpss_cfg->m_en_dci);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_en_dci);
	}
	sub_json = cJSON_GetObjectItem(json, "en_die_mode");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"en_die_mode", p_vpss_cfg->m_en_die_mode);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_en_die_mode);
	}
	sub_json = cJSON_GetObjectItem(json, "en_es");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"en_es", p_vpss_cfg->m_en_es);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_en_es);
	}
	sub_json = cJSON_GetObjectItem(json, "en_hist");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"en_hist", p_vpss_cfg->m_en_hist);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_en_hist);
	}
	sub_json = cJSON_GetObjectItem(json, "en_ie");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"en_ie", p_vpss_cfg->m_en_ie);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_en_ie);
	}
	sub_json = cJSON_GetObjectItem(json, "en_nr");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"en_nr", p_vpss_cfg->m_en_nr);
	}
	else
	{
		cJSON_SetNumberValue(sub_json,p_vpss_cfg->m_en_nr);
	}
	sub_json = cJSON_GetObjectItem(json, "group_number");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"group_number", p_vpss_cfg->m_group_number);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_group_number);
	}
	sub_json = cJSON_GetObjectItem(json, "ieStrength");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"ieStrength", p_vpss_cfg->m_ieStrength);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_ieStrength);
	}
	sub_json = cJSON_GetObjectItem(json, "max_height");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"max_height", p_vpss_cfg->m_max_height);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_max_height);
	}
	sub_json = cJSON_GetObjectItem(json, "max_width");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"max_width", p_vpss_cfg->m_max_width);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_max_width);
	}
	sub_json = cJSON_GetObjectItem(json, "pixel_fmt_type");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"pixel_fmt_type", p_vpss_cfg->m_pixel_fmt_type);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_pixel_fmt_type);
	}
	sub_json = cJSON_GetObjectItem(json, "sfStrength");
	if (!sub_json)
	{
		cJSON_AddNumberToObject(sub_json,"sfStrength", p_vpss_cfg->m_sfStrength);
	}
	else
	{
		cJSON_SetNumberValue(sub_json, p_vpss_cfg->m_sfStrength);
	}
	//m_vpss_bind_cnt  不处理，开机一定是0
}

int core_channel_init_vpss_cfg(int channel_index)
{
	if (channel_index < 0 || channel_index >= g_av_platform_ctx.m_vichn_cnt)
	{
		return AV_ERR_INVALID_PARAM;
	}
	channel_data_t *p_chn_data = g_av_platform_ctx.m_all_channel_ptr[channel_index];
	channel_cfg_t *p_chn_cfg = &p_chn_data->m_cfg;
	vpss_cfg_t *p_vpss_cfg = &p_chn_cfg->m_vpss_cfg;

	p_vpss_cfg->m_max_width = HD_WIDTH;
	p_vpss_cfg->m_max_height = HD_HEIGHT;
	p_vpss_cfg->m_en_ie = 1;
	p_vpss_cfg->m_en_dci = 1;
	p_vpss_cfg->m_en_nr = 0;
	p_vpss_cfg->m_en_hist = 0;
	p_vpss_cfg->m_en_es = 0;
	p_vpss_cfg->m_en_die_mode = 0;
	p_vpss_cfg->m_pixel_fmt_type = PIXEL_FMT_TYPE_YUV420;
	return AV_OK;
}

int core_channel_unint_vpss_cfg(int channel_index, cJSON *json)
{
	if (channel_index < 0 || channel_index >= g_av_platform_ctx.m_vichn_cnt || !json)
	{
		return AV_ERR_INVALID_PARAM;
	}

	return AV_OK;
}

int core_channel_load_vpss_cfg(int channel_index, cJSON *json)
{
	if (channel_index < 0 || channel_index >= g_av_platform_ctx.m_vichn_cnt || !json)
	{
		return AV_ERR_INVALID_PARAM;
	}
	cJSON *sub_json;
	channel_data_t *p_chn_data = g_av_platform_ctx.m_all_channel_ptr[channel_index];
	channel_cfg_t *p_chn_cfg = &p_chn_data->m_cfg;
	vpss_cfg_t *p_vpss_cfg = &p_chn_cfg->m_vpss_cfg;

	sub_json = cJSON_GetObjectItem(json, "cfStrength");
	if (!sub_json)
	{
// 		p_vpss_cfg->m_cfStrength = 0;
// 		cJSON_AddNumberToObject(sub_json,"cfStrength", p_vpss_cfg->m_cfStrength);
	}
	else
	{
		p_vpss_cfg->m_cfStrength = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "contrast");
	if (!sub_json)
	{
		p_vpss_cfg->m_contrast = 0;
		cJSON_AddNumberToObject(sub_json,"contrast", p_vpss_cfg->m_contrast);
	}
	else
	{
		p_vpss_cfg->m_contrast = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "cTfStrength");
	if (!sub_json)
	{
		p_vpss_cfg->m_cTfStrength = 0;
		cJSON_AddNumberToObject(sub_json,"cTfStrength", p_vpss_cfg->m_cTfStrength);
	}
	else
	{
		p_vpss_cfg->m_cTfStrength = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "cvbsStrength");
	if (!sub_json)
	{
		p_vpss_cfg->m_cvbsStrength = 0;
		cJSON_AddNumberToObject(sub_json,"cvbsStrength", p_vpss_cfg->m_cvbsStrength);
	}
	else
	{
		p_vpss_cfg->m_cvbsStrength = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "deMotionBlurring");
	if (!sub_json)
	{
		p_vpss_cfg->m_deMotionBlurring = 0;
		cJSON_AddNumberToObject(sub_json,"deMotionBlurring", p_vpss_cfg->m_deMotionBlurring);
	}
	else
	{
		p_vpss_cfg->m_deMotionBlurring = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "dieStrength");
	if (!sub_json)
	{
		p_vpss_cfg->m_dieStrength = 0;
		cJSON_AddNumberToObject(sub_json,"dieStrength", p_vpss_cfg->m_dieStrength);
	}
	else
	{
		p_vpss_cfg->m_dieStrength = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "en_dci");
	if (!sub_json)
	{
		p_vpss_cfg->m_en_dci = 0;
		cJSON_AddNumberToObject(sub_json,"en_dci", p_vpss_cfg->m_en_dci);
	}
	else
	{
		p_vpss_cfg->m_en_dci = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "en_die_mode");
	if (!sub_json)
	{
		p_vpss_cfg->m_en_die_mode = 0;
		cJSON_AddNumberToObject(sub_json,"en_die_mode", p_vpss_cfg->m_en_die_mode);
	}
	else
	{
		p_vpss_cfg->m_en_die_mode = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "en_es");
	if (!sub_json)
	{
		p_vpss_cfg->m_en_es = 0;
		cJSON_AddNumberToObject(sub_json,"en_es", p_vpss_cfg->m_en_es);
	}
	else
	{
		p_vpss_cfg->m_en_es = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "en_hist");
	if (!sub_json)
	{
		p_vpss_cfg->m_en_hist = 0;
		cJSON_AddNumberToObject(sub_json,"en_hist", p_vpss_cfg->m_en_hist);
	}
	else
	{
		p_vpss_cfg->m_en_hist = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "en_ie");
	if (!sub_json)
	{
		p_vpss_cfg->m_en_ie = 0;
		cJSON_AddNumberToObject(sub_json,"en_ie", p_vpss_cfg->m_en_ie);
	}
	else
	{
		p_vpss_cfg->m_en_ie = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "en_nr");
	if (!sub_json)
	{
		p_vpss_cfg->m_en_nr = 0;
		cJSON_AddNumberToObject(sub_json,"en_nr", p_vpss_cfg->m_en_nr);
	}
	else
	{
		p_vpss_cfg->m_en_nr = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "group_number");
	if (!sub_json)
	{
		p_vpss_cfg->m_group_number = 0;
		cJSON_AddNumberToObject(sub_json,"group_number", p_vpss_cfg->m_group_number);
	}
	else
	{
		p_vpss_cfg->m_group_number = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "ieStrength");
	if (!sub_json)
	{
		p_vpss_cfg->m_ieStrength = 0;
		cJSON_AddNumberToObject(sub_json,"ieStrength", p_vpss_cfg->m_ieStrength);
	}
	else
	{
		p_vpss_cfg->m_ieStrength = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "max_height");
	if (!sub_json)
	{
		p_vpss_cfg->m_max_height = 0;
		cJSON_AddNumberToObject(sub_json,"max_height", p_vpss_cfg->m_max_height);
	}
	else
	{
		p_vpss_cfg->m_max_height = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "max_width");
	if (!sub_json)
	{
		p_vpss_cfg->m_max_width = 0;
		cJSON_AddNumberToObject(sub_json,"max_width", p_vpss_cfg->m_max_width);
	}
	else
	{
		p_vpss_cfg->m_max_width = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "pixel_fmt_type");
	if (!sub_json)
	{
		p_vpss_cfg->m_pixel_fmt_type = (pixel_fmt_type_t)0;
		cJSON_AddNumberToObject(sub_json,"pixel_fmt_type", p_vpss_cfg->m_pixel_fmt_type);
	}
	else
	{
		p_vpss_cfg->m_pixel_fmt_type = (pixel_fmt_type_t)sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(json, "sfStrength");
	if (!sub_json)
	{
		p_vpss_cfg->m_sfStrength = (pixel_fmt_type_t)0;
		cJSON_AddNumberToObject(sub_json,"sfStrength", p_vpss_cfg->m_sfStrength);
	}
	else
	{
		p_vpss_cfg->m_sfStrength = (pixel_fmt_type_t)sub_json->valueint;
	}
	//m_vpss_bind_cnt  不处理，开机一定是0

	return AV_OK;
}

int core_channel_save_cfg()
{

	return RETURN_OK;
}

int core_channel_load_cfg(int channel_index)
{
	if (channel_index < 0 || channel_index > g_av_platform_ctx.m_vichn_cnt + g_av_platform_ctx.m_filechn_cnt + g_av_platform_ctx.m_remotechn_cnt)
	{
		return AV_ERR_INVALID_PARAM;
	}
	char str[64];
	int ret = -1;
	cJSON *chn_json, *sub_json, *vi_cfg_json, *vpss_cfg_json;
	channel_data_t *p_chn_data = g_av_platform_ctx.m_all_channel_ptr[channel_index];
	channel_cfg_t *p_chn_cfg = &p_chn_data->m_cfg;

	memset(p_chn_cfg, 0, sizeof(channel_cfg_t));
	snprintf(str, sizeof(str), "%s/chn%d.json", CFG_FILE, channel_index);
	ret = core_tools_get_platform_json(str, &chn_json);
	if (AV_OK != ret)
	{
		DBG_PRT("core_tools_get_platform_json is failed with %d\n", ret);
		return AV_FALSE;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"raw_fps");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_raw_fps = -1;
		cJSON_AddNumberToObject(chn_json, "raw_fps", p_chn_cfg->m_raw_fps);
	}
	else
	{
		p_chn_cfg->m_raw_fps = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"raw_width");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_raw_width = HD_WIDTH;
		cJSON_AddNumberToObject(chn_json, "raw_width", p_chn_cfg->m_raw_width);
	}
	else
	{
		p_chn_cfg->m_raw_width = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"raw_height");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_raw_height = HD_HEIGHT;
		cJSON_AddNumberToObject(chn_json, "raw_height", p_chn_cfg->m_raw_height);
	}
	else
	{
		p_chn_cfg->m_raw_height = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"major_venc_fps");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_major_venc_fps = DEFAULT_MAJOR_FPS;
		cJSON_AddNumberToObject(chn_json, "major_venc_fps", p_chn_cfg->m_major_venc_fps);
	}
	else
	{
		p_chn_cfg->m_major_venc_fps = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"major_venc_width");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_major_venc_width = DEFAULT_MAJOR_FPS;
		cJSON_AddNumberToObject(chn_json, "major_venc_width", p_chn_cfg->m_major_venc_width);
	}
	else
	{
		p_chn_cfg->m_major_venc_width = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"major_venc_height");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_major_venc_height = DEFAULT_MAJOR_FPS;
		cJSON_AddNumberToObject(chn_json, "major_venc_height", p_chn_cfg->m_major_venc_height);
	}
	else
	{
		p_chn_cfg->m_major_venc_height = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"minor_venc_fps");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_minor_venc_fps = DEFAULT_MINOR_FPS;
		cJSON_AddNumberToObject(chn_json, "minor_venc_fps", p_chn_cfg->m_minor_venc_fps);
	}
	else
	{
		p_chn_cfg->m_minor_venc_fps = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"minor_venc_width");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_minor_venc_width = 640;
		cJSON_AddNumberToObject(chn_json, "minor_venc_width", p_chn_cfg->m_minor_venc_width);
	}
	else
	{
		p_chn_cfg->m_minor_venc_width = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"minor_venc_height");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_minor_venc_height = 360;
		cJSON_AddNumberToObject(chn_json, "minor_venc_height", p_chn_cfg->m_minor_venc_height);
	}
	else
	{
		p_chn_cfg->m_minor_venc_height = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"minor2_venc_fps");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_minor2_venc_fps = 360;
		cJSON_AddNumberToObject(chn_json, "minor2_venc_fps", p_chn_cfg->m_minor2_venc_fps);
	}
	else
	{
		p_chn_cfg->m_minor2_venc_fps = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"minor2_venc_width");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_minor2_venc_width = 960;
		cJSON_AddNumberToObject(chn_json, "minor2_venc_width", p_chn_cfg->m_minor2_venc_width);
	}
	else
	{
		p_chn_cfg->m_minor2_venc_width = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"minor2_venc_height");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_minor2_venc_height = 540;
		cJSON_AddNumberToObject(chn_json, "minor2_venc_height", p_chn_cfg->m_minor2_venc_height);
	}
	else
	{
		p_chn_cfg->m_minor2_venc_height = sub_json->valueint;
	}
	sub_json = cJSON_GetObjectItem(chn_json,"pix_format");
	if (sub_json == NULL)
	{
		DBG_PRT("cJSON_GetObjectItem raw_fps is failed,set defalut value\n");
		p_chn_cfg->m_pix_format = YUV420;
		cJSON_AddNumberToObject(chn_json, "pix_format", p_chn_cfg->m_pix_format);
	}
	else
	{
		p_chn_cfg->m_pix_format = sub_json->valueint;
	}
	if(0 == p_chn_cfg->m_reset_vi_cfg)
	{
		//_只有机器恢复出场设置后，才从硬件配置文件中加载vi参数，否则从通道配置文件中加载
		vi_cfg_json = cJSON_GetObjectItem(chn_json, VI_CHN_INFO);
		core_channel_load_vi_cfg(channel_index, 0, vi_cfg_json);	
	}
	else
	{
		cJSON *hardware_json;
		int hardware_id = 0;
		char str[64];

		ret = PareseFile_Read_ini_int(UPDATE_FILE, "[DESC]" ,"HARDWARE_ID", &hardware_id);
		if (AV_OK != ret)
		{
			DBG_PRT("PareseFile_Read_ini_int is failed with %d\n", ret);
		}
		snprintf(str, sizeof(str), "%s/%d", PLATFORM_FILE_PATH, hardware_id);
		ret = core_tools_get_platform_json(str, &hardware_json);
		if (AV_OK != ret)
		{
			DBG_PRT("core_tools_get_platform_json is failed with %d\n", ret);
		}
		else
		{
			vi_cfg_json = cJSON_GetObjectItem(hardware_json, VI_CHN_INFO);
			core_channel_init_vi_cfg(channel_index, vi_cfg_json);
			cJSON_Delete(hardware_json);
		}
	}
	if (1 == p_chn_cfg->m_vpss_had_init)
	{
		vpss_cfg_json = cJSON_GetObjectItem(chn_json, VPSS_INFO);
		core_channel_load_vpss_cfg(channel_index, vpss_cfg_json);
	}
	
	return RETURN_OK;
}

int core_channel_init_cfg(int channel_index)
{
	if (channel_index < 0 || channel_index > g_av_platform_ctx.m_vichn_cnt + g_av_platform_ctx.m_filechn_cnt + g_av_platform_ctx.m_remotechn_cnt)
	{
		return AV_ERR_INVALID_PARAM;
	}
	cJSON *hardware_json;
	cJSON *vi_cfg_json;
	int hardware_id = 0;
	char str[64];
	int ret = AV_FALSE;
	channel_data_t *p_chn_data = g_av_platform_ctx.m_all_channel_ptr[channel_index];
	channel_cfg_t *p_chn_cfg = &p_chn_data->m_cfg;

	memset(p_chn_cfg, 0, sizeof(channel_cfg_t));
	p_chn_cfg->m_raw_fps = -1;
	p_chn_cfg->m_raw_width = -1;
	p_chn_cfg->m_raw_height = -1;
	p_chn_cfg->m_major_venc_fps = DEFAULT_MAJOR_FPS;
	p_chn_cfg->m_major_venc_width = HD_WIDTH;
	p_chn_cfg->m_major_venc_height = HD_HEIGHT;
	p_chn_cfg->m_minor_venc_fps = DEFAULT_MINOR_FPS;
	p_chn_cfg->m_minor_venc_width = 640;
	p_chn_cfg->m_minor_venc_height = 360;
	p_chn_cfg->m_minor2_venc_fps = 25;
	p_chn_cfg->m_minor2_venc_width = 960;
	p_chn_cfg->m_minor2_venc_height = 540;
	p_chn_cfg->m_pix_format = YUV420;
	if (channel_index < VI_CHN_START || channel_index >= g_av_platform_ctx.m_vichn_cnt)
	{
		p_chn_cfg->m_vi_json_id = -1;
	}
	else
	{
		p_chn_cfg->m_vi_json_id = channel_index - VI_CHN_START;
	}
	p_chn_cfg->m_reset_vi_cfg = 0;
	ret = PareseFile_Read_ini_int(UPDATE_FILE, "[DESC]" ,"HARDWARE_ID", &hardware_id);
	if (AV_OK != ret)
	{
		DBG_PRT("PareseFile_Read_ini_int is failed with %d\n", ret);
	}
	snprintf(str, sizeof(str), "%s/%d", PLATFORM_FILE_PATH, hardware_id);
	ret = core_tools_get_platform_json(str, &hardware_json);
	if (AV_OK != ret)
	{
		DBG_PRT("core_tools_get_platform_json is failed with %d\n", ret);
	}	
	vi_cfg_json = cJSON_GetObjectItem(hardware_json, VI_CHN_INFO);
	core_channel_init_vi_cfg(channel_index, vi_cfg_json);
	core_channel_init_vpss_cfg(channel_index);
	
	return RETURN_OK;
}

int core_channel_uninit_cfg()
{

	return RETURN_OK;
}

int core_channel_local_setup()
{
	
	
	return RETURN_OK;
}

int core_channel_local_desetup()
{

	return RETURN_OK;
}