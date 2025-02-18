/* drivers/input/touchscreen/sec_ts_fn.c
 *
 * Copyright (C) 2015 Samsung Electronics Co., Ltd.
 * http://www.samsungsemi.com/
 *
 * Core file for Samsung TSC driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "sec_ts.h"

static void fw_update(void *device_data);
static void get_fw_ver_bin(void *device_data);
static void get_fw_ver_ic(void *device_data);
static void get_config_ver(void *device_data);
static void get_threshold(void *device_data);
static void module_off_master(void *device_data);
static void module_on_master(void *device_data);
static void get_chip_vendor(void *device_data);
static void get_chip_name(void *device_data);
static void get_mis_cal_info(void *device_data);
static void get_wet_mode(void *device_data);
static void get_x_num(void *device_data);
static void get_y_num(void *device_data);
static void get_checksum_data(void *device_data);
static void run_reference_read(void *device_data);
static void run_reference_read_all(void *device_data);
static void get_reference(void *device_data);
static void run_rawcap_read(void *device_data);
static void run_rawcap_read_all(void *device_data);
static void get_rawcap(void *device_data);
static void run_delta_read(void *device_data);
static void run_delta_read_all(void *device_data);
static void run_delta_cm_read_all(void *device_data);
static void get_delta(void *device_data);
static void run_decoded_raw_read_all(void *device_data);
static void run_delta_cm_read_all(void *device_data);
static void run_raw_p2p_read(void *device_data);
static void run_raw_p2p_min_read_all(void *device_data);
static void run_raw_p2p_max_read_all(void *device_data);
static void run_self_reference_read(void *device_data);
static void run_self_reference_read_all(void *device_data);
static void run_self_rawcap_read(void *device_data);
static void run_self_rawcap_read_all(void *device_data);
static void run_self_delta_read(void *device_data);
static void run_self_delta_read_all(void *device_data);
static void run_self_raw_p2p_min_read_all(void *device_data);
static void run_self_raw_p2p_max_read_all(void *device_data);
static void run_rawdata_read_all(void *device_data);
static void run_force_calibration(void *device_data);
static void get_force_calibration(void *device_data);
static void get_gap_data_x_all(void *device_data);
static void get_gap_data_y_all(void *device_data);
static void run_trx_short_test(void *device_data);

#ifdef TCLM_CONCEPT
static void set_external_factory(void *device_data);
static void get_pat_information(void *device_data);
static void tclm_test_cmd(void *device_data);
static void get_calibration(void *device_data);
#endif

static void set_tsp_test_result(void *device_data);
static void get_tsp_test_result(void *device_data);
static void clear_tsp_test_result(void *device_data);
/*static void run_lowpower_selftest(void *device_data);*/
static void increase_disassemble_count(void *device_data);
static void get_disassemble_count(void *device_data);
static void glove_mode(void *device_data);
static void clear_cover_mode(void *device_data);
static void dead_zone_enable(void *device_data);
static void drawing_test_enable(void *device_data);
static void set_lowpower_mode(void *device_data);
static void set_wirelesscharger_mode(void *device_data);
static void spay_enable(void *device_data);
static void set_aod_rect(void *device_data);
static void get_aod_rect(void *device_data);
static void aod_enable(void *device_data);
static void aot_enable(void *device_data);
static void fod_enable(void *device_data);
static void singletap_enable(void *device_data);
static void set_grip_data(void *device_data);
static void external_noise_mode(void *device_data);
static void brush_enable(void *device_data);
static void set_touchable_area(void *device_data);
static void set_log_level(void *device_data);
static void debug(void *device_data);
static void factory_cmd_result_all(void *device_data);
static void set_factory_level(void *device_data);
static void check_connection(void *device_data);
static void fix_active_mode(void *device_data);
static void touch_aging_mode(void *device_data);
static void set_rear_selfie_mode(void *device_data);
static void fp_int_control(void *device_data);
static void set_low_power_sensitivity(void *device_data);
static void not_support_cmd(void *device_data);

static int execute_selftest(struct sec_ts_data *ts, bool save_result);
static void sec_ts_print_frame(struct sec_ts_data *ts, short *min, short *max);

#ifdef MINORITY_REPORT
static void minority_report_calculate_rawdata(struct sec_ts_data *ts);
static void minority_report_calculate_ito(struct sec_ts_data *ts);
#endif

static struct sec_cmd sec_cmds[] = {
	{SEC_CMD("fw_update", fw_update),},
	{SEC_CMD("get_fw_ver_bin", get_fw_ver_bin),},
	{SEC_CMD("get_fw_ver_ic", get_fw_ver_ic),},
	{SEC_CMD("get_config_ver", get_config_ver),},
	{SEC_CMD("get_threshold", get_threshold),},
	{SEC_CMD("module_off_master", module_off_master),},
	{SEC_CMD("module_on_master", module_on_master),},
	{SEC_CMD("get_chip_vendor", get_chip_vendor),},
	{SEC_CMD("get_chip_name", get_chip_name),},
	{SEC_CMD("get_mis_cal_info", get_mis_cal_info),},
	{SEC_CMD("get_wet_mode", get_wet_mode),},
	{SEC_CMD("get_x_num", get_x_num),},
	{SEC_CMD("get_y_num", get_y_num),},
	{SEC_CMD("get_checksum_data", get_checksum_data),},
	{SEC_CMD("run_reference_read", run_reference_read),},
	{SEC_CMD("run_reference_read_all", run_reference_read_all),},
	{SEC_CMD("get_reference", get_reference),},
	{SEC_CMD("run_rawcap_read", run_rawcap_read),},
	{SEC_CMD("run_rawcap_read_all", run_rawcap_read_all),},
	{SEC_CMD("run_gap_data_x_all", get_gap_data_x_all),},
	{SEC_CMD("run_gap_data_y_all", get_gap_data_y_all),},
	{SEC_CMD("get_rawcap", get_rawcap),},
	{SEC_CMD("run_delta_read", run_delta_read),},
	{SEC_CMD("run_delta_read_all", run_delta_read_all),},
	{SEC_CMD("get_delta", get_delta),},
	{SEC_CMD("run_cs_raw_read_all", run_decoded_raw_read_all),},
	{SEC_CMD("run_cs_delta_read_all", run_delta_cm_read_all),},
	{SEC_CMD("run_raw_p2p_read", run_raw_p2p_read),},
	{SEC_CMD("run_raw_p2p_min_read_all", run_raw_p2p_min_read_all),},
	{SEC_CMD("run_raw_p2p_max_read_all", run_raw_p2p_max_read_all),},
	{SEC_CMD("run_self_reference_read", run_self_reference_read),},
	{SEC_CMD("run_self_reference_read_all", run_self_reference_read_all),},
	{SEC_CMD("run_self_rawcap_read", run_self_rawcap_read),},
	{SEC_CMD("run_self_rawcap_read_all", run_self_rawcap_read_all),},
	{SEC_CMD("run_self_delta_read", run_self_delta_read),},
	{SEC_CMD("run_self_delta_read_all", run_self_delta_read_all),},
	{SEC_CMD("run_self_raw_p2p_min_read_all", run_self_raw_p2p_min_read_all),},
	{SEC_CMD("run_self_raw_p2p_max_read_all", run_self_raw_p2p_max_read_all),},
	{SEC_CMD("run_rawdata_read_all_for_ghost", run_rawdata_read_all),},
	{SEC_CMD("run_force_calibration", run_force_calibration),},
	{SEC_CMD("get_force_calibration", get_force_calibration),},
	{SEC_CMD("get_gap_data_x_all", get_gap_data_x_all),},
	{SEC_CMD("get_gap_data_y_all", get_gap_data_y_all),},
	{SEC_CMD("run_trx_short_test", run_trx_short_test),},
#ifdef TCLM_CONCEPT
	{SEC_CMD("get_pat_information", get_pat_information),},
	{SEC_CMD("set_external_factory", set_external_factory),},
	{SEC_CMD("tclm_test_cmd", tclm_test_cmd),},
	{SEC_CMD("get_calibration", get_calibration),},
#endif
	{SEC_CMD("set_tsp_test_result", set_tsp_test_result),},
	{SEC_CMD("get_tsp_test_result", get_tsp_test_result),},
	{SEC_CMD("clear_tsp_test_result", clear_tsp_test_result),},
/*	{SEC_CMD("run_lowpower_selftest", run_lowpower_selftest),}, */
	{SEC_CMD("increase_disassemble_count", increase_disassemble_count),},
	{SEC_CMD("get_disassemble_count", get_disassemble_count),},
	{SEC_CMD_H("glove_mode", glove_mode),},
	{SEC_CMD_H("clear_cover_mode", clear_cover_mode),},
	{SEC_CMD("dead_zone_enable", dead_zone_enable),},
	{SEC_CMD("drawing_test_enable", drawing_test_enable),},
	{SEC_CMD_H("set_lowpower_mode", set_lowpower_mode),},
	{SEC_CMD_H("set_wirelesscharger_mode", set_wirelesscharger_mode),},
	{SEC_CMD_H("spay_enable", spay_enable),},
	{SEC_CMD("set_aod_rect", set_aod_rect),},
	{SEC_CMD("get_aod_rect", get_aod_rect),},
	{SEC_CMD_H("aod_enable", aod_enable),},
	{SEC_CMD_H("aot_enable", aot_enable),},
	{SEC_CMD_H("fod_enable", fod_enable),},
	{SEC_CMD_H("singletap_enable", singletap_enable),},
	{SEC_CMD("set_grip_data", set_grip_data),},
	{SEC_CMD_H("external_noise_mode", external_noise_mode),},
	{SEC_CMD_H("brush_enable", brush_enable),},
	{SEC_CMD_H("set_touchable_area", set_touchable_area),},
	{SEC_CMD("set_log_level", set_log_level),},
	{SEC_CMD("debug", debug),},
	{SEC_CMD("factory_cmd_result_all", factory_cmd_result_all),},
	{SEC_CMD("set_factory_level", set_factory_level),},
	{SEC_CMD("check_connection", check_connection),},
	{SEC_CMD("fix_active_mode", fix_active_mode),},
	{SEC_CMD("touch_aging_mode", touch_aging_mode),},
	{SEC_CMD_H("set_rear_selfie_mode", set_rear_selfie_mode),},
	{SEC_CMD_H("fp_int_control", fp_int_control),},
	{SEC_CMD_H("set_low_power_sensitivity", set_low_power_sensitivity),},
	{SEC_CMD("not_support_cmd", not_support_cmd),},
};

static ssize_t scrub_position_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[256] = { 0 };

#ifdef CONFIG_SAMSUNG_PRODUCT_SHIP
	input_info(true, &ts->client->dev,
			"%s: scrub_id: %d\n", __func__, ts->scrub_id);
#else
	input_info(true, &ts->client->dev,
			"%s: scrub_id: %d, X:%d, Y:%d\n", __func__,
			ts->scrub_id, ts->scrub_x, ts->scrub_y);
#endif
	snprintf(buff, sizeof(buff), "%d %d %d", ts->scrub_id, ts->scrub_x, ts->scrub_y);

	ts->scrub_x = 0;
	ts->scrub_y = 0;

	return snprintf(buf, PAGE_SIZE, "%s", buff);
}

static DEVICE_ATTR(scrub_pos, 0444, scrub_position_show, NULL);

static ssize_t read_ito_check_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[256] = { 0 };

	input_info(true, &ts->client->dev, "%s: %02X%02X%02X%02X\n", __func__,
			ts->ito_test[0], ts->ito_test[1],
			ts->ito_test[2], ts->ito_test[3]);

	snprintf(buff, sizeof(buff), "%02X%02X%02X%02X",
			ts->ito_test[0], ts->ito_test[1],
			ts->ito_test[2], ts->ito_test[3]);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%s", buff);
}

static ssize_t read_raw_check_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	int ii, ret = 0;
	char *buffer = NULL;
	char temp[CMD_RESULT_WORD_LEN] = { 0 };

	buffer = vzalloc(ts->rx_count * ts->tx_count * 6);
	if (!buffer)
		return -ENOMEM;

	memset(buffer, 0x00, ts->rx_count * ts->tx_count * 6);

	for (ii = 0; ii < (ts->rx_count * ts->tx_count - 1); ii++) {
		snprintf(temp, CMD_RESULT_WORD_LEN, "%d ", ts->pFrame[ii]);
		strlcat(buffer, temp, ts->rx_count * ts->tx_count * 6);

		memset(temp, 0x00, CMD_RESULT_WORD_LEN);
	}

	snprintf(temp, CMD_RESULT_WORD_LEN, "%d", ts->pFrame[ii]);
	strlcat(buffer, temp, ts->rx_count * ts->tx_count * 6);

	ret = snprintf(buf, ts->rx_count * ts->tx_count * 6, buffer);
	vfree(buffer);

	return ret;
}

static ssize_t read_multi_count_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d\n", __func__,
			ts->multi_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d", ts->multi_count);
}

static ssize_t clear_multi_count_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->multi_count = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_wet_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d, %d\n", __func__,
			ts->wet_count, ts->dive_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d", ts->wet_count);
}

static ssize_t clear_wet_mode_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->wet_count = 0;
	ts->dive_count= 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_noise_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d\n", __func__, ts->noise_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d", ts->noise_count);
}

static ssize_t clear_noise_mode_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->noise_count = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_comm_err_count_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d\n", __func__,
			ts->multi_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d", ts->comm_err_count);
}

static ssize_t clear_comm_err_count_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->comm_err_count = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_module_id_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[256] = { 0 };

	snprintf(buff, sizeof(buff), "SE%02X%02X%02X%02X%c%01X",
		ts->plat_data->img_version_of_bin[1], ts->plat_data->img_version_of_bin[2],
		ts->plat_data->img_version_of_bin[3], ts->nv,
#ifdef TCLM_CONCEPT
		ts->tdata->tclm_string[ts->tdata->nvdata.cal_position].s_name, ts->tdata->nvdata.cal_count & 0xF);
#else
		" ",0);
#endif

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%s", buff);
}

static ssize_t read_vendor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	unsigned char buffer[10] = { 0 };

	if (ts->plat_data->firmware_name)
		snprintf(buffer, 5, ts->plat_data->firmware_name + 8);
	else
		snprintf(buffer, 5, "SEC");


	return snprintf(buf, SEC_CMD_BUF_SIZE, "LSI_%s", buffer);
}

static ssize_t clear_checksum_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->checksum_result = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_checksum_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d\n", __func__,
			ts->checksum_result);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d", ts->checksum_result);
}

static ssize_t read_all_touch_count_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: touch:%d, aod:%d, spay:%d\n", __func__,
			ts->all_finger_count, ts->all_aod_tap_count,
			ts->all_spay_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE,
			"\"TTCN\":\"%d\",\"TACN\":\"%d\",\"TSCN\":\"%d\"",
			ts->all_finger_count, ts->all_aod_tap_count,
			ts->all_spay_count);
}

static ssize_t clear_all_touch_count_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->all_aod_tap_count = 0;
	ts->all_spay_count = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_mode_change_failed_count_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d\n", __func__,
			ts->mode_change_failed_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d",
			ts->mode_change_failed_count);
}

static ssize_t clear_mode_change_failed_count_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->mode_change_failed_count= 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t read_ic_reset_count_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d\n", __func__,
			ts->ic_reset_count);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d",
			ts->ic_reset_count);
}

static ssize_t clear_ic_reset_count_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	ts->ic_reset_count = 0;

	input_info(true, &ts->client->dev, "%s: clear\n", __func__);

	return count;
}

static ssize_t sensitivity_mode_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char value_result[10] = { 0 };
	int ret;
	int value[5];

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_SENSITIVITY_VALUE, value_result, 10);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, ret);
		return ret;
	}

	value[0] = value_result[0]<<8 | value_result[1];
	value[1] = value_result[2]<<8 | value_result[3];
	value[2] = value_result[4]<<8 | value_result[5];
	value[3] = value_result[6]<<8 | value_result[7];
	value[4] = value_result[8]<<8 | value_result[9];

	input_info(true, &ts->client->dev, "%s: sensitivity mode,%d,%d,%d,%d,%d\n", __func__,
		value[0], value[1], value[2], value[3], value[4]);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d,%d,%d,%d,%d",
			value[0], value[1], value[2], value[3], value[4]);
}

static ssize_t sensitivity_mode_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	int ret;
	u8 temp;
	unsigned long value = 0;

	if (count > 2)
		return -EINVAL;

	ret = kstrtoul(buf, 10, &value);
	if (ret != 0)
		return ret;

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: power off in IC\n", __func__);
		return 0;
	}

	input_err(true, &ts->client->dev, "%s: enable:%d\n", __func__, value);

	if (value == 1) {
		temp = 0x1;
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSITIVITY_MODE, &temp, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: send sensitivity mode on fail!\n", __func__);
			return ret;
		}
		sec_ts_delay(30);
		input_info(true, &ts->client->dev, "%s: enable end\n", __func__);
	} else {
		temp = 0x0;
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSITIVITY_MODE, &temp, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: send sensitivity mode off fail!\n", __func__);
			return ret;
		}
		input_info(true, &ts->client->dev, "%s: disable end\n", __func__);
	}

	input_info(true, &ts->client->dev, "%s: done\n", __func__);

	return count;
}

static ssize_t read_ambient_info_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: max: %d(%d,%d), min: %d(%d,%d)\n", __func__,
			ts->max_ambient, ts->max_ambient_channel_tx, ts->max_ambient_channel_rx,
			ts->min_ambient, ts->min_ambient_channel_tx, ts->min_ambient_channel_rx);
/*
	return snprintf(buf, SEC_CMD_BUF_SIZE,
			"\"TAMB_MAX\":\"%d\",\"TAMB_MAX_TX\":\"%d\",\"TAMB_MAX_RX\":\"%d\","
			"\"TAMB_MIN\":\"%d\",\"TAMB_MIN_TX\":\"%d\",\"TAMB_MIN_RX\":\"%d\"",
			ts->max_ambient, ts->max_ambient_channel_tx, ts->max_ambient_channel_rx,
			ts->min_ambient, ts->min_ambient_channel_tx, ts->min_ambient_channel_rx);
*/
	return snprintf(buf, SEC_CMD_BUF_SIZE,
			"\"TAMB_MAX2\":\"%d\",\"TAMB_MIN2\":\"%d\"",
			ts->max_ambient, ts->min_ambient);
}

static ssize_t read_ambient_channel_info_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char *buffer;
	int ret;
	int i;
	char temp[30];

	buffer = vmalloc((ts->tx_count + ts->rx_count) * 25);
	if (!buffer)
		return -ENOMEM;

	memset(buffer, 0x00, (ts->tx_count + ts->rx_count) * 25);

	for (i = 0; i < ts->tx_count; i++) {
		snprintf(temp, sizeof(temp), "\"TAMB_TX%02d\":\"%d\",",
				i, ts->ambient_tx[i]);
		strlcat(buffer, temp, (ts->tx_count + ts->rx_count) * 25);
	}

	for (i = 0; i < ts->rx_count; i++) {
		snprintf(temp, sizeof(temp), "\"TAMB_RX%02d\":\"%d\"",
				i, ts->ambient_rx[i]);
		strlcat(buffer, temp, (ts->tx_count + ts->rx_count) * 25);
		if (i  != (ts->rx_count - 1))
			strlcat(buffer, ",", (ts->tx_count + ts->rx_count) * 25);
	}

	ret = snprintf(buf, (ts->tx_count + ts->rx_count) * 25, buffer);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buffer);
	vfree(buffer);

	return ret;
}

static ssize_t read_ambient_channel_delta_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char *buffer;
	int ret;
	int i;
	char temp[30];

	buffer = vmalloc((ts->tx_count + ts->rx_count) * 25);
	if (!buffer)
		return -ENOMEM;

	memset(buffer, 0x00, (ts->tx_count + ts->rx_count) * 25);

	for (i = 0; i < ts->tx_count; i++) {
		snprintf(temp, sizeof(temp), "\"TCDT%02d\":\"%d\",",
				i, ts->ambient_tx_delta[i]);
		strlcat(buffer, temp, (ts->tx_count + ts->rx_count) * 25);
	}

	for (i = 0; i < ts->rx_count; i++) {
		snprintf(temp, sizeof(temp), "\"TCDR%02d\":\"%d\"",
				i, ts->ambient_rx_delta[i]);
		strlcat(buffer, temp, (ts->tx_count + ts->rx_count) * 25);
		if (i  != (ts->rx_count - 1))
			strlcat(buffer, ",", (ts->tx_count + ts->rx_count) * 25);
	}

	ret = snprintf(buf, (ts->tx_count + ts->rx_count) * 25, buffer);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buffer);
	vfree(buffer);

	return ret;
}

static ssize_t get_lp_dump(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts;
	u8 string_data[10] = {0, };
	u16 current_index;
	u8 dump_format, dump_num;
	u16 dump_start, dump_end;
	int i, j, ret;
	u8 buffer[30];
	u8 position = SEC_TS_SPONGE_LP_DUMP_DATA_FORMAT_10_LEN;

	if (!sec)
		return -ENODEV;

	ts = container_of(sec, struct sec_ts_data, sec);

	if (!ts->lp_dump)
		return -ENOMEM;
	
	disable_irq(ts->client->irq);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		goto read_buf;
	}

	if (ts->reset_is_on_going) {
		input_err(true, &ts->client->dev, "%s: Reset is ongoing!\n", __func__);
		goto read_buf;
	}

	if (ts->lp_dump_readmore == 0)
		goto read_buf;

	string_data[0] = SEC_TS_CMD_SPONGE_LP_DUMP & 0xFF;
	string_data[1] = (SEC_TS_CMD_SPONGE_LP_DUMP & 0xFF00) >> 8;

	ret = ts->sec_ts_read_sponge(ts, string_data, 4);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Failed to read rect\n", __func__);
		if (buf)
			snprintf(buf, SEC_CMD_BUF_SIZE, "NG, Failed to read rect");
		goto read_buf;
	}
	dump_format = string_data[0];
	dump_num = string_data[1];
	dump_start = SEC_TS_CMD_SPONGE_LP_DUMP + 4;
	dump_end = dump_start + (dump_format * (dump_num - 1));

	current_index = (string_data[3] & 0xFF) << 8 | (string_data[2] & 0xFF);
	if (current_index > dump_end || current_index < dump_start) {
		input_err(true, &ts->client->dev,
				"Failed to Sponge LP log %d\n", current_index);
		if (buf)
			snprintf(buf, SEC_CMD_BUF_SIZE,
					"NG, Failed to Sponge LP log, current_index=%d",
					current_index);
		goto read_buf;
	}

	input_info(true, &ts->client->dev, "%s: DEBUG format=%d, num=%d, start=%d, end=%d, current_index=%d\n",
				__func__, dump_format, dump_num, dump_start, dump_end, current_index);

	for (i = dump_num - 1 ; i >= 0 ; i--) {
		u16 string_addr;
		int value = 0;

		if (current_index < (dump_format * i))
			string_addr = (dump_format * dump_num) + current_index - (dump_format * i);
		else
			string_addr = current_index - (dump_format * i);

		if (string_addr < dump_start)
			string_addr += (dump_format * dump_num);

		string_data[0] = string_addr & 0xFF;
		string_data[1] = (string_addr & 0xFF00) >> 8;

		ret = ts->sec_ts_read_sponge(ts, string_data, dump_format);
		if (ret < 0) {
			input_err(true, &ts->client->dev,
					"%s: Failed to read rect\n", __func__);
			if (buf)
				snprintf(buf, SEC_CMD_BUF_SIZE,
						"NG, Failed to read rect, addr=%d",
						string_addr);
			goto read_buf;
		}

		for (j = 0; j < 10; j++)
			value += string_data[j];

		if (value == 0)
			continue;

		ts->lp_dump[ts->lp_dump_index] = (string_addr >> 8) & 0xFF;
		ts->lp_dump[ts->lp_dump_index + 1] = string_addr & 0xFF;

		for (j = 0; j < 10; j += 2) {
			ts->lp_dump[ts->lp_dump_index + 2 + j] = string_data[j + 1];
			ts->lp_dump[ts->lp_dump_index + 2 + j + 1] = string_data[j];
		}

		ts->lp_dump_index += position;

		if (ts->lp_dump_index >= SEC_TS_SPONGE_LP_DUMP_DATA_FORMAT_10_LEN * SEC_TS_SPONGE_LP_DUMP_LENGTH)
			ts->lp_dump_index = 0;
	}

