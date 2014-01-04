#
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

$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_TAGS += dalvik.gc.type-precise

PRODUCT_AAPT_CONFIG := normal mdpi hdpi
PRODUCT_AAPT_PREF_CONFIG := mdpi

DEVICE_PACKAGE_OVERLAYS += device/bq/maxwell2lite/overlay

# Hardware-specific features
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml

# Config files
PRODUCT_COPY_FILES += \
    device/bq/maxwell2lite/config/serial_number.sh:system/bin/serial_number.sh \
    device/bq/maxwell2lite/config/audio_policy.conf:system/etc/audio_policy.conf \
    device/bq/maxwell2lite/config/media_profiles.xml:system/etc/media_profiles.xml \
    device/bq/maxwell2lite/config/media_codecs.xml:system/etc/media_codecs.xml \
    device/bq/maxwell2lite/config/egl.cfg:system/lib/egl/egl.cfg \
    device/bq/maxwell2lite/config/rk29-keypad.kl:/system/usr/keylayout/rk29-keypad.kl \
    device/bq/maxwell2lite/config/vold.fstab:system/etc/vold.fstab \
	
# Rootdir
PRODUCT_COPY_FILES += \
    device/bq/maxwell2lite/rootdir/init:root/init \
    device/bq/maxwell2lite/rootdir/fstab.rk30board:root/fstab.rk30board \
    device/bq/maxwell2lite/rootdir/init.rk30board.rc:root/init.rk30board.rc \
    device/bq/maxwell2lite/rootdir/init.rk30board.usb.rc:root/init.rk30board.usb.rc \
    device/bq/maxwell2lite/rootdir/ueventd.rk30board.rc:root/ueventd.rk30board.rc \
    device/bq/maxwell2lite/rootdir/rk30xxnand_ko.ko.3.0.8+:root/rk30xxnand_ko.ko.3.0.8+ \
    device/bq/maxwell2lite/rootdir/charger:root/charger \
    $(call find-copy-subdir-files,*,device/bq/maxwell2lite/rootdir/images,root/res/images/charger)

# HAL
PRODUCT_PACKAGES += \
    lights.rk30board \
    power.rk30xx

# Audio
PRODUCT_PACKAGES += \
    audio.primary.rk30board \
    audio_policy.rk30board \
    audio.a2dp.default \
    audio.r_submix.default \
    audio.usb.default

# Other
PRODUCT_PACKAGES += \
    make_ext4fs \
    setup_fs \
    librs_jni \
    libnetcmdiface \
    com.android.future.usb.accessory
	
# Default config
PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=131072 \
    ro.sf.lcd_density=160 \
    wifi.interface=wlan0 \
    wifi.supplicant_scan_interval=30 \
    testing.mediascanner.skiplist=/storage/sdcard0/Android/ \
    ro.config.facelock=enable_facelock \
    persist.facelock.detect_cutoff=5000 \
    persist.facelock.recog_cutoff=5000 \
    persist.sys.strictmode.visual=false \
    persist.sys.usb.config=mtp \
    persist.sys.timezone=Europe/Amsterdam \
    ro.product.locale.language=es \
    ro.product.locale.region=ES
	
# Dalvik heap config
$(call inherit-product, frameworks/native/build/tablet-7in-hdpi-1024-dalvik-heap.mk)
