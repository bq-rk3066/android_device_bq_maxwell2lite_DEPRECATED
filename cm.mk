# Inherit some common CyanogenMod stuff
$(call inherit-product, vendor/cm/config/common_full_tablet_wifionly.mk)

# Inherit device configuration
$(call inherit-product, device/bq/maxwell2lite/maxwell2lite.mk)

# Boot animation
TARGET_SCREEN_HEIGHT := 600
TARGET_SCREEN_WIDTH := 1024

# Device identifier
PRODUCT_DEVICE := maxwell2lite
PRODUCT_NAME := cm_maxwell2lite

# Set build fingerprint, ID, product name, etc.
PRODUCT_BUILD_PROP_OVERRIDES += PRODUCT_NAME=bq_Maxwell2Lite BUILD_FINGERPRINT=bq/bq_Maxwell2Lite/bq_rk:4.1.1/1.0.1_20130812-09:59/46:user/release-keys PRIVATE_BUILD_DESC="bq_Maxwell2Lite-user 4.1.1 1.0.1_20130812-09:59 46 release-keys" BUILD_NUMBER=46