read_buf:
	if (buf) {
		int pos = ts->lp_dump_index;

		for (i = 0; i < SEC_TS_SPONGE_LP_DUMP_LENGTH; i++) {
			if (pos >= SEC_TS_SPONGE_LP_DUMP_DATA_FORMAT_10_LEN * SEC_TS_SPONGE_LP_DUMP_LENGTH)
				pos = 0;

			if (ts->lp_dump[pos] == 0 && ts->lp_dump[pos + 1] == 0) {
				pos += position;
				continue;
			}

			snprintf(buffer, sizeof(buffer), "%d: ", ts->lp_dump[pos] << 8 | ts->lp_dump[pos + 1]);
			strlcat(buf, buffer, PAGE_SIZE);
			memset(buffer, 0x00, sizeof(buffer));
			for (j = 0; j < 10; j++) {
				snprintf(buffer, sizeof(buffer), "%02x", ts->lp_dump[pos + 2 + j]);
				strlcat(buf, buffer, PAGE_SIZE);
				memset(buffer, 0x00, sizeof(buffer));
			}
			snprintf(buffer, sizeof(buffer), "\n");
			strlcat(buf, buffer, PAGE_SIZE);
			memset(buffer, 0x00, sizeof(buffer));

			pos += position;
		}
	}

	ts->lp_dump_readmore = 0;

	enable_irq(ts->client->irq);

	if (buf) {
		if (strlen(buf) == 0)
			snprintf(buf, SEC_CMD_BUF_SIZE, "empty");

		return strnlen(buf, PAGE_SIZE);
	}

	return 0;
}

ssize_t sec_ts_get_lp_dump(struct device *dev, struct device_attribute *attr, char *buf)
{
	return get_lp_dump(dev, attr, buf);
}

static ssize_t ic_status_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[512] = { 0 };
	char temp[128] = { 0 };
	u8 data[2] = { 0 };
	int ret;

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SET_TOUCHFUNCTION, data, 2);
	if (ret < 0)
		goto out;

	snprintf(temp, sizeof(temp), "mutual:%d, ", data[0] & SEC_TS_BIT_SETFUNC_MUTUAL ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "hover:%d, ", data[0] & SEC_TS_BIT_SETFUNC_HOVER ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "cover:%d, ", data[0] & SEC_TS_BIT_SETFUNC_COVER ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "glove:%d, ", data[0] & SEC_TS_BIT_SETFUNC_GLOVE ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "stylus:%d, ", data[0] & SEC_TS_BIT_SETFUNC_STYLUS ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "palm:%d, ", data[0] & SEC_TS_BIT_SETFUNC_PALM ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "wet:%d, ", data[0] & SEC_TS_BIT_SETFUNC_WET ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "prox:%d, ", data[0] & SEC_TS_BIT_SETFUNC_PROXIMITY ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));

	data[0] = 0;
	data[1] = 0;
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SET_POWER_MODE, data, 1);
	if (ret < 0)
		goto out;

	snprintf(temp, sizeof(temp), "npm:%d, ", data[0] == TO_TOUCH_MODE);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "lpm:%d, ", data[0] == TO_LOWPOWER_MODE);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "test:%d, ", data[0] == TO_SELFTEST_MODE);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "flash:%d, ", data[0] == TO_FLASH_MODE);
	strlcat(buff, temp, sizeof(buff));

	data[0] = 0;
	ret = ts->sec_ts_i2c_read(ts, SET_TS_CMD_SET_CHARGER_MODE, data, 1);
	if (ret < 0)
		goto out;

	snprintf(temp, sizeof(temp), "no_charger:%d, ",
			data[0] == SEC_TS_BIT_CHARGER_MODE_NO);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "wired_charger:%d, ",
			data[0] == SEC_TS_BIT_CHARGER_MODE_WIRE_CHARGER);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "wireless_charger:%d, ",
			data[0] == SEC_TS_BIT_CHARGER_MODE_WIRELESS_CHARGER);
	strlcat(buff, temp, sizeof(buff));

	data[0] = 0;
	ret = ts->sec_ts_i2c_read(ts, SET_TS_CMD_SET_NOISE_MODE, data, 1);
	if (ret < 0)
		goto out;

	snprintf(temp, sizeof(temp), "noise:%d, ", data[0] & 0x0F);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "supported_noise_mode:%d, ", (data[0] >> 4) & 0x0F);
	strlcat(buff, temp, sizeof(buff));

	data[0] = 0;
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SET_MONITOR_NOISE_MODE, data, 1);
	if (ret < 0)
		goto out;

	snprintf(temp, sizeof(temp), "monitor_noise:%d, ", data[0]);
	strlcat(buff, temp, sizeof(buff));

	data[0] = 0;
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SET_COVERTYPE, data, 1);
	if (ret < 0)
		goto out;

	snprintf(temp, sizeof(temp), "cover_type:%d, ", data[0]);
	strlcat(buff, temp, sizeof(buff));

	data[0] = 0;
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_SET_BRUSH_MODE, data, 1);
	if (ret < 0)
		goto out;

	snprintf(temp, sizeof(temp), "brush_mode:%d, ", data[0]);
	strlcat(buff, temp, sizeof(buff));

	data[0] = 0;
	ret = ts->sec_ts_read_sponge(ts, data, 1);
	if (ret < 0)
		goto out;

	snprintf(temp, sizeof(temp), "aod:%d, ", data[0] & SEC_TS_MODE_SPONGE_AOD ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "aot:%d", data[0] & SEC_TS_MODE_SPONGE_DOUBLETAP_TO_WAKEUP ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "spay:%d, ", data[0] & SEC_TS_MODE_SPONGE_SWIPE ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "fod:%d, ", data[0] & SEC_TS_MODE_SPONGE_PRESS ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));
	snprintf(temp, sizeof(temp), "singletap:%d", data[0] & SEC_TS_MODE_SPONGE_SINGLE_TAP ? 1 : 0);
	strlcat(buff, temp, sizeof(buff));

out:
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
	return snprintf(buf, SEC_CMD_BUF_SIZE, "%s\n", buff);
}

static int sec_ts_write_factory_level(struct sec_ts_data *ts, u8 pos)
{
	int ret = 0;

	input_info(true, &ts->client->dev,
			"%s: set factory level[%d]\n", __func__, pos);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_FACTORY_LEVEL, &pos, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev,
				"%s: failed to set factory level,%d\n", __func__, pos);

	sec_ts_delay(30);
	return ret;
}

static int sec_ts_set_factory_data_type(struct sec_ts_data *ts, u8 pos)
{
	int ret = 0;

	input_info(true, &ts->client->dev,
			"%s: set factory data type[%d]\n", __func__, pos);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_SET_FACTORY_DATA_TYPE, &pos, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev,
				"%s: failed to set factory data type[%d]\n", __func__, pos);

	sec_ts_delay(30);
	return ret;
}

static void set_factory_level(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		goto NG;
	}

	if (sec->cmd_param[0] < OFFSET_FAC_SUB || sec->cmd_param[0] > OFFSET_FAC_MAIN) {
		input_err(true, &ts->client->dev, "%s: cmd data is abnormal, %d\n", __func__, sec->cmd_param[0]);
		goto NG;
	}

	ts->factory_position = sec->cmd_param[0] + 1;	// for FW index
	ts->factory_level = true;

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	return;

NG:
	snprintf(buff, sizeof(buff), "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
}

#define OPEN_SHORT_TEST		1
#define CRACK_TEST			2
#define BRIDGE_SHORT_TEST	3
#define CHECK_ONLY_OPEN_TEST	1
#define CHECK_ONLY_SHORT_TEST	2

/* SELFTEST FAIL HISTORY : (cm1_1 + cm1_2 + cm2_1 + cm2_2 + cm3_1 + cm3_2) * 48 byte */
ssize_t get_selftest_fail_hist_dump_all(struct sec_ts_data *ts, char *buf, u8 position)
{
	int i, j;
	int ii, jj;
	int ret;
	char tempn[40] = {0};
	char tempv[25] = {0};
	u8 *buff;
	u8 read_event_buff[6][48] = { { 0, } };
	u8 *rBuff;
	u8 temp_result = 0;
	u8 defect_tx, defect_rx;
	u16 defective_node_data;
	struct sec_ts_selftest_fail_hist *p_fail_hist;

	if (ts->power_status != SEC_TS_STATE_POWER_ON) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n", __func__);
		return -EBUSY;
	}

	if (ts->reset_is_on_going) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Reset is ongoing\n", __func__);
		return -EBUSY;
	}

	if (ts->sec.cmd_is_running) {
		input_err(true, &ts->client->dev, "%s: [ERROR] cmd is running\n", __func__);
		return -EBUSY;
	}

	/* set Factory level */
	ret = sec_ts_write_factory_level(ts, position);
	if (ret < 0)
		goto err_exit;

	/* set Factory Data Type */
	ret = sec_ts_set_factory_data_type(ts, OFFSET_FAC_DATA_SELF_FAIL);
	if (ret < 0)
		goto err_exit;

	rBuff = kzalloc(SEC_CM_HIST_DATA_SIZE, GFP_KERNEL);
	if (!rBuff)
		goto err_mem_1;

	buff = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!buff)
		goto err_mem_2;

	/* read full data */
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_GET_FACTORY_DATA, (u8 *)read_event_buff[0], SEC_CM_HIST_DATA_SIZE);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: read fail history data failed!\n", __func__);
		goto err_i2c;
	}
	input_info(true, &ts->client->dev, "%s: read data read_data_size[%d]  ret[%d]\n",
								__func__, SEC_CM_HIST_DATA_SIZE, ret);

	memset(buf, 0x00, ts->proc_fail_hist_size);

	// CM 1 & 2 & 3 each 2 times
	for (i = 1 ; i < 4 ; ++i ) {
		for (j = 1 ; j < 3 ; ++j) {
			p_fail_hist = (struct sec_ts_selftest_fail_hist *)read_event_buff[(i - 1) * 2 + (j - 1)];

			if (position == OFFSET_FW_SDC) {
				snprintf(buff, PAGE_SIZE, "%s", "SDC  ");
			} else if (position == OFFSET_FW_SUB) {
				snprintf(buff, PAGE_SIZE, "%s", "SUB  ");
			} else if (position == OFFSET_FW_MAIN) {
				snprintf(buff, PAGE_SIZE, "%s", "MAIN ");
			}
			strlcat(buf, buff, ts->proc_fail_hist_size);

			input_info(true, &ts->client->dev, "%s: CM%d #%d\n", __func__, i, j);

			if (p_fail_hist->tsp_signature == 0 || p_fail_hist->tsp_signature == 0xFFFFFFFF) {
				input_err(true, &ts->client->dev, "%s: CM%d #%d : Data empty\n", __func__, i, j);
				snprintf(buff, PAGE_SIZE, "CM%d #%d : Data empty(0x%X)\n", i, j, p_fail_hist->tsp_signature);
				strlcat(buf, buff, ts->proc_fail_hist_size);
				continue;

			} else if (p_fail_hist->tsp_signature != SEC_FAIL_HIST_SIGNATURE) {
				input_err(true, &ts->client->dev, "%s: signature is mismatched :%8X != (%8X)\n",
							__func__, p_fail_hist->tsp_signature, SEC_FAIL_HIST_SIGNATURE);
				snprintf(buff, PAGE_SIZE, "CM%d #%d : SIGNATURE mismatched(0x%X)\n", i, j, p_fail_hist->tsp_signature);
				strlcat(buf, buff, ts->proc_fail_hist_size);
				continue;
			}

			input_info(true, &ts->client->dev, "%s: PRINT CM%d #%d INFO\n", __func__, i, j);
			input_info(true, &ts->client->dev, "%s: FW VER : 0x%X, Selftest Parm : 0x%X, fail_cnt1/2 : %d/%d, Test Result : 0x%X\n",
								__func__, p_fail_hist->tsp_fw_version, p_fail_hist->selftest_exec_parm,
								p_fail_hist->fail_cnt1, p_fail_hist->fail_cnt2, p_fail_hist->test_result);

			snprintf(buff, PAGE_SIZE, "CM%d #%d :  FW VER : 0x%X, Selftest Parm : 0x%X, fail_cnt1/2 : %d/%d, Test Result : 0x%X\n",
							i, j, p_fail_hist->tsp_fw_version, p_fail_hist->selftest_exec_parm,
							p_fail_hist->fail_cnt1, p_fail_hist->fail_cnt2, p_fail_hist->test_result);
			strlcat(buf, buff, ts->proc_fail_hist_size);
			
			input_info(true, &ts->client->dev, "%s: Primary Failure Type : 0x%X\n", __func__, p_fail_hist->fail_type);
			snprintf(buff, PAGE_SIZE, "Fail Type : 0x%X\n", p_fail_hist->fail_type);
			strlcat(buf, buff, ts->proc_fail_hist_size);

			memset(buff, 0x00, PAGE_SIZE);
			memset(tempn, 0x00, 40);

			if (p_fail_hist->fail_type == 0x01)
				snprintf(tempn, 40, "S2S Short Fail (Tx/Rx to Tx/Rx):");
			else if (p_fail_hist->fail_type == 0x02)
				snprintf(tempn, 40, "S2G Short Fail (Tx/Rx to GND):");
			else if (p_fail_hist->fail_type == 0x03)
				snprintf(tempn, 40, "S2O Short Fail (Tx/Rx to Rx/Tx):");
			else if (p_fail_hist->fail_type == 0x04)
				snprintf(tempn, 40, "Open Fail:");
			else if (p_fail_hist->fail_type == 0x05)
				snprintf(tempn, 40, "Slope Fail:");
			else if (p_fail_hist->fail_type == 0x06)
				snprintf(tempn, 40, "CM2 Fail:");
			else if (p_fail_hist->fail_type == 0x07)
				snprintf(tempn, 40, "CM3 Fail:");
			else if (p_fail_hist->fail_type == 0x16)
				snprintf(tempn, 40, "S2S Short in Boundary Range:");

			strlcat(buff, tempn, PAGE_SIZE);

			for (ii = 0; ii < 8; ii++) {
				temp_result = p_fail_hist->fail_data[ii];
				for (jj = ii * 8; jj < (ii + 1) * 8; jj++) {
					memset(tempv, 0x00, 25);
					if (temp_result & 0x1)
						snprintf(tempv, 20, "%s%d,",
								jj < ts->tx_count ? "TX":"RX",
								jj < ts->tx_count ? jj : jj - ts->tx_count);
					strlcat(buff, tempv, PAGE_SIZE);
					temp_result = temp_result >> 1;
				}
			}
			strlcat(buff, "\n", PAGE_SIZE);
			input_info(true, &ts->client->dev, "%s: %s", __func__, buff);

			strlcat(buf, buff, ts->proc_fail_hist_size);

			//factory failure data
			for (ii = 0 ; ii < 5 ; ii++) {
				defect_tx = p_fail_hist->defective_data[ii] & 0xFF;
				defect_rx = (p_fail_hist->defective_data[ii] >> 8 ) & 0xFF;
				defective_node_data = (p_fail_hist->defective_data[ii] >> 16) & 0xFFFF;

				input_info(true, &ts->client->dev, "%s: RX : %d, TX : %d, Data : %d\n", __func__, 
									defect_rx, defect_tx, defective_node_data);

				snprintf(buff, PAGE_SIZE, "RX,TX[%d,%d] %d\n", defect_rx, defect_tx, defective_node_data);
				strlcat(buf, buff, ts->proc_fail_hist_size);
			}
		}
	}

	input_info(true, &ts->client->dev, "%s: total buf size:%d\n", __func__, strlen(buf));

	sec_ts_write_factory_level(ts, OFFSET_FW_NOSAVE);
	sec_ts_set_factory_data_type(ts, OFFSET_FAC_DATA_NO);
	kfree(rBuff);
	kfree(buff);
	return 0;

err_i2c:
	kfree(buff);
err_mem_2:
	kfree(rBuff);
err_mem_1:
	sec_ts_write_factory_level(ts, OFFSET_FW_NOSAVE);
	sec_ts_set_factory_data_type(ts, OFFSET_FAC_DATA_NO);
err_exit:
	snprintf(buf, ts->proc_cmoffset_size, "NG, error");
	return 0;
}

ssize_t get_cmoffset_dump_all(struct sec_ts_data *ts, char *buf, u8 position)
{
	u8 *rBuff;
	int i, j, ret, type;
	int value;
	int signature_val = 0;
	u16 avg, try_cnt, status;
	int max_node = 8 + ts->tx_count * ts->rx_count;
	char buff[80] = {0, };
	char data[6] = {0, };
	u16 temp;

	if (ts->power_status != SEC_TS_STATE_POWER_ON) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n", __func__);
		return -EBUSY;
	}

	if (ts->reset_is_on_going) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Reset is ongoing\n", __func__);
		return -EBUSY;
	}

	if (ts->sec.cmd_is_running) {
		input_err(true, &ts->client->dev, "%s: [ERROR] cmd is running\n", __func__);
		return -EBUSY;
	}

	input_info(true, &ts->client->dev, "%s: set power mode to test mode\n", __func__);
	data[0] = 0x02;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, data, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: set test mode failed\n", __func__);
	}

	input_info(true, &ts->client->dev, "%s: clear event stack\n", __func__);
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CLEAR_EVENT_STACK, NULL, 0);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: clear event stack failed\n", __func__);
	}

	/* set Factory level */
	ret = sec_ts_write_factory_level(ts, position);
	if (ret < 0)
		goto err_exit;
	sec_ts_delay(30);

	memset(buf, 0x00, ts->proc_cmoffset_size);

#if 0	// y761 only support cm3 data
	type = OFFSET_FAC_DATA_CM2;
#else
	type = OFFSET_FAC_DATA_CM3;
#endif

	for( ; type <= OFFSET_FAC_DATA_CM3; type++) {

		if (position == OFFSET_FW_SDC) {
			snprintf(buff, sizeof(buff), "%s", "SDC  ");
		} else if (position == OFFSET_FW_SUB) {
			snprintf(buff, sizeof(buff), "%s", "SUB  ");
		} else if (position == OFFSET_FW_MAIN) {
			snprintf(buff, sizeof(buff), "%s", "MAIN ");
		}
		strlcat(buf, buff, ts->proc_cmoffset_size);

		/* set Factory Data Type */
		ret = sec_ts_set_factory_data_type(ts, type);
		if (ret < 0)
			goto err_exit;
		sec_ts_delay(30);

		rBuff = kzalloc(max_node, GFP_KERNEL);
		if (!rBuff)
			goto err_mem;

		/* read full data */
		ret = ts->sec_ts_i2c_read(ts, SEC_TS_GET_FACTORY_DATA, rBuff, max_node);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: read rawdata failed!\n", __func__);
			goto err_i2c;
		}

		if (type == OFFSET_FAC_DATA_CM2) {
			signature_val = SEC_CM2_SIGNATURE;
		} else if (type == OFFSET_FAC_DATA_CM3) {
			signature_val = SEC_CM3_SIGNATURE;
		} else {
			input_err(true, &ts->client->dev, "%s: cmoffset pos[%d] type[%d], type is abnormal\n",
						__func__, position, type);
			snprintf(buff, sizeof(buff), "CM%d is abnormal\n", type);
			strlcat(buf, buff, ts->proc_cmoffset_size);
			goto err_invalid;
		}

		/* check Header */
		value = rBuff[3] << 24 | rBuff[2] << 16 | rBuff[1] << 8 | rBuff[0];
		input_info(true, &ts->client->dev, "%s: signature:%8X (%8X)\n", __func__, value, signature_val);

		if (value == 0 || value == 0xFFFFFFFF) {
			input_err(true, &ts->client->dev, "%s: cmoffset pos[%d] type[%d]: Data empty\n", __func__, position, type);
			snprintf(buff, sizeof(buff), "CM%d : Data empty\n", type);
			strlcat(buf, buff, ts->proc_cmoffset_size);
			continue;

		} else if (value != signature_val) {
			input_err(true, &ts->client->dev, "%s: cmoffset pos[%d] type[%d], signature is mismatched %08X != %08X\n",
							__func__, position, type, value, signature_val);
			
			snprintf(buff, sizeof(buff), "CM%d : signature is mismatched %08X != %08X\n", type, value, signature_val);
			strlcat(buf, buff, ts->proc_cmoffset_size);
			continue;
		}

		status = rBuff[4];
		try_cnt = rBuff[5];
		avg = rBuff[7] << 8 | rBuff[6];
		input_info(true, &ts->client->dev, "%s: CM%d pos[%d], AVG:0x%X, Try cnt:%d, Status:%d\n",
										__func__, type, position, avg, try_cnt, status);

		snprintf(buff, sizeof(buff), "CM%d pos[%d], AVG:%d, Try cnt:%d, Status:%d\n", type, position, avg, try_cnt, status);
		strlcat(buf, buff, ts->proc_cmoffset_size);

		for (i = 0; i < ts->rx_count; i++) {
			for (j = 0; j < ts->tx_count; j++) {
				temp = rBuff[8 + (j * ts->rx_count) + i];

				if (temp == 127)
					temp = 4095;
				else if (temp == 128)
					temp = 0;
				else if (temp > 127)
					temp = avg + (temp - 256) * 2;
				else
					temp = avg + temp * 2;
				snprintf(buff, sizeof(buff), " %4d", temp);
				strlcat(buf, buff, ts->proc_cmoffset_size);
			}
			snprintf(buff, sizeof(buff), "\n");
			strlcat(buf, buff, ts->proc_cmoffset_size);
		}
	}
	input_err(true, &ts->client->dev, "%s: total buf size:%d\n", __func__, strlen(buf));

