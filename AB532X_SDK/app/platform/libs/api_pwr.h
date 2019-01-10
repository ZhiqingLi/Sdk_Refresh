#ifndef _API_PWR_H_
#define _API_PWR_H_

void pmu_init(u8 hvbko);
u16 pmu_get_vbg(void);
u16 pmu_get_vddio(void);

void charge_into_lowpower(u32 *save_buf);
void charge_outof_lowpower(u32 *save_buf);
void charge_lowpower_power_down(void);
bool bat_lowpower_recharge(u32 recharge_v);
bool check_vusb_voltage(u32 recharge_v);
#endif // _API_PWR_H_

