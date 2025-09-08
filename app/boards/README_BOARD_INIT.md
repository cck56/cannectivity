# Board Initialization for STM32G0 Dual CAN-FD Boards

## Overview

This document describes the board-specific initialization feature added to CANnectivity for STM32G0-based dual CAN-FD boards. The primary purpose is to configure option bytes, specifically the `nBOOT_SEL` bit, to ensure proper boot mode selection via the BOOT0 pin.

## Features

### STM32G0 Option Byte Configuration

For STM32G0 devices, the board initialization automatically:
- Checks the `nBOOT_SEL` option bit status
- If `nBOOT_SEL = 1` (BOOT0 pin ignored), it reconfigures it to `0`
- When `nBOOT_SEL = 0`, the BOOT0 pin controls boot mode selection
- Triggers a system reset after option byte modification (required for changes to take effect)

## Configuration

### Automatic Configuration

The board initialization is automatically enabled for supported STM32G0 boards:
- `candlelightfd_stm32g0b1xx` - Single channel CAN-FD
- `candlelightfd_stm32g0b1xx_dual` - Dual channel CAN-FD
- `usb2canfdv1_stm32g0b1xx` - USB2CAN FD v1

### Manual Configuration

To enable board initialization for other boards, add to your board's `.conf` file:

```conf
# Enable board-specific initialization
CONFIG_CANNECTIVITY_BOARD_INIT=y

# For STM32G0 devices, enable option byte configuration
CONFIG_CANNECTIVITY_BOARD_INIT_STM32G0_OPTION_BYTES=y
```

### Configuration Options

- `CONFIG_CANNECTIVITY_BOARD_INIT` - Master enable for board initialization
- `CONFIG_CANNECTIVITY_BOARD_INIT_EARLY` - Run initialization early in boot (default: y)
- `CONFIG_CANNECTIVITY_BOARD_INIT_PRIORITY` - Init priority (0-99, default: 50)
- `CONFIG_CANNECTIVITY_BOARD_INIT_STM32G0_OPTION_BYTES` - Enable STM32G0 option byte config
- `CONFIG_CANNECTIVITY_BOARD_INIT_CUSTOM` - Enable custom board init hook

## Custom Board Initialization

For boards requiring additional initialization beyond option bytes:

1. Enable custom initialization in your board config:
```conf
CONFIG_CANNECTIVITY_BOARD_INIT=y
CONFIG_CANNECTIVITY_BOARD_INIT_CUSTOM=y
```

2. Implement the custom initialization function in your board-specific source:
```c
int cannectivity_board_init_custom(void)
{
    /* Your custom board initialization code */
    return 0; /* Return 0 on success, negative errno on failure */
}
```

## Technical Details

### STM32G0 Boot Mode Selection

The STM32G0 series uses option bytes to configure boot behavior:
- `nBOOT_SEL = 0`: Boot mode selected by BOOT0 pin state
  - BOOT0 = 0: Boot from Flash memory
  - BOOT0 = 1: Boot from system memory (bootloader)
- `nBOOT_SEL = 1`: BOOT0 pin ignored, boot from address in option bytes

For proper DFU and bootloader support, `nBOOT_SEL` must be `0`.

### Implementation Notes

- Option byte modification requires FLASH unlock sequence
- Changes require a system reset to take effect
- The initialization runs once at boot, only modifying if needed
- Errors are logged but don't prevent firmware from running

## Troubleshooting

### Board doesn't enter DFU mode
- Check that `CONFIG_CANNECTIVITY_BOARD_INIT_STM32G0_OPTION_BYTES` is enabled
- Verify BOOT0 pin hardware connection
- Check debug logs for option byte configuration status

### System resets unexpectedly
- This is normal when option bytes are first configured
- The system will automatically reset once after changing `nBOOT_SEL`
- Subsequent boots will not trigger a reset

### Build errors
- Ensure your board is based on STM32G0 series
- Check that required STM32 HAL headers are available
- Verify Zephyr STM32 support is properly configured