err_invalid:
	sec_ts_write_factory_level(ts, OFFSET_FW_NOSAVE);
	sec_ts_set_factory_data_type(ts, OFFSET_FAC_DATA_NO);

	input_info(true, &ts->client->dev, "%s: set power mode to normal mode\n", __func__);
	sec_ts_locked_release_all_finger(ts);
	data[0] = 0x00;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, data, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: set test normal failed\n", __func__);
	}

	kfree(rBuff);
	return 0;

err_i2c:
	kfree(rBuff);
err_mem:
	sec_ts_write_factory_level(ts, OFFSET_FW_NOSAVE);
	sec_ts_set_factory_data_type(ts, OFFSET_FAC_DATA_NO);

	input_info(true, &ts->client->dev, "%s: set power mode to normal mode\n", __func__);
	sec_ts_locked_release_all_finger(ts);
	data[0] = 0x00;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, data, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: set test normal failed\n", __func__);
	}
err_exit:
	snprintf(buf, ts->proc_cmoffset_size, "NG, error");
	return 0;
}

/*
 * CM 1 not used
 * CM 2 beyond 1/2/5G
 * CM 3 beyond 0/1/2/5G
 * */
static ssize_t prox_power_off_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	input_info(true, &ts->client->dev, "%s: %d\n", __func__,
			ts->prox_power_off);

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%ld", ts->prox_power_off);
}

static ssize_t prox_power_off_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	long data;
	int ret;

	ret = kstrtol(buf, 10, &data);
	if (ret < 0)
		return ret;

	input_info(true, &ts->client->dev, "%s: %ld\n", __func__, data);

	ts->prox_power_off = data;

	return count;
}

static ssize_t read_support_feature(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct sec_cmd_data *sec = dev_get_drvdata(dev);
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	u32 feature = 0;

	if (ts->plat_data->enable_settings_aot)
		feature |= INPUT_FEATURE_ENABLE_SETTINGS_AOT;

	if (ts->plat_data->sync_reportrate_120)
		feature |= INPUT_FEATURE_ENABLE_SYNC_RR120;

	input_info(true, &ts->client->dev, "%s: %d%s%s%s\n",
			__func__, feature,
			feature & INPUT_FEATURE_ENABLE_SETTINGS_AOT ? " aot" : "",
			feature & INPUT_FEATURE_ENABLE_PRESSURE ? " pressure" : "",
			feature & INPUT_FEATURE_ENABLE_SYNC_RR120 ? " RR120hz" : "");

	return snprintf(buf, SEC_CMD_BUF_SIZE, "%d", feature);
}

static DEVICE_ATTR(ito_check, 0444, read_ito_check_show, NULL);
static DEVICE_ATTR(raw_check, 0444, read_raw_check_show, NULL);
static DEVICE_ATTR(multi_count, 0664, read_multi_count_show, clear_multi_count_store);
static DEVICE_ATTR(wet_mode, 0664, read_wet_mode_show, clear_wet_mode_store);
static DEVICE_ATTR(noise_mode, 0664, read_noise_mode_show, clear_noise_mode_store);
static DEVICE_ATTR(comm_err_count, 0664, read_comm_err_count_show, clear_comm_err_count_store);
static DEVICE_ATTR(checksum, 0664, read_checksum_show, clear_checksum_store);
static DEVICE_ATTR(all_touch_count, 0664, read_all_touch_count_show, clear_all_touch_count_store);
static DEVICE_ATTR(mode_change_failed_count, 0664, read_mode_change_failed_count_show, clear_mode_change_failed_count_store);
static DEVICE_ATTR(ic_reset_count, 0664, read_ic_reset_count_show, clear_ic_reset_count_store);
static DEVICE_ATTR(module_id, 0444, read_module_id_show, NULL);
static DEVICE_ATTR(vendor, 0444, read_vendor_show, NULL);
static DEVICE_ATTR(read_ambient_info, 0444, read_ambient_info_show, NULL);
static DEVICE_ATTR(read_ambient_channel_info, 0444, read_ambient_channel_info_show, NULL);
static DEVICE_ATTR(read_ambient_channel_delta, 0444, read_ambient_channel_delta_show, NULL);
static DEVICE_ATTR(get_lp_dump, 0444, get_lp_dump, NULL);
static DEVICE_ATTR(status, 0444, ic_status_show, NULL);
static DEVICE_ATTR(sensitivity_mode, 0664, sensitivity_mode_show, sensitivity_mode_store);
#if 0
static DEVICE_ATTR(cmoffset_sdc, 0444, get_cmoffset_sdc, NULL);
static DEVICE_ATTR(cmoffset_sub, 0444, get_cmoffset_sub, NULL);
static DEVICE_ATTR(cmoffset_main, 0444, get_cmoffset_main, NULL);
#endif
static DEVICE_ATTR(prox_power_off, 0664, prox_power_off_show, prox_power_off_store);
static DEVICE_ATTR(support_feature, 0444, read_support_feature, NULL);

static struct attribute *cmd_attributes[] = {
	&dev_attr_scrub_pos.attr,
	&dev_attr_ito_check.attr,
	&dev_attr_raw_check.attr,
	&dev_attr_multi_count.attr,
	&dev_attr_wet_mode.attr,
	&dev_attr_noise_mode.attr,
	&dev_attr_comm_err_count.attr,
	&dev_attr_checksum.attr,
	&dev_attr_all_touch_count.attr,
	&dev_attr_mode_change_failed_count.attr,
	&dev_attr_ic_reset_count.attr,
	&dev_attr_module_id.attr,
	&dev_attr_vendor.attr,
	&dev_attr_read_ambient_info.attr,
	&dev_attr_read_ambient_channel_info.attr,
	&dev_attr_read_ambient_channel_delta.attr,
	&dev_attr_get_lp_dump.attr,
	&dev_attr_status.attr,
	&dev_attr_sensitivity_mode.attr,
	&dev_attr_prox_power_off.attr,
	&dev_attr_support_feature.attr,
	NULL,
};

static struct attribute_group cmd_attr_group = {
	.attrs = cmd_attributes,
};

static int sec_ts_check_index(struct sec_ts_data *ts)
{
	struct sec_cmd_data *sec = &ts->sec;
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int node;

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > ts->tx_count
			|| sec->cmd_param[1] < 0 || sec->cmd_param[1] > ts->rx_count) {

		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		input_info(true, &ts->client->dev, "%s: parameter error: %u, %u\n",
				__func__, sec->cmd_param[0], sec->cmd_param[0]);
		node = -1;
		return node;
	}
	node = sec->cmd_param[1] * ts->tx_count + sec->cmd_param[0];
	input_info(true, &ts->client->dev, "%s: node = %d\n", __func__, node);
	return node;
}
static void fw_update(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[64] = { 0 };
	int retval = 0;

	sec_cmd_set_default_result(sec);
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	retval = sec_ts_firmware_update_on_hidden_menu(ts, sec->cmd_param[0]);
	if (retval < 0) {
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		input_err(true, &ts->client->dev, "%s: failed [%d]\n", __func__, retval);
	} else {
		snprintf(buff, sizeof(buff), "OK");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_OK;
		input_info(true, &ts->client->dev, "%s: success [%d]\n", __func__, retval);
	}
}

int sec_ts_fix_tmode(struct sec_ts_data *ts, u8 mode, u8 state)
{
	int ret;
	u8 onoff[1] = {STATE_MANAGE_OFF};
	u8 tBuff[2] = { mode, state };

	input_info(true, &ts->client->dev, "%s\n", __func__);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, onoff, 1);
	sec_ts_delay(20);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CHG_SYSMODE, tBuff, sizeof(tBuff));
	sec_ts_delay(20);

	return ret;
}

int sec_ts_p2p_tmode(struct sec_ts_data *ts)
{
	int ret;
	u8 mode[2] = {0x0F, 0x11};

	input_info(true, &ts->client->dev, "%s\n", __func__);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CLEAR_EVENT_STACK, NULL, 0);
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_P2P_MODE, mode, sizeof(mode));
	sec_ts_delay(30);

	return ret;
}

static int execute_p2ptest(struct sec_ts_data *ts)
{
	int ret;
	u8 test[2] = {0x00, 0x64};
	u8 tBuff[10] = {0};
	int retry = 0;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_P2P_TEST, test, sizeof(test));
	sec_ts_delay(2000);

	ret = -1;

	while (ts->sec_ts_i2c_read(ts, SEC_TS_READ_ONE_EVENT, tBuff, 8)) {
		if (((tBuff[0] >> 2) & 0xF) == TYPE_STATUS_EVENT_VENDOR_INFO) {
			if (tBuff[1] == SEC_TS_VENDOR_ACK_CMR_TEST_DONE) {
				ts->cm_raw_set_p2p_max = (tBuff[2] & 0xFF) << 8 | (tBuff[3] & 0xFF);
				ts->cm_raw_set_p2p_min = (tBuff[4] & 0xFF) << 8 | (tBuff[5] & 0xFF);
				ts->cm_raw_set_p2p_diff = (tBuff[7] & 0xC0) << 2 | (tBuff[6] & 0xFF);
			} else if (tBuff[1] == SEC_TS_VENDOR_ACK_CSR_TX_TEST_DONE) {
				ts->self_raw_set_p2p_x_max = (tBuff[2] & 0xFF) << 8 | (tBuff[3] & 0xFF);
				ts->self_raw_set_p2p_x_min = (tBuff[4] & 0xFF) << 8 | (tBuff[5] & 0xFF);
				ts->self_raw_set_p2p_x_diff = (tBuff[7] & 0xC0) << 2 | (tBuff[6] & 0xFF);
			} else if (tBuff[1] == SEC_TS_VENDOR_ACK_CSR_RX_TEST_DONE) {
				ts->self_raw_set_p2p_y_max = (tBuff[2] & 0xFF) << 8 | (tBuff[3] & 0xFF);
				ts->self_raw_set_p2p_y_min = (tBuff[4] & 0xFF) << 8 | (tBuff[5] & 0xFF);
				ts->self_raw_set_p2p_y_diff = (tBuff[7] & 0xC0) << 2 | (tBuff[6] & 0xFF);
			} else if (tBuff[1] == SEC_TS_VENDOR_ACK_CMR_KEY_TEST_DONE) {
				ts->cm_raw_key_p2p_max = (tBuff[2] & 0xFF) << 8 | (tBuff[3] & 0xFF);
				ts->cm_raw_key_p2p_min = (tBuff[4] & 0xFF) << 8 | (tBuff[5] & 0xFF);
				ts->cm_raw_key_p2p_diff = (tBuff[7] & 0xC0) << 2 | (tBuff[6] & 0xFF);
			}
		}

		if ((tBuff[7] & 0x3F) == 0x00) {
			input_info(true, &ts->client->dev, "%s: left event is 0\n", __func__);
			ret = 0;
			break;
		}

		if (retry++ > SEC_TS_WAIT_RETRY_CNT) {
			input_err(true, &ts->client->dev, "%s: Time Over\n", __func__);
			break;
		}
		sec_ts_delay(20);
	}
	return ret;
}

int sec_ts_release_tmode(struct sec_ts_data *ts)
{
	int ret;
	u8 onoff[1] = {STATE_MANAGE_ON};

	input_info(true, &ts->client->dev, "%s\n", __func__);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, onoff, 1);
	sec_ts_delay(20);

	return ret;
}

static void sec_ts_print_frame(struct sec_ts_data *ts, short *min, short *max)
{
	int i = 0;
	int j = 0;
	unsigned char *pStr = NULL;
	unsigned char pTmp[16] = { 0 };
	int lsize = 7 * (ts->tx_count + 1);

	input_raw_info_d(true, &ts->client->dev, "%s\n", __func__);

	pStr = kzalloc(lsize, GFP_KERNEL);
	if (pStr == NULL)
		return;

	memset(pStr, 0x0, lsize);
	snprintf(pTmp, sizeof(pTmp), "      TX");
	strlcat(pStr, pTmp, lsize);

	for (i = 0; i < ts->tx_count; i++) {
		snprintf(pTmp, sizeof(pTmp), " %02d ", i);
		strlcat(pStr, pTmp, lsize);
	}

	input_raw_info_d(true, &ts->client->dev, "%s\n", pStr);
	memset(pStr, 0x0, lsize);
	snprintf(pTmp, sizeof(pTmp), " +");
	strlcat(pStr, pTmp, lsize);

	for (i = 0; i < ts->tx_count; i++) {
		snprintf(pTmp, sizeof(pTmp), "----");
		strlcat(pStr, pTmp, lsize);
	}

	input_raw_info_d(true, &ts->client->dev, "%s\n", pStr);

	for (i = 0; i < ts->rx_count; i++) {
		memset(pStr, 0x0, lsize);
		snprintf(pTmp, sizeof(pTmp), "Rx%02d | ", i);
		strlcat(pStr, pTmp, lsize);

		for (j = 0; j < ts->tx_count; j++) {
			snprintf(pTmp, sizeof(pTmp), " %3d", ts->pFrame[(j * ts->rx_count) + i]);

			if (ts->pFrame[(j * ts->rx_count) + i] < *min)
				*min = ts->pFrame[(j * ts->rx_count) + i];

			if (ts->pFrame[(j * ts->rx_count) + i] > *max)
				*max = ts->pFrame[(j * ts->rx_count) + i];

			strlcat(pStr, pTmp, lsize);
		}
		input_raw_info_d(true, &ts->client->dev, "%s\n", pStr);
	}
	kfree(pStr);
}

static int sec_ts_read_frame(struct sec_ts_data *ts, u8 type, short *min,
		short *max, bool save_result)
{
	unsigned int readbytes = 0xFF;
	unsigned char *pRead = NULL;
	u8 mode = TYPE_INVALID_DATA;
	int ret = 0;
	int i = 0;
	int j = 0;
	short *temp = NULL;

	input_raw_info_d(true, &ts->client->dev, "%s: type %d\n", __func__, type);

	/* set data length, allocation buffer memory */
	readbytes = ts->rx_count * ts->tx_count * 2;

	pRead = kzalloc(readbytes, GFP_KERNEL);
	if (!pRead)
		return -ENOMEM;

	/* set OPCODE and data type */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_MUTU_RAW_TYPE, &type, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Set rawdata type failed\n", __func__);
		goto ErrorExit;
	}

	sec_ts_delay(50);

	if (type == TYPE_OFFSET_DATA_SDC) {
		/* excute selftest for real cap offset data, because real cap data is not memory data in normal touch. */
		char para = TO_TOUCH_MODE;

		disable_irq(ts->client->irq);

		ret = execute_selftest(ts, save_result);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: execute_selftest failed\n", __func__);
			enable_irq(ts->client->irq);
			goto ErrorRelease;
		}

		sec_ts_locked_release_all_finger(ts);
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: Set rawdata type failed\n", __func__);
			enable_irq(ts->client->irq);
			goto ErrorRelease;
		}

		enable_irq(ts->client->irq);
	}

	/* read data */
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_TOUCH_RAWDATA, pRead, readbytes);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: read rawdata failed!\n", __func__);
		goto ErrorRelease;
	}

	memset(ts->pFrame, 0x00, readbytes);

	for (i = 0; i < readbytes; i += 2)
		ts->pFrame[i / 2] = pRead[i + 1] + (pRead[i] << 8);

	*min = *max = ts->pFrame[0];

#ifdef DEBUG_MSG
	input_info(true, &ts->client->dev, "%s: 02X%02X%02X readbytes=%d\n", __func__,
			pRead[0], pRead[1], pRead[2], readbytes);
#endif
	sec_ts_print_frame(ts, min, max);

	temp = kzalloc(readbytes, GFP_KERNEL);
	if (!temp)
		goto ErrorRelease;

	memcpy(temp, ts->pFrame, ts->tx_count * ts->rx_count * 2);
	memset(ts->pFrame, 0x00, ts->tx_count * ts->rx_count * 2);

	for (i = 0; i < ts->tx_count; i++) {
		for (j = 0; j < ts->rx_count; j++)
			ts->pFrame[(j * ts->tx_count) + i] = temp[(i * ts->rx_count) + j];
	}

	kfree(temp);

ErrorRelease:
	/* release data monitory (unprepare AFE data memory) */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_MUTU_RAW_TYPE, &mode, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: Set rawdata type failed\n", __func__);

ErrorExit:
	kfree(pRead);

	return ret;
}

static void sec_ts_print_channel(struct sec_ts_data *ts)
{
	unsigned char *pStr = NULL;
	unsigned char pTmp[16] = { 0 };
	int i = 0, j = 0, k = 0;

	if (!ts->tx_count)
		return;

	pStr = vzalloc(7 * (ts->tx_count + 1));
	if (!pStr)
		return;

	memset(pStr, 0x0, 7 * (ts->tx_count + 1));
	snprintf(pTmp, sizeof(pTmp), " TX");
	strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));

	for (k = 0; k < ts->tx_count; k++) {
		snprintf(pTmp, sizeof(pTmp), "    %02d", k);
		strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));
	}
	input_raw_info_d(true, &ts->client->dev, "%s\n", pStr);

	memset(pStr, 0x0, 7 * (ts->tx_count + 1));
	snprintf(pTmp, sizeof(pTmp), " +");
	strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));

	for (k = 0; k < ts->tx_count; k++) {
		snprintf(pTmp, sizeof(pTmp), "------");
		strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));
	}
	input_raw_info_d(true, &ts->client->dev, "%s\n", pStr);

	memset(pStr, 0x0, 7 * (ts->tx_count + 1));
	snprintf(pTmp, sizeof(pTmp), " | ");
	strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));

	for (i = 0; i < (ts->tx_count + ts->rx_count) * 2; i += 2) {
		if (j == ts->tx_count) {
			input_raw_info_d(true, &ts->client->dev, "%s\n", pStr);
			input_raw_info_d(true, &ts->client->dev, "\n");
			memset(pStr, 0x0, 7 * (ts->tx_count + 1));
			snprintf(pTmp, sizeof(pTmp), " RX");
			strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));

			for (k = 0; k < ts->tx_count; k++) {
				snprintf(pTmp, sizeof(pTmp), "    %02d", k);
				strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));
			}

			input_raw_info_d(true, &ts->client->dev, "%s\n", pStr);

			memset(pStr, 0x0, 7 * (ts->tx_count + 1));
			snprintf(pTmp, sizeof(pTmp), " +");
			strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));

			for (k = 0; k < ts->tx_count; k++) {
				snprintf(pTmp, sizeof(pTmp), "------");
				strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));
			}
			input_raw_info_d(true, &ts->client->dev, "%s\n", pStr);

			memset(pStr, 0x0, 7 * (ts->tx_count + 1));
			snprintf(pTmp, sizeof(pTmp), " | ");
			strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));
		} else if (j && !(j % ts->tx_count)) {
			input_raw_info_d(true, &ts->client->dev, "%s\n", pStr);
			memset(pStr, 0x0, 7 * (ts->tx_count + 1));
			snprintf(pTmp, sizeof(pTmp), " | ");
			strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));
		}

		snprintf(pTmp, sizeof(pTmp), " %5d", ts->pFrame[j]);
		strlcat(pStr, pTmp, 7 * (ts->tx_count + 1));

		j++;
	}
	input_raw_info_d(true, &ts->client->dev, "%s\n", pStr);
	vfree(pStr);
}

static int sec_ts_read_channel(struct sec_ts_data *ts, u8 type,
				short *min, short *max, bool save_result)
{
	unsigned char *pRead = NULL;
	u8 mode = TYPE_INVALID_DATA;
	int ret = 0;
	int ii = 0;
	int jj = 0;
	unsigned int data_length = (ts->tx_count + ts->rx_count) * 2;
	u8 w_data;

	input_raw_info_d(true, &ts->client->dev, "%s: type %d\n", __func__, type);

	pRead = kzalloc(data_length, GFP_KERNEL);
	if (!pRead)
		return -ENOMEM;

	/* set OPCODE and data type */
	w_data = type;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SELF_RAW_TYPE, &w_data, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Set rawdata type failed\n", __func__);
		goto out_read_channel;
	}

	sec_ts_delay(50);

	if (type == TYPE_OFFSET_DATA_SDC) {
		/* execute selftest for real cap offset data,
		 * because real cap data is not memory data in normal touch.
		 */
		char para = TO_TOUCH_MODE;
		disable_irq(ts->client->irq);
		ret = execute_selftest(ts, save_result);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: execute_selftest failed!\n", __func__);
			enable_irq(ts->client->irq);
			goto err_read_data;
		}

		sec_ts_locked_release_all_finger(ts);

		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: set rawdata type failed!\n", __func__);
			enable_irq(ts->client->irq);
			goto err_read_data;
		}
		enable_irq(ts->client->irq);
		/* end */
	}
	/* read data */
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_TOUCH_SELF_RAWDATA, pRead, data_length);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: read rawdata failed!\n", __func__);
		goto err_read_data;
	}

	/* clear all pFrame data */
	memset(ts->pFrame, 0x00, data_length);

	for (ii = 0; ii < data_length; ii += 2) {
		ts->pFrame[jj] = ((pRead[ii] << 8) | pRead[ii + 1]);

		if (ii == 0)
			*min = *max = ts->pFrame[jj];

		*min = min(*min, ts->pFrame[jj]);
		*max = max(*max, ts->pFrame[jj]);

		jj++;
	}

	sec_ts_print_channel(ts);

err_read_data:
	/* release data monitory (unprepare AFE data memory) */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SELF_RAW_TYPE, &mode, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: Set rawdata type failed\n", __func__);

out_read_channel:
	kfree(pRead);

	return ret;
}

static int get_gap_data(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	int ii;
	int node_gap_tx = 0;
	int node_gap_rx = 0;
	int tx_max = 0;
	int rx_max = 0;

	for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
		if ((ii + 1) % (ts->tx_count) != 0) {
			if (ts->pFrame[ii] > ts->pFrame[ii + 1])
				node_gap_tx = 100 - (ts->pFrame[ii + 1] * 100 / ts->pFrame[ii]);
			else
				node_gap_tx = 100 - (ts->pFrame[ii] * 100 / ts->pFrame[ii + 1]);
			tx_max = max(tx_max, node_gap_tx);
		}

		if (ii < (ts->rx_count - 1) * ts->tx_count) {
			if (ts->pFrame[ii] > ts->pFrame[ii + ts->tx_count])
				node_gap_rx = 100 - (ts->pFrame[ii + ts->tx_count] * 100 / ts->pFrame[ii]);
			else
				node_gap_rx = 100 - (ts->pFrame[ii] * 100 / ts->pFrame[ii + ts->tx_count]);
			rx_max = max(rx_max, node_gap_rx);
		}
	}

	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING) {
		snprintf(buff, sizeof(buff), "%d,%d", 0, tx_max);
		sec_cmd_set_cmd_result_all(sec, buff, SEC_CMD_STR_LEN, "GAP_DATA_X");
		snprintf(buff, sizeof(buff), "%d,%d", 0, rx_max);
		sec_cmd_set_cmd_result_all(sec, buff, SEC_CMD_STR_LEN, "GAP_DATA_Y");
	}

	sec->cmd_state = SEC_CMD_STATUS_OK;
	return 0;
}

