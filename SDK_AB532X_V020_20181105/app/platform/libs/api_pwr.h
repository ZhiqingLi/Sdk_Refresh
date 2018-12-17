#ifndef _API_PWR_H_
#define _API_PWR_H_

void pmu_init(u8 hvbko);
u16 pmu_get_vbg(void);
u16 pmu_get_vddio(void);

#endif // _API_PWR_H_

