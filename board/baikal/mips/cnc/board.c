/*
 * File:        board.c
 * Description: T-platforms CNC SFBT1/MSBT2 based boards initialization
 * Authors:     Sergey Semin <sergey.semin@t-platforms.ru>
 *
 * Copyright 2018 T-platforms JSC
 */

#include <common.h>
#include <asm/io.h>
#include <miiphy.h>
#include <netdev.h>
#include <pca953x.h>

#include "detect.h"

DECLARE_GLOBAL_DATA_PTR;

#define BIT(x)			(1<<(x))

#ifdef CONFIG_BOARD_EARLY_INIT_R
int board_early_init_r(void)
{
	return 0;
}
#endif /* CONFIG_BOARD_EARLY_INIT_R */

#ifdef CONFIG_BOARD_LATE_INIT
static int cnc_clear_pcie_reset(void)
{
#if defined(CONFIG_PCIE_RST_PIN) && defined(CONFIG_TPLATFORMS_CNC_MSBT2)
	int ret, pcie_mask = BIT(CONFIG_PCIE_RST_PIN);

	ret = pca953x_set_dir(CONFIG_SYS_SHRED_I2C_ADDR, pcie_mask, 0);
	if (ret) {
		printf("PCIe:  Can't set reset-GPIO dir\n");
		return ret;
	}

	ret = pca953x_set_val(CONFIG_SYS_SHRED_I2C_ADDR, pcie_mask, pcie_mask);
	if (!ret) {
		/* Need to create a pulse since the pca953x GPIO-expander doesn't have
		 * reset pin, so it isn't reset on restart. */
		udelay(500);
		ret = pca953x_set_val(CONFIG_SYS_SHRED_I2C_ADDR, pcie_mask, 0);
	}
	if (ret) {
		printf("PCIe:  Can't set reset-GPIO val\n");
		return ret;
	}

#endif

	return 0;
}

int board_late_init(void)
{
	cnc_detect_board();

	cnc_clear_pcie_reset();

	return 0;
}
#endif /* CONFIG_BOARD_LATE_INIT */

/* Initialization of network */
int board_eth_init(bd_t *bis)
{
	int err = 0;

#if defined(CONFIG_DESIGNWARE_ETH0_BASE)
	if (designware_initialize(CONFIG_DESIGNWARE_ETH0_BASE,
			  PHY_INTERFACE_MODE_RGMII) < 0)
		err |= (1 << 0);
#endif /* CONFIG_DESIGNWARE_ETH0_BASE */
#if defined(CONFIG_DESIGNWARE_ETH1_BASE)
	if (designware_initialize(CONFIG_DESIGNWARE_ETH1_BASE,
			  PHY_INTERFACE_MODE_RGMII) < 0)
		err |= (1 << 1);
#endif /* CONFIG_DESIGNWARE_ETH1_BASE */
#if defined(CONFIG_DESIGNWARE_ETH2_BASE)
	if (designware_initialize(CONFIG_DESIGNWARE_ETH2_BASE,
			  PHY_INTERFACE_MODE_RGMII) < 0)
		err |= (1 << 1);
#endif /* CONFIG_DESIGNWARE_ETH2_BASE */
	return (! err);
}