static void get_gap_data_x_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char *buff = NULL;
	int ii;
	int node_gap = 0;
	char temp[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	buff = kzalloc(ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN, GFP_KERNEL);
	if (!buff)
		return;

	for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
		if ((ii + 1) % (ts->tx_count) != 0) {
			if (ts->pFrame[ii] > ts->pFrame[ii + 1])
				node_gap = 100 - (ts->pFrame[ii + 1] * 100 / ts->pFrame[ii]);
			else
				node_gap = 100 - (ts->pFrame[ii] * 100 / ts->pFrame[ii + 1]);

			snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", node_gap);
			strlcat(buff, temp, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN);
			memset(temp, 0x00, SEC_CMD_STR_LEN);
		}
	}

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	kfree(buff);
}

static void get_gap_data_y_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char *buff = NULL;
	int ii;
	int node_gap = 0;
	char temp[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	buff = kzalloc(ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN, GFP_KERNEL);
	if (!buff)
		return;

	for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
		if (ii < (ts->rx_count - 1) * ts->tx_count) {
			if (ts->pFrame[ii] > ts->pFrame[ii + ts->tx_count])
				node_gap = 100 - (ts->pFrame[ii + ts->tx_count] * 100 / ts->pFrame[ii]);
			else
				node_gap = 100 - (ts->pFrame[ii] * 100 / ts->pFrame[ii + ts->tx_count]);

			snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", node_gap);
			strlcat(buff, temp, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN);
			memset(temp, 0x00, SEC_CMD_STR_LEN);
		}
	}

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	kfree(buff);
}

static int get_self_channel_data(void *device_data, u8 type)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	int ii;
	short tx_min = 0;
	short rx_min = 0;
	short tx_max = 0;
	short rx_max = 0;
	char *item_name = "NULL";
	char temp[SEC_CMD_STR_LEN] = { 0 };

	switch (type) {
	case TYPE_OFFSET_DATA_SDC:
		item_name = "SELF_OFFSET_MODULE";
		break;
	case TYPE_RAW_DATA:
		item_name = "SELF_DELTA";
		break;
	case TYPE_OFFSET_DATA_SEC:
		item_name = "SELF_OFFSET_SET";
		break;
	default:
		break;
	}

	for (ii = 0; ii < ts->tx_count; ii++) {
		if (ii == 0)
			tx_min = tx_max = ts->pFrame[ii];

		tx_min = min(tx_min, ts->pFrame[ii]);
		tx_max = max(tx_max, ts->pFrame[ii]);
	}
	snprintf(buff, sizeof(buff), "%d,%d", tx_min, tx_max);
	snprintf(temp, sizeof(temp), "%s%s", item_name, "_X");
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, sizeof(buff), temp);

	memset(temp, 0x00, SEC_CMD_STR_LEN);

	for (ii = ts->tx_count; ii < ts->tx_count + ts->rx_count; ii++) {
		if (ii == ts->tx_count)
			rx_min = rx_max = ts->pFrame[ii];

		rx_min = min(rx_min, ts->pFrame[ii]);
		rx_max = max(rx_max, ts->pFrame[ii]);
	}
	snprintf(buff, sizeof(buff), "%d,%d", rx_min, rx_max);
	snprintf(temp, sizeof(temp), "%s%s", item_name, "_Y");
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, sizeof(buff), temp);

	return 0;
}

static int sec_ts_read_raw_data(struct sec_ts_data *ts,
		struct sec_cmd_data *sec, struct sec_ts_test_mode *mode)
{
	int ii;
	int ret = 0;
	char temp[SEC_CMD_STR_LEN] = { 0 };
	char *buff;
	char *item_name = "NULL";

	switch (mode->type) {
	case TYPE_OFFSET_DATA_SDC:
		item_name = "CM_OFFSET_MODULE";
		break;
	case TYPE_RAW_DATA:
		item_name = "CM_DELTA";
		break;
	case TYPE_OFFSET_DATA_SEC:
		item_name = "CM_OFFSET_SET";
		break;
	default:
		break;
	}

	buff = kzalloc(ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN, GFP_KERNEL);
	if (!buff)
		goto error_alloc_mem;

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		goto error_power_state;
	}

	input_raw_info_d(true, &ts->client->dev, "%s: %d, %s\n",
			__func__, mode->type, mode->allnode ? "ALL" : "");

	ret = sec_ts_fix_tmode(ts, TOUCH_SYSTEM_MODE_TOUCH, TOUCH_MODE_STATE_TOUCH);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix tmode\n",
				__func__);
		goto error_test_fail;
	}

	if (mode->frame_channel)
		ret = sec_ts_read_channel(ts, mode->type, &mode->min, &mode->max, true);
	else
		ret = sec_ts_read_frame(ts, mode->type, &mode->min, &mode->max, true);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to read frame\n",
				__func__);
		goto error_test_fail;
	}

	if (mode->allnode) {
		if (mode->frame_channel) {
			for (ii = 0; ii < (ts->rx_count + ts->tx_count); ii++) {
				snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", ts->pFrame[ii]);
				strlcat(buff, temp, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN);

				memset(temp, 0x00, SEC_CMD_STR_LEN);
			}
		} else {
			for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
				snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", ts->pFrame[ii]);
				strlcat(buff, temp, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN);

				memset(temp, 0x00, SEC_CMD_STR_LEN);
			}
		}
	} else {
		snprintf(buff, SEC_CMD_STR_LEN, "%d,%d", mode->min, mode->max);
	}

	ret = sec_ts_release_tmode(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to release tmode\n",
				__func__);
		goto error_test_fail;
	}

	if (!sec)
		goto out_rawdata;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN));

	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING && (!mode->frame_channel))
		sec_cmd_set_cmd_result_all(sec, buff,
				strnlen(buff, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN), item_name);

	sec->cmd_state = SEC_CMD_STATUS_OK;

out_rawdata:
	kfree(buff);

	sec_ts_locked_release_all_finger(ts);

	return ret;

error_test_fail:
error_power_state:
	kfree(buff);
error_alloc_mem:
	if (!sec)
		return ret;

	snprintf(temp, SEC_CMD_STR_LEN, "NG");
	sec_cmd_set_cmd_result(sec, temp, SEC_CMD_STR_LEN);
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING && (!mode->frame_channel))
		sec_cmd_set_cmd_result_all(sec, temp, SEC_CMD_STR_LEN, item_name);
	sec->cmd_state = SEC_CMD_STATUS_FAIL;

	sec_ts_locked_release_all_finger(ts);

	return ret;
}

static int sec_ts_read_rawp2p_data(struct sec_ts_data *ts,
		struct sec_cmd_data *sec, struct sec_ts_test_mode *mode)
{
	int ii;
	int ret = 0;
	char temp[SEC_CMD_STR_LEN] = { 0 };
	char *buff;
	char para = TO_TOUCH_MODE;

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		goto error_power_state;
	}

	buff = kzalloc(ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN, GFP_KERNEL);
	if (!buff)
		goto error_alloc_mem;

	input_info(true, &ts->client->dev, "%s: %d, %s\n",
			__func__, mode->type, mode->allnode ? "ALL" : "");

	disable_irq(ts->client->irq);

	ret = sec_ts_p2p_tmode(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix p2p tmode\n",
				__func__);
		goto error_tmode_fail;
	}

	ret = execute_p2ptest(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix p2p test\n",
				__func__);
		goto error_test_fail;
	}

	if (mode->frame_channel)
		ret = sec_ts_read_channel(ts, mode->type, &mode->min, &mode->max, true);
	else
		ret = sec_ts_read_frame(ts, mode->type, &mode->min, &mode->max, true);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to read frame\n",
				__func__);
		goto error_test_fail;
	}

	if (mode->allnode) {
		if (mode->frame_channel) {
			for (ii = 0; ii < (ts->rx_count + ts->tx_count); ii++) {
				snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", ts->pFrame[ii]);
				strlcat(buff, temp, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN);

				memset(temp, 0x00, SEC_CMD_STR_LEN);
			}
		} else {
			for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
				snprintf(temp, CMD_RESULT_WORD_LEN, "%d,", ts->pFrame[ii]);
				strlcat(buff, temp, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN);

				memset(temp, 0x00, SEC_CMD_STR_LEN);
			}
		}
	} else {
		snprintf(buff, SEC_CMD_STR_LEN, "%d,%d", mode->min, mode->max);
	}

	if (!sec)
		goto out_rawdata;

	sec_ts_locked_release_all_finger(ts);
	sec_ts_delay(30);
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: set rawdata type failed!\n", __func__);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, ts->tx_count * ts->rx_count * CMD_RESULT_WORD_LEN));
	sec->cmd_state = SEC_CMD_STATUS_OK;

out_rawdata:
	enable_irq(ts->client->irq);
	kfree(buff);

	return ret;
error_test_fail:
error_tmode_fail:
	kfree(buff);
	enable_irq(ts->client->irq);
error_alloc_mem:
error_power_state:
	if (!sec)
		return ret;

	snprintf(temp, SEC_CMD_STR_LEN, "NG");
	sec_cmd_set_cmd_result(sec, temp, SEC_CMD_STR_LEN);
	sec->cmd_state = SEC_CMD_STATUS_FAIL;

	return ret;
}

static int sec_ts_read_rawp2p_data_all(struct sec_ts_data *ts,
		struct sec_cmd_data *sec, struct sec_ts_test_mode *mode)
{
	int ret = 0;
	char buff[SEC_CMD_STR_LEN] = { 0 };
	char para = TO_TOUCH_MODE;

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		goto error_power_state;
	}

	input_info(true, &ts->client->dev, "%s: start\n", __func__);

	disable_irq(ts->client->irq);
	ret = sec_ts_p2p_tmode(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix p2p tmode\n",
				__func__);
		goto error_tmode_fail;
	}

	ret = execute_p2ptest(ts);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix p2p test\n",
				__func__);
		goto error_test_fail;
	}

	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING) {
		snprintf(buff, sizeof(buff), "%d,%d", ts->cm_raw_set_p2p_min, ts->cm_raw_set_p2p_max);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "CM_RAW_SET_P2P");
		snprintf(buff, sizeof(buff), "%d,%d", 0, ts->cm_raw_set_p2p_diff);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "CM_RAW_SET_P2P_DIFF");
		snprintf(buff, sizeof(buff), "%d,%d", ts->self_raw_set_p2p_x_min, ts->self_raw_set_p2p_x_max);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_X");
		snprintf(buff, sizeof(buff), "%d,%d", ts->self_raw_set_p2p_y_min, ts->self_raw_set_p2p_y_max);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_Y");
		snprintf(buff, sizeof(buff), "%d,%d", 0, ts->self_raw_set_p2p_x_diff);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_X_DIFF");
		snprintf(buff, sizeof(buff), "%d,%d", 0, ts->self_raw_set_p2p_y_diff);
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_Y_DIFF");
	}
	sec->cmd_state = SEC_CMD_STATUS_OK;

	sec_ts_locked_release_all_finger(ts);

	sec_ts_delay(30);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: set rawdata type failed!\n", __func__);

	enable_irq(ts->client->irq);

	return ret;

error_test_fail:
	sec_ts_locked_release_all_finger(ts);
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: set rawdata type failed!\n", __func__);

error_tmode_fail:
	enable_irq(ts->client->irq);
error_power_state:
	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING) {
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "CM_RAW_SET_P2P");
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "CM_RAW_SET_P2P_DIFF");
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_X");
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_Y");
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_X_DIFF");
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "SELF_RAW_SET_P2P_Y_DIFF");
	}
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	return ret;
}

static void get_fw_ver_bin(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	sec_cmd_set_default_result(sec);

	snprintf(buff, sizeof(buff), "SE%02X%02X%02X%02X",
			ts->plat_data->img_version_of_bin[0], ts->plat_data->img_version_of_bin[1],
			ts->plat_data->img_version_of_bin[2], ts->plat_data->img_version_of_bin[3]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "FW_VER_BIN");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_fw_ver_ic(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	int ret;
	u8 fw_ver[4];

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_IMG_VERSION, fw_ver, 4);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: firmware version read error\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	snprintf(buff, sizeof(buff), "SE%02X%02X%02X%02X",
			fw_ver[0], fw_ver[1], fw_ver[2], fw_ver[3]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "FW_VER_IC");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_config_ver(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[22] = { 0 };

	sec_cmd_set_default_result(sec);

	snprintf(buff, sizeof(buff), "%s_SE_%02X%02X",
			ts->plat_data->model_name,
			ts->plat_data->config_version_of_ic[2], ts->plat_data->config_version_of_ic[3]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

#ifdef TCLM_CONCEPT
static void get_pat_information(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[50] = { 0 };

	sec_cmd_set_default_result(sec);

#ifdef CONFIG_SEC_FACTORY
	if (ts->factory_position == 1) {
		sec_tclm_initialize(ts->tdata);
		sec_tclm_data_read(ts->client, SEC_TCLM_NVM_ALL_DATA);
	}
#endif
	/* fixed tune version will be saved at execute autotune */
	snprintf(buff, sizeof(buff), "C%02XT%04X.%4s%s%c%d%c%d%c%d",
		ts->tdata->nvdata.cal_count, ts->tdata->nvdata.tune_fix_ver, ts->tdata->tclm_string[ts->tdata->nvdata.cal_position].f_name,
		(ts->tdata->tclm_level == TCLM_LEVEL_LOCKDOWN) ? ".L " : " ",
		ts->tdata->cal_pos_hist_last3[0], ts->tdata->cal_pos_hist_last3[1],
		ts->tdata->cal_pos_hist_last3[2], ts->tdata->cal_pos_hist_last3[3],
		ts->tdata->cal_pos_hist_last3[4], ts->tdata->cal_pos_hist_last3[5]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void set_external_factory(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[22] = { 0 };

	sec_cmd_set_default_result(sec);

	ts->tdata->external_factory = true;
	snprintf(buff, sizeof(buff), "OK");

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}
#endif

static void get_threshold(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[20] = { 0 };
	char threshold[2] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		goto err;
	}

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_TOUCH_MODE_FOR_THRESHOLD, threshold, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: threshold write type failed. ret: %d\n", __func__, ret);
		snprintf(buff, sizeof(buff), "NG");
		goto err;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_TOUCH_THRESHOLD, threshold, 2);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: read threshold fail!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		goto err;
	}

	input_info(true, &ts->client->dev, "0x%02X, 0x%02X\n",
			threshold[0], threshold[1]);

	snprintf(buff, sizeof(buff), "%d", (threshold[0] << 8) | threshold[1]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

	return;
err:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	return;
}

static void module_off_master(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[3] = { 0 };
	int ret = 0;

	ret = sec_ts_stop_device(ts);

	if (ret == 0)
		snprintf(buff, sizeof(buff), "OK");
	else
		snprintf(buff, sizeof(buff), "NG");

	sec_cmd_set_default_result(sec);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (strncmp(buff, "OK", 2) == 0)
		sec->cmd_state = SEC_CMD_STATUS_OK;
	else
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void module_on_master(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[3] = { 0 };
	int ret = 0;

	ret = sec_ts_start_device(ts);

	if (ts->input_dev->disabled) {
		sec_ts_set_lowpowermode(ts, TO_LOWPOWER_MODE);
		ts->power_status = SEC_TS_STATE_LPM;
	}

	if (ret == 0)
		snprintf(buff, sizeof(buff), "OK");
	else
		snprintf(buff, sizeof(buff), "NG");

	sec_cmd_set_default_result(sec);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (strncmp(buff, "OK", 2) == 0)
		sec->cmd_state = SEC_CMD_STATUS_OK;
	else
		sec->cmd_state = SEC_CMD_STATUS_FAIL;

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_chip_vendor(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	strncpy(buff, "SEC", sizeof(buff));
	sec_cmd_set_default_result(sec);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "IC_VENDOR");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_chip_name(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	if (ts->plat_data->img_version_of_ic[0] == 0x02)
		strncpy(buff, "MC44", sizeof(buff));
	else if (ts->plat_data->img_version_of_ic[0] == 0x05)
		strncpy(buff, "A552", sizeof(buff));
	else if (ts->plat_data->img_version_of_ic[0] == 0x09)
		strncpy(buff, "Y661", sizeof(buff));
	else if (ts->plat_data->img_version_of_ic[0] == 0x10)
		strncpy(buff, "Y761", sizeof(buff));
	else if (ts->plat_data->img_version_of_ic[0] == 0x17)
		strncpy(buff, "Y771", sizeof(buff));
	else
		strncpy(buff, "N/A", sizeof(buff));

	sec_cmd_set_default_result(sec);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "IC_NAME");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

/*
 *	## Mis Cal result ##
 *	FF : initial value in Firmware.
 *	FD : Cal fail case
 *	F4 : i2c fail case (5F)
 *	F3 : i2c fail case (5E)
 *	F2 : power off state
 *	F1 : not support mis cal concept
 *	F0 : initial value in fucntion
 *	08 : Ambient Ambient condition check(PEAK) result 0 (PASS), 1(FAIL)
 *	04 : Ambient Ambient condition check(DIFF MAX TX) result 0 (PASS), 1(FAIL)
 *	02 : Ambient Ambient condition check(DIFF MAX RX) result 0 (PASS), 1(FAIL)
 *	01 : Wet Wet mode result 0 (PASS), 1(FAIL)
 *	00 : Pass
 */
static void get_mis_cal_info(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	char mis_cal_data = 0xF0;
	char wreg[5] = { 0 };
	int ret;
	char buff_all[16] = { 0 };
	int i = 0;

	sec_cmd_set_default_result(sec);

	if (ts->plat_data->mis_cal_check == 0) {
		input_err(true, &ts->client->dev, "%s: [ERROR] not support, %d\n", __func__);
		mis_cal_data = 0xF1;
		goto NG;
	} else if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n", __func__);
		mis_cal_data = 0xF2;
		goto NG;
	} else {
		for (i = 0; i < 3; i++) {
			ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_MIS_CAL_READ, &mis_cal_data, 1);
			if (ret < 0) {
				input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, ret);
				mis_cal_data = 0xF3;
				goto NG;
			} else {
				input_info(true, &ts->client->dev, "%s: miss cal data[%d] : %d\n", __func__, i, mis_cal_data);
				if (mis_cal_data != 0xFF)
					break;
			}
		}

		ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_MIS_CAL_SPEC, wreg, 4);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, ret);
			mis_cal_data = 0xF4;
			goto NG;
		} else {
			input_info(true, &ts->client->dev, "%s: miss cal spec : %d,%d,%d,%d\n",
						__func__, wreg[0], wreg[1], wreg[2], wreg[3]);
		}
	}

	snprintf(buff, sizeof(buff), "%d", mis_cal_data);
	snprintf(buff_all, sizeof(buff_all), "%d,%d,%d,%d,%d", mis_cal_data, wreg[0], wreg[1], wreg[2], wreg[3]);

	sec_cmd_set_cmd_result(sec, buff_all, strnlen(buff_all, sizeof(buff_all)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "MIS_CAL");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff_all);
	return;

NG:
	snprintf(buff, sizeof(buff), "%d", mis_cal_data);
	snprintf(buff_all, sizeof(buff_all), "%d,%d,%d,%d", mis_cal_data, 0, 0, 0);
	sec_cmd_set_cmd_result(sec, buff_all, strnlen(buff_all, sizeof(buff_all)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "MIS_CAL");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff_all);
}

static void get_wet_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	char wet_mode_info = 0;
	int ret;

	sec_cmd_set_default_result(sec);

	sec_ts_delay(300);
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_WET_MODE, &wet_mode_info, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, ret);
		goto NG;
	}

	snprintf(buff, sizeof(buff), "%d", wet_mode_info);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "WET_MODE");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
	return;

NG:
	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	if (sec->cmd_all_factory_state == SEC_CMD_STATUS_RUNNING)
		sec_cmd_set_cmd_result_all(sec, buff, strnlen(buff, sizeof(buff)), "WET_MODE");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

}

