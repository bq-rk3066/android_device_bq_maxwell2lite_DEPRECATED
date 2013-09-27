# Copyright (C) 2012 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This file lists the product definition files that define
# configurations which are actually buildable (e.g. through lunch)
#

-include vendor/bq/maxwell2lite/BoardConfigVendor.mk

# CPU and Platform
TARGET_BOARD_PLATFORM := rk30xx
TARGET_BOARD_PLATFORM_GPU := mali400

TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true

TARGET_CPU_ABI  := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_ARCH_VARIANT_CPU := cortex-a9
TARGET_ARCH_VARIANT_FPU := neon
ARCH_ARM_HAVE_NEON := true
ARCH_ARM_HAVE_ARMV7A := true
ARCH_ARM_HAVE_TLS_REGISTER := true

TARGET_GLOBAL_CFLAGS += -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp
TARGET_GLOBAL_CPPFLAGS += -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp

TARGET_SPECIFIC_HEADER_PATH += device/bq/maxwell2lite/include

# Kernel & Bootloader
TARGET_PREBUILT_KERNEL := device/bq/maxwell2lite/kernel
TARGET_BOOTLOADER_BOARD_NAME := bq_Maxwell2lite
BOARD_KERNEL_BASE := 0x60400000
BOARD_KERNEL_PAGESIZE := 16384

# Graphics
BOARD_EGL_CFG := device/bq/maxwell2lite/config/egl.cfg
USE_OPENGL_RENDERER := true
TARGET_USES_ION := true
BOARD_USES_HDMI := true
TARGET_HAVE_HDMI_OUT := true
BOARD_USES_HWCOMPOSER := true

# Camera
BOARD_NEEDS_MEMORYHEAPPMEM := true
USE_CAMERA_STUB := true

# Wi-Fi
BOARD_WLAN_DEVICE := bcmdhd
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_bcmdhd
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_bcmdhd
WPA_SUPPLICANT_VERSION := VER_0_8_X
WIFI_DRIVER_MODULE_NAME := "wlan"
WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/rkwifi.ko"
WIFI_DRIVER_FW_PATH_STA := "/etc/firmware/fw_RK903b2.bin"
WIFI_DRIVER_FW_PATH_AP := "/etc/firmware/fw_RK903b2_apsta.bin"
WIFI_DRIVER_FW_PATH_P2P := "/etc/firmware/fw_RK903b2_p2p.bin"

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/bq/maxwell2lite/bluetooth
BOARD_BLUEDROID_VENDOR_CONF := device/bq/maxwell2lite/bluetooth/vnd_maxwell2lite.txt

# Audio
BOARD_USES_GENERIC_AUDIO := true
BOARD_USES_ALSA_AUDIO := false

# Avoid the generation of ldrcc instructions
NEED_WORKAROUND_CORTEX_A9_745320 := true

# Web Rendering
ENABLE_WEBGL := true
TARGET_FORCE_CPU_UPLOAD := true

# OTA
TARGET_OTA_ASSERT_DEVICE := maxwell2lite,bq_Maxwell2Lite
TARGET_RELEASETOOL_OTA_FROM_TARGET_SCRIPT := device/bq/maxwell2lite/releasetools/rk_ota_from_target_files

# Recovery
TARGET_PROVIDES_INIT_RC := true
TARGET_RECOVERY_INITRC := device/bq/maxwell2lite/recovery.rc
TARGET_RECOVERY_FSTAB := device/bq/maxwell2lite/recovery.fstab
TARGET_RECOVERY_PRE_COMMAND := "busybox dd if=/misc.img of=/dev/block/mtdblock0; sync"
TARGET_USERIMAGES_USE_EXT4 := true
RECOVERY_SDCARD_ON_DATA := true
TW_FLASH_FROM_STORAGE := true
TW_INTERNAL_STORAGE_PATH := "/data/media"
TW_INTERNAL_STORAGE_MOUNT_POINT := "data"
TW_EXTERNAL_STORAGE_PATH := "/external_sd"
TW_EXTERNAL_STORAGE_MOUNT_POINT := "external_sd"
BOARD_HAS_LARGE_FILESYSTEM := true
BOARD_HAS_NO_SELECT_BUTTON := true
TW_BRIGHTNESS_PATH := /sys/devices/platform/rk29_backlight/backlight/rk28_bl/brightness
TW_MAX_BRIGHTNESS := 255
DEVICE_RESOLUTION := 1024x600

# UMS
BOARD_USE_USB_MASS_STORAGE_SWITCH := true
BOARD_UMS_LUNFILE := /sys/class/android_usb/android0/f_mass_storage/lun/file
BOARD_UMS_2ND_LUNFILE := /sys/class/android_usb/android0/f_mass_storage/lun1/file
TARGET_USE_CUSTOM_LUN_FILE_PATH := /sys/class/android_usb/android0/f_mass_storage/lun/file

# Partition sizes
BOARD_BOOTIMAGE_PARTITION_SIZE := 16777216
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 16777216
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 576716800
BOARD_USERDATAIMAGE_PARTITION_SIZE := 14906032128
BOARD_FLASH_BLOCK_SIZE := 16384