static void get_x_num(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	sec_cmd_set_default_result(sec);
	snprintf(buff, sizeof(buff), "%d", ts->tx_count);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_y_num(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	sec_cmd_set_default_result(sec);
	snprintf(buff, sizeof(buff), "%d", ts->rx_count);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static u32 checksum_sum(struct sec_ts_data *ts, u8 *chunk_data, u32 size)
{
	u32 data_32 = 0;
	u32 checksum = 0;
	u8 *fd = chunk_data;
	int i;

	for (i = 0; i < size/4; i++) {
		data_32 = (((fd[(i*4)+0]&0xFF)<<0) | ((fd[(i*4)+1]&0xFF)<<8) |
					((fd[(i*4)+2]&0xFF)<<16) | ((fd[(i*4)+3]&0xFF)<<24));
		checksum += data_32;
	}

	checksum &= 0xFFFFFFFF;

	input_info(true, &ts->client->dev, "%s: checksum = [%x]\n", __func__, checksum);

	return checksum;
}

static u32 get_bin_checksum(struct sec_ts_data *ts, const u8 *data, size_t size)
{

	int i;
	fw_header *fw_hd;
	fw_chunk *fw_ch;
	u8 *fd = (u8 *)data;
	u32 data_32;
	u32 checksum = 0;
	u32 chunk_checksum[3];
	u32 img_checksum;

	fw_hd = (fw_header *)fd;
	fd += sizeof(fw_header);

	if (fw_hd->signature != SEC_TS_FW_HEADER_SIGN) {
		input_err(true, &ts->client->dev, "%s: firmware header error = %08X\n", __func__, fw_hd->signature);
		return 0;
	}

	for (i = 0; i < (fw_hd->totalsize/4); i++) {
		data_32 = (((data[(i*4)+0]&0xFF)<<0) | ((data[(i*4)+1]&0xFF)<<8) |
					((data[(i*4)+2]&0xFF)<<16) | ((data[(i*4)+3]&0xFF)<<24));
		checksum += data_32;
	}

	input_info(true, &ts->client->dev, "%s: fw_hd->totalsize = [%d] checksum = [%x]\n",
				__func__, fw_hd->totalsize, checksum);

	checksum &= 0xFFFFFFFF;
	if (checksum != 0) {
		input_err(true, &ts->client->dev, "%s: Checksum fail! = %08X\n", __func__, checksum);
		return 0;
	}

	for (i = 0; i < fw_hd->num_chunk; i++) {
		fw_ch = (fw_chunk *)fd;

		input_info(true, &ts->client->dev, "%s: [%d] 0x%08X, 0x%08X, 0x%08X, 0x%08X\n", __func__, i,
				fw_ch->signature, fw_ch->addr, fw_ch->size, fw_ch->reserved);

		if (fw_ch->signature != SEC_TS_FW_CHUNK_SIGN) {
			input_err(true, &ts->client->dev, "%s: firmware chunk error = %08X\n",
						__func__, fw_ch->signature);
			return 0;
		}
		fd += sizeof(fw_chunk);

		checksum = checksum_sum(ts, fd, fw_ch->size);
		chunk_checksum[i] = checksum;
		fd += fw_ch->size;
	}

	img_checksum = chunk_checksum[0] + chunk_checksum[1];

	return img_checksum;
}

static void get_checksum_data(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	char csum_result[4] = { 0 };
	char data[5] = { 0 };
	u8 cal_result;
	u8 nv_result;
	u8 temp;
	u8 csum = 0;
	int ret, i;

	u32 ic_checksum;
	u32 img_checksum;
	const struct firmware *fw_entry;
	char fw_path[SEC_TS_MAX_FW_PATH];
	u8 fw_ver[4];

	sec_cmd_set_default_result(sec);
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	disable_irq(ts->client->irq);

	ts->plat_data->power(ts, false);
	ts->power_status = SEC_TS_STATE_POWER_OFF;
	sec_ts_delay(50);

	ts->plat_data->power(ts, true);
	ts->power_status = SEC_TS_STATE_POWER_ON;
	sec_ts_delay(70);

	ret = sec_ts_wait_for_ready(ts, SEC_TS_ACK_BOOT_COMPLETE);
	if (ret < 0) {
		enable_irq(ts->client->irq);
		input_err(true, &ts->client->dev, "%s: boot complete failed\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		goto err;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_FIRMWARE_INTEGRITY, &data[0], 1);
	if (ret < 0 || (data[0] != 0x80)) {
		enable_irq(ts->client->irq);
		input_err(true, &ts->client->dev, "%s: firmware integrity failed, ret:%d, data:%X\n",
				__func__, ret, data[0]);
		snprintf(buff, sizeof(buff), "NG");
		goto err;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_BOOT_STATUS, &data[1], 1);
	if (ret < 0 || (data[1] != SEC_TS_STATUS_APP_MODE)) {
		enable_irq(ts->client->irq);
		input_err(true, &ts->client->dev, "%s: boot status failed, ret:%d, data:%X\n", __func__, ret, data[0]);
		snprintf(buff, sizeof(buff), "NG");
		goto err;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_TS_STATUS, &data[2], 4);
	if (ret < 0 || (data[3] == TOUCH_SYSTEM_MODE_FLASH)) {
		enable_irq(ts->client->irq);
		input_err(true, &ts->client->dev, "%s: touch status failed, ret:%d, data:%X\n", __func__, ret, data[3]);
		snprintf(buff, sizeof(buff), "NG");
		goto err;
	}

	sec_ts_reinit(ts);

	enable_irq(ts->client->irq);

	input_err(true, &ts->client->dev, "%s: data[0]:%X, data[1]:%X, data[3]:%X\n", __func__, data[0], data[1], data[3]);

	temp = DO_FW_CHECKSUM | DO_PARA_CHECKSUM;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_GET_CHECKSUM, &temp, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: send get_checksum_cmd fail!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		goto err;
	}

	sec_ts_delay(20);

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_GET_CHECKSUM, csum_result, 4);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: read get_checksum result fail!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		goto err;
	}

	nv_result = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);
	nv_result += get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_CAL_COUNT);

	cal_result = sec_ts_read_calibration_report(ts);

	for (i = 0; i < 4; i++)
		csum += csum_result[i];

	csum += nv_result;
	csum += cal_result;

	csum = ~csum;

	if (ts->plat_data->firmware_name != NULL) {

		ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_IMG_VERSION, fw_ver, 4);
		if (ret < 0)
			input_err(true, &ts->client->dev, "%s: firmware version read error\n", __func__);

		for (i = 0; i < 4; i++) {
			if (ts->plat_data->img_version_of_bin[i] !=  fw_ver[i]) {
				input_err(true, &ts->client->dev, "%s: do not matched version info [%d]:[%x]!=[%x] and skip!\n",
							__func__, i, ts->plat_data->img_version_of_bin[i], fw_ver[i]);
				goto out;
			}
		}

		ic_checksum = (((csum_result[0]&0xFF)<<24) | ((csum_result[1]&0xFF)<<16) |
						((csum_result[2]&0xFF)<<8) | ((csum_result[3]&0xFF)<<0));

		snprintf(fw_path, SEC_TS_MAX_FW_PATH, "%s", ts->plat_data->firmware_name);

		if (request_firmware(&fw_entry, fw_path, &ts->client->dev) !=  0) {
			input_err(true, &ts->client->dev, "%s: firmware is not available\n", __func__);
			snprintf(buff, sizeof(buff), "NG");
			goto err;
		}

		img_checksum = get_bin_checksum(ts, fw_entry->data, fw_entry->size);

		release_firmware(fw_entry);

		input_info(true, &ts->client->dev, "%s: img_checksum=[0x%X], ic_checksum=[0x%X]\n",
						__func__, img_checksum, ic_checksum);

		if (img_checksum != ic_checksum) {
			input_err(true, &ts->client->dev, "%s: img_checksum=[0x%X] != ic_checksum=[0x%X]!!!\n",
							__func__, img_checksum, ic_checksum);
			snprintf(buff, sizeof(buff), "NG");
			goto err;
		}
	}

out:
	if (ts->input_dev->disabled) {
		input_err(true, &ts->client->dev, "%s: status is changed\n", __func__);

		if (ts->lowpower_mode && !ts->prox_power_off) {
			mutex_lock(&ts->modechange);
			sec_ts_set_lowpowermode(ts, TO_LOWPOWER_MODE);
			sec_ts_set_aod_rect(ts);
			mutex_unlock(&ts->modechange);
		} else {
			sec_ts_stop_device(ts);
		}
	}

	input_info(true, &ts->client->dev, "%s: checksum = %02X\n", __func__, csum);
	snprintf(buff, sizeof(buff), "%02X", csum);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	return;

err:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	if (ts->input_dev->disabled) {
		input_err(true, &ts->client->dev, "%s: status is changed\n", __func__);

		if (ts->lowpower_mode && !ts->prox_power_off) {
			mutex_lock(&ts->modechange);
			sec_ts_set_lowpowermode(ts, TO_LOWPOWER_MODE);
			sec_ts_set_aod_rect(ts);
			mutex_unlock(&ts->modechange);
		}
	}
}

static void run_reference_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SEC;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_reference_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SEC;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void get_reference(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	short val = 0;
	int node = 0;

	sec_cmd_set_default_result(sec);
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	node = sec_ts_check_index(ts);
	if (node < 0)
		return;

	val = ts->pFrame[node];
	snprintf(buff, sizeof(buff), "%d", val);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void run_rawcap_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SDC;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_rawcap_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SDC;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void get_rawcap(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	short val = 0;
	int node = 0;

	sec_cmd_set_default_result(sec);
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	node = sec_ts_check_index(ts);
	if (node < 0)
		return;

	val = ts->pFrame[node];
	snprintf(buff, sizeof(buff), "%d", val);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void run_delta_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_delta_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void get_delta(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	short val = 0;
	int node = 0;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	node = sec_ts_check_index(ts);
	if (node < 0)
		return;

	val = ts->pFrame[node];
	snprintf(buff, sizeof(buff), "%d", val);
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void run_decoded_raw_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_DECODED_DATA;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_delta_cm_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_REMV_AMB_DATA;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_raw_p2p_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));

	/* both mutual, self for factory_cmd_result_all*/
	sec_ts_read_rawp2p_data_all(ts, sec, &mode);
}

static void run_raw_p2p_min_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA_P2P_MIN;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_rawp2p_data(ts, sec, &mode);
}

static void run_raw_p2p_max_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA_P2P_MAX;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_rawp2p_data(ts, sec, &mode);
}

/* self reference : send TX power in TX channel, receive in TX channel */
static void run_self_reference_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SEC;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_reference_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SEC;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_rawcap_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SDC;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_rawcap_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_OFFSET_DATA_SDC;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_delta_read(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_delta_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_raw_data(ts, sec, &mode);
}

static void run_self_raw_p2p_min_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA_P2P_MIN;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_rawp2p_data(ts, sec, &mode);
}

static void run_self_raw_p2p_max_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_mode mode;

	sec_cmd_set_default_result(sec);

	memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
	mode.type = TYPE_RAW_DATA_P2P_MAX;
	mode.frame_channel = TEST_MODE_READ_CHANNEL;
	mode.allnode = TEST_MODE_ALL_NODE;

	sec_ts_read_rawp2p_data(ts, sec, &mode);
}

void sec_ts_get_saved_cmoffset(struct sec_ts_data *ts)
{
	int rc;
	u8 *rBuff = NULL;
	int i;
	int result_size = SEC_TS_SELFTEST_REPORT_SIZE + ts->tx_count * ts->rx_count * 2;
	short min, max;

	input_raw_info_d(true, &ts->client->dev, "%s:\n", __func__);

	rBuff = kzalloc(result_size, GFP_KERNEL);
	if (!rBuff)
		return;

	rc = ts->sec_ts_i2c_read(ts, SEC_TS_READ_SELFTEST_RESULT, rBuff, result_size);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_exit;
	}

	memset(ts->pFrame, 0x00, result_size - SEC_TS_SELFTEST_REPORT_SIZE);

	for (i = 0; i < result_size - SEC_TS_SELFTEST_REPORT_SIZE; i += 2)
		ts->pFrame[i / 2] = (rBuff[SEC_TS_SELFTEST_REPORT_SIZE + i + 1] << 8)
				| rBuff[SEC_TS_SELFTEST_REPORT_SIZE + i];

	min = max = ts->pFrame[0];

	sec_ts_print_frame(ts, &min, &max);

err_exit:
	kfree(rBuff);
}

/*
 * sec_ts_run_rawdata_all : read all raw data
 *
 * when you want to read full raw data (full_read : true)
 * "mutual/self 3, 5, 29, 1, 19" data will be saved in log
 *
 * otherwise, (full_read : false, especially on boot time)
 * only "mutual 3, 5, 29" data will be saved in log
 */
void sec_ts_run_rawdata_all(struct sec_ts_data *ts, bool full_read)
{
	short min, max;
	int ret, i, read_num;
	u8 test_type[5] = {TYPE_AMBIENT_DATA, TYPE_DECODED_DATA,
		TYPE_SIGNAL_DATA, TYPE_OFFSET_DATA_SEC, TYPE_OFFSET_DATA_SDC};

	ts->tsp_dump_lock = 1;

#ifdef CONFIG_TOUCHSCREEN_DUAL_FOLDABLE
	input_raw_data_clear(SUB_TOUCH);
#endif
	input_raw_info_d(true, &ts->client->dev,
			"%s: start (noise:%d, wet:%d)##\n",
			__func__, ts->touch_noise_status, ts->wet_mode);

	ret = sec_ts_fix_tmode(ts, TOUCH_SYSTEM_MODE_TOUCH, TOUCH_MODE_STATE_TOUCH);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: failed to fix tmode\n",
				__func__);
		goto out;
	}

	if (full_read) {
		read_num = 5;
	} else {
		read_num = 3;
		test_type[read_num - 1] = TYPE_OFFSET_DATA_SDC;
	}

	for (i = 0; i < read_num; i++) {
		ret = sec_ts_read_frame(ts, test_type[i], &min, &max, false);
		if (ret < 0) {
			input_raw_info_d(true, &ts->client->dev,
					"%s: mutual %d : error ## ret:%d\n",
					__func__, test_type[i], ret);
			goto out;
		} else {
			input_raw_info_d(true, &ts->client->dev,
					"%s: mutual %d : Max/Min %d,%d ##\n",
					__func__, test_type[i], max, min);
		}
		sec_ts_delay(20);

#ifdef MINORITY_REPORT
		if (test_type[i] == TYPE_AMBIENT_DATA) {
			minority_report_calculate_rawdata(ts);
		} else if (test_type[i] == TYPE_OFFSET_DATA_SDC) {
			minority_report_calculate_ito(ts);
			minority_report_sync_latest_value(ts);
		}
#endif
		if (full_read) {
			ret = sec_ts_read_channel(ts, test_type[i], &min, &max, false);
			if (ret < 0) {
				input_raw_info_d(true, &ts->client->dev,
						"%s: self %d : error ## ret:%d\n",
						__func__, test_type[i], ret);
				goto out;
			} else {
				input_raw_info_d(true, &ts->client->dev,
						"%s: self %d : Max/Min %d,%d ##\n",
						__func__, test_type[i], max, min);
			}
			sec_ts_delay(20);
		}
	}

	sec_ts_release_tmode(ts);

	if (full_read)
		sec_ts_get_saved_cmoffset(ts);

out:
	input_raw_info_d(true, &ts->client->dev, "%s: ito : %02X %02X %02X %02X\n",
			__func__, ts->ito_test[0], ts->ito_test[1]
			, ts->ito_test[2], ts->ito_test[3]);

	input_raw_info_d(true, &ts->client->dev, "%s: done (noise:%d, wet:%d)##\n",
			__func__, ts->touch_noise_status, ts->wet_mode);
	ts->tsp_dump_lock = 0;

	sec_ts_locked_release_all_finger(ts);
}

static void run_rawdata_read_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };

	sec_cmd_set_default_result(sec);

	if (ts->tsp_dump_lock == 1) {
		input_err(true, &ts->client->dev, "%s: already checking now\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: IC is power off\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	sec_ts_run_rawdata_all(ts, true);

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
out:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

/* Use TSP NV area
 * buff[0] : offset from user NVM storage
 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
 * buff[2] : write data
 * buff[..] : cont.
 */
static void set_tsp_nvm_data_clear(struct sec_ts_data *ts, u8 offset)
{
	char buff[4] = { 0 };
	int ret;

	input_dbg(true, &ts->client->dev, "%s\n", __func__);

	buff[0] = offset;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 3);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);

	sec_ts_delay(20);
}

int get_tsp_nvm_data(struct sec_ts_data *ts, u8 offset)
{
	char buff[2] = { 0 };
	int ret;

	/* SENSE OFF -> CELAR EVENT STACK -> READ NV -> SENSE ON */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSE_OFF, NULL, 0);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: fail to write Sense_off\n", __func__);
		goto out_nvm;
	}

	input_dbg(false, &ts->client->dev, "%s: SENSE OFF\n", __func__);

	sec_ts_delay(100);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CLEAR_EVENT_STACK, NULL, 0);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: i2c write clear event failed\n", __func__);
		goto out_nvm;
	}

	input_dbg(false, &ts->client->dev, "%s: CLEAR EVENT STACK\n", __func__);

	sec_ts_delay(100);

	sec_ts_locked_release_all_finger(ts);

	/* send NV data using command
	 * Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 */
	memset(buff, 0x00, 2);
	buff[0] = offset;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 2);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm send command failed. ret: %d\n", __func__, ret);
		goto out_nvm;
	}

	sec_ts_delay(20);

	/* read NV data
	 * Use TSP NV area : in this model, use only one byte
	 */
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_NVM, buff, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm send command failed. ret: %d\n", __func__, ret);
		goto out_nvm;
	}

	input_info(true, &ts->client->dev, "%s: offset:%u  data:%02X\n", __func__, offset, buff[0]);

out_nvm:
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSE_ON, NULL, 0);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: fail to write Sense_on\n", __func__);

	sec_ts_delay(300);

	input_dbg(false, &ts->client->dev, "%s: SENSE ON\n", __func__);

	return buff[0];
}

int get_tsp_nvm_data_by_size(struct sec_ts_data *ts, u8 offset, int length, u8 *data)
{
	char *buff = NULL;
	int ret;

	buff = kzalloc(length, GFP_KERNEL);
	if (!buff)
		return -ENOMEM;

	input_info(true, &ts->client->dev, "%s: offset:%u, length:%d, size:%d\n", __func__, offset, length, sizeof(data));

	/* SENSE OFF -> CELAR EVENT STACK -> READ NV -> SENSE ON */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSE_OFF, NULL, 0);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: fail to write Sense_off\n", __func__);
		goto out_nvm;
	}

	input_dbg(true, &ts->client->dev, "%s: SENSE OFF\n", __func__);

	sec_ts_delay(100);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CLEAR_EVENT_STACK, NULL, 0);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: i2c write clear event failed\n", __func__);
		goto out_nvm;
	}

	input_dbg(true, &ts->client->dev, "%s: CLEAR EVENT STACK\n", __func__);

	sec_ts_delay(100);

	sec_ts_locked_release_all_finger(ts);

	/* send NV data using command
	 * Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 */
	memset(buff, 0x00, 2);
	buff[0] = offset;
	buff[1] = length - 1;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 2);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm send command failed. ret: %d\n", __func__, ret);
		goto out_nvm;
	}

	sec_ts_delay(20);

	/* read NV data
	 * Use TSP NV area : in this model, use only one byte
	 */
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_NVM, buff, length);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: nvm send command failed. ret: %d\n", __func__, ret);
		goto out_nvm;
	}

	memcpy(data, buff, length);

out_nvm:
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SENSE_ON, NULL, 0);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: fail to write Sense_on\n", __func__);

	sec_ts_delay(300);

	input_dbg(true, &ts->client->dev, "%s: SENSE ON\n", __func__);

	kfree(buff);

	return ret;
}

static int set_tsp_nvm_data_by_size(struct sec_ts_data *ts, u8 reg, int size, u8 *data)
{
	int rc;
	u8 buff[SEC_CMD_STR_LEN] = {0};

	buff[0] = reg;
	buff[1] = size - 1;	/* 1bytes */
	memcpy(&buff[2], data, size);
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, size + 2);
	if (rc < 0) {
		input_err(true, &ts->client->dev,
			"%s: nvm write failed. ret: %d\n", __func__, rc);
	}
	sec_ts_delay(20);
	return rc;
}

int sec_tclm_data_read(struct i2c_client *client, int address)
{
	struct sec_ts_data *ts = i2c_get_clientdata(client);
	int ret = 0;
	u8 buff[4];
	u8 nbuff[SEC_TS_NVM_OFFSET_LENGTH - SEC_TS_NVM_OFFSET_CAL_COUNT];

	switch (address) {
	case SEC_TCLM_NVM_OFFSET_IC_FIRMWARE_VER:
		sec_ts_delay(100);
		ret = ts->sec_ts_i2c_read(ts, SEC_TS_READ_IMG_VERSION, buff, 4);
		if (ret < 0)
			return ret;
		input_err(true, &ts->client->dev, "%s SEC_TS_READ_IMG_VERSION buff[2]:%02X buff[3]:%02X \n",
			__func__, buff[2], buff[3]);
		ret = (buff[2] << 8) | buff[3];
		return ret;
	case SEC_TCLM_NVM_ALL_DATA:
		memset(&ts->tdata->nvdata, 0x00, sizeof(struct sec_tclm_nvdata));
		
		ret = get_tsp_nvm_data_by_size(ts, SEC_TS_NVM_OFFSET_CAL_COUNT, sizeof(struct sec_tclm_nvdata), nbuff);
		if (ret < 0)
			return ret;

		memcpy(&ts->tdata->nvdata, nbuff, sizeof(struct sec_tclm_nvdata));
		return ret;
	case SEC_TCLM_NVM_TEST:
		input_info(true, &ts->client->dev, "%s: dt: tclm_level [%d] afe_base [%04X]\n",
			__func__, ts->tdata->tclm_level, ts->tdata->afe_base);
		ret = get_tsp_nvm_data_by_size(ts, SEC_TS_NVM_TOTAL_OFFSET_LENGTH + SEC_TCLM_NVM_OFFSET,
			SEC_TCLM_NVM_OFFSET_LENGTH, ts->tdata->tclm);
		if (ts->tdata->tclm[0] != 0xFF) {
			ts->tdata->tclm_level = ts->tdata->tclm[0];
			ts->tdata->afe_base = (ts->tdata->tclm[1] << 8) | ts->tdata->tclm[2];
		input_info(true, &ts->client->dev, "%s: nv: tclm_level [%d] afe_base [%04X]\n",
			__func__, ts->tdata->tclm_level, ts->tdata->afe_base);
		}
		return ret;
	default:
		return ret;
	}
}

int sec_tclm_data_write(struct i2c_client *client, int address)
{
	struct sec_ts_data *ts = i2c_get_clientdata(client);
	int ret = 1;
	u8 nbuff[SEC_TS_NVM_OFFSET_LENGTH - SEC_TS_NVM_OFFSET_CAL_COUNT];

	memset(nbuff, 0x00, sizeof(struct sec_tclm_nvdata));
	switch (address) {
	case SEC_TCLM_NVM_ALL_DATA:
		memcpy(nbuff, &ts->tdata->nvdata, sizeof(struct sec_tclm_nvdata));
		ret = set_tsp_nvm_data_by_size(ts, SEC_TS_NVM_OFFSET_CAL_COUNT, sizeof(struct sec_tclm_nvdata), nbuff);
		return ret;
	case SEC_TCLM_NVM_TEST:
		ret = set_tsp_nvm_data_by_size(ts, SEC_TS_NVM_TOTAL_OFFSET_LENGTH + SEC_TCLM_NVM_OFFSET,
			SEC_TCLM_NVM_OFFSET_LENGTH, ts->tdata->tclm);
		return ret;
	default:
		return ret;
	}
}

#if 0
static void sec_ts_tclm_set_nvm_data_2byte(struct sec_ts_data *ts, u8 reg, u8 data1, u8 data2)
{
	char buff[SEC_CMD_STR_LEN] = {0};
	int rc;

	buff[0] = reg;
	buff[1] = 1;	/* 2bytes */
	buff[2] = data1;
	buff[3] = data2;
	input_info(true, &ts->client->dev, "%s: write nvm (%2X %2X)\n", __func__, buff[2], buff[3]);

	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 4);
	if (rc < 0) {
		input_err(true, &ts->client->dev,
			"%s: nvm write failed. ret: %d\n", __func__, rc);
	}
	sec_ts_delay(20);
}
#endif

#ifdef MINORITY_REPORT

/*	ts->defect_probability is FFFFF
 *
 *	0	is 100% normal.
 *	1~9	is normal, but need check
 *	A~E	is abnormal, must check
 *	F	is Device defect
 *
 *	F----	: ito
 *	-F---	: rawdata
 *	--A--	: crc
 *	---A-	: i2c_err
 *	----A	: wet
 */
static void minority_report_calculate_rawdata(struct sec_ts_data *ts)
{
	int ii, jj;
	int temp = 0;
	int max = -30000;
	int min = 30000;
	int node_gap = 1;
	short tx_max[TOUCH_TX_CHANNEL_NUM] = { 0 };
	short tx_min[TOUCH_TX_CHANNEL_NUM] = { 0 };
	short rx_max[TOUCH_RX_CHANNEL_NUM] = { 0 };
	short rx_min[TOUCH_RX_CHANNEL_NUM] = { 0 };

	for (ii = 0; ii < TOUCH_TX_CHANNEL_NUM; ii++) {
		tx_max[ii] = -30000;
		tx_min[ii] = 30000;
	}

	for (ii = 0; ii < TOUCH_RX_CHANNEL_NUM; ii++) {
		rx_max[ii] = -30000;
		rx_min[ii] = 30000;
	}

	if (!ts->tx_count) {
		ts->item_rawdata = 0xD;
		return;
	}

	for (ii = 0; ii < (ts->rx_count * ts->tx_count); ii++) {
		if (max < ts->pFrame[ii]) {
			ts->max_ambient = max = ts->pFrame[ii];
			ts->max_ambient_channel_tx = ii % ts->tx_count;
			ts->max_ambient_channel_rx = ii / ts->tx_count;
		}
		if (min > ts->pFrame[ii]) {
			ts->min_ambient = min = ts->pFrame[ii];
			ts->min_ambient_channel_tx = ii % ts->tx_count;
			ts->min_ambient_channel_rx = ii / ts->tx_count;
		}

		if ((ii + 1) % (ts->tx_count) != 0) {
			temp = ts->pFrame[ii] - ts->pFrame[ii+1];
			if (temp < 0)
				temp = -temp;
			if (temp > node_gap)
				node_gap = temp;
		}

		if (ii < (ts->rx_count - 1) * ts->tx_count) {
			temp = ts->pFrame[ii] - ts->pFrame[ii + ts->tx_count];
			if (temp < 0)
				temp = -temp;
			if (temp > node_gap)
				node_gap = temp;
		}

		ts->ambient_rx[ii / ts->tx_count] += ts->pFrame[ii];
		ts->ambient_tx[ii % ts->tx_count] += ts->pFrame[ii];

		rx_max[ii / ts->tx_count] = max(rx_max[ii / ts->tx_count], ts->pFrame[ii]);
		rx_min[ii / ts->tx_count] = min(rx_min[ii / ts->tx_count], ts->pFrame[ii]);
		tx_max[ii % ts->tx_count] = max(tx_max[ii % ts->tx_count], ts->pFrame[ii]);
		tx_min[ii % ts->tx_count] = min(tx_min[ii % ts->tx_count], ts->pFrame[ii]);

	}

	for (ii = 0; ii < ts->tx_count; ii++)
		ts->ambient_tx[ii] /= ts->rx_count;

	for (ii = 0; ii < ts->rx_count; ii++)
		ts->ambient_rx[ii] /= ts->tx_count;

	for (ii = 0; ii < ts->rx_count; ii++)
		ts->ambient_rx_delta[ii] = rx_max[ii] - rx_min[ii];

	for (jj = 0; jj < ts->tx_count; jj++)
		ts->ambient_tx_delta[jj] = tx_max[jj] - tx_min[jj];

	if (max >= 80 || min <= -80)
		ts->item_rawdata = 0xF;
	else if ((max >= 50 || min <= -50) && (node_gap > 40))
		ts->item_rawdata = 0xC;
	else if (max >= 50 || min <= -50)
		ts->item_rawdata = 0xB;
	else if (node_gap > 40)
		ts->item_rawdata = 0xA;
	else if ((max >= 40 || min <= -40) && (node_gap > 30))
		ts->item_rawdata = 0x3;
	else if (max >= 40 || min <= -40)
		ts->item_rawdata = 0x2;
	else if (node_gap > 30)
		ts->item_rawdata = 0x1;
	else
		ts->item_rawdata = 0;

	input_info(true, &ts->client->dev, "%s min:%d,max:%d,node gap:%d =>%X\n",
			__func__, min, max, node_gap, ts->item_rawdata);

}

static void minority_report_calculate_ito(struct sec_ts_data *ts)
{

	if (ts->ito_test[0] ||  ts->ito_test[1] || ts->ito_test[2] || ts->ito_test[3])
		ts->item_ito = 0xF;
	else
		ts->item_ito = 0;
}

u8 minority_report_check_count(int value)
{
	u8 ret;

	if (value > 160)
		ret = 0xA;
	else if (value > 90)
		ret = 3;
	else if (value > 40)
		ret = 2;
	else if (value > 10)
		ret = 1;
	else
		ret = 0;

	return ret;
}

void minority_report_sync_latest_value(struct sec_ts_data *ts)
{
	u32 temp = 0;

	/* crc */
	if (ts->checksum_result == 1)
		ts->item_crc = 0xA;

	/* i2c_err */
	ts->item_i2c_err = minority_report_check_count(ts->comm_err_count);

	/* wet */
	ts->item_wet = minority_report_check_count(ts->wet_count);

	temp |= (ts->item_ito & 0xF) << 16;
	temp |= (ts->item_rawdata & 0xF) << 12;
	temp |= (ts->item_crc & 0xF) << 8;
	temp |= (ts->item_i2c_err & 0xF) << 4;
	temp |= (ts->item_wet & 0xF);

	ts->defect_probability = temp;
}
#endif

/* FACTORY TEST RESULT SAVING FUNCTION
 * bit 3 ~ 0 : OCTA Assy
 * bit 7 ~ 4 : OCTA module
 * param[0] : OCTA modue(1) / OCTA Assy(2)
 * param[1] : TEST NONE(0) / TEST FAIL(1) / TEST PASS(2) : 2 bit
 */

#define TEST_OCTA_MODULE	1
#define TEST_OCTA_ASSAY		2

#define TEST_OCTA_NONE		0
#define TEST_OCTA_FAIL		1
#define TEST_OCTA_PASS		2

static void set_tsp_test_result(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	struct sec_ts_test_result *result;
	char buff[SEC_CMD_STR_LEN] = { 0 };
	char r_data[1] = { 0 };
	int ret = 0;

	input_info(true, &ts->client->dev, "%s\n", __func__);

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	r_data[0] = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);
	if (r_data[0] == 0xFF)
		r_data[0] = 0;

	result = (struct sec_ts_test_result *)r_data;

	if (sec->cmd_param[0] == TEST_OCTA_ASSAY) {
		result->assy_result = sec->cmd_param[1];
		if (result->assy_count < 3)
			result->assy_count++;
	}

	if (sec->cmd_param[0] == TEST_OCTA_MODULE) {
		result->module_result = sec->cmd_param[1];
		if (result->module_count < 3)
			result->module_count++;
	}

	input_info(true, &ts->client->dev, "%s: %d, %d, %d, %d, 0x%X\n", __func__,
			result->module_result, result->module_count,
			result->assy_result, result->assy_count, result->data[0]);

	/* Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * buff[2] : write data
	 */
	memset(buff, 0x00, SEC_CMD_STR_LEN);
	buff[2] = *result->data;

	input_info(true, &ts->client->dev, "%s: command (1)%X, (2)%X: %X\n",
			__func__, sec->cmd_param[0], sec->cmd_param[1], buff[2]);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 3);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);

	sec_ts_delay(20);

	ts->nv = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);

	snprintf(buff, sizeof(buff), "OK");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

static void get_tsp_test_result(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	struct sec_ts_test_result *result;

	input_info(true, &ts->client->dev, "%s\n", __func__);

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	memset(buff, 0x00, SEC_CMD_STR_LEN);
	buff[0] = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);
	if (buff[0] == 0xFF) {
		set_tsp_nvm_data_clear(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);
		buff[0] = 0;
	}

	ts->nv = buff[0];

	result = (struct sec_ts_test_result *)buff;

	input_info(true, &ts->client->dev, "%s: [0x%X][0x%X] M:%d, M:%d, A:%d, A:%d\n",
			__func__, *result->data, buff[0],
			result->module_result, result->module_count,
			result->assy_result, result->assy_count);

	snprintf(buff, sizeof(buff), "M:%s, M:%d, A:%s, A:%d",
			result->module_result == 0 ? "NONE" :
			result->module_result == 1 ? "FAIL" :
			result->module_result == 2 ? "PASS" : "A",
			result->module_count,
			result->assy_result == 0 ? "NONE" :
			result->assy_result == 1 ? "FAIL" :
			result->assy_result == 2 ? "PASS" : "A",
			result->assy_count);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

static void clear_tsp_test_result(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret = 0;

	input_info(true, &ts->client->dev, "%s\n", __func__);

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	/* Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * buff[2] : write data
	 */
	memset(buff, 0x00, SEC_CMD_STR_LEN);
	buff[0] = SEC_TS_NVM_OFFSET_FAC_RESULT;
	buff[1] = 0x00;
	buff[2] = 0x00;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 3);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);

	sec_ts_delay(20);

	ts->nv = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_FAC_RESULT);

	snprintf(buff, sizeof(buff), "OK");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

static void increase_disassemble_count(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[3] = { 0 };
	int ret = 0;

	input_info(true, &ts->client->dev, "%s\n", __func__);

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	buff[2] = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_DISASSEMBLE_COUNT);

	input_info(true, &ts->client->dev, "%s: disassemble count is #1 %d\n", __func__, buff[2]);

	if (buff[2] == 0xFF)
		buff[2] = 0;

	if (buff[2] < 0xFE)
		buff[2]++;

	/* Use TSP NV area : in this model, use only one byte
	 * buff[0] : offset from user NVM storage
	 * buff[1] : length of stroed data - 1 (ex. using 1byte, value is  1 - 1 = 0)
	 * buff[2] : write data
	 */
	buff[0] = SEC_TS_NVM_OFFSET_DISASSEMBLE_COUNT;
	buff[1] = 0;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_NVM, buff, 3);
	if (ret < 0)
		input_err(true, &ts->client->dev, "%s: nvm write failed. ret: %d\n", __func__, ret);

	sec_ts_delay(20);

	memset(buff, 0x00, 3);
	buff[0] = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_DISASSEMBLE_COUNT);
	input_info(true, &ts->client->dev, "%s: check disassemble count: %d\n", __func__, buff[0]);

	snprintf(buff, sizeof(buff), "OK");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

static void get_disassemble_count(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	input_info(true, &ts->client->dev, "%s\n", __func__);

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: [ERROR] Touch is stopped\n",
				__func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	memset(buff, 0x00, SEC_CMD_STR_LEN);
	buff[0] = get_tsp_nvm_data(ts, SEC_TS_NVM_OFFSET_DISASSEMBLE_COUNT);
	if (buff[0] == 0xFF) {
		set_tsp_nvm_data_clear(ts, SEC_TS_NVM_OFFSET_DISASSEMBLE_COUNT);
		buff[0] = 0;
	}

	input_info(true, &ts->client->dev, "%s: read disassemble count: %d\n", __func__, buff[0]);

	snprintf(buff, sizeof(buff), "%d", buff[0]);

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
}

static void glove_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		if (sec->cmd_param[0])
			ts->touch_functions |= SEC_TS_BIT_SETFUNC_GLOVE;
		else
			ts->touch_functions &= ~SEC_TS_BIT_SETFUNC_GLOVE;

		ret = sec_ts_set_touch_function(ts);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: failed, retval = %d\n", __func__, ret);
			snprintf(buff, sizeof(buff), "NG");
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
		} else {
			snprintf(buff, sizeof(buff), "OK");
			sec->cmd_state = SEC_CMD_STATUS_OK;
		}
	}

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s cmd_param: %d\n", __func__, buff, sec->cmd_param[0]);
}

static void clear_cover_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	input_info(true, &ts->client->dev, "%s: start clear_cover_mode %s\n", __func__, buff);
	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 3) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		if (sec->cmd_param[0] > 1) {
			ts->cover_type = sec->cmd_param[1];
			ts->cover_cmd = (u8)ts->cover_type;
			sec_ts_set_cover_type(ts, true);
		} else {
			sec_ts_set_cover_type(ts, false);
		}

		snprintf(buff, sizeof(buff), "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	}
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
};

static void dead_zone_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;
	char data = 0;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		data = sec->cmd_param[0];

		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_EDGE_DEADZONE, &data, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev,
					"%s: failed to set deadzone\n", __func__);
			snprintf(buff, sizeof(buff), "NG");
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
			goto err_set_dead_zone;
		}

		snprintf(buff, sizeof(buff), "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	}

err_set_dead_zone:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
};

/*
 * drawing_test_enable
 * - It is called when the *#0*# touch drawing test mode is started.
 */
static void drawing_test_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		snprintf(buff, sizeof(buff), "NA");
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
	}

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
};

static void sec_ts_swap(u8 *a, u8 *b)
{
	u8 temp = *a;

	*a = *b;
	*b = temp;
}

static void rearrange_sft_result(u8 *data, int length)
{
	int i, nlength;

	nlength = length - (length % 4);

	for (i = 0; i < nlength; i += 4) {
		sec_ts_swap(&data[i], &data[i + 3]);
		sec_ts_swap(&data[i + 1], &data[i + 2]);
	}
}

static int execute_selftest(struct sec_ts_data *ts, bool save_result)
{
	u8 pStr[50] = {0};
	u8 pTmp[20];
	int rc = 0;
	u8 tpara[2] = {0x23, 0x40};
	u8 *rBuff;
	int i;
	int result_size = SEC_TS_SELFTEST_REPORT_SIZE + ts->tx_count * ts->rx_count * 2;

	/* set Factory level */
	if (ts->factory_level) {
		rc = sec_ts_write_factory_level(ts, ts->factory_position);
		if (rc < 0)
			goto err_set_level;
	}

	/* save selftest result in flash */
	if (save_result)
		tpara[0] = 0x23;
	else
		tpara[0] = 0xA3;

	rBuff = kzalloc(result_size, GFP_KERNEL);
	if (!rBuff)
		goto err_mem;

	input_info(true, &ts->client->dev, "%s: Self test start!\n", __func__);
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SELFTEST, tpara, 2);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Send selftest cmd failed!\n", __func__);
		goto err_exit;
	}

	sec_ts_delay(350);

	rc = sec_ts_wait_for_ready(ts, SEC_TS_VENDOR_ACK_SELF_TEST_DONE);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_exit;
	}

	input_raw_info_d(true, &ts->client->dev, "%s: Self test done!\n", __func__);

	rc = ts->sec_ts_i2c_read(ts, SEC_TS_READ_SELFTEST_RESULT, rBuff, result_size);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_exit;
	}

	rearrange_sft_result(rBuff, SEC_TS_SELFTEST_REPORT_SIZE);

	for (i = 0; i < 80; i += 4) {
		if (i / 4 == 0)
			strlcat(pStr, "SIG ", sizeof(pStr));
		else if (i / 4 == 1)
			strlcat(pStr, "VER ", sizeof(pStr));
		else if (i / 4 == 2)
			strlcat(pStr, "SIZ ", sizeof(pStr));
		else if (i / 4 == 3)
			strlcat(pStr, "CRC ", sizeof(pStr));
		else if (i / 4 == 4)
			strlcat(pStr, "RES ", sizeof(pStr));
		else if (i / 4 == 5)
			strlcat(pStr, "COU ", sizeof(pStr));
		else if (i / 4 == 6)
			strlcat(pStr, "PAS ", sizeof(pStr));
		else if (i / 4 == 7)
			strlcat(pStr, "FAI ", sizeof(pStr));
		else if (i / 4 == 8)
			strlcat(pStr, "CHA ", sizeof(pStr));
		else if (i / 4 == 9)
			strlcat(pStr, "AMB ", sizeof(pStr));
		else if (i / 4 == 10)
			strlcat(pStr, "RXS ", sizeof(pStr));
		else if (i / 4 == 11)
			strlcat(pStr, "TXS ", sizeof(pStr));
		else if (i / 4 == 12)
			strlcat(pStr, "RXO ", sizeof(pStr));
		else if (i / 4 == 13)
			strlcat(pStr, "TXO ", sizeof(pStr));
		else if (i / 4 == 14)
			strlcat(pStr, "RXG ", sizeof(pStr));
		else if (i / 4 == 15)
			strlcat(pStr, "TXG ", sizeof(pStr));
		else if (i / 4 == 16)
			strlcat(pStr, "RXR ", sizeof(pStr));
		else if (i / 4 == 17)
			strlcat(pStr, "TXT ", sizeof(pStr));
		else if (i / 4 == 18)
			strlcat(pStr, "RXT ", sizeof(pStr));
		else if (i / 4 == 19)
			strlcat(pStr, "TXR ", sizeof(pStr));

		snprintf(pTmp, sizeof(pTmp), "%2X, %2X, %2X, %2X",
			rBuff[i], rBuff[i + 1], rBuff[i + 2], rBuff[i + 3]);
		strlcat(pStr, pTmp, sizeof(pStr));

		if (i / 4 == 4) {
			if ((rBuff[i + 3] & 0x30) != 0)// RX, RX open check.
				rc = 0;
			else
				rc = 1;

			ts->ito_test[0] = rBuff[i];
			ts->ito_test[1] = rBuff[i + 1];
			ts->ito_test[2] = rBuff[i + 2];
			ts->ito_test[3] = rBuff[i + 3];
		}
		if (i % 8 == 4) {
			input_raw_info_d(true, &ts->client->dev, "%s\n", pStr);
			memset(pStr, 0x00, sizeof(pStr));
		} else {
			strlcat(pStr, "  ", sizeof(pStr));
		}
	}

err_exit:
	kfree(rBuff);
err_mem:
	if (ts->factory_level)
		sec_ts_write_factory_level(ts, OFFSET_FW_NOSAVE);
err_set_level:
	return rc;
}

/*
 * bit		| [0]	[1]	..	[8]	[9]	..	[16]	 [17]	..	[24]	..	[31]
 * byte[0]	| TX0	TX1	..	TX8	TX9	..	TX16 RX0	..	RX7	..	RX14
 * byte[1]	| RX15	RX16	..	RX24	RX31	..	RX34 F0 F1 F2	..
 */
static void run_trx_short_test(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	char tempn[40] = {0};
	char tempv[25] = {0};
	int rc;
	int size = SEC_TS_SELFTEST_REPORT_SIZE + ts->tx_count * ts->rx_count * 2;
	char para = TO_TOUCH_MODE;
	u8 *rBuff = NULL;
	short *pFrame = NULL;
	int ii, jj;
	u8 data[32] = { 0 };
	int len = 0;
	int delay = 0;
	int checklen = 0;
	int start_point = 0;
	int sum = 0;
	char test[32];
	char result[32];
	u64 temp_result;
	u8 *test_result_buff;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[1])
		snprintf(test, sizeof(test), "TEST=%d,%d", sec->cmd_param[0], sec->cmd_param[1]);
	else
		snprintf(test, sizeof(test), "TEST=%d", sec->cmd_param[0]);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	if (sec->cmd_param[0] == OPEN_SHORT_TEST && sec->cmd_param[1] == 0) {
		input_err(true, &ts->client->dev,
				"%s: %s: seperate cm1 test open / short test result\n", __func__, buff);

		snprintf(buff, sizeof(buff), "%s", "CONT");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_OK;
		return;
	}

	test_result_buff = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if (!test_result_buff) {
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	rBuff = kzalloc(size, GFP_KERNEL);
	if (!rBuff) {
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		kfree(test_result_buff);
		return;
	}

	memset(rBuff, 0x00, size);
	memset(data, 0x00, 32);

	disable_irq(ts->client->irq);

	/*
	 * old version remaining
	 * old version is not send parameter
	 */
	if (sec->cmd_param[0] == 0) {
		rc = execute_selftest(ts, false);

		sec_ts_locked_release_all_finger(ts);

		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
		enable_irq(ts->client->irq);

		if (rc > 0) {
			snprintf(buff, sizeof(buff), "OK");
			sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
			sec->cmd_state = SEC_CMD_STATUS_OK;
		} else {
			snprintf(buff, sizeof(buff), "NG");
			sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
		}

		input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
		kfree(test_result_buff);
		kfree(rBuff);
		return;
	}

	input_info(true, &ts->client->dev, "%s: set power mode to test mode\n", __func__);
	data[0] = 0x02;
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, data, 1);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: set test mode failed\n", __func__);
		goto err_trx_short;
	}

	input_info(true, &ts->client->dev, "%s: clear event stack\n", __func__);
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CLEAR_EVENT_STACK, NULL, 0);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: clear event stack failed\n", __func__);
		goto err_trx_short;
	}

	if (sec->cmd_param[0] == OPEN_SHORT_TEST &&
			sec->cmd_param[1] == CHECK_ONLY_OPEN_TEST) {
		data[0] = 0xB7;
		len = 1;
		delay = 700;
		checklen = 8;
		start_point = 0;
	} else if (sec->cmd_param[0] == OPEN_SHORT_TEST &&
			sec->cmd_param[1] == CHECK_ONLY_SHORT_TEST) {
		data[0] = 0xB7;
		len = 1;
		delay = 700;
		checklen = 8 * 4;
		start_point = 8;
	} else if (sec->cmd_param[0] == CRACK_TEST) {
		data[0] = 0x81;
		data[1] = 0x01;
		len = 2;
		delay = 200;
		checklen = 8;
		start_point = 0;
	} else if (sec->cmd_param[0] == BRIDGE_SHORT_TEST) {
		data[0] = 0x81;
		data[1] = 0x02;
		len = 2;
		delay = 1000;
		checklen = 8;
		start_point = 0;
	}

	/* set Factory level */
	if (ts->factory_level) {
		rc = sec_ts_write_factory_level(ts, ts->factory_position);
		if (rc < 0)
			goto err_trx_short;
	}

	input_info(true, &ts->client->dev, "%s: self test start\n", __func__);
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SELFTEST, data, len);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Send selftest cmd failed!\n", __func__);
		goto err_trx_short;
	}

	sec_ts_delay(delay);

	rc = sec_ts_wait_for_ready(ts, SEC_TS_VENDOR_ACK_SELF_TEST_DONE);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_trx_short;
	}

	input_info(true, &ts->client->dev, "%s: self test done\n", __func__);

	rc = ts->sec_ts_i2c_read(ts, SEC_TS_READ_SELFTEST_RESULT, rBuff, size);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_trx_short;
	}

	sec_ts_locked_release_all_finger(ts);

	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: mode changed failed!\n", __func__);
		goto err_trx_short;
	}

	sec_ts_reinit(ts);

	input_info(true, &ts->client->dev, "%s: %02X, %02X, %02X, %02X\n",
			__func__, rBuff[16], rBuff[17], rBuff[18], rBuff[19]);

	pFrame = vmalloc(ts->tx_count * ts->rx_count * 2);
	if (!pFrame)
		goto err_trx_short;

	for (ii = 0; ii < (ts->tx_count * ts->rx_count * 2); ii += 2)
		pFrame[ii / 2] = (rBuff[80 + ii + 1] << 8) + rBuff[80 + ii];

	input_info(true, &ts->client->dev, "%s: #29\n", __func__);
	for (ii = 0; ii < ts->rx_count; ii++) {
		pr_info("[sec_input]: ");
		for (jj = 0; jj < ts->tx_count; jj++)
			pr_cont(" %3d", pFrame[(jj * ts->rx_count) + ii]);
		pr_cont("\n");
	}
	vfree(pFrame);
	enable_irq(ts->client->irq);

	memcpy(data, &rBuff[48], 32);

	for (ii = start_point ; ii < checklen; ii++)
		sum += data[ii];

	if (!sum)
		goto test_ok;

	// set start check point for cm1 short test
	for (ii = start_point ; ii < checklen; ii += 8) {
		int jj;
		long long lldata = 0;

		memcpy(&lldata, &data[ii], 8);
		if (!lldata)
			continue;

		memset(tempn, 0x00, 40);

		if (sec->cmd_param[0] == OPEN_SHORT_TEST) {
			if (ii / 8 == 0)
				snprintf(tempn, 40, " TX/RX_OPEN:");
			else if (ii / 8 == 1)
				snprintf(tempn, 40, " TX/RX_SHORT_TO_GND:");
			else if (ii / 8 == 2)
				snprintf(tempn, 40, " TX/RX_SHORT_TO_TX/RX:");
			else if (ii / 8 == 3)
				snprintf(tempn, 40, " TX/RX_SHORT_TO_RX/TX:");
		} else if (sec->cmd_param[0] == CRACK_TEST) {
			snprintf(tempn, 40, " CRACK:");
		} else if (sec->cmd_param[0] == BRIDGE_SHORT_TEST) {
			snprintf(tempn, 40, "BRIDGE_SHORT:");
		}
		strlcat(test_result_buff, tempn, PAGE_SIZE);
		memcpy(&temp_result, &data[ii], 8);

		for (jj = 0; jj < ts->tx_count + ts->rx_count; jj++) {
			memset(tempv, 0x00, 25);
			if (temp_result & 0x1)
				snprintf(tempv, 20, "%cX%d,",
						jj < ts->tx_count ? 'T':'R',
						jj < ts->tx_count ? jj : jj - ts->tx_count);
			strlcat(test_result_buff, tempv, PAGE_SIZE);
			temp_result = temp_result >> 1;
		}
	}

	sec_cmd_set_cmd_result(sec, test_result_buff, strnlen(test_result_buff, PAGE_SIZE));
	sec->cmd_state = SEC_CMD_STATUS_FAIL;

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, test_result_buff);

	sec_ts_write_factory_level(ts, OFFSET_FW_NOSAVE);

	kfree(rBuff);
	kfree(test_result_buff);
	snprintf(result, sizeof(result), "RESULT=FAIL");
	sec_cmd_send_event_to_user(&ts->sec, test, result);
	return;

test_ok:
	snprintf(buff, sizeof(buff), "OK");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);

	sec_ts_write_factory_level(ts, OFFSET_FW_NOSAVE);

	kfree(rBuff);
	kfree(test_result_buff);
	snprintf(result, sizeof(result), "RESULT=PASS");
	sec_cmd_send_event_to_user(&ts->sec, test, result);
	return;

err_trx_short:

	sec_ts_locked_release_all_finger(ts);

	ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &para, 1);
	enable_irq(ts->client->irq);

	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_FAIL;

	sec_ts_write_factory_level(ts, OFFSET_FW_NOSAVE);

	kfree(rBuff);
	kfree(test_result_buff);
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
	snprintf(result, sizeof(result), "RESULT=FAIL");
	sec_cmd_send_event_to_user(&ts->sec, test, result);
	return;
}
#if 0	// not use star~
static void run_lowpower_selftest(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[16] = { 0 };
	char rwbuf[2] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	disable_irq(ts->client->irq);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, rwbuf, 1);
	if (ret < 0)
		goto err_exit;

	rwbuf[0] = 0x5;
	rwbuf[1] = 0x0;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CHG_SYSMODE, rwbuf, 2);
	if (ret < 0)
		goto err_exit;

	sec_ts_delay(20);

	rwbuf[0] = 0x1;
	rwbuf[1] = 0x0;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_START_LOWPOWER_TEST, rwbuf, 1);
	if (ret < 0)
		goto err_exit;

	ret = sec_ts_wait_for_ready(ts,  SEC_TS_VENDOR_ACK_LOWPOWER_SELF_TEST_DONE);
	if (ret < 0)
		goto err_exit;

	rwbuf[0] = 0;
	rwbuf[1] = 0;
	/* success : 1, fail : 0 */
	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_START_LOWPOWER_TEST, rwbuf, 1);
	if (rwbuf[0] != 1) {
		input_err(true, &ts->client->dev,
				"%s: result failed, %d\n", __func__, rwbuf[0]);
		ret = -EIO;
		goto err_exit;
	}

	rwbuf[0] = 0x2;
	rwbuf[1] = 0x2;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CHG_SYSMODE, rwbuf, 2);
	if (ret < 0)
		goto err_exit;

	rwbuf[0] = 1;
	rwbuf[1] = 0;
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, rwbuf, 1);
	if (ret < 0)
		goto err_exit;

err_exit:
	enable_irq(ts->client->irq);

	if (ret < 0) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		snprintf(buff, sizeof(buff), "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	}
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}
#endif

static int sec_ts_execute_force_calibration(struct sec_ts_data *ts, int cal_mode)
{
	int rc = -1;
	u8 cmd;

	input_info(true, &ts->client->dev, "%s: %d\n", __func__, cal_mode);

	if (cal_mode == OFFSET_CAL_SEC)
		cmd = SEC_TS_CMD_FACTORY_PANELCALIBRATION;
	else if (cal_mode == AMBIENT_CAL)
		cmd = SEC_TS_CMD_CALIBRATION_AMBIENT;
	else
		return rc;

	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CLEAR_EVENT_STACK, NULL, 0);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: clear event stack failed\n", __func__);
		return rc;
	}

	rc = ts->sec_ts_i2c_write(ts, cmd, NULL, 0);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Write Cal commend failed!\n", __func__);
		return rc;
	}

	sec_ts_delay(3000);

	rc = sec_ts_wait_for_ready(ts, SEC_TS_VENDOR_ACK_OFFSET_CAL_DONE);

#ifdef TCLM_CONCEPT
	if (rc >= 0 && (cal_mode == OFFSET_CAL_SEC) && ts->tdata->support_tclm_test)
		ts->is_cal_done = true;

	if (rc < 0) {
		ts->tdata->nvdata.cal_fail_cnt++;
		ts->tdata->nvdata.cal_fail_falg = 0;
		ts->tdata->tclm_write(ts->tdata->client, SEC_TCLM_NVM_ALL_DATA);
		return rc;
	}

	ts->tdata->nvdata.cal_fail_falg = SEC_CAL_PASS;
	ts->tdata->tclm_write(ts->tdata->client, SEC_TCLM_NVM_ALL_DATA);
#endif
	return rc;
}

int sec_tclm_execute_force_calibration(struct i2c_client *client, int cal_mode)
{
	struct sec_ts_data *ts = i2c_get_clientdata(client);
	int rc;
	/* cal_mode is same tclm and sec. if they are different, modify source */
	rc = sec_ts_execute_force_calibration(ts, cal_mode);

	return rc;
}

static void run_force_calibration(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int rc;
	struct sec_ts_test_mode mode;
	char mis_cal_data = 0xF0;
	int i = 0;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out_force_cal_before_irq_ctrl;
	}

	if (ts->touch_count > 0) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out_force_cal_before_irq_ctrl;
	}

	disable_irq(ts->client->irq);

	rc = sec_ts_execute_force_calibration(ts, OFFSET_CAL_SEC);
	if (rc < 0) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out_force_cal;
	}

	if (ts->plat_data->mis_cal_check) {
		sec_ts_delay(50);

		buff[0] = 0;
		rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, buff, 1);
		if (rc < 0) {
			input_err(true, &ts->client->dev,
					"%s: mis_cal_check error[1] ret: %d\n", __func__, rc);
		}

		buff[0] = 0x2;
		buff[1] = 0x2;
		rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CHG_SYSMODE, buff, 2);
		if (rc < 0) {
			input_err(true, &ts->client->dev,
					"%s: mis_cal_check error[2] ret: %d\n", __func__, rc);
		}
		sec_ts_delay(20);

		rc = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_CHG_SYSMODE, buff, 2);
		input_err(true, &ts->client->dev, "%s: Current Mode : %d %d\n", __func__, buff[0], buff[1]);

		input_err(true, &ts->client->dev, "%s: try mis Cal. check\n", __func__);
		rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_MIS_CAL_CHECK, NULL, 0);
		if (rc < 0) {
			input_err(true, &ts->client->dev,
					"%s: mis_cal_check error[3] ret: %d\n", __func__, rc);
		}
		sec_ts_delay(200);

		for (i = 0; i < 3; i++) {
			rc = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_MIS_CAL_READ, &mis_cal_data, 1);
			if (rc < 0) {
				input_err(true, &ts->client->dev, "%s: i2c fail!, %d\n", __func__, rc);
				mis_cal_data = 0xF3;
				break;
			} else {
				input_info(true, &ts->client->dev, "%s: miss cal data[%d] : %d\n", __func__, i, mis_cal_data);
				if (mis_cal_data != 0xFF)
					break;
			}
		}

		buff[0] = 1;
		rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATEMANAGE_ON, buff, 1);
		if (rc < 0) {
			input_err(true, &ts->client->dev,
					"%s: mis_cal_check error[4] ret: %d\n", __func__, rc);
		}

		if (mis_cal_data) {
			memset(&mode, 0x00, sizeof(struct sec_ts_test_mode));
			mode.type = TYPE_AMBIENT_DATA;
			mode.allnode = TEST_MODE_ALL_NODE;

			sec_ts_read_raw_data(ts, NULL, &mode);
			snprintf(buff, sizeof(buff), "%s", "MIS CAL");
			sec->cmd_state = SEC_CMD_STATUS_FAIL;
			goto out_force_cal;
		}
	}

#ifdef TCLM_CONCEPT
	/* devide tclm case */
	sec_tclm_case(ts->tdata, sec->cmd_param[0]);

	input_info(true, &ts->client->dev, "%s: param, %d, %c, %d\n", __func__,
		sec->cmd_param[0], sec->cmd_param[0], ts->tdata->root_of_calibration);

	rc = sec_execute_tclm_package(ts->tdata, 1);
	if (rc < 0) {
		input_err(true, &ts->client->dev,
					"%s: sec_execute_tclm_package\n", __func__);
	}
	sec_tclm_root_of_cal(ts->tdata, CALPOSITION_NONE);
#endif

	ts->cal_status = sec_ts_read_calibration_report(ts);
	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;

out_force_cal:
	enable_irq(ts->client->irq);

out_force_cal_before_irq_ctrl:
	/* not to enter external factory mode without setting everytime */
	ts->tdata->external_factory = false;

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void get_force_calibration(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int rc;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	rc = sec_ts_read_calibration_report(ts);
	if (rc < 0) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else if (rc == SEC_TS_STATUS_CALIBRATION_SEC) {
		snprintf(buff, sizeof(buff), "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	} else {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	}

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void factory_cmd_result_all(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);

	sec->item_count = 0;
	memset(sec->cmd_result_all, 0x00, SEC_CMD_RESULT_STR_LEN);

	if (ts->tsp_dump_lock == 1) {
		input_err(true, &ts->client->dev, "%s: already checking now\n", __func__);
		sec->cmd_all_factory_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}
	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: IC is power off\n", __func__);
		sec->cmd_all_factory_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	sec->cmd_all_factory_state = SEC_CMD_STATUS_RUNNING;

	get_chip_vendor(sec);
	get_chip_name(sec);
	get_fw_ver_bin(sec);
	get_fw_ver_ic(sec);

	run_rawcap_read(sec);
	get_gap_data(sec);
	run_reference_read(sec);

	run_self_rawcap_read(sec);
	get_self_channel_data(sec, TYPE_OFFSET_DATA_SDC);
	run_self_reference_read(sec);
	get_self_channel_data(sec, TYPE_OFFSET_DATA_SEC);

	run_raw_p2p_read(sec);

	get_wet_mode(sec);
	get_mis_cal_info(sec);

	sec->cmd_all_factory_state = SEC_CMD_STATUS_OK;

out:
	input_info(true, &ts->client->dev, "%s: %d%s\n", __func__, sec->item_count, sec->cmd_result_all);
}

static void set_lowpower_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		snprintf(buff, sizeof(buff), "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	}
#if 0
	/* set lowpower mode by spay, edge_swipe function. */
	ts->lowpower_mode = sec->cmd_param[0];
#endif
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));

	sec_cmd_set_cmd_exit(sec);

	return;
}

static void set_wirelesscharger_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	switch (sec->cmd_param[0]) {
	case TYPE_WIRELESS_CHARGER_NONE:
		ts->charger_mode = SEC_TS_BIT_CHARGER_MODE_NO;
		break;
	case TYPE_WIRELESS_CHARGER:
		ts->charger_mode = SEC_TS_BIT_CHARGER_MODE_WIRELESS_CHARGER;
		break;
	default:
		input_err(true, &ts->client->dev,
				"%s: invalid param %d\n", __func__, sec->cmd_param[0]);
		goto NG;
	}

	ret = ts->sec_ts_i2c_write(ts, SET_TS_CMD_SET_CHARGER_MODE, &ts->charger_mode, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev,
				"%s: Failed to write mode 0x%02X\n", __func__, ts->charger_mode);
		goto NG;
	}

	input_err(true, &ts->client->dev, "%s: %sabled, status=0x%02X\n",
			__func__, ts->charger_mode == SEC_TS_BIT_CHARGER_MODE_NO ? "dis" : "en",
			ts->charger_mode);

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;

NG:
	snprintf(buff, sizeof(buff), "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}

static void spay_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	} else if (!ts->use_sponge) {
		snprintf(buff, sizeof(buff), "NA");
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	}

	if (sec->cmd_param[0])
		ts->lowpower_mode |= SEC_TS_MODE_SPONGE_SWIPE;
	else
		ts->lowpower_mode &= ~SEC_TS_MODE_SPONGE_SWIPE;

	input_info(true, &ts->client->dev, "%s: %s, %02X\n",
			__func__, sec->cmd_param[0] ? "on" : "off", ts->lowpower_mode);

#ifdef CONFIG_TOUCHSCREEN_DUAL_FOLDABLE
	sec_ts_chk_tsp_ic_status(ts, SEC_TS_STATE_CHK_POS_SYSFS);
#endif

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;
}

int sec_ts_set_aod_rect(struct sec_ts_data *ts)
{
	u8 data[10] = {0x02, 0};
	int ret, i;

	if (!ts->use_sponge || !(ts->lowpower_mode & SEC_TS_MODE_SPONGE_AOD))
		return 0;

	for (i = 0; i < 4; i++) {
		data[i * 2 + 2] = ts->rect_data[i] & 0xFF;
		data[i * 2 + 3] = (ts->rect_data[i] >> 8) & 0xFF;
	}

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SPONGE_WRITE_PARAM, &data[0], 10);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Failed to write offset\n", __func__);
		return ret;
	}

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SPONGE_NOTIFY_PACKET, NULL, 0);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Failed to send notify\n", __func__);
		return ret;
	}

	/* optional reg : SEC_TS_CMD_LPM_AOD_OFF_ON(0x9B)			*/
	/* 0 : aod off : change tsp scan freq , avoid wacom scan noise	*/
	/* 1 : aod on->off : change tsp scan freq , avoid wacom scan noise in lpm mode*/
	/* 2 : aod on : scan based on vsync/hsync					*/
	if (ts->power_status == SEC_TS_STATE_LPM) {
		if (ts->rect_data[0] == 0 && ts->rect_data[1] == 0 &&
			ts->rect_data[2] == 0 && ts->rect_data[3] == 0 ) {
	
			data[0] = SEC_TS_CMD_LPM_AOD_ON_OFF;
		} else {
			data[0] = SEC_TS_CMD_LPM_AOD_ON;
		}
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_LPM_AOD_OFF_ON, &data[0], 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: Failed to send aod off_on cmd\n", __func__);
			return ret;
		}
	}

	return 0;
}

static void set_aod_rect(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret, i;

	sec_cmd_set_default_result(sec);

	input_info(true, &ts->client->dev, "%s: w:%d, h:%d, x:%d, y:%d, lowpower_mode:0x%02X\n",
			__func__, sec->cmd_param[0], sec->cmd_param[1],
			sec->cmd_param[2], sec->cmd_param[3], ts->lowpower_mode);

	if (ts->use_sponge)
		ts->lowpower_mode |= SEC_TS_MODE_SPONGE_AOD;

	for (i = 0; i < 4; i++)
		ts->rect_data[i] = sec->cmd_param[i];

	ret = sec_ts_set_aod_rect(ts);
	if (ret < 0)
		goto NG;

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;

NG:
	snprintf(buff, sizeof(buff), "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}


static void get_aod_rect(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	u8 data[8] = {0x02, 0};
	u16 rect_data[4] = {0, };
	int ret, i;

	sec_cmd_set_default_result(sec);

	ret = ts->sec_ts_read_sponge(ts, data, 8);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Failed to read rect\n", __func__);
		goto NG;
	}

	for (i = 0; i < 4; i++)
		rect_data[i] = (data[i * 2 + 1] & 0xFF) << 8 | (data[i * 2] & 0xFF);

	input_info(true, &ts->client->dev, "%s: w:%d, h:%d, x:%d, y:%d\n",
			__func__, rect_data[0], rect_data[1], rect_data[2], rect_data[3]);

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;

NG:
	snprintf(buff, sizeof(buff), "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}

static void aod_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	} else if (!ts->use_sponge) {
		snprintf(buff, sizeof(buff), "NA");
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	}
	
	if (sec->cmd_param[0])
		ts->lowpower_mode |= SEC_TS_MODE_SPONGE_AOD;
	else
		ts->lowpower_mode &= ~SEC_TS_MODE_SPONGE_AOD;

	input_info(true, &ts->client->dev, "%s: %s, %02X\n",
			__func__, sec->cmd_param[0] ? "on" : "off", ts->lowpower_mode);

#ifdef CONFIG_TOUCHSCREEN_DUAL_FOLDABLE
	sec_ts_chk_tsp_ic_status(ts, SEC_TS_STATE_CHK_POS_SYSFS);
#endif

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;
}

static void aot_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	} else if (!ts->use_sponge) {
		snprintf(buff, sizeof(buff), "NA");
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	}

	if (sec->cmd_param[0])
		ts->lowpower_mode |= SEC_TS_MODE_SPONGE_DOUBLETAP_TO_WAKEUP;
	else
		ts->lowpower_mode &= ~SEC_TS_MODE_SPONGE_DOUBLETAP_TO_WAKEUP;

	input_info(true, &ts->client->dev, "%s: %s, %02X\n",
			__func__, sec->cmd_param[0] ? "on" : "off", ts->lowpower_mode);

#ifdef CONFIG_TOUCHSCREEN_DUAL_FOLDABLE
	sec_ts_chk_tsp_ic_status(ts, SEC_TS_STATE_CHK_POS_SYSFS);
#endif

	snprintf(buff, sizeof(buff), "%s", "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;
}

static void fod_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	} else if (!ts->plat_data->support_fod || !ts->use_sponge) {
		snprintf(buff, sizeof(buff), "NA");
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	}

	if (sec->cmd_param[0]) {
		ts->lowpower_mode |= SEC_TS_MODE_SPONGE_PRESS;
		ts->fod_enabled = 1;
	} else {
		ts->lowpower_mode &= ~SEC_TS_MODE_SPONGE_PRESS;
		ts->fod_enabled = 0;
	}

	input_info(true, &ts->client->dev, "%s: %s, %02X\n",
			__func__, sec->cmd_param[0] ? "on" : "off", ts->lowpower_mode);

	sec_ts_set_custom_library(ts);

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;
}

static void singletap_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	} else if (!ts->use_sponge) {
		snprintf(buff, sizeof(buff), "NA");
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	}

	if (sec->cmd_param[0])
		ts->lowpower_mode |= SEC_TS_MODE_SPONGE_SINGLE_TAP;
	else
		ts->lowpower_mode &= ~SEC_TS_MODE_SPONGE_SINGLE_TAP;

	input_info(true, &ts->client->dev, "%s: %s, %02X\n",
			__func__, sec->cmd_param[0] ? "on" : "off", ts->lowpower_mode);

	sec_ts_set_custom_library(ts);

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;
}

/*
 *	flag     1  :  set edge handler
 *		2  :  set (portrait, normal) edge zone data
 *		4  :  set (portrait, normal) dead zone data
 *		8  :  set landscape mode data
 *		16 :  mode clear
 *	data
 *		0x30, FFF (y start), FFF (y end),  FF(direction)
 *		0x31, FFFF (edge zone)
 *		0x32, FF (up x), FF (down x), FFFF (y)
 *		0x33, FF (mode), FFF (edge), FFF (dead zone)
 *	case
 *		edge handler set :  0x30....
 *		booting time :  0x30...  + 0x31...
 *		normal mode : 0x32...  (+0x31...)
 *		landscape mode : 0x33...
 *		landscape -> normal (if same with old data) : 0x33, 0
 *		landscape -> normal (etc) : 0x32....  + 0x33, 0
 */

void set_grip_data_to_ic(struct sec_ts_data *ts, u8 flag)
{
	u8 data[8] = { 0 };

	input_info(true, &ts->client->dev, "%s: flag: %02X (clr,lan,nor,edg,han)\n", __func__, flag);

	if (flag & G_SET_EDGE_HANDLER) {
		if (ts->grip_edgehandler_direction == 0) {
			data[0] = 0x0;
			data[1] = 0x0;
			data[2] = 0x0;
			data[3] = 0x0;
		} else {
			data[0] = (ts->grip_edgehandler_start_y >> 4) & 0xFF;
			data[1] = (ts->grip_edgehandler_start_y << 4 & 0xF0) | ((ts->grip_edgehandler_end_y >> 8) & 0xF);
			data[2] = ts->grip_edgehandler_end_y & 0xFF;
			data[3] = ts->grip_edgehandler_direction & 0x3;
		}
		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_EDGE_HANDLER, data, 4);
		input_info(true, &ts->client->dev, "%s: 0x%02X %02X,%02X,%02X,%02X\n",
				__func__, SEC_TS_CMD_EDGE_HANDLER, data[0], data[1], data[2], data[3]);
	}

	if (flag & G_SET_EDGE_ZONE) {
		data[0] = (ts->grip_edge_range >> 8) & 0xFF;
		data[1] = ts->grip_edge_range  & 0xFF;
		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_EDGE_AREA, data, 2);
		input_info(true, &ts->client->dev, "%s: 0x%02X %02X,%02X\n",
				__func__, SEC_TS_CMD_EDGE_AREA, data[0], data[1]);
	}

	if (flag & G_SET_NORMAL_MODE) {
		data[0] = ts->grip_deadzone_up_x & 0xFF;
		data[1] = ts->grip_deadzone_dn_x & 0xFF;
		data[2] = (ts->grip_deadzone_y >> 8) & 0xFF;
		data[3] = ts->grip_deadzone_y & 0xFF;
		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_DEAD_ZONE, data, 4);
		input_info(true, &ts->client->dev, "%s: 0x%02X %02X,%02X,%02X,%02X\n",
				__func__, SEC_TS_CMD_DEAD_ZONE, data[0], data[1], data[2], data[3]);
	}

	if (flag & G_SET_LANDSCAPE_MODE) {
		data[0] = ts->grip_landscape_mode & 0x1;
		data[1] = (ts->grip_landscape_edge >> 4) & 0xFF;
		data[2] = (ts->grip_landscape_edge << 4 & 0xF0) | ((ts->grip_landscape_deadzone >> 8) & 0xF);
		data[3] = ts->grip_landscape_deadzone & 0xFF;
		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_LANDSCAPE_MODE, data, 4);
		input_info(true, &ts->client->dev, "%s: 0x%02X %02X,%02X,%02X,%02X\n",
				__func__, SEC_TS_CMD_LANDSCAPE_MODE, data[0], data[1], data[2], data[3]);
	}

	if (flag & G_CLR_LANDSCAPE_MODE) {
		data[0] = ts->grip_landscape_mode;
		ts->sec_ts_i2c_write(ts, SEC_TS_CMD_LANDSCAPE_MODE, data, 1);
		input_info(true, &ts->client->dev, "%s: 0x%02X %02X\n",
				__func__, SEC_TS_CMD_LANDSCAPE_MODE, data[0]);
	}
}

/*
 *	index  0 :  set edge handler
 *		1 :  portrait (normal) mode
 *		2 :  landscape mode
 *
 *	data
 *		0, X (direction), X (y start), X (y end)
 *		direction : 0 (off), 1 (left), 2 (right)
 *			ex) echo set_grip_data,0,2,600,900 > cmd
 *
 *		1, X (edge zone), X (dead zone up x), X (dead zone down x), X (dead zone y)
 *			ex) echo set_grip_data,1,200,10,50,1500 > cmd
 *
 *		2, 1 (landscape mode), X (edge zone), X (dead zone)
 *			ex) echo set_grip_data,2,1,200,100 > cmd
 *
 *		2, 0 (portrait mode)
 *			ex) echo set_grip_data,2,0  > cmd
 */

static void set_grip_data(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	u8 mode = G_NONE;

	sec_cmd_set_default_result(sec);

	memset(buff, 0, sizeof(buff));

	mutex_lock(&ts->device_mutex);

	if (sec->cmd_param[0] == 0) {	// edge handler
		if (sec->cmd_param[1] == 0) {	// clear
			ts->grip_edgehandler_direction = 0;
		} else if (sec->cmd_param[1] < 3) {
			ts->grip_edgehandler_direction = sec->cmd_param[1];
			ts->grip_edgehandler_start_y = sec->cmd_param[2];
			ts->grip_edgehandler_end_y = sec->cmd_param[3];
		} else {
			input_err(true, &ts->client->dev, "%s: cmd1 is abnormal, %d (%d)\n",
					__func__, sec->cmd_param[1], __LINE__);
			goto err_grip_data;
		}

		mode = mode | G_SET_EDGE_HANDLER;
		set_grip_data_to_ic(ts, mode);

	} else if (sec->cmd_param[0] == 1) {	// normal mode
		if (ts->grip_edge_range != sec->cmd_param[1])
			mode = mode | G_SET_EDGE_ZONE;

		ts->grip_edge_range = sec->cmd_param[1];
		ts->grip_deadzone_up_x = sec->cmd_param[2];
		ts->grip_deadzone_dn_x = sec->cmd_param[3];
		ts->grip_deadzone_y = sec->cmd_param[4];
		mode = mode | G_SET_NORMAL_MODE;

		if (ts->grip_landscape_mode == 1) {
			ts->grip_landscape_mode = 0;
			mode = mode | G_CLR_LANDSCAPE_MODE;
		}
		set_grip_data_to_ic(ts, mode);
	} else if (sec->cmd_param[0] == 2) {	// landscape mode
		if (sec->cmd_param[1] == 0) {	// normal mode
			ts->grip_landscape_mode = 0;
			mode = mode | G_CLR_LANDSCAPE_MODE;
		} else if (sec->cmd_param[1] == 1) {
			ts->grip_landscape_mode = 1;
			ts->grip_landscape_edge = sec->cmd_param[2];
			ts->grip_landscape_deadzone	= sec->cmd_param[3];
			mode = mode | G_SET_LANDSCAPE_MODE;
		} else {
			input_err(true, &ts->client->dev, "%s: cmd1 is abnormal, %d (%d)\n",
					__func__, sec->cmd_param[1], __LINE__);
			goto err_grip_data;
		}
		set_grip_data_to_ic(ts, mode);
	} else {
		input_err(true, &ts->client->dev, "%s: cmd0 is abnormal, %d", __func__, sec->cmd_param[0]);
		goto err_grip_data;
	}

	mutex_unlock(&ts->device_mutex);

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;

err_grip_data:
	mutex_unlock(&ts->device_mutex);

	snprintf(buff, sizeof(buff), "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}

/*
 * Enable or disable external_noise_mode
 *
 * If mode has EXT_NOISE_MODE_MAX,
 * then write enable cmd for all enabled mode. (set as ts->external_noise_mode bit value)
 * This routine need after IC power reset. TSP IC need to be re-wrote all enabled modes.
 *
 * Else if mode has specific value like EXT_NOISE_MODE_MONITOR,
 * then write enable/disable cmd about for that mode's latest setting value.
 *
 * If you want to add new mode,
 * please define new enum value like EXT_NOISE_MODE_MONITOR,
 * then set cmd for that mode like below. (it is in this function)
 * noise_mode_cmd[EXT_NOISE_MODE_MONITOR] = SEC_TS_CMD_SET_MONITOR_NOISE_MODE;
 */
int sec_ts_set_external_noise_mode(struct sec_ts_data *ts, u8 mode)
{
	int i, ret, fail_count = 0;
	u8 mode_bit_to_set, check_bit, mode_enable;
	u8 noise_mode_cmd[EXT_NOISE_MODE_MAX] = { 0 };

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		return -ENODEV;
	}

	if (mode == EXT_NOISE_MODE_MAX) {
		/* write all enabled mode */
		mode_bit_to_set = ts->external_noise_mode;
	} else {
		/* make enable or disable the specific mode */
		mode_bit_to_set = 1 << mode;
	}

	input_info(true, &ts->client->dev, "%s: %sable %d\n", __func__,
			ts->external_noise_mode & mode_bit_to_set ? "en" : "dis", mode_bit_to_set);

	/* set cmd for each mode */
	noise_mode_cmd[EXT_NOISE_MODE_MONITOR] = SEC_TS_CMD_SET_MONITOR_NOISE_MODE;

	/* write mode */
	for (i = EXT_NOISE_MODE_NONE + 1; i < EXT_NOISE_MODE_MAX; i++) {
		check_bit = 1 << i;
		if (mode_bit_to_set & check_bit) {
			mode_enable = !!(ts->external_noise_mode & check_bit);
			ret = ts->sec_ts_i2c_write(ts, noise_mode_cmd[i], &mode_enable, 1);
			if (ret < 0) {
				input_err(true, &ts->client->dev, "%s: failed to set 0x%02X %d\n",
						__func__, noise_mode_cmd[i], mode_enable);
				fail_count++;
			}
		}
	}

	if (fail_count != 0)
		return -EIO;
	else
		return 0;
}

/*
 * Enable or disable specific external_noise_mode (sec_cmd)
 *
 * This cmd has 2 params.
 * param 0 : the mode that you want to change.
 * param 1 : enable or disable the mode.
 *
 * For example,
 * enable EXT_NOISE_MODE_MONITOR mode,
 * write external_noise_mode,1,1
 * disable EXT_NOISE_MODE_MONITOR mode,
 * write external_noise_mode,1,0
 */
static void external_noise_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] <= EXT_NOISE_MODE_NONE || sec->cmd_param[0] >= EXT_NOISE_MODE_MAX ||
			sec->cmd_param[1] < 0 || sec->cmd_param[1] > 1) {
		input_err(true, &ts->client->dev, "%s: not support param\n", __func__);
		goto NG;
	}

	if (sec->cmd_param[1] == 1)
		ts->external_noise_mode |= 1 << sec->cmd_param[0];
	else
		ts->external_noise_mode &= ~(1 << sec->cmd_param[0]);

	ret = sec_ts_set_external_noise_mode(ts, sec->cmd_param[0]);
	if (ret < 0)
		goto NG;

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
	return;

NG:
	snprintf(buff, sizeof(buff), "NG");
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);
}

static void brush_enable(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	ts->brush_mode = sec->cmd_param[0];

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	input_info(true, &ts->client->dev,
			"%s: set brush mode %s\n", __func__, ts->brush_mode ? "enable" : "disable");

	/*	- 0: Disable Artcanvas min phi mode
	 *	- 1: Enable Artcanvas min phi mode
	 */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_BRUSH_MODE, &ts->brush_mode, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev,
				"%s: failed to set brush mode\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;

out:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void set_touchable_area(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	ts->touchable_area = sec->cmd_param[0];

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	input_info(true, &ts->client->dev,
			"%s: set 16:9 mode %s\n", __func__, ts->touchable_area ? "enable" : "disable");

	/*  - 0: Disable 16:9 mode
	 *  - 1: Enable 16:9 mode
	 */
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_TOUCHABLE_AREA, &ts->touchable_area, 1);
	if (ret < 0) {
		input_err(true, &ts->client->dev,
				"%s: failed to set 16:9 mode\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;

out:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void set_log_level(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	char tBuff[2] = { 0 };
	u8 w_data[1] = {0x00};
	int ret;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	if ((sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) ||
			(sec->cmd_param[1] < 0 || sec->cmd_param[1] > 1) ||
			(sec->cmd_param[2] < 0 || sec->cmd_param[2] > 1) ||
			(sec->cmd_param[3] < 0 || sec->cmd_param[3] > 1) ||
			(sec->cmd_param[4] < 0 || sec->cmd_param[4] > 1) ||
			(sec->cmd_param[5] < 0 || sec->cmd_param[5] > 1) ||
			(sec->cmd_param[6] < 0 || sec->cmd_param[6] > 1) ||
			(sec->cmd_param[7] < 0 || sec->cmd_param[7] > 1)) {
		input_err(true, &ts->client->dev, "%s: para out of range\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	ret = ts->sec_ts_i2c_read(ts, SEC_TS_CMD_STATUS_EVENT_TYPE, tBuff, 2);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Read Event type enable status fail\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		goto err;
	}

	input_info(true, &ts->client->dev, "%s: STATUS_EVENT enable = 0x%02X, 0x%02X\n",
			__func__, tBuff[0], tBuff[1]);

	tBuff[0] = BIT_STATUS_EVENT_VENDOR_INFO(sec->cmd_param[6]);
	tBuff[1] = BIT_STATUS_EVENT_ERR(sec->cmd_param[0]) |
		BIT_STATUS_EVENT_INFO(sec->cmd_param[1]) |
		BIT_STATUS_EVENT_USER_INPUT(sec->cmd_param[2]);

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_STATUS_EVENT_TYPE, tBuff, 2);
	if (ret < 0) {
		input_err(true, &ts->client->dev, "%s: Write Event type enable status fail\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		goto err;
	}

	if (sec->cmd_param[0] == 1 && sec->cmd_param[1] == 1 &&
			sec->cmd_param[2] == 1 && sec->cmd_param[3] == 1 &&
			sec->cmd_param[4] == 1 && sec->cmd_param[5] == 1 &&
			sec->cmd_param[6] == 1 && sec->cmd_param[7] == 1) {
		w_data[0] = 0x1;
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_VENDOR_EVENT_LEVEL, w_data, 1);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: Write Vendor Event Level fail\n", __func__);
			snprintf(buff, sizeof(buff), "NG");
			goto err;
		}
	} else {
		w_data[0] = 0x0;
		ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_VENDOR_EVENT_LEVEL, w_data, 0);
		if (ret < 0) {
			input_err(true, &ts->client->dev, "%s: Write Vendor Event Level fail\n", __func__);
			snprintf(buff, sizeof(buff), "NG");
			goto err;
		}
	}

	input_info(true, &ts->client->dev, "%s: ERROR : %d, INFO : %d, USER_INPUT : %d, INFO_SPONGE : %d, VENDOR_INFO : %d, VENDOR_EVENT_LEVEL : %d\n",
			__func__, sec->cmd_param[0], sec->cmd_param[1], sec->cmd_param[2], sec->cmd_param[5], sec->cmd_param[6], w_data[0]);

	snprintf(buff, sizeof(buff), "OK");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	return;

err:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_FAIL;
}

static void debug(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	ts->debug_flag = sec->cmd_param[0];

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
}

static void check_connection(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = {0};
	int rc;
	int size = SEC_TS_SELFTEST_REPORT_SIZE + ts->tx_count * ts->rx_count * 2;
	u8 *rBuff = NULL;
	int ii;
	u8 data[8] = { 0 };
	int result = 0;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	rBuff = kzalloc(size, GFP_KERNEL);
	if (!rBuff) {
		snprintf(buff, sizeof(buff), "NG");
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		return;
	}

	memset(rBuff, 0x00, size);
	memset(data, 0x00, 8);

	disable_irq(ts->client->irq);

	sec_ts_locked_release_all_finger(ts);

	input_info(true, &ts->client->dev, "%s: set power mode to test mode\n", __func__);
	data[0] = 0x02;
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, data, 1);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: set test mode failed\n", __func__);
		goto err_conn_check;
	}

	input_info(true, &ts->client->dev, "%s: clear event stack\n", __func__);
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_CLEAR_EVENT_STACK, NULL, 0);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: clear event stack failed\n", __func__);
		goto err_conn_check;
	}

	data[0] = 0xB7;
	input_info(true, &ts->client->dev, "%s: self test start\n", __func__);
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SELFTEST, data, 1);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Send selftest cmd failed!\n", __func__);
		goto err_conn_check;
	}

	sec_ts_delay(700);

	rc = sec_ts_wait_for_ready(ts, SEC_TS_VENDOR_ACK_SELF_TEST_DONE);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_conn_check;
	}

	input_info(true, &ts->client->dev, "%s: self test done\n", __func__);
	rc = ts->sec_ts_i2c_read(ts, SEC_TS_READ_SELFTEST_RESULT, rBuff, size);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: Selftest execution time out!\n", __func__);
		goto err_conn_check;
	}

	data[0] = TO_TOUCH_MODE;
	rc = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, data, 1);
	if (rc < 0) {
		input_err(true, &ts->client->dev, "%s: mode changed failed!\n", __func__);
		goto err_conn_check;
	}

	input_info(true, &ts->client->dev, "%s: %02X, %02X, %02X, %02X\n",
			__func__, rBuff[16], rBuff[17], rBuff[18], rBuff[19]);

	memcpy(data, &rBuff[48], 8);

	for (ii = 0; ii < 8; ii++) {
		input_info(true, &ts->client->dev, "%s: [%d] %02X\n", __func__, ii, data[ii]);

		result += data[ii];
	}

	if (result != 0)
		goto err_conn_check;

	enable_irq(ts->client->irq);

	snprintf(buff, sizeof(buff), "OK");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;

	kfree(rBuff);
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
	return;

err_conn_check:
	data[0] = TO_TOUCH_MODE;
	ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, data, 1);
	enable_irq(ts->client->irq);

	snprintf(buff, sizeof(buff), "NG");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_FAIL;

	kfree(rBuff);
	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void fix_active_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	ts->fix_active_mode = sec->cmd_param[0];

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	if (ts->power_status != SEC_TS_STATE_LPM) {
		if (ts->fix_active_mode)
			sec_ts_fix_tmode(ts, TOUCH_SYSTEM_MODE_TOUCH, TOUCH_MODE_STATE_TOUCH);
		else
			sec_ts_release_tmode(ts);
	}

	snprintf(buff, sizeof(buff), "OK");
	sec->cmd_state = SEC_CMD_STATUS_OK;

out:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void touch_aging_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;
	u8 data;

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		goto out;
	}

	if (sec->cmd_param[0] == 1)
		data = 5;
	else
		data = 0;

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_POWER_MODE, &data, 1);
	if (ret < 0) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		snprintf(buff, sizeof(buff), "OK");
		sec->cmd_state = SEC_CMD_STATUS_OK;
	}

out:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void set_rear_selfie_mode(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	} else {
		ts->rear_selfie_mode = sec->cmd_param[0];
		snprintf(buff, sizeof(buff), "OK");

#ifdef CONFIG_TOUCHSCREEN_DUAL_FOLDABLE
		if (!ts->rear_selfie_mode)
			sec_ts_chk_tsp_ic_status(ts, SEC_TS_STATE_CHK_POS_SYSFS);
#endif
	}

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_WAITING;
	sec_cmd_set_cmd_exit(sec);

	input_info(true, &ts->client->dev, "%s: %s\n", __func__, buff);
}

static void fp_int_control(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;
	u8 data[2] = { 1, 0 }; /* byte[0]:1 - INTR2 */

	sec_cmd_set_default_result(sec);

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	} else if (!ts->plat_data->support_fod) {
		snprintf(buff, sizeof(buff), "NA");
		sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	}
	
	data[1] = sec->cmd_param[0];
	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_INPUT_GPIO_CONTROL, data, 2);
	if (ret < 0) {
		snprintf(buff, sizeof(buff), "NG");
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
		sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
		sec_cmd_set_cmd_exit(sec);
		return;
	}

	snprintf(buff, sizeof(buff), "OK");
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_OK;
	input_info(true, &ts->client->dev, "%s: %d %s\n", __func__, data[1], buff);
	return;
}

static void set_low_power_sensitivity(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	int ret;

	sec_cmd_set_default_result(sec);

	if (ts->power_status == SEC_TS_STATE_POWER_OFF) {
		input_err(true, &ts->client->dev, "%s: Touch is stopped!\n", __func__);
		snprintf(buff, sizeof(buff), "NG");
		goto out;
	}

	if (sec->cmd_param[0] < 0 || sec->cmd_param[0] > 1) {
		snprintf(buff, sizeof(buff), "NG");
		goto out;
	}

	ts->lp_sensitivity = sec->cmd_param[0];

	ret = ts->sec_ts_i2c_write(ts, SEC_TS_CMD_SET_LOW_POWER_SENSITIVITY, &ts->lp_sensitivity, 1);
	if (ret < 0) {
		snprintf(buff, sizeof(buff), "NG");
		goto out;
	}

	snprintf(buff, sizeof(buff), "OK");
out:
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_WAITING;
	sec_cmd_set_cmd_exit(sec);
}

#ifdef TCLM_CONCEPT
static void tclm_test_cmd(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };
	struct sec_tclm_data *data = ts->tdata;
	int ret = 0;

	sec_cmd_set_default_result(sec);
	if (!ts->tdata->support_tclm_test)
		goto not_support;

	ret = tclm_test_command(data, sec->cmd_param[0], sec->cmd_param[1], sec->cmd_param[2], buff);
	if (ret < 0)
		sec->cmd_state = SEC_CMD_STATUS_FAIL;
	else
		sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	return;

not_support:
	snprintf(buff, sizeof(buff), "NA");
	sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
}

static void get_calibration(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	struct sec_ts_data *ts = container_of(sec, struct sec_ts_data, sec);
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);
	if (!ts->tdata->support_tclm_test)
		goto not_support;
	snprintf(buff, sizeof(buff), "%d", ts->is_cal_done);

	ts->is_cal_done = false;
	sec->cmd_state = SEC_CMD_STATUS_OK;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	return;

not_support:
	snprintf(buff, sizeof(buff), "NA");
	sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
}
#endif

static void not_support_cmd(void *device_data)
{
	struct sec_cmd_data *sec = (struct sec_cmd_data *)device_data;
	char buff[SEC_CMD_STR_LEN] = { 0 };

	sec_cmd_set_default_result(sec);
	snprintf(buff, sizeof(buff), "NA");

	sec_cmd_set_cmd_result(sec, buff, strnlen(buff, sizeof(buff)));
	sec->cmd_state = SEC_CMD_STATUS_NOT_APPLICABLE;
	sec_cmd_set_cmd_exit(sec);
}

int sec_ts_fn_init(struct sec_ts_data *ts)
{
	int retval = 0;

#ifdef CONFIG_TOUCHSCREEN_DUAL_FOLDABLE
	retval = sec_cmd_init(&ts->sec, sec_cmds,
			ARRAY_SIZE(sec_cmds), SEC_CLASS_DEVT_TSP2);
#else
	retval = sec_cmd_init(&ts->sec, sec_cmds,
			ARRAY_SIZE(sec_cmds), SEC_CLASS_DEVT_TSP);
#endif
	if (retval < 0) {
		input_err(true, &ts->client->dev,
				"%s: Failed to sec_cmd_init\n", __func__);
		goto exit;
	}

	retval = sysfs_create_group(&ts->sec.fac_dev->kobj,
			&cmd_attr_group);
	if (retval < 0) {
		input_err(true, &ts->client->dev,
				"%s: Failed to create sysfs attributes\n", __func__);
		goto exit;
	}

	retval = sysfs_create_link(&ts->sec.fac_dev->kobj,
			&ts->input_dev->dev.kobj, "input");
	if (retval < 0) {
		input_err(true, &ts->client->dev,
				"%s: Failed to create input symbolic link\n",
				__func__);
		goto exit;
	}

	return 0;

exit:
	return retval;
}

void sec_ts_fn_remove(struct sec_ts_data *ts)
{
	input_err(true, &ts->client->dev, "%s\n", __func__);

	sysfs_delete_link(&ts->sec.fac_dev->kobj, &ts->input_dev->dev.kobj, "input");

	sysfs_remove_group(&ts->sec.fac_dev->kobj,
			&cmd_attr_group);

#ifdef CONFIG_TOUCHSCREEN_DUAL_FOLDABLE
	sec_cmd_exit(&ts->sec, SEC_CLASS_DEVT_TSP2);
#else
	sec_cmd_exit(&ts->sec, SEC_CLASS_DEVT_TSP);
#endif
}